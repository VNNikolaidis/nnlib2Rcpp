//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		component.h					Version 0.1
//		-----------------------------------------------------------
//		base class for ANS components (parts of topology)
//              Νοte: ANS components is where the actual processing is done
//		-----------------------------------------------------------


#ifndef NN_COMPONENT_H
#define NN_COMPONENT_H

#include <iostream>

#include "nnlib2_string.h"
#include "nnlib2_error.h"

namespace nnlib2 {

//-----------------------------------------------------------------------

enum component_type    {cmpnt_unknown,                   // no type.
                        cmpnt_nn,                        // a neural network.
                        cmpnt_layer,                     // a layer of one or more processing elements (PEs) than can be processed in parallel.
                        cmpnt_connection_set,            // a set of one or more connections between PEs than can be processed in parallel.
                        cmpnt_aux_control};              // a component for special functionality and control commands.

extern char component_type_name [][100];                 // NOTE: should corresponding names for component_type

//-----------------------------------------------------------------------

class component
 {
 private:

 DATA m_auxiliary_parameter;    // can be used for various purposes

 protected:

 static unsigned int current_id;
 static unsigned int counter;

 unsigned int		m_id;
 component_type		m_type;
 streamable_string	m_name;

 void set_auxiliary_parameter(DATA d);

 public:

 component();
 component(const std::string name, const component_type type);
 virtual ~component();

 virtual void reset ()		                { error(NN_SYSTEM_ERR,"reset() not implemented"); }
 string REF name ()                             { return m_name; }
 virtual string description ();
 virtual string item_description(int item)      { error(NN_SYSTEM_ERR,"item_description() not implemented"); return("No item description"); }

 virtual void encode () = 0;
 virtual void recall ()	= 0;

 virtual void from_stream ( std::istream REF s );
 virtual void to_stream   ( std::ostream REF s );

 int id()                       { return m_id; }
 component_type type()          { return m_type; }
 DATA auxiliary_parameter()     { return m_auxiliary_parameter; }

 virtual int size () {return 0;};
 };

//-----------------------------------------------------------------------
// can communicate with its environment & accept input (from DATA vector)

class data_receiver
{
public:
virtual bool input_data_from_vector(DATA * data, int dimension) = 0;
virtual bool send_input_to(int index, DATA d) = 0;
};

//-----------------------------------------------------------------------
// can communicate with its environment & provide output (to DATA vector)

class data_provider
{
public:
virtual bool output_data_to_vector(DATA * buffer, int dimension) = 0;
virtual DATA get_output_from (int index) = 0;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class data_io_component : public component, public data_provider, public data_receiver
{};

}   // end of namespace nnlib2

#endif // NN_COMPONENT_H
