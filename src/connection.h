//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		connection.h	 							Version 0.1
//		-----------------------------------------------------------
//		Definition - implementation of general connection
//		functionality.
//		-----------------------------------------------------------


#ifndef NN_CONNECTION_H
#define NN_CONNECTION_H

#include "nnlib2.h"
#include <iostream>
#include <string>

namespace nnlib2 {
using std::string;

class pe;
class connection_set;

//*-----------------------------------------------------------------------*/
// Neural link (connection)
/*-----------------------------------------------------------------------*/
// if processing is done in related NN component object connection_set
// then this only handles the link (synapsis) data (usually weight)
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class connection
 {
 protected:

 connection_set PTR mp_connection_set;                                                  // connection_set to which this connection belongs, or NULL if none.

 int	m_source_pe;                                                                    // a pe identifier (usually position in a layer)
 int	m_destin_pe;                                                                    // a pe identifier (usually position in a layer)
 DATA	m_weight;									// connection weight.

 public:

 DATA	misc;										// a public local buffer.

 connection();
 ~connection();
 bool operator == (const connection REF i);                                             // IMPORTANT: this only checks if connected PEs are the same, an invalid check if multiple connections connect the two PEs!

 void setup (connection_set PTR p_connection_set,                                       // connection_set to which this connection belongs
             const int source_pe,                                                       // source_pe is a pe identifier (usually position in a layer)
             const int destin_pe,                                                       // destin_pe is a pe identifier (usually position in a layer)
             const DATA initial_weight);

 string description ();

 DATA REF weight ();

 int source_pe_id();                                                                    // returns m_source_pe (a pe identifier (usually position in a layer))
 int destin_pe_id();                                                                    // returns m_destin_pe (a pe identifier (usually position in a layer))

 pe REF source_pe();                                                                    // returns pe (regardless PE_TYPE) IF mp_connection set is not NULL;
 pe REF destin_pe();                                                                    // returns pe (regardless PE_TYPE) IF mp_connection set is not NULL;

 virtual void encode();                                                                 // should be overiden by derived classes, only produces runtime error message
 virtual void recall();                                                                 // should be overiden by derived classes, only produces runtime error message

 friend std::istream& operator>> ( std::istream REF s, connection REF it );
 friend std::ostream& operator<< ( std::ostream REF s, connection REF it );
 };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}   // end of namespace nnlib2

#endif // NN_CONNECTION_H
