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
// directly add (arithmetically) the value to current pe input (summation function)

DATA pe::add_to_input(DATA value)
 {
 input = input + value;
 return input;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// sometimes useful, also resets input to 0 (as it has now been processed).

void pe::move_input_to_output()
 {
 input_function();
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
// Important:
//
// As nnlib2 is now used under a fully interactive environment (via R) and due to the need
// for a fully working "get_input_at" method (in NN module) for layers, the ways PEs receive
// input must change. Currently there are three ways to send input to a PE (class pe):
// 	(a) its receive_input_value() function (which connections usually use to send a single
//         value that will collected with other similar ones and later processed by pe's
//         input_function()) to set the pe's internal "input" variable,
//     (b) direct access to pe's "input" variable" (sometimes used by layer code) and
// 	(c) use of add_to_input() which is similar to (b).
// Having all three methods available may be somewhat confusing to the user of the nnlib2 C++
// library, but these were maintained for versatility, backwards-compatibility and possibly
// performance (as (b) and (c) are slightly faster than (a)).
// However, in order for interactive NN methods to work (especially get_input_at), pe class
// (in nnlib2 C++ library) will have to change as follows:
// 	(1) all externally incoming input data to a pe should come ONLY via the pe's
// receive_input_value() while "input" and "add_to_input" options should be removed or change
// to protected (for use by pe's only').
// 	(2) The pe should have a method that calculates and provides the current "final" input
// value (by calling its input_function()) even if it is not encoding or recalling; doing so
// should not reset inputs (as is done when encoding or recalling). These changes were
// investigated and some preparation was done for them in this version. As fully implementing
// this recommendation appears relatively simple, it will probably be done in a next version.
//
// The pe's preview_current_input() method is a PATCH to estimate current input value without
// changing the pe's current state.
//
// Note. Below, when storing current state there may be an issue (at least the way it is currently
// quickly implemented) as it assumes PES that are pretty close to generic ones. If the
// PEs are heavily modified, this may not work.
// Another side-effect is that it changes "current" item in the list of received_values.
// Therefore it should be improved in future version.

DATA pe::preview_current_input()
{
	if(received_values.size()<=0) return input;

	// we have received_values, call input_function() to process them:

	// store current state. See notes above.

	dllist<DATA> st_received_values(received_values);			// list (queue) of input values, be processed by input_function

	DATA st_input	= input;							    	// final input to this pe, may be accessed directly or result from input_function.
	DATA st_bias	= bias;					        			// bias
	DATA st_output	= output;					        		// output of this pe.
	DATA st_misc	= misc;										// helper register for misc use.

	// call input_function to gather and process received_values...

	DATA v = 0;

	// as there may have been direct manipulation of input variable OR
	// data incoming via received_values but USUALLY not both, add the two:

	if(received_values.size()>0) v = input_function();

	// restore previous PE state (that may have changed when input_function was called)
	// whatever that state was, try to recover it...

	received_values.reset();
	received_values.append_from(st_received_values);
	input  = st_input;
	bias   = st_bias;
	output = st_output;
	misc   = st_misc;

	return v;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // end of namespace nnlib2

