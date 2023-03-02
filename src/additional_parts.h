#ifndef NNLIB2_ADDITIONAL_PARTS_H
#define NNLIB2_ADDITIONAL_PARTS_H

#include "nn.h"
#include "connection_matrix.h"

using namespace nnlib2;

#include "additional_parts_other.h"
#include "additional_parts_softmax.h"

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

//--------------------------------------------------------------------------------------------
// minimal examples of matrix based (derived from generic_connection_matrix) definitions:
//--------------------------------------------------------------------------------------------

class example_connection_matrix: public generic_connection_matrix
{
public:

	example_connection_matrix(string name):generic_connection_matrix(name,true){};

	void encode () {};           // Just an example, does nothing
	void recall () {};           // Just an example, does nothing
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

	if(name == "softmax")			return new softmax_layer(name, size);

	if(name == "BP-hidden-softmax")
	{
		bp_comput_softmax_layer PTR pl = new bp_comput_softmax_layer;
		// depending on their specifics, some type of layers may have additional setup steps. For BP layers:
		pl->setup(name,size);
		pl->randomize_biases(-1,1);
		DATA bp_learnrate = 0.6;
		if(optional_parameter!=DATA_MIN)
			bp_learnrate = optional_parameter;
		pl->set_learning_rate(bp_learnrate);
		TEXTOUT << "(This " << name << " layer uses learning rate = " << bp_learnrate << ")\n";
		return pl;
	}

	if(name == "BP-output-softmax")
	{
		bp_output_softmax_layer PTR pl = new bp_output_softmax_layer;
		// depending on their specifics, some type of layers may have additional setup steps. For BP layers:
		pl->setup(name,size);
		pl->randomize_biases(-1,1);
		DATA bp_learnrate = 0.6;
		if(optional_parameter!=DATA_MIN)
			bp_learnrate = optional_parameter;
		pl->set_learning_rate(bp_learnrate);
		TEXTOUT << "(This " << name << " layer uses learning rate = " << bp_learnrate << ")\n";
		return pl;
	}

	return NULL;
}

//------------------------------------------------------------------------------
// register new connection_set classes here:
// given the name, it should return a pointer to a newly created connection_set
//------------------------------------------------------------------------------

connection_set PTR generate_custom_connection_set(string name, DATA optional_parameter=DATA_MIN)
{
	// examples using 'connection' class:

	if(name == "perceptron")                return new perceptron_connection_set(name);
	if(name == "MEX")						return new MEX_connection_set(name);

	if(name == "example_connection_set_0")  return new Connection_Set<example_connection>(name);
	if(name == "example_connection_set_1")  return new example_connection_set_1(name);
	if(name == "example_connection_set_2")  return new example_connection_set_2(name);

	if(name == "example_connection_matrix") return new example_connection_matrix(name);

	return NULL;
}

//------------------------------------------------------------------------------

#endif // NNLIB2_ADDITIONAL_PARTS_H
