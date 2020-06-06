//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		pe.h		 				Version 0.1
//		-----------------------------------------------------------
//		Definition - implementation of general processing element
//		(pe) node functionality.
//		-----------------------------------------------------------


#ifndef NN_PE_H
#define NN_PE_H

#include "nnlib2.h"
#include "component.h"
#include "connection.h"
#include "nnlib2_vector.h"
#include "nnlib2_dllist.h"

/*-----------------------------------------------------------------------*/
/* Processing Element (pe)			                         */
/*-----------------------------------------------------------------------*/
// Note: this handles the PE data,
// processing is done in related ANS component object layer

namespace nnlib2 {

class pe
 {
 protected:

 dllist<DATA> received_values;                          // list (queue) of input values, be processed by input_function

 public:

 DATA input;					        // final input to this pe, may be accessed directly or result from input_function.
 DATA bias;					        // bias
 DATA output;					        // output of this pe.
 DATA misc;						// helper register for misc use.

 pe();
 ~pe();

 void reset();						// partial reset of this pe.
 string description();                                  // text containing status and information of PE

 virtual DATA input_function();                         // optional: overload with input function, default: sums received_values and puts result to input variable
 virtual DATA activation_function(DATA value);          // optional: overload with activation function, otherwise returns value
 virtual DATA threshold_function(DATA value);           // optional: overload with threshold function, otherwise returns value

 DATA add_to_input (DATA value);        	        // directly add (arithmeticaly) the value to input (summation function)
 bool receive_input_value (DATA value);                 // append to list of input values (received_values), later to be processed by input_function for producing final input value.
 int  reset_received_values();                          // empties list of received input values (received_values).
 void move_input_to_output();                           // sometimes useful, also sets input to 0 and resets received values.

 virtual void encode();                                 // should be overiden by derived classes. Default employs input->activation->threshold function sequence and places results to output
 virtual void recall();                                 // should be overiden by derived classes. Default employs input->activation->threshold function sequence and places results to output

 friend std::istream& operator>> ( std::istream REF s, pe REF it );
 friend std::ostream& operator<< ( std::ostream REF s, pe REF it );
 };

} // end of namespace nnlib2

#endif // NN_PE_H
