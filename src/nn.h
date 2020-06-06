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

#include "layer.h"
#include "connection_set.h"

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

 int  m_topology_component_for_input;                                   // (index position in topology, 0 is first) which component in the topology is used for output
 int  m_topology_component_for_output;                                  // (index position in topology, 0 is first) which component in the topology is used for output

 dllist <DATA>	parameters;						// to store additional parameters that may be needed by various NNs.

 void set_is_ready_flag() { m_nn_is_ready = true; }

 public:

 nn();
 nn(string name);
 ~nn();

 virtual void reset();
 virtual bool set_additional_parameters(double param1,...);

 void encode();                                                         // (required by component class) performs a typical encode, in the direction from input to output. Assumes data is available. Overide if something else is needed.
 void recall();                                                         // (required by component class) Performs a typical recall, in the direction from input to output. Assumes data is available. Overide if something else is needed.

 // unsupervised and supervised variations to customize if needed by model.
 // By default, they present the data vectors and use encode() and recall() on the NN topology.
 virtual DATA encode_u(DATA PTR input, int input_dim, int iteration=0);
 virtual DATA encode_s(DATA PTR input, int input_dim, DATA PTR desired_output, int output_dim, int iteration=0);

 // variations to customize if needed by model. By default, they use recall() above.
 virtual bool recall(DATA PTR input, int dim);
 virtual bool recall(DATA PTR input,int input_dim, DATA PTR output_buffer, int output_dim);

 bool input_data_from_vector(DATA * data, int dimension);               // (data_receiver virtual method) attemps to place data on the m_topology_component_for_input component (sets it to first if unspecified), assuming it [is a layer that] can input data (data_receiver)
 bool send_input_to(int index, DATA d);                                 // (data_receiver virtual method) as above, sets value to corresponding pe input
 bool output_data_to_vector(DATA * buffer, int dimension);              // (data_provider virtual method) attemps to get data from the m_topology_component_for_output component (sets it to last if unspecified) component, assuming it [is a layer that] can output data (data_provider)
 DATA get_output_from (int index);                                      // (data_provider virtual method) as above, gets value from corresponding pe output

 virtual int input_dimension();                                         // overide if not using topology.
 virtual int output_dimension();                                        // overide if not using topology.

 int input_length()     { return input_dimension();  }
 int output_length()    { return output_dimension(); }

 int size();								// returns number of components in topology

 bool is_ready()        { return (no_error() && m_nn_is_ready); }
 virtual string description ();
 string outline (bool show_first_index_as_one=false);                   // output a textual summary of the NN structure
 string item_description (int item);
 void from_stream ( std::istream REF s );			        // overides virtual method in component, only reads header
 void to_stream   ( std::ostream REF s );			        // overides virtual method in component

 bool set_component_for_input(int index);                               // set which component in the topology is used for input (by index position in topology)
 bool set_component_for_input_by_id(int id);                            // set which component in the topology is used for input (by component id)
 bool set_component_for_output(int index);                              // set which component in the topology is used for output (by index position in topology)
 bool set_component_for_output_by_id(int id);                           // set which component in the topology is used for output (by component id)

 bool call_component_encode(int index);                                 // Trigger encode for component at specified topology index position
 bool call_component_recall(int index);                   	        // Trigger recall for component at specified topology index position

 bool call_component_encode_all(bool fwd = true);                       // Trigger encode for all components in the topology (1st to last if fwd=true, last to 1st if fwd=false)
 bool call_component_recall_all(bool fwd = true);              	        // Trigger recall for all components in the topology (1st to last if fwd=true, last to 1st if fwd=false)

 bool add_component      (component      * p_component);                // append any component to topology
 bool add_layer          (layer          * p_layer);                    // append a layer to topology
 bool add_connection_set (connection_set * p_connection_set);           // append a connection_set to topology

 bool connect_consecutive_layers(bool fully_connect = true,             // locate layer+connection_set+layer sequences in topology and create internal connections
                                 bool set_ready_to_encode_fwd = true,
                                 DATA min_random_weight=0,              // if creating connections, randomize weights between...
                                 DATA max_random_weight=0);             // these two values.


 bool connect_layers_with_ids(    int source_layer_id,                  // source layer id
                                  int destination_layer_id,             // destination layer id
                                  connection_set PTR p_connection_set,  // preallocated new connection set to use for connecting the layers (will be inserted in topology after source layer)
                                  bool fully_connect,                   // create connections between all pes in layers
                                  DATA min_random_weight=0,             // if creating connections, randomize weights between...
                                  DATA max_random_weight=0);            // these two values.

 bool connect_layers_at_topology_indexes(                               // connect two layers using their current position in topology:
                                  int source_layer_index,               // current index position (in topology) of source layer
                                  int destination_layer_index,          // current index position (in topology) of destination layer
                                  connection_set PTR p_connection_set,  // preallocated new connection set to use for connecting the layers (will be inserted in topology after source layer)
                                  bool fully_connect,                   // create connections between all pes in layers
                                  DATA min_random_weight=0,             // if creating connections, randomize weights between...
                                  DATA max_random_weight=0);            // these two values.

 int component_topology_index_from_id(int id);                          // returns the position in topology for component with given id, negative (-1) if not available
 int component_id_from_topology_index(int index);                       // returns the id of the "index"-th component in topology, -1 if not available

 component * component_from_id(int id);                                 // returns a pointer to component with given id in topology, NULL if not available
 component * component_from_topology_index(int index);                  // returns a pointer to the "index"-th component in topology, NULL if not available

 // some patches that may be useful, especialy for interactive use:

 layer PTR get_layer_at(int index);                                     // NULL if not found or not layer
 connection_set PTR get_connection_set_at(int index);                   // NULL if not found or not connection_set
 bool add_connection(int index, int source_pe, int destin_pe, DATA weight);
 bool get_input_data_at_component (int index, DATA * buffer, int dimension);
 bool get_weights_at_component (int index, DATA * buffer, int dimension);
 void change_is_ready_flag(bool new_state);                             // avoid using, nn should set m_nn_is_ready flag itself, once its setup is completed

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
