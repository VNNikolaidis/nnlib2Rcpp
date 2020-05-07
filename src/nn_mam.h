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

//----------------------------------------------------------------------------

namespace nnlib2 {
namespace mam {

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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	mam_nn()
		:nn("MAM Neural Network") {}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	bool setup(int input_length,int output_length)
		{
		reset();

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

		return no_error();

		}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

};

//----------------------------------------------------------------------------

} // end of namespace mam
} // end of namespace nnlib2

#endif // NN_MAM_H
