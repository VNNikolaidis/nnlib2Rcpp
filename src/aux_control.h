//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis         All rights reserved.
//		-----------------------------------------------------------
//		aux_control.h		 							Version 0.1
//		-----------------------------------------------------------
//		component for misc functionality
//		-----------------------------------------------------------
//      This is meant to fit within NN's topology, & provide encode/
//      recall functions that can be used to implement additional,
//      user-defined functionality. This may be control code or
//      functionality such as output, user-break, communication w/
//      other components, creation/deletion of components, data
//      operations (which_max,feature extraction) etc.
//		-----------------------------------------------------------


#ifndef NN_AUX_CONTROL_H
#define NN_AUX_CONTROL_H

#include "component.h"
#include "layer.h"

namespace nnlib2 {

/*-----------------------------------------------------------------------*/
//		component for misc functionality
/*-----------------------------------------------------------------------*/

class aux_control : public data_io_component, public error_flag_client
{
public:
aux_control();
void encode (){};														   // overrides virtual method in component, override if required
void recall (){};														   // overrides virtual method in component, override if required
bool input_data_from_vector(DATA * data, int dimension){return false;}     // overrides virtual method in data_receiver, override if required
bool output_data_to_vector(DATA * buffer, int dimension){return false;}    // overrides virtual method in data_provider, override if required
bool send_input_to(int index, DATA d){return false;}                       // overrides virtual method in data_receiver, override if required
DATA get_output_from (int index){return 0;}                                // overrides virtual method in data_provider, override if required
};

/*-----------------------------------------------------------------------*/
// example of component for misc functionality
/*-----------------------------------------------------------------------*/

class aux_txt_printer : public aux_control
{
private:
		layer PTR mp_layer;
protected:
	void print();
public:
	aux_txt_printer(bool PTR error_flag_to_use=NULL);
	aux_txt_printer(layer PTR p_layer, bool PTR error_flag_to_use=NULL);
	void setup(layer PTR p_layer, bool PTR error_flag_to_use=NULL);
	void encode ();
	void recall ();
};

/*-----------------------------------------------------------------------*/

}  // end of namespace nnlib2

#endif // NN_AUX_CONTROL_H
