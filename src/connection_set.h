//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		connection_set.h								Version 0.1
//		-----------------------------------------------------------
//		Definition - implementation of group of connections to be
//		processed concurrently.
//		-----------------------------------------------------------


#ifndef NN_CONNECTION_SET_H
#define NN_CONNECTION_SET_H

#include "layer.h"
#include "connection.h"
#include "nnlib2_dllist.h"
#include "nnlib2_misc.h"

#include <sstream>

namespace nnlib2 {

/*-----------------------------------------------------------------------*/
/* Neural connection_set virtual (methods regardless CONNECTION_TYPE )   */
/*-----------------------------------------------------------------------*/

class connection_set : public component, public error_flag_client
{
public:
	virtual bool connection_properties( int connection,int REF source_component_id,int REF source_item,int REF destin_component_id,int REF destin_item, DATA REF weight) = 0;
	virtual layer REF source_layer() = 0;
	virtual layer REF destin_layer() = 0;
	virtual bool has_source_layer()  = 0;
	virtual bool has_destin_layer()  = 0;
	virtual pe REF source_pe(int c)  = 0;
	virtual pe REF destin_pe(int c)  = 0;
	virtual	bool setup (layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers = false) = 0;
	virtual	bool setup (string name, layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers = false) = 0;
    virtual	bool setup (string name, layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers, DATA min_random_weight, DATA max_random_weight) = 0;
	virtual bool add_connection(const int source_pe, const int destin_pe, const DATA initial_weight) = 0;
 };

/*-----------------------------------------------------------------------*/
/* Neural connection set template for any CONNECTION_TYPE       		 */
/*-----------------------------------------------------------------------*/

template <class CONNECTION_TYPE>
class Connection_Set : public connection_set
 {
 private:

 layer PTR mp_source_layer;                                     // note: this is not PE_TYPE specific (layer)
 layer PTR mp_destin_layer;                                     // note: this is not PE_TYPE specific (layer)

 protected:

 dllist <CONNECTION_TYPE> connections;                          // connections in connection_set.

 public:

 Connection_Set();
 Connection_Set(string name);
 Connection_Set(string name, bool PTR error_flag_to_use);
 Connection_Set(string name, layer PTR source_layer, layer PTR destin_layer);
 Connection_Set(string name, layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use,bool fully_connect_layers = false);
 ~Connection_Set();

 bool setup (layer PTR source_layer, layer PTR destin_layer);
 bool setup (layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers = false);
 bool setup (string name, layer PTR source_layer, layer PTR destin_layer);
 bool setup (string name, layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers = false);
 bool setup (string name, layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers, DATA min_random_weight, DATA max_random_weight);

 bool operator == (const Connection_Set REF i);                 // only checks if the two connect the same layers...

 bool connect (const int source_pe, const int destin_pe, const DATA initial_weight);
 bool fully_connect (bool group_by_source = false);

 // Note: the set_connection_weight functions are added for initializing weights. Using in processing is not recommented.
 void set_connection_weights (DATA value);
 void set_connection_weights_random(DATA min_random_value, DATA max_random_value);
 bool set_connection_weight (const int source_pe, const int destin_pe, const DATA new_weight);

 string description ();
 void draw ();
 int  size();						                                      // number of connections in set
 string item_description (int item);
 void from_stream (std::istream REF s);
 void to_stream (std::ostream REF s);

 pe REF source_pe(connection REF c);                            // returns pe (regardless PE_TYPE) for given connection
 pe REF source_pe(int c);                                       // returns pe (regardless PE_TYPE) for given connection number
 pe REF destin_pe(connection REF c);                            // returns pe (regardless PE_TYPE) for given connection
 pe REF destin_pe(int c);                                       // returns pe (regardless PE_TYPE) for given connection number
 layer REF source_layer();                                      // note: this is not PE_TYPE specific (layer), returns mp_source_layer as a reference to layer (or to a dummy_layer if error)
 layer REF destin_layer();                                      // note: this is not PE_TYPE specific (layer), returns mp_destin_layer as a reference to layer (or to a dummy_layer if error)

