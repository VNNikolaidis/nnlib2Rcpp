#ifndef NNLIB2_ADDITIONAL_PARTS_H
#define NNLIB2_ADDITIONAL_PARTS_H

#include "nn.h"
namespace nnlib2 {

//--------------------------------------------------------------------------------------------
// example (for R-Journal article): a pe that adds 10 to the sum of inputs

class rjournal_pe : public pe
{
public:
void recall() {	pe::recall(); output = output + 10; }
};

//--------------------------------------------------------------------------------------------
// minimalistic examples of pe and layer creation:
//--------------------------------------------------------------------------------------------

class example_pe : public pe {};

typedef Layer<example_pe> example_pe_layer_1;

class example_pe_layer_2: public Layer<example_pe>
 {
 public:
 example_pe_layer_2(string name, int size) : Layer(name,size) {};
 };

//--------------------------------------------------------------------------------------------
// minimalistic examples of connection and connection_set creation:
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

layer PTR generate_custom_layer(string name, int size)
{
	if(name == "rjournal") 		  return new Layer<rjournal_pe>(name,size);
	if(name == "example_layer_1") return new example_pe_layer_1(name, size);
	if(name == "example_layer_2") return new example_pe_layer_2(name, size);

return NULL;
}

//------------------------------------------------------------------------------
// register new connection_set classes here:
// given the name, it should return a pointer to a newly created connection_set
//------------------------------------------------------------------------------

connection_set PTR generate_custom_connection_set(string name)
{
	if(name == "example_connection_set_1")  return new example_connection_set_1(name);
	if(name == "example_connection_set_2")  return new example_connection_set_2(name);

return NULL;
}

//------------------------------------------------------------------------------

} // namespace nnlib2
#endif // NNLIB2_ADDITIONAL_PARTS_H
