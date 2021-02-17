/*-----------------------------------------------------------------------*/
/* Artificial Neural Network (ans)				                               */
/*-----------------------------------------------------------------------*/

#include "nnlib2.h"
#include "nn.h"
#include "layer.h"
#include "connection_set.h"

#include <stdarg.h>
#include <sstream>

namespace nnlib2 {

// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// create

nn::nn(string name)
  :component(name,cmpnt_nn)
 {
 reset();
 }

nn::nn()
  :component("Neural Network",cmpnt_nn)
 {
 reset();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

nn::~nn()
 {
 reset();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool nn::set_additional_parameters(double param1,  ...)
 {
 if(no_error())
  {
  double d = param1;
  va_list marker;

  parameters.reset();
  va_start (marker,param1);

  while (d NEQL (double)DATA_MIN )
   {
   parameters.append((DATA)d);
   d = va_arg(marker,double);
   }

  va_end( marker);
  }
 return no_error();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void nn::reset(bool clear_additional_parameters)
 {
 m_nn_is_ready = false;

 if(clear_additional_parameters) parameters.reset();

 topology.set_error_flag(my_error_flag());

 reset_error();

 while(topology.goto_last())
   {
   delete(topology.current());		  // first free the component...
   topology.remove_last();	        // then remove its entry from pointer list.
   }

 topology.check();

 m_topology_component_for_input = -1;
 m_topology_component_for_output = -1;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

DATA nn::encode_u(DATA PTR input, int input_dim, int iteration)
 {
 error(NN_SYSTEM_ERR,"Unsupervised encode not implemented");
 return 0;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// performs a typical supervised encoding, assuming input will go to first layer (unless otherwise defined), while
// desired output will be presented as _input_ to the last layer in topology (unless otherwise defined).
// May return indication of encoding success.
// Should be overiden with needed behaviour, as this is very basic.

DATA nn::encode_s(DATA PTR input, int input_dim, DATA PTR desired_output, int output_dim, int iteration)
 {
 if(NOT is_ready()) return DATA_MAX;

 if(topology.size()<=0)
   {
   error(NN_SYSTEM_ERR,"Supervised encoding failed, no items in topology");
   return DATA_MAX;
   }

 if(input_data_from_vector(input,input_dim))
 if(topology.goto_last())
  {
  layer * p_last_layer = dynamic_cast <layer *> (topology.current());
  if (p_last_layer==NULL)
    {
    error(NN_INTEGR_ERR,"Last component is not a layer");
    return DATA_MAX;
    }
  if(NOT p_last_layer->input_data_from_vector(desired_output,output_dim))
    {
    return DATA_MAX;
    }
  encode();
  return 0;
  }

 return DATA_MAX;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Performs a typical recall, in the direction from input to output
// in topology. Assumes data is already there. Overide if needed.

void nn::recall()
{
if(NOT is_ready()) return;

if(m_topology_component_for_input<=m_topology_component_for_output)
 {
 if(topology.goto_first())
  do { topology.current()->recall(); }
  while(topology.goto_next());
 }
else
 {
 if(topology.goto_last())
  do { topology.current()->recall(); }
  while(topology.goto_previous());
 }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Performs a typical feed forward encode, in the direction from input to output
// in topology. Assumes data is already there. Overide if needed.

void nn::encode()
{
if(NOT is_ready()) return;

if(m_topology_component_for_input<=m_topology_component_for_output)
  {
  if(topology.goto_first())
  do { topology.current()->encode(); }
  while(topology.goto_next());
  }
else
  {
  if(topology.goto_last())
  do { topology.current()->encode(); }
  while(topology.goto_previous());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// overide if necessary...

int nn::input_dimension()
 {
 // we are probably making too many unneeded checks here
 if(m_topology_component_for_input<0) return 0;
 if(topology.is_empty()) return 0;
 if(topology.size()<=m_topology_component_for_input)return 0;
 if(topology[m_topology_component_for_input]->type()!=cmpnt_layer) return 0;
 layer PTR p_input_layer = reinterpret_cast<layer PTR>(topology[m_topology_component_for_input]);
 if (p_input_layer!=NULL) return p_input_layer->size();
 return 0;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// overide if necessary...

int nn::output_dimension()
 {
 // we are probably making too many unneeded checks here
 if(m_topology_component_for_output<0) return 0;
 if(topology.is_empty()) return 0;
 if(topology.size()<=m_topology_component_for_output)return 0;
 if(topology[m_topology_component_for_output]->type()!=cmpnt_layer) return 0;
 layer PTR p_output_layer = reinterpret_cast<layer PTR>(topology[m_topology_component_for_output]);
 if (p_output_layer!=NULL) return p_output_layer->size();
 return 0;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// (required by data_receiver class)
// attemps to place data on the first component, unless otherwise defined.
// assuming it [is a layer that] can input data (data_receiver)

bool nn::input_data_from_vector(DATA * data, int dimension)
 {
 if (NOT is_ready()) return false;
 if (data == NULL) return false;
 if (topology.is_empty())          {error(NN_INTEGR_ERR,"Topology is empty)"); return false;}
 // use first component, unless otherwise defined
 if(m_topology_component_for_input<0)
  if(NOT set_component_for_input(0))
    return false;
 data_receiver * pl = dynamic_cast <data_receiver *> (topology[m_topology_component_for_input]);
 if (pl==NULL) {error(NN_INTEGR_ERR,"Requested component cannot accept data");return false;}
 return(pl->input_data_from_vector(data,dimension));
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// (required by data_receiver class)
// attemps to place data on the m_topology_component_for_input component
// (sets it to first if not specified)
// assuming it [is a layer that] can input data (data_receiver)
// overides virtual method in data_receiver, as above, sets value to corresponding pe input

bool nn::send_input_to(int index, DATA d)
{
if (NOT is_ready()) return false;
// use first component, unless otherwise defined
if(m_topology_component_for_input<0)
  if(NOT set_component_for_input(0))
    return false;
data_receiver * pl = dynamic_cast <data_receiver *> (topology[m_topology_component_for_input]);
if (pl==NULL) {error(NN_INTEGR_ERR,"Requested component cannot accept data");return false;}
return pl->send_input_to(index,d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// (required by data_provider class)
// attemps to get data from the last component, unless otherwise defined
// assuming it [is a layer that] outputs data (data_provider)

bool nn::output_data_to_vector(DATA * buffer, int dimension)
{
 if (buffer == NULL) return false;
 if (topology.is_empty())           {error(NN_INTEGR_ERR,"Topology is empty)"); return false;}
 // use last component, unless otherwise defined
 if(m_topology_component_for_output<0)
  if(NOT set_component_for_output(topology.size()-1))
   return false;
 data_provider * pl = dynamic_cast <data_provider *> (topology[m_topology_component_for_output]);
 if (pl==NULL) {error(NN_INTEGR_ERR,"Requested component does not output data");return false;}
 return(pl->output_data_to_vector(buffer,dimension));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// (required by data_provider class)
// attemps to get data from the m_topology_component_for_output component in topology
// (sets it to last if not specified)
// assuming it [is a layer that] outputs data (data_provider)
// overides virtual method in data_provider, as above, gets value from corresponding pe output

DATA nn::get_output_from (int index)
{
 if (NOT is_ready()) return false;
 if (topology.is_empty()) return false;
 // use last component, unless otherwise defined
 if(m_topology_component_for_output<0)
  if(NOT set_component_for_output(topology.size()-1))
   return false;
 data_provider * pl = dynamic_cast <data_provider *> (topology[m_topology_component_for_output]);
 if (pl==NULL) {error(NN_INTEGR_ERR,"Requested component does not output data");return false;}
 return pl->get_output_from(index);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// set which component in the topology is used for input (by index position in topology)

bool nn::set_component_for_input(int index)
 {
 if((index<0) OR
    (index>=topology.size()))
     {warning("Requested input component is not in topology"); return false;}
 if(topology[index]->type()!=cmpnt_layer)
     {warning("Requested input component is not a layer"); return false;}
 m_topology_component_for_input = index;
 #ifndef NNLIB2_WITH_GUI
 // TEXTOUT << "Note: Using NN component in topology index position " << m_topology_component_for_input << " for input.\n";
 #endif
 return true;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// set which component in the topology is used for output

bool nn::set_component_for_output(int index)
 {
 if((index<0) OR
   (index>=topology.size()))
    {warning("Requested output component is not in topology"); return false;}
 if(topology[index]->type()!=cmpnt_layer)
    {warning("Requested output component must be a layer"); return false;}
 m_topology_component_for_output = index;
 #ifndef NNLIB2_WITH_GUI
 // TEXTOUT << "Note: Using NN component in topology index position " << m_topology_component_for_output << " for output.\n";
 #endif
 return true;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool nn::recall(DATA PTR input,int dim)
 {
 if(input_data_from_vector(input,dim))
  {
  recall();
  return true;
  }
 return false;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool nn::recall(DATA PTR input,int input_dim,DATA PTR output_buffer,int output_dim)
 {
 if(recall(input,input_dim))
   {
   output_data_to_vector(output_buffer,output_dim);
   return no_error();
   }
 return false;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int nn::size()
 {
 return topology.number_of_items();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

string nn::item_description (int item)
 {
 return topology[item]->description().c_str();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

string nn::description ()
 {
 string s = m_name;
 if(m_nn_is_ready)	s += " (Ready -";
 else				        s += " (Not Ready -";
 if(no_error())		  s += " No Error)";
 else				        s += " Error)";
 return s;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// input it : This is very generic and should be replaced by custom code
// by child-classes. Actual components should be read in child class.
// This only reads a header (useful in derived classes).
// Also read I/O dims (as was done in early versions) but does not store them

void nn::from_stream ( std::istream REF s )
 {
 reset_error();
 component::from_stream(s);
 if(no_error())
  {
  string comment;
  int i_dim;
  int o_dim;

  if(s.rdstate()) {error(NN_IOFILE_ERR,"Error reading stream (Neural Net)");return;}

  s >> comment >> i_dim ;
  s >> comment >> o_dim ;

  if ((i_dim>0) OR (o_dim>0))
   if ((input_dimension()>0) OR (output_dimension()>0))							// to avoid complaints when loading into an empty NN
    if ((i_dim NEQL input_dimension()) OR (o_dim NEQL output_dimension()))
      warning("Current neural net used different input-output dimensions from stored ones.");
  }
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// output it :

void nn::to_stream   ( std::ostream REF s )
 {

 if(NOT m_nn_is_ready) warning("Neural net is not initialized!");

 component::to_stream(s);

 if(no_error())
  {
  if(s.rdstate()) {error(NN_IOFILE_ERR,"Error writing stream (Neural Net)");return;}

  s << "Input_Dim: " << input_dimension()  << "\n";
  s << "OutputDim: " << output_dimension() << "\n";

// since 'topology' is a list of pointer to items, it outputs the items (not the pointers)

  s << "NumCompon: " << topology.number_of_items() << "\n";

  if(topology.goto_first())
   {
   do topology.current()->to_stream(s);
   while (topology.goto_next());
   }
  }
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// output a textual summary of the NN structure

string nn::outline (bool show_first_index_as_one)
 {
 std::stringstream s;
 s << description() << "\n";

 if(topology.goto_first())
 {
   int c=0;
   s << "Current NN topology:\n";
   do
    {
    if(show_first_index_as_one)
     {
     s << "@" << c+1 << " (c=" << c << ")";
     }
    else
     {
     s << "@" << c;
     }
    s << " component (id=" << topology.current()->id() << ")";
    s << " is " << topology.current()->description();
    s << " of size " <<  topology.current()->size() << "\n";
    c++;
    }
     while (topology.goto_next());
 }
 return s.str();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// append components to topology

bool nn::add_component(component * p_component)
  {
  if(p_component!=NULL) return topology.append(p_component);
  return false;
  }

bool nn::add_layer(layer * p_layer)
  {
  if(add_component(p_layer))
   {
   p_layer->set_error_flag(my_error_flag());
   return true;
   }
  return false;
  }

bool nn::add_connection_set(connection_set * p_connection_set)
{
  if(add_component(p_connection_set))
  {
    p_connection_set->set_error_flag(my_error_flag());
    return true;
  }
  return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Trigger encode for component at specified topology index position

bool nn::call_component_encode(int index)
  {
  if(index<0) return false;
  if(index>=topology.size()) return false;
  topology[index]->encode();
  return true;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Trigger recall for component at specified topology index position

bool nn::call_component_recall(int index)
  {
  if(index<0) return false;
  if(index>=topology.size()) return false;
  topology[index]->recall();
  return true;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Trigger encode for all components in the topology
// (1st to last if fwd=true, last to 1st if fwd=false)

bool nn::call_component_encode_all(bool fwd)
{
if(fwd)
  if(topology.goto_first())
  {
  do topology.current()->encode();
  while (topology.goto_next());
  return true;
  }
if(NOT fwd)
  if(topology.goto_last())
  {
  do topology.current()->encode();
  while (topology.goto_previous());
  return true;
  }
return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Trigger encode for all components in the topology
// (1st to last if fwd=true, last to 1st if fwd=false)

bool nn::call_component_recall_all(bool fwd)
{
  if(fwd)
    if(topology.goto_first())
    {
    do topology.current()->recall();
    while (topology.goto_next());
    return true;
    }
  if(NOT fwd)
    if(topology.goto_last())
    {
    do topology.current()->recall();
    while (topology.goto_previous());
    return true;
    }
return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// returns a pointer to component with given id in topology, NULL if not available

component * nn::component_from_id(int id)
{
if(topology.goto_first())
  {
    do
      if(topology.current()->id()==id) return topology.current();
    while (topology.goto_next());
  }
return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// returns a pointer to the "index"-th component in topology, NULL if not available

component * nn::component_from_topology_index(int index)
{
  int i=0;
  if(topology.goto_first())
  {
    do
    {
      if(index==i) return topology.current();
      i++;
    }
    while (topology.goto_next());
  }
  return NULL;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// returns the index position in topology for component with given id, negative (-1) if not available

int nn::component_topology_index_from_id(int id)
{
  component PTR pc = component_from_id(id);
  if(pc==NULL) return -1;
  return pc->id();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// returns the id of the "index"-th component in topology, -1 if not available

int nn::component_id_from_topology_index(int index)
{
  if(topology.goto_first())
  {
    int i = 0;
    do
    {
      if(index==i) return topology.current()->id();
      i++;
    }
    while (topology.goto_next());
  }
  warning("No component with requested id is found in topology");
  return -1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// only suitable for 2 layer topologies, so disabled (to avoid errors)

/*
bool nn::add_to_topology( layer * pl1,
                          connection_set * pcs,
                          layer * pl2,
                          bool fully_connect)
{
  if(!no_error()) return false;

  if(pl1==NULL) {error(NN_NULLPT_ERR,"Cannot add layer"); return false;}
  if(pcs==NULL) {error(NN_NULLPT_ERR,"Cannot add connection set"); return false;}
  if(pl2==NULL) {error(NN_NULLPT_ERR,"Cannot add layer"); return false;}

  if(pl1->type()!=cmpnt_layer)          {error(NN_INTEGR_ERR,"Cannot add layer"); return false;}
  if(pcs->type()!=cmpnt_connection_set) {error(NN_INTEGR_ERR,"Cannot add connection set"); return false;}
  if(pl2->type()!=cmpnt_layer)          {error(NN_INTEGR_ERR,"Cannot add layer"); return false;}

  if(pl1->size()<=0)                    {error(NN_INTEGR_ERR,"Cannot add layer that has not been setup"); return false;}
  if(pcs->has_source_layer())           {error(NN_INTEGR_ERR,"Cannot add connection set that is already attached to source layer"); return false;}
  if(pcs->has_destin_layer())           {error(NN_INTEGR_ERR,"Cannot add connection set that is already attached to destination layer"); return false;}
  if(pl2->size()<=0)                    {error(NN_INTEGR_ERR,"Cannot add layer that has not been setup"); return false;}

  topology.append(pl1);									// add (register) the components to topology. These will be deleted when NN is deleted.t;
  topology.append(pcs);
  topology.append(pl2);

  // layers need to exist before the connection_set connecting them is setup
  pcs->setup("Connections",pl1,pl2,my_error_flag(),fully_connect);

  return no_error();
}
*/

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// locate layer+connection_set+layer sequences in topology and create
// connections (in the connection_set)

bool nn::connect_consecutive_layers(bool fully_connect, bool set_ready_to_encode_fwd, DATA min_random_weight, DATA max_random_weight)
{
if(!no_error()) return false;

component PTR p_c1 = NULL;
component PTR p_c2 = NULL;
component PTR p_c3 = NULL;

bool all_connected = true;

if(topology.size()<3)           {error(NN_INTEGR_ERR,"not enough (3) components in topology"); return false;}
if(NOT topology.goto_first())   {error(NN_INTEGR_ERR,"nn topology is empty"); return false;}

// scan topology to find layers...

dllist<int> layer_indexes_in_topology;
for(int index=0;index<topology.size();index++)
  {
  p_c1 = topology[index];
  if(p_c1!=NULL)
   if(p_c1->type()==cmpnt_layer)
     layer_indexes_in_topology.append(index);
  }
if(layer_indexes_in_topology.size()<2) {error(NN_INTEGR_ERR,"not enough layers (<2) in topology"); return false;}

// at least 2 layers found...

for(int i=0;i<layer_indexes_in_topology.size()-1;i++)
 {
 bool pair_connected = false;
 // connect all connections sets between layer pairs....
 int c1_pos = layer_indexes_in_topology[i];
 int c3_pos = layer_indexes_in_topology[i+1];
 p_c1 = topology[c1_pos];
 p_c3 = topology[c3_pos];
 if((p_c1 == NULL) OR (p_c3 == NULL)) {error(NN_INTEGR_ERR,"Something went wrong while connecting layers");return false;}
 for(int c2_pos=c1_pos+1;c2_pos<c3_pos;c2_pos++)
  {
  p_c2 = topology[c2_pos];
  if(p_c2!=NULL)
   if(p_c2->type()==cmpnt_connection_set)
    {
    layer * play1 = reinterpret_cast<layer *>(p_c1);
    connection_set * pconx = reinterpret_cast<connection_set *>(p_c2);
    layer * play2 = reinterpret_cast<layer *>(p_c3);

    bool do_connect = true;

    // found layer+connection_set+layer sequence
    if(play1->size()<=0)              {warning("Cannot connect, source layer has not been setup"); do_connect = false;}
    if(pconx->has_source_layer())     {warning("Cannot setup connection set that is already attached to source layer"); do_connect = false;}
    if(pconx->has_destin_layer())     {warning("Cannot setup connection set that is already attached to destination layer"); do_connect = false;}
    if(play2->size()<=0)              {warning("Cannot connect, destination layer has not been setup"); do_connect = false;}

    // layers exist and are setup, so now setup internal connection_set connecting them.
    if(do_connect)
     {
     pconx->setup(pconx->name(),play1,play2,my_error_flag(),fully_connect,min_random_weight,max_random_weight);
     pair_connected = true;
     }
    }
  }
 all_connected = all_connected AND pair_connected;
 }

if (NOT all_connected)  warning("Could not connect all layer pairs.");
bool check_ok = all_connected AND no_error();
if (set_ready_to_encode_fwd AND check_ok)
 {
 set_component_for_input(0);            // first component
 set_component_for_output(size()-1);    // last component
 set_is_ready_flag();
 }
return check_ok;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// connect two layers using component ids

bool nn::connect_layers_with_ids(int source_layer_id,                  // source layer (component) id
                                 int destination_layer_id,             // destination source layer (component) id
                                 connection_set PTR p_connection_set,  // preallocated new connection set to use for connecting the layers (will be inserted in topology after source layer)
                                 bool fully_connect,                   // create connections between all pes in layers
                                 DATA min_random_weight,               // if creating connections, randomize weights between...
                                 DATA max_random_weight)               // these two values.
  {
  int p1 = component_topology_index_from_id(source_layer_id);
  int p2 = component_topology_index_from_id(destination_layer_id);
  if(p1<0) return false;
  if(p2<0) return false;
  return connect_layers_at_topology_indexes(p1,p2,p_connection_set,fully_connect,min_random_weight,max_random_weight);
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// connect two layers using their current index positions in topology

bool nn::connect_layers_at_topology_indexes(                                // connect two layers using their current index positions in topology:
                                      int source_layer_index,               // current index position (in topology) of source layer
                                      int destination_layer_index,          // current index position (in topology) of destination layer
                                      connection_set PTR p_connection_set,  // preallocated new connection set to use for connecting the layers (will be inserted in topology after source layer)
                                      bool fully_connect,                   // create connections between all pes in layers
                                      DATA min_random_weight,               // if creating connections, randomize weights between...
                                      DATA max_random_weight)               // these two values.
 {
  if(p_connection_set==NULL) return false;
  if((source_layer_index<0) OR (source_layer_index>=topology.size())) return false;
  if((destination_layer_index<0) OR (destination_layer_index>=topology.size())) return false;

  component PTR p_c1 = topology[source_layer_index];
  component PTR p_c2 = topology[destination_layer_index];

  if((p_c1==NULL) OR (p_c2==NULL)) return false;

  if(p_c1->type()!=cmpnt_layer) { warning("Source is not a layer"); return false;}
  if(p_c2->type()!=cmpnt_layer) { warning("Destination is not a layer"); return false;}

  if(NOT topology.insert(source_layer_index+1,p_connection_set)) return false;

  layer PTR p_l1 = reinterpret_cast<layer PTR>(p_c1);
  layer PTR p_l2 = reinterpret_cast<layer PTR>(p_c2);

  return p_connection_set->setup(p_connection_set->name(),
                                 p_l1,
                                 p_l2,
                                 my_error_flag(),
                                 fully_connect, min_random_weight, max_random_weight);
 }


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// patch. NULL if not found or not layer

layer PTR nn::get_layer_at(int index)
{
  if(index<0) return NULL;
  if(index>=topology.size()) return NULL;
  if(topology.is_empty()) return NULL;

  component PTR p_comp = topology[index];
  if(p_comp==NULL) return NULL;

  if(p_comp->type()!=cmpnt_layer) return NULL;
  // component found and seems to be a layer

  layer PTR p_la = reinterpret_cast<layer PTR>(p_comp);
  return p_la;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// patch. NULL if not found or not connection_set

connection_set PTR nn::get_connection_set_at(int index)
{
  if(index<0) return NULL;
  if(index>=topology.size()) return NULL;
  if(topology.is_empty()) return NULL;

  component PTR p_comp = topology[index];
  if(p_comp==NULL) return NULL;

  if(p_comp->type()!=cmpnt_connection_set) return NULL;
  // component found and seems to be a connection_set

  connection_set PTR p_cs = reinterpret_cast<connection_set PTR>(p_comp);
  return p_cs;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// patch.

bool nn::add_connection(int index, int source_pe, int destin_pe, DATA weight)
{
  connection_set PTR p_cs = get_connection_set_at(index);
  if(p_cs == NULL) return false;
  return p_cs->add_connection(source_pe,destin_pe,weight);
  return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// patch.

bool nn::remove_connection(int index, int connection_number)
{
  connection_set PTR p_cs = get_connection_set_at(index);
  if(p_cs == NULL) return false;
  return p_cs->remove_connection(connection_number);
  return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// patch.

bool nn::get_input_data_at_component (int index, DATA * buffer, int dimension)
{
  if(buffer==NULL) return false;
  if(dimension<=0) return false;
  if(index<0) return false;
  if(index>=topology.size()) return false;
  if(topology.is_empty()) return false;

  component PTR p_comp = topology[index];
  if(p_comp==NULL) return false;

  int num_items = p_comp->size();
  if(num_items!=dimension)
    {
    warning("Cannot retreive inputs, sizes do not match");
    return false;
    }

  if(p_comp->type()==cmpnt_layer)                // component found and it is a layer
  {
  layer PTR p_la = reinterpret_cast<layer PTR>(p_comp);
  for(int i=0;i<num_items;i++) buffer[i] = p_la->PE(i).input;
  return true;
  }

  if(p_comp->type()==cmpnt_connection_set)       // component found and it is a connection_set
  {
  connection_set PTR p_cs = reinterpret_cast<connection_set PTR>(p_comp);
  for(int i=0;i<num_items;i++) buffer[i] = p_cs->source_pe(i).output;
  return true;
  }

  return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// patch.

bool nn::get_weights_at_component (int index, DATA * buffer, int dimension)
{
  if(buffer==NULL) return false;
  if(dimension<=0) return false;
  if(index<0) return false;
  if(index>=topology.size()) return false;
  if(topology.is_empty()) return false;

  component PTR p_comp = topology[index];
  if(p_comp==NULL) return false;
  if(p_comp->type()!=cmpnt_connection_set) return false;

  connection_set PTR p_cs = reinterpret_cast<connection_set PTR>(p_comp);
  if(p_cs->size()!=dimension) return false;

  for(int i=0;i<dimension;i++)
      {
      int source_component_id, source_item, destin_component_id, destin_item;
      DATA weight;
      if(p_cs->connection_properties(i, source_component_id, source_item, destin_component_id, destin_item, weight))
        buffer[i]=weight;
      else
        return false;
      }

  return true;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// patch. Returns 0 if not successful

DATA nn::get_weight_at_component(int index, int connection_number)
{
  connection_set PTR p_cs = get_connection_set_at(index);
  if(p_cs==NULL) {warning("Invalid connection set"); return 0;}
  return p_cs->get_connection_weight(connection_number);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// patch

bool nn::set_weight_at_component(int index, int connection_number, DATA weight)
{
  connection_set PTR p_cs = get_connection_set_at(index);
  if(p_cs==NULL) {warning("Invalid connection set"); return 0;}
  return p_cs->set_connection_weight(connection_number, weight);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// patch

bool nn::set_misc_at_component(int index, DATA * data, int dimension)
{
  layer PTR p_lay = get_layer_at(index);
  if (p_lay != NULL) return p_lay->set_misc(data,dimension);
  connection_set PTR p_cs = get_connection_set_at(index);
  if (p_cs != NULL) return p_cs->set_misc(data,dimension);
  return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// patch, currently only works only for layers
// overrides current pe output registers with the provided data values

bool nn::set_output_at_component(int index, DATA * data, int dimension)
{
  layer PTR p_lay = get_layer_at(index);
  if (p_lay != NULL) return p_lay->set_output(data,dimension);
  return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// patch: avoid using, nn should set m_nn_is_ready flag itself, once its setup is completed

void nn::change_is_ready_flag(bool new_state)
 {
  m_nn_is_ready=new_state;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}   // end of namespace nnlib2
