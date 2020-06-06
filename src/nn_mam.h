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

typedef Layer<pe> mam_layer;								// not used below, MAM layers are generic
typedef Connection_Set<mam_connection> mam_connection_set;  // not used below, MAM connection sets simply consist of MAM connections

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

		add_layer(new pe_layer ("Input layer",input_length));
		add_connection_set(new mam_connection_set);
		add_layer(new pe_layer ("Output layer",output_length));

		// setup connections for all layer+connection_set+layer sequences, fully connecting them
		connect_consecutive_layers();

//		set_component_for_input(0);				// optional (as it is the first layer)
//		set_component_for_output(2);			// optional (as it is the last layer)

		return no_error();

		}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

};

//----------------------------------------------------------------------------

} // end of namespace mam
} // end of namespace nnlib2

#endif // NN_MAM_H
