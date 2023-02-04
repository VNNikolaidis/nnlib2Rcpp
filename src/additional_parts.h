#ifndef NNLIB2_ADDITIONAL_PARTS_H
#define NNLIB2_ADDITIONAL_PARTS_H

#include "nn.h"
namespace nnlib2 {

//--------------------------------------------------------------------------------------------
// example for manual.pdf

class MEX_connection: public connection
{
public:

	// model-specific behavior during mapping stage:

	void recall()
	{
		destin_pe().receive_input_value(pow( source_pe().output - weight() , 2) );
	}

	// model-specific behavior during training stage:
	// in this example, only the current  connection weight (i.e. weight())
	// and incoming value from the source node (i.e. source_pe().output) are
	// used in a series of calculations that eventually change the
	// connection weight (see last line).

	void encode()
	{
		double x  = source_pe().output - weight();
		double s  = .3;
		double m  = weight();
		double pi = 2*acos(0.0);
		double f  = 1/(s*sqrt(2*pi)) * exp(-0.5*pow((x-m)/s,2));
		double d = (f * x) / 2;
		weight() = weight() + d;
	}

};

typedef Connection_Set < MEX_connection > MEX_connection_set;

//--------------------------------------------------------------------------------------------
// example for manual.pdf

class MEX_pe : public pe
 {
 public:

 void recall()
 	{
 	pe::recall();
 	output = sqrt(output);
 	}
 };

typedef Layer < MEX_pe > MEX_layer;

//--------------------------------------------------------------------------------------------
// example (for RBloggers blog post): Perceptron components

// Perceptron nodes:
class perceptron_pe : public pe
{
public:
        DATA threshold_function(DATA value)
         {
         if(value>0) return 1;
         return 0;
         }
};

// Percepton layer:
typedef Layer < perceptron_pe > perceptron_layer;

// Perceptron connections:
class perceptron_connection: public connection
{
public:

	 // multiply incoming (from source node) value by weight and send it to destination node.
 void recall()
  	{
  	destin_pe().receive_input_value( weight() * source_pe().output );
  	}

  // for simplicity, learning rate is fixed to 0.3 and input contains desired output:
 void encode()
	{
	weight() = weight() + 0.3 * (destin_pe().input - destin_pe().output) * source_pe().output;
	}
};

// Perceptron group of connections
typedef Connection_Set< perceptron_connection > perceptron_connection_set;

//--------------------------------------------------------------------------------------------
// example: a (useless) pe that just adds 10 to the sum of its inputs when recalling data

class JustAdd10_pe : public pe
{
public:
void recall() {	pe::recall(); output = output + 10; }
};

typedef Layer < JustAdd10_pe > JustAdd10_layer;

//--------------------------------------------------------------------------------------------
// minimal examples of pe and layer definitions:
//--------------------------------------------------------------------------------------------

class example_pe : public pe {};

typedef Layer<example_pe> example_pe_layer_1;

class example_pe_layer_2: public Layer<example_pe>
 {
 public:
 example_pe_layer_2(string name, int size) : Layer(name,size) {};
 };

//--------------------------------------------------------------------------------------------
// minimal examples of connection and connection_set definitions:
//--------------------------------------------------------------------------------------------

class example_connection: public connection {};

typedef Connection_Set<example_connection> example_connection_set_1;

class example_connection_set_2: public Connection_Set<example_connection>
 {
 public:
 example_connection_set_2(string name) : Connection_Set(name) {};
 };

//------------------------------------------------------------------------------
// register new layer classes here:
// given the name, it should return a pointer to a newly created layer
//------------------------------------------------------------------------------

layer PTR generate_custom_layer(string name, int size, DATA optional_parameter=DATA_MIN)
{
	if(name == "JustAdd10")         return new JustAdd10_layer (name,size);
    if(name == "perceptron")        return new perceptron_layer(name,size);
    if(name == "MEX")				return new MEX_layer(name, size);

	if(name == "example_layer_0")   return new Layer<example_pe> (name, size);
	if(name == "example_layer_1")   return new example_pe_layer_1(name, size);
	if(name == "example_layer_2")   return new example_pe_layer_2(name, size);

return NULL;
}

//------------------------------------------------------------------------------
// register new connection_set classes here:
// given the name, it should return a pointer to a newly created connection_set
//------------------------------------------------------------------------------

connection_set PTR generate_custom_connection_set(string name, DATA optional_parameter=DATA_MIN)
{
	if(name == "perceptron")                return new perceptron_connection_set(name);
	if(name == "MEX")						return new MEX_connection_set(name);

	if(name == "example_connection_set_0")  return new Connection_Set<example_connection>(name);
	if(name == "example_connection_set_1")  return new example_connection_set_1(name);
	if(name == "example_connection_set_2")  return new example_connection_set_2(name);

return NULL;
}

//------------------------------------------------------------------------------

} // namespace nnlib2
#endif // NNLIB2_ADDITIONAL_PARTS_H
