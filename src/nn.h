//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		nn.h		 				Version 0.1
//		-----------------------------------------------------------
//		Definition - implementation of general artificial neural
//		system (ans) functionality.
//		-----------------------------------------------------------


#ifndef NN_H
#define NN_H

#include "nnlib2.h"
#include "nnlib2_dllist.h"
#include "component.h"

#ifdef NNLIB2_FOR_MFC_UI
#include "..\nnlib2.mfcgui\nnlib2_mfc_ui.h"
#endif

namespace nnlib2 {

/*-----------------------------------------------------------------------*/
/* Artificial Neural Network (ans)					 */
/*-----------------------------------------------------------------------*/

class nn : public component, public data_receiver, public data_provider, public error_flag_server
 {
 private:

 bool m_nn_is_ready;							// indicates that setup - or load - has been performed and nn is ready for encode/decode...

 protected:

 pointer_dllist <component PTR>	topology;		                // ordered list of pointers to major nn components; indicates FeedForward/FeedBackWard processing order; added items (components) are displayed/serialised, and also are deleted in nn's destructor () (when nn is deleted).

 int  m_topology_component_for_input;                                   // which component in the topology is used for output
 int  m_topology_component_for_output;                                  // which component in the topology is used for output

 dllist <DATA>	parameters;						// to store additional parameters that may be needed by various NNs.

 bool connect_consequent_layers(bool fully_connect = true);             // locate layer+connection_set+layer sequences in topology and create internal connections
 bool set_component_for_input(int i);                                   // set which component in the topology is used for input
 bool set_component_for_output(int i);                                  // set which component in the topology is used for output

 void set_ready() { m_nn_is_ready = true; }

 public:

 nn(string name);
 ~nn();

 virtual void reset();
 virtual bool set_additional_parameters(double param1,...);

 void encode();                                                         // (required by component class) performs a typical feed forward encode, from first to last component in topology. Assumes data is available. Overide if something else is needed.
 void recall();                                                         // (required by component class) performs a typical feed forward recall, from first to last component in topology. Assumes data is available. Overide if something else is needed.

 // unsupervised and supervised variations to customize if needed by model.
 // By default, they present the data vectors and use encode() and recall() on the NN topology.
 virtual DATA encode_u(DATA PTR input, int input_dim, int iteration=0);
 virtual DATA encode_s(DATA PTR input, int input_dim, DATA PTR desired_output, int output_dim, int iteration=0);

 // variations to customize if needed by model. By default, they use recall() above.
 virtual bool recall(DATA PTR input, int dim);
 virtual bool recall(DATA PTR input,int input_dim, DATA PTR output_buffer, int output_dim);

 bool input_data_from_vector(DATA * data, int dimension);               // (data_receiver virtual method) attemps to place data on the m_topology_component_for_input component (sets it to first if unspecified), assuming it [is a layer that] can input data (data_receiver)
 bool send_input_to(int position, DATA d);                              // (data_receiver virtual method) as above, sets value to corresponding pe input
 bool output_data_to_vector(DATA * buffer, int dimension);              // (data_provider virtual method) attemps to get data from the m_topology_component_for_output component (sets it to last if unspecified) component, assuming it [is a layer that] can output data (data_provider)
 DATA get_output_from (int position);                                   // (data_provider virtual method) as above, gets value from corresponding pe output

 virtual int input_dimension();                                         // overide if not using topology.
 virtual int output_dimension();                                        // overide if not using topology.

 int input_length()     { return input_dimension();  }
 int output_length()    { return output_dimension(); }

 int size();								// returns number of components in topology

 bool is_ready()        { return (no_error() && m_nn_is_ready); }
 virtual string description ();
 string item_description (int item);
 void from_stream ( std::istream REF s );			        // overides virtual method in component, only reads header
 void to_stream   ( std::ostream REF s );				    // overides virtual method in component

 component * component_from_id(int id);                                 // returns a pointer to component with given id in topology, NULL if not available
 component * component_from_topology_position(int position);            // returns a pointer to the p-th component in topology, NULL if not available
 };

}   // end of namespace nnlib2

/*-----------------------------------------------------------------------*/
#ifdef  NNLIB2_FOR_MFC_UI
#define NN_PARENT_CLASS	nn_with_ui					// use a nn base class polluted with MFC support (for ms-windows dialogs)
#include "..\nnlib2.mfcgui\nnlib2_nn_with_mfc_ui.h"
#else
#define NN_PARENT_CLASS	nn					        // use the standard nn base class
#endif
/*-----------------------------------------------------------------------*/

#endif // NN_H
