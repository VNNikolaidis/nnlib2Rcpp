//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		pe.h		 						                          	Version 0.1
//		-----------------------------------------------------------
//		Definition - implementation of a processing element (PE)
//		Note: this handles the PE data,
//    processing is done in related NN component object layer
//		-----------------------------------------------------------

#include <sstream>
#include <string>

#include "pe.h"

/*-----------------------------------------------------------------------*/
/* Processing Element (pe)											                      	 */
/*-----------------------------------------------------------------------*/

namespace nnlib2 {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

pe::pe()
 {
 input = 0;
 bias = 0;
 output = 0;
 misc = 0;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

pe::~pe() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void pe::reset()
 {
 input = 0;
 output = 0;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// directly add (arithmeticaly) the value to current pe input (summation function)

DATA pe::add_to_input(DATA value)
 {
 input = input + value;
 return input;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// sometimes usefull, also sets input to 0.

void pe::move_input_to_output()
 {
 output = input;
 input = 0;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// append to list of inputs, later to be processed by input_function to produce final input.

bool pe::receive_input_value(DATA value)
  {
  return(received_values.append(value));
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// if PE input function is not overloaded, it just sums received_values to input

DATA pe::input_function()
  {
  input=0;

  if(received_values.goto_first())
  do input = input + received_values.current();
  while(received_values.goto_next());

  received_values.reset();
  return input;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// empties list of received input values (received_values)

int pe::reset_received_values()
 {
 int n=received_values.size();
 received_values.reset();
 return n;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// if PE activation function is not overloaded, it just returns value

DATA pe::activation_function(DATA value)
 {
 return value;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// if PE threshold function is not overloaded, it just returns value

DATA pe::threshold_function(DATA value)
 {
 return value;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// should be overriden. By default, employs input->activation->threshold
// function sequence and places results to output

void pe::encode()
{
DATA v;
v = input_function();
v = activation_function(v);
v = threshold_function(v);
output = v;
}

void pe::recall()
{
  DATA v;
  v = input_function();
  v = activation_function(v);
  v = threshold_function(v);
  output = v;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

string pe::description ()
 {
 std::stringstream s;
 s << "PE (node) \ninput = "<< (float) input <<"\nbias = " << (float) bias << "\noutput = " << (float) output;
 return s.str();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// input it :

 std::istream& operator>> ( std::istream REF s, pe REF it )
  {
  string comment;
  if(NOT(s.rdstate()))
   s >> comment >> comment >> it.bias >> comment >> it.misc ;
  return s;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// output it :

 std::ostream& operator<< ( std::ostream REF s, pe REF it )
  {
  if(NOT(s.rdstate()))
   s << "PE" << " B: " << it.bias << " M: " << it.misc << "\n";
  return s;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // end of namespace nnlib2

