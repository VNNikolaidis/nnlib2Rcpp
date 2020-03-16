/*-----------------------------------------------------------------------*/
/* Artificial Neural Network (ans)				                               */
/*-----------------------------------------------------------------------*/

#include "nnlib2.h"
#include "nn.h"
#include "layer.h"
#include "connection_set.h"

#include <stdarg.h>

namespace nnlib2 {

// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// create

nn::nn(string name)
  :component(name,cmpnt_nn)
 {
 m_nn_is_ready = false;
 parameters.reset();
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

void nn::reset()
 {
 m_nn_is_ready = false;
 reset_error();

 while(topology.goto_last())
   {
   delete(topology.current());			// first free the component...
   topology.remove_last();				// then remove its entry from pointer list.
   }
 topology.looks_ok();

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
// performs a typical supervised encoding, assuming input will go to first layer, while
// desired output will be presented as _input_ to the last layer in topology.
// May return indication of encoding success.
// Should overide with needed behaviour, as this is very basic.

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
// Performs a typical feed forward recall, from first to last component
// in topology. Assumes data is already there. Overide if needed.

void nn::recall()
{
if(is_ready() AND topology.goto_first())
  do { topology.current()->recall(); }
  while(topology.goto_next());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Performs a typical feed forward encode, from first to last component
// in topology. Assumes data is already there. Overide if needed.

void nn::encode()
{
  if(is_ready() AND topology.goto_first())
    do { topology.current()->encode(); }
    while(topology.goto_next());
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
// attemps to place data on the first component
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

bool nn::send_input_to(int position, DATA d)
{
if (NOT is_ready()) return false;
// use first component, unless otherwise defined
if(m_topology_component_for_input<0)
  if(NOT set_component_for_input(0))
    return false;
data_receiver * pl = dynamic_cast <data_receiver *> (topology[m_topology_component_for_input]);
if (pl==NULL) {error(NN_INTEGR_ERR,"Requested component cannot accept data");return false;}
return pl->send_input_to(position,d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// (required by data_provider class)
// attemps to get data from the last component
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
// attemps to get data from the m_topology_component_for_output component
// (sets it to last if not specified)
// assuming it [is a layer that] outputs data (data_provider)
// overides virtual method in data_provider, as above, gets value from corresponding pe output

DATA nn::get_output_from (int position)
{
 if (NOT is_ready()) return false;
 if (topology.is_empty()) return false;
 // use last component, unless otherwise defined
 if(m_topology_component_for_output<0)
  if(NOT set_component_for_output(topology.size()-1))
   return false;
 data_provider * pl = dynamic_cast <data_provider *> (topology[m_topology_component_for_output]);
 if (pl==NULL) {error(NN_INTEGR_ERR,"Requested component does not output data");return false;}
 return pl->get_output_from(position);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// set which component in the topology is used for input

bool nn::set_component_for_input(int i)
 {
 if((i<0) OR
    (i>=topology.size()))
     {error(NN_INTEGR_ERR,"Requested input component is not in topology"); return false;}
 if(topology[i]->type()!=cmpnt_layer)
     {error(NN_INTEGR_ERR,"Requested input component is not a layer"); return false;}
 m_topology_component_for_input = i;
 #ifndef NNLIB2_WITH_GUI
 TEXTOUT << "Note: Using NN component " << m_topology_component_for_input << " in topology for input.\n";
 #endif
 return true;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// set which component in the topology is used for output

bool nn::set_component_for_output(int i)
 {
 if((i<0) OR
   (i>=topology.size()))
    {error(NN_INTEGR_ERR,"Requested output component is not in topology"); return false;}
 if(topology[i]->type()!=cmpnt_layer)
    {error(NN_INTEGR_ERR,"Requested output component is not a layer"); return false;}
 m_topology_component_for_output = i;
 #ifndef NNLIB2_WITH_GUI
 TEXTOUT << "Note: Using NN component " << m_topology_component_for_output << " in topology for output.\n";
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

 if(NOT m_nn_is_ready) warning("Saving uninitialized neural net!");

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
// returns a pointer to the p-th component in topology, NULL if not available

component * nn::component_from_topology_position(int id)
{
  int i=0;
  if(topology.goto_first())
  {
    do
    {
      if(id==i) return topology.current();
      i++;
    }
    while (topology.goto_next());
  }
  return NULL;
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

bool nn::connect_consequent_layers(bool fully_connect)
{
if(!no_error()) return false;

component PTR p_c1 = NULL;
component PTR p_c2 = NULL;
component PTR p_c3 = NULL;

bool seq_found     = false;
bool seq_connected = false;

if(NOT topology.goto_first())         {error(NN_INTEGR_ERR,"nn topology is empty"); return false;}

p_c1 = topology.current();
if(!topology.goto_next())            {error(NN_INTEGR_ERR,"not enough (3) components in topology"); return false;}
p_c2 = topology.current();
if(!topology.goto_next())            {error(NN_INTEGR_ERR,"not enough (3) components in topology"); return false;}
p_c3 = topology.current();

while( (p_c1 != NULL) AND (p_c2 != NULL) AND (p_c3 != NULL) )
 {
 if( (p_c1->type()==cmpnt_layer) AND
     (p_c2->type()==cmpnt_connection_set) AND
     (p_c3->type()==cmpnt_layer) )
     {
     layer * play1 = reinterpret_cast<layer *>(p_c1);
     connection_set * pconx = reinterpret_cast<connection_set *>(p_c2);
     layer * play2 = reinterpret_cast<layer *>(p_c3);

     // found layer+connection_set+layer sequence
     seq_found = true;
     if(play1->size()<=0)              {error(NN_INTEGR_ERR,"Cannot connect, source layer has not been setup"); return false;}
     if(pconx->has_source_layer())     {error(NN_INTEGR_ERR,"Cannot setup connection set that is already attached to source layer"); return false;}
     if(pconx->has_destin_layer())     {error(NN_INTEGR_ERR,"Cannot setup connection set that is already attached to destination layer"); return false;}
     if(play2->size()<=0)              {error(NN_INTEGR_ERR,"Cannot connect, destination layer has not been setup"); return false;}

     // layers exist and are setup, so now setup internal connection_set connecting them.
     pconx->setup("Connections",play1,play2,my_error_flag(),fully_connect);
     seq_connected = true;
     }
 p_c1 = p_c3;
 p_c2 = NULL;
 p_c3 = NULL;
 if(topology.goto_next())    p_c2 = topology.current();
 if(topology.goto_next())    p_c3 = topology.current();
 }
return (seq_found AND seq_connected AND no_error() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}   // end of namespace nnlib2
