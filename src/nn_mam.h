//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		nn_mam.h		 								Version 0.1
//		-----------------------------------------------------------
//		Definition - implementation of MAM artificial neural
//		system (mam_nn) functionality.
//		NOTE:
//		To use MAM as example in the accompaning paper, a second
//		implementation variant is also found below.
//		-----------------------------------------------------------


#ifndef NN_MAM_H
#define NN_MAM_H

#include "nn.h"

#include "layer.h"
#include "connection_set.h"

//----------------------------------------------------------------------------
//	Note: to present MAM as example in the accompaning paper, two functionaly-
// equivalent MAM variants (implementations) were created (below)
// The second (mam1_nn) uses member variables, is more verbose, etc
//----------------------------------------------------------------------------

//#define MAM_USE_VARIANT

#ifndef MAM_USE_VARIANT
#define MAM_VARIANT mam_nn
#else
#define MAM_VARIANT mam1_nn
#endif

//----------------------------------------------------------------------------

namespace nnlib2 {
namespace mam {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// MAM similar to that discussed in paper...
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// define what MAM connections (synapses) do:

class mam_connection: public connection
{
public:
	// note: desired_output is "input" to destination (output) layer
	void encode() { weight() = weight() + source_pe().output * destin_pe().input; }
	void recall() { destin_pe().receive_input_value ( weight()*source_pe().output ); }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// define MAM components (layers containing generic pes and connections_sets of mam_connections)

typedef Layer<pe> mam_layer;
typedef Connection_Set<mam_connection> mam_connection_set;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// define the actual MAM nn class here:
// Dynamic allocation version (using nn topology). This is better-fit for
// implementing more complex, dynamically-defined and/or multi-layer
// (especially feed-FW/BW) NN models. MAM is not the best example...
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Using MAM nodes and connections
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class mam_nn : public NN_PARENT_CLASS
{
public:

	mam_nn(int input_length,int output_length)
		:nn("MAM Neural Network")
	{
		// setup the components (two layers and a set of connections)
		// note: optionally a common error flag is used so runtime errors in individual
		// components are reported to the entire neural net.
		// also add (register) the components to topology. These will be deleted when NN is deleted.

		topology.append(new mam_layer ("Input layer",input_length,my_error_flag()));
		topology.append(new mam_connection_set);
		topology.append(new mam_layer("Output layer",output_length,my_error_flag()));

		// setup connections for all layer+connection_set+layer sequences, fully connecting them
		connect_consequent_layers();

		set_component_for_input(0);				// optional (as it is the first layer)
		set_component_for_output(2);			// optional (as it is the last layer), but will this will be set at first recall

		// indicate NN is ready to encode/decode
		if (no_error()) set_ready();
	}
};


//----------------------------------------------------------------------------
#ifdef MAM_USE_VARIANT
//----------------------------------------------------------------------------
// MAM variant (processing defined in components (layers and connection_sets),
// components are member variables, and more detailed instructions are provided)
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// define what a mam layers do:

class mam1_layer : public pe_layer 		// aka layer<pe>
{
public:

	void encode() { move_all_pe_input_to_output(); }			// in MAM encode, just trasfer input to output for all PEs.
	void recall() { move_all_pe_input_to_output(); }  			// in MAM recall, just trasfer input to output for all PEs.
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// define what a MAM (set of) connections do:

class mam1_connection_set : public generic_connection_set  	// aka connection_set<connection>
{
public:

	void encode()
	{
		if(no_error())
			for(int i=0;i<connections.size();i++)
			{
				connection REF c = connections[i];
				c.weight() = c.weight() + source_pe(c).output * destin_pe(c).output;
			}
	}

	void recall()
	{
		if(no_error())
			if(connections.goto_first())								// Note: more efficient than above, as it avoids []:
				do
				{
					connection REF c = connections.current();
					destin_pe(c).input = destin_pe(c).input + c.weight() * source_pe(c).output;
				}
				while(connections.goto_next());
	}

};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// define the actual MAM nn class:

class mam1_nn : public NN_PARENT_CLASS
{
protected:

	mam1_layer 			layer1;
	mam1_connection_set connections;
	mam1_layer 			layer2;

public:

	mam1_nn(int input_dimension,int output_dimension)
		:NN_PARENT_CLASS("MAM (variant 1)")
	{
		// setup the components (two layers and a set of connections)
		// note: optionaly a common error flag is used so runtime errors in individual
		// components are reported to the entire neural net.

		layer1.setup("Input",input_dimension,my_error_flag());
		layer2.setup("Output",output_dimension,my_error_flag());

		connections.setup("Connections",&layer1,&layer2,my_error_flag());
		connections.fully_connect();
		connections.set_connection_weights(0);			// in MAM NNs, connections are initialy 0

		if(no_error()) set_ready();
	}

	int input_dimension()  {return layer1.size();}
	int output_dimension() {return layer2.size();}

	void encode_s(DATA PTR input, int input_dim, DATA PTR desired_output, int output_dim)
	{
		if(NOT is_ready()) return;
		// in MAM both 1st and last layer get data before encode...
		layer1.input_data_from_vector(input,input_dim);
		layer2.input_data_from_vector(desired_output,output_dim);
		layer1.encode();
		layer2.encode();
		connections.encode(); 		//...then update connection weights
	}


	bool recall(DATA PTR input,int input_dim, DATA PTR output_buffer, int output_dim)
	{
		// note: this is a typical feed-forward recall...
		if(NOT is_ready()) return false;
		layer1.input_data_from_vector(input,input_dim);
		layer1.recall();
		connections.recall();
		layer2.recall();
		layer2.output_data_to_vector(output_buffer,output_dim);
		return(no_error());
	}
};
//----------------------------------------------------------------------------
#endif // MAM_USE_VARIANT
//----------------------------------------------------------------------------

#endif // NN_MAM_H

} // end of namespace mam
} // end of namespace nnlib2