 bool connection_properties( int connection,int REF source_component_id,int REF source_item,int REF destin_component_id,int REF destin_item, DATA REF weight);
 bool has_source_layer();
 bool has_destin_layer();

 void encode();													// (virtual in component) may be overiden by derived classes with specific layer functiobality.
 void recall();													// (virtual in component) may be overiden by derived classes with specific layer functiobality.

 bool add_connection(const int source_pe, const int destin_pe, const DATA initial_weight);
 };


//-------------------------------------------------------------------------
// For explitit instantiation of connection_set template per connection type (not needed
// as implementation is in header below) use code similar to:
// template class connection_set<connection>;

//-------------------------------------------------------------------------
// instantiate a set of generic connections
// define set of generic "dumb" connections where most processing will be done in connection_set code.

typedef Connection_Set<connection> generic_connection_set;

//-------------------------------------------------------------------------
// Neural connection_set implementation follows:

template <class CONNECTION_TYPE>
Connection_Set<CONNECTION_TYPE>::Connection_Set()
 {
 mp_source_layer = NULL;
 mp_destin_layer = NULL;
 if(no_error())
  {
  m_type = cmpnt_connection_set;
  m_name = "Connection set";
  }
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
Connection_Set<CONNECTION_TYPE>::Connection_Set(string name)
 :Connection_Set()
 {
 if(no_error()) m_name = name;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
Connection_Set<CONNECTION_TYPE>::Connection_Set(string name, bool PTR error_flag_to_use)
 :Connection_Set<CONNECTION_TYPE>(name)
 {
 if(no_error())
  if(error_flag_to_use!=NULL)
  	set_error_flag(error_flag_to_use);
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
Connection_Set<CONNECTION_TYPE>::Connection_Set(string name, layer PTR source_layer, layer PTR destin_layer)
 :Connection_Set<CONNECTION_TYPE>()
 {
		if((source_layer==NULL)OR(destin_layer==NULL))
		{
		error(NN_INTEGR_ERR,"Cannot connect non-existant layers");
		}

        if(no_error())
        {
        setup(name,source_layer,destin_layer);
        }
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
Connection_Set<CONNECTION_TYPE>::Connection_Set(string name, layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers)
	{
        set_error_flag(error_flag_to_use);

		if((source_layer==NULL)OR(destin_layer==NULL))
		{
		error(NN_INTEGR_ERR,"Cannot connect non-existant layers");
		}

        if(no_error())
        {
        m_type = cmpnt_connection_set;
        setup(name,source_layer,destin_layer);
        if(fully_connect_layers) fully_connect();
        }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
Connection_Set<CONNECTION_TYPE>::~Connection_Set() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
bool Connection_Set<CONNECTION_TYPE>::setup (layer PTR source_layer, layer PTR destin_layer)
 {
 mp_source_layer = source_layer;
 mp_destin_layer = destin_layer;
 connections.set_error_flag(my_error_flag());
 return no_error();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
bool Connection_Set<CONNECTION_TYPE>::setup (layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers)
{
	set_error_flag(error_flag_to_use);
	setup(source_layer,destin_layer);
	if(fully_connect_layers) fully_connect();
	return(no_error());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
bool Connection_Set<CONNECTION_TYPE>::setup (string name, layer PTR source_layer, layer PTR destin_layer)
 {
 m_name = name;
 return(setup(source_layer,destin_layer));
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
bool Connection_Set<CONNECTION_TYPE>::setup (string name, layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers)
 {
 m_name = name;
 return(setup(source_layer, destin_layer, error_flag_to_use, fully_connect_layers));
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


template <class CONNECTION_TYPE>
bool Connection_Set<CONNECTION_TYPE>::setup(string name, layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers, DATA min_random_weight, DATA max_random_weight)
 {
 if (setup(name, source_layer, destin_layer, error_flag_to_use, fully_connect_layers))
    {
    set_connection_weights_random(min_random_weight, max_random_weight);
    return true;
    }
 return false;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// only checks if the two connect the same layers...

template <class CONNECTION_TYPE>
bool Connection_Set<CONNECTION_TYPE>:: operator == (const Connection_Set REF i)
{
return ((mp_source_layer == i.mp_source_layer) AND (mp_destin_layer == i.mp_destin_layer));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
void Connection_Set<CONNECTION_TYPE>::set_connection_weights (DATA value)
{
if(no_error())
 if(connections.goto_first())
  do
   connections.current().weight() = value;
  while(connections.goto_next());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
void Connection_Set<CONNECTION_TYPE>::set_connection_weights_random(DATA min_random_value, DATA max_random_value)
{
    DATA rmin = min_random_value;
    DATA rmax = max_random_value;
    if (rmin > rmax) { warning("Invalid weight initialization"); rmin = rmax; }
    if (rmin == rmax) { set_connection_weights(rmax); return; }
    if (no_error())
    if (connections.goto_first())
     do
      connections.current().weight() = random(rmin, rmax);
     while (connections.goto_next());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Not for general use. Added for initializing weights to particular values etc.

template <class CONNECTION_TYPE>
bool Connection_Set<CONNECTION_TYPE>::set_connection_weight (const int source_pe, const int destin_pe, const DATA new_weight)
{
if(NOT no_error()) return false;
connection dummy;
dummy.setup(this,source_pe,destin_pe,0.0);
if(connections.find(dummy))
        {
        connections.current().weight()=new_weight;
        return true;
        }
error(NN_INTEGR_ERR,"PEs not connected, no weight to adjust");
return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
string Connection_Set<CONNECTION_TYPE>::description ()
{
std::stringstream s;
s << component::description();
if((mp_source_layer==NULL)OR(mp_destin_layer==NULL))
 {
 s << " (Not Connected)";
 return s.str();
 }
s << " " << mp_source_layer->id() <<  "-->"  << mp_destin_layer->id();
return s.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
void Connection_Set<CONNECTION_TYPE>::draw()
{}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
int Connection_Set<CONNECTION_TYPE>::size()
{
return connections.number_of_items();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
string Connection_Set<CONNECTION_TYPE>::item_description (int item)
{
return connections[item].description();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
bool Connection_Set<CONNECTION_TYPE>::connect (const int source_pe, const int destin_pe, const DATA initial_weight)
{
	if(no_error())
	{
		connections.append();
		connections.current().setup(reinterpret_cast<connection_set *>(this),source_pe,destin_pe,initial_weight);
	}
	return no_error();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
bool Connection_Set<CONNECTION_TYPE>::fully_connect(bool group_by_source)
{
if(no_error())
 {
 if( (mp_source_layer NEQL NULL) AND (mp_destin_layer NEQL NULL)  )
  {
  if(group_by_source)
   {
   for(int s=0;s<mp_source_layer->size();s++)
    for(int d=0;d<mp_destin_layer->size();d++)
     connect(s,d,0.0);
   }
  else
   {
   for(int d=0;d<mp_destin_layer->size();d++)
    for(int s=0;s<mp_source_layer->size();s++)
     connect(s,d,0.0);
   }
  m_name.append(" (Fully Connected)");
  return no_error();
  }
 error(NN_INTEGR_ERR,"Cannot fully connect layers");
 }
return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
bool Connection_Set<CONNECTION_TYPE>::connection_properties (
                                           int connection,
                                           int REF source_component_id,
                                           int REF source_item,
                                           int REF destin_component_id,
                                           int REF destin_item,
										   DATA REF weight)
{
		if ((connection<0) OR (connection>=connections.size())) return false;
        source_component_id=mp_source_layer->id();   	 	  // this is component (layer) id
        source_item=connections[connection].source_pe_id();	  // this is pe's index in layer
        destin_component_id=mp_destin_layer->id();	    	  // this is component (layer) id
        destin_item=connections[connection].destin_pe_id();	  // this is pe's index in layer
        weight=connections[connection].weight();
        return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


template <class CONNECTION_TYPE>
bool Connection_Set<CONNECTION_TYPE>::add_connection(const int source_pe, const int destin_pe, const DATA initial_weight)
{
if((mp_source_layer==NULL) OR (mp_destin_layer==NULL)) return false;
if((source_pe<0) OR (source_pe)>=mp_source_layer->size()) return false;
if((destin_pe<0) OR (destin_pe)>=mp_destin_layer->size()) return false;
if(NOT connections.append()) return false;
CONNECTION_TYPE REF c = connections.last();
c.setup(this,source_pe,destin_pe,initial_weight);
return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// input it :

template <class CONNECTION_TYPE>
void Connection_Set<CONNECTION_TYPE>::from_stream (std::istream REF s)
{
        string comment;

        if(no_error())
        {
                component::from_stream(s);
                s >> comment >> comment;
                s >> comment >> comment;
                connections.from_stream(s);								// changed for VC7 port,was	s >> connections;
        }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// output it :

template <class CONNECTION_TYPE>
void Connection_Set<CONNECTION_TYPE>::to_stream (std::ostream REF s)
{
        if(no_error())
        {
                component::to_stream(s);
        		if((mp_source_layer==NULL)OR(mp_destin_layer==NULL)) return;
                s << "SourceCom: " << mp_source_layer->id() << "\n";		// this is the id, not the original pointer.
        		s << "DestinCom: " << mp_destin_layer->id() << "\n";		// this is the id, not the original pointer.
                connections.to_stream(s);					                // changed for VC7 port,was	s << connections;
        }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// may be overiden by derived classes.

template <class CONNECTION_TYPE>
void Connection_Set<CONNECTION_TYPE>::encode()
{
if(connections.goto_first())
do connections.current().encode();
while(connections.goto_next());
}

template <class CONNECTION_TYPE>
void Connection_Set<CONNECTION_TYPE>::recall()
{
if(connections.goto_first())
do connections.current().recall();
while(connections.goto_next());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// a dummy layer, useless return value if actual are invalid
static pe_layer dummy_layer;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// returns mp_destin_layer as a reference to layer (or to dummy_layer if error)

template <class CONNECTION_TYPE>
layer REF Connection_Set<CONNECTION_TYPE>::source_layer()
{
if(mp_source_layer!=NULL)
 if(mp_source_layer->type() EQL cmpnt_layer)
   return (ATPTR mp_source_layer);
error(NN_INTEGR_ERR,"Source component is not a layer");
return dummy_layer;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// returns mp_destin_layer as a reference to layer (or to dummy_layer if error)

template <class CONNECTION_TYPE>
layer REF Connection_Set<CONNECTION_TYPE>::destin_layer()
{
if(mp_destin_layer!=NULL)
 if(mp_destin_layer->type() EQL cmpnt_layer)
   return (ATPTR mp_destin_layer);
error(NN_INTEGR_ERR,"Destination component is not a layer");
return dummy_layer;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// these return pe regardless PE_TYPE

template <class CONNECTION_TYPE>
pe REF Connection_Set<CONNECTION_TYPE>::source_pe(connection REF c)
{
return source_layer().PE(c.source_pe_id());
}

template <class CONNECTION_TYPE>
pe REF Connection_Set<CONNECTION_TYPE>::source_pe(int c)
{
return source_layer().PE(connections[c].source_pe_id());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// these return pe regardless PE_TYPE

template <class CONNECTION_TYPE>
pe REF Connection_Set<CONNECTION_TYPE>::destin_pe(connection REF c)
{
return destin_layer().PE(c.destin_pe_id());
}

template <class CONNECTION_TYPE>
pe REF Connection_Set<CONNECTION_TYPE>::destin_pe(int c)
{
return destin_layer().PE(connections[c].destin_pe_id());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class CONNECTION_TYPE>
bool Connection_Set<CONNECTION_TYPE>::has_source_layer()
{
return (mp_source_layer != NULL);
}

template <class CONNECTION_TYPE>
bool Connection_Set<CONNECTION_TYPE>::has_destin_layer()
{
return (mp_destin_layer != NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // end of namespace nnlib2

#endif // NN_CONNECTION_SET_H
