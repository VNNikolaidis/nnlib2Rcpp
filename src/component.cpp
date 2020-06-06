//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis     		All rights reserved.
//		-----------------------------------------------------------
//		component.h										Version 0.1
//		-----------------------------------------------------------
//		base class for ANS components (parts of topology)
//		-----------------------------------------------------------


#include "component.h"

// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace nnlib2 {

char component_nn_type_name [][100] = {
									  "Unknown",
									  "Artificial Neural System",
									  "Layer",
									  "Connection Set",
									  "control component"
								      };

unsigned int component::current_id = 0;
unsigned int component::counter = 0;

component::component()
 {
 counter ++;
 m_id = current_id ++;
 m_name = "Unnamed Component";
 m_type = cmpnt_unknown;
 m_auxiliary_parameter = 0;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

component::component(const std::string name, const component_type type)
 {
 counter ++;
 m_id = current_id ++;
 m_name = name;
 m_type = type;
 m_auxiliary_parameter = 0;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

component::~component()
 {
 counter --;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

string component::description()
 {
 string s;
 s = component_nn_type_name[m_type];
 s = s + " : " + m_name;
 return s;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void component::set_auxiliary_parameter(DATA d)
 {m_auxiliary_parameter=d;}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// input it :

void component::from_stream (std::istream REF s)
 {
 int id;
 string comment;

 if(s.rdstate()) {error(NN_IOFILE_ERR,"Error reading stream (component)");return;}

 s >> comment >> m_name;
 s >> comment >> id ;
 s >> comment >> comment ;
 s >> comment >> m_auxiliary_parameter;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// output it :

void component::to_stream (std::ostream REF s)
 {
 streamable_string type("");

  if(s.rdstate()) {error(NN_IOFILE_ERR,"Error writing stream (component)");return;}

  type = component_nn_type_name [m_type];

  s << "Component: " 	<< m_name << "\n";
  s << "ID: " 			<< m_id   << "\n";
  s << "Type: " 		<< type   << "\n";
  s << "Aux.Param: " 	<< m_auxiliary_parameter  << "\n";
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}   // end of namespace nnlib2

