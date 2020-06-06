//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		connection.h	 							Version 0.1
//		-----------------------------------------------------------
//		Definition - implementation of connection (link)
// 		Note: this handles the connection data,
// 		processing is done in related ANS component object connection_set
//		-----------------------------------------------------------

#include <iostream>
#include <sstream>
#include <string>

#include "connection.h"
#include "connection_set.h"

/*-----------------------------------------------------------------------*/
// Neural link (connection)
/*-----------------------------------------------------------------------*/

namespace nnlib2 {

// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

connection::connection()
 {
 mp_connection_set = NULL;
 m_source_pe = -1;
 m_destin_pe = -1;
 m_weight = (DATA)0;
 misc = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

connection::~connection() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool connection:: operator == (const connection REF i)
  {
  return ((m_source_pe == i.m_source_pe) AND
		  (m_destin_pe == i.m_destin_pe));
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void connection::setup (connection_set PTR p_connection_set,
                        const int source_pe,
                        const int destin_pe,
                        const DATA initial_weight)
  {
  mp_connection_set = p_connection_set;
  m_source_pe = source_pe;
  m_destin_pe = destin_pe;
  m_weight = initial_weight;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

string connection::description ()
 {
 std::stringstream s;
 s << "Connection\nfrom source layer PE ";
 s << m_source_pe;
 s << "\nto destination layer PE ";
 s << m_destin_pe;
 s << "\nweight ";
 s << (float)m_weight;
 return s.str();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

DATA REF connection::weight ()
 {
 return m_weight;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int connection::source_pe_id()
 {
 return m_source_pe;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int connection::destin_pe_id()
 {
 return m_destin_pe;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// return pe (regardless PE_TYPE)

static pe dummy_pe;						// something to return when everything else fails

pe REF connection::source_pe()
{
if(mp_connection_set==NULL)
 {
 error(NN_INTEGR_ERR,"Connection is unbound to any connection_set. Cannot find source PE");
 return dummy_pe;
 }
return (mp_connection_set->source_layer().PE(m_source_pe));
}

pe REF connection::destin_pe()
{
if(mp_connection_set==NULL)
 {
 error(NN_INTEGR_ERR,"Connection is unbound to any connection_set. Cannot find source PE");
 return dummy_pe;
 }
return (mp_connection_set->destin_layer().PE(m_destin_pe));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void connection::encode()
 {error(NN_SYSTEM_ERR,"Default connection encode function called, should be overiden!");}

void connection::recall()
{error(NN_SYSTEM_ERR,"Default connection recall function called, should be overiden!");}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// input it :

 std::istream& operator>> ( std::istream REF s, connection REF it )
  {
  int s_id, d_id;
  DATA w;

  std::string comment;
  if(NOT(s.rdstate()))
   {
   s >> comment
	 >> comment >> s_id
	 >> comment >> d_id
	 >> comment >> w;
	it.setup(it.mp_connection_set,s_id,d_id,w);
   }
  return s;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// output it :

 std::ostream& operator<< ( std::ostream REF s, connection REF it )
  {
  if(NOT(s.rdstate()))
   s << "CON"
	 << " FR: "  << it.source_pe_id()
	 << " TO: "  << it.destin_pe_id()
	 << " WGT: " << it.weight() << '\n';
  return s;
  }


//*-----------------------------------------------------------------------*/
// a generic connection that passes source output to destination input
// multiplied by m_weight (which is 1 and is NOT adjusted during encode)
// (note: m_weight may be adjusted externaly)
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// passes source output to destination input (via its receive_input_value())

void weighted_pass_through_connection::encode()
	{
	destin_pe().receive_input_value ( m_weight * source_pe().output );
	}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// passes source output to destination input (via its receive_input_value())

void pass_through_connection::recall()
	{
	destin_pe().receive_input_value ( source_pe().output );
	}

//*-----------------------------------------------------------------------*/
// a generic connection that passes source output to destination input
// (note: ignores m_weight)
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// passes source output to destination input (via its receive_input_value())

void pass_through_connection::encode()
	{
	destin_pe().receive_input_value ( source_pe().output );
	}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// passes source output to destination input (via its receive_input_value())

void weighted_pass_through_connection::recall()
{
	destin_pe().receive_input_value ( m_weight * source_pe().output );
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}   // end of namespace nnlib2


