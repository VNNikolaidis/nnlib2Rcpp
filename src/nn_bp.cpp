//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		nn_bp.h		 							Version 0.1
//		-----------------------------------------------------------
//		Definition - implementation of bp artificial neural
//		system (bp_nn ANS) functionality.
//		-----------------------------------------------------------
// 		NOTE: this NN was implemented with a much earlier version of the library
//		and does not take full advantage of its current features and syntax.
//		I plan to rewrite this with newer (clearer) code if I get the time.
//		-----------------------------------------------------------

#include <cmath>
#include <sstream>

#include "nn_bp.h"

#include "layer.h"
#include "connection_set.h"

namespace nnlib2 {

namespace bp {

#define INPUT_LAYER  (*(reinterpret_cast <bp_input_layer *> (topology.first())))
#define OUTPUT_LAYER (*(reinterpret_cast <bp_output_layer *> (topology.last())))
#define BP_RND_MIN -1
#define BP_RND_MAX +1

bool bp_nn::display_squared_error = false;			// true = display squared error when encoding, false = display absolute error when encoding

/*-----------------------------------------------------------------------*/
/* Back Propagation Perceptron Layers									 */
/*-----------------------------------------------------------------------*/
// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void bp_layer::set_learning_rate(DATA lrate)
{
	m_learning_rate=lrate;
}

/*-----------------------------------------------------------------------*/

class bp_input_layer : public bp_layer
 {
 public:
 void encode();
 void recall();
 };

// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void bp_input_layer::encode()
  {
  move_all_pe_input_to_output();
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void bp_input_layer::recall()
  {
  move_all_pe_input_to_output();
  }

/*-----------------------------------------------------------------------*/
// computing layer (includes hidden and output layers).
// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void bp_comput_layer::encode()
  {
  if(no_error())
  for(int i=0;i<size();i++)
   {
   pe REF p = pes[i];

   DATA current = p.output;										// and here is the last output produced.
   DATA d = p.input;											// sum of connection-relative errors is already stored here (via connections).
   DATA e = current * ((DATA)1 - current) * d;					// (SIMPSON 5-163)
   p.misc = e;													// store relative error in 'misc'.
   p.input=0;													// reset input.
   p.bias += m_learning_rate * e;								// adjust bias (SIMPSON 5-167)
   }
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void bp_comput_layer::recall()
  {
  if(no_error())
  for(int i=0;i<size();i++)
   {
   pe REF p = pes[i];

   DATA x = p.input;											// input is already summated;
   x = x + p.bias;												// add bias,
   p.output=(DATA)1/(1+exp(-x));								// and apply logistic sigmoid threshold.
																// Note:sigmoid maps output to range [0..1]
   p.input=0;													// reset input.
   }
  }

/*-----------------------------------------------------------------------*/
// in BP output layer has similar functionality to hidden...
// it is a computing layer.
// but also needs to be able to accept "desired output" as input when encoding.
// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void bp_output_layer::encode()
  {
  if(no_error())
  for(int i=0;i<size();i++)
   {
   pe REF p = pes[i];

   DATA desired = p.input;											// desired value is stored here, should be >= 0
   DATA current = p.output;											// and here is the last output produced.
   DATA d = current * ((DATA)1 - current) * ( desired - current );	// compute delta (SIMPSON 5-162)
   p.misc = d;														// store delta in 'misc'.
   p.input=0;														// reset input.
   p.bias += m_learning_rate * d;									// adjust bias (SIMPSON 5-165)
   }
  }

/*-----------------------------------------------------------------------*/
/* Back Propagation Perceptron Connections								 */
/*-----------------------------------------------------------------------*/
// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void bp_connection_set::encode()
  {
  layer REF source = source_layer();
  layer REF destin = destin_layer();

  if(no_error() AND (connections.goto_first()))
  do
   {
   connection REF c = connections.current();
   pe REF source_pe = source.PE(c.source_pe_id());
   pe REF destin_pe = destin.PE(c.destin_pe_id());

   DATA b = source_pe.output;
   DATA d = destin_pe.misc;									// get discrepancy at destination pe...
   DATA w = c.weight();										// get connection weight...
   DATA x = w * d;											// and multiply the two values...
   source_pe.add_to_input(x);								// feeding it back to the previous layer.

   c.weight() += (m_learning_rate * b * d);					// adjust weight (SIMPSON 5-164/6)
   }
  while(connections.goto_next());
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void bp_connection_set::recall()
  {
  int source_pe,destin_pe;
  layer REF source = source_layer();
  layer REF destin = destin_layer();

  if(no_error() AND (connections.goto_first()))
  do
   {
   connection REF c = (connections.current());

   source_pe = c.source_pe_id();
   destin_pe = c.destin_pe_id();

   DATA x = source.PE(source_pe).output;
   x = x * c.weight();

   destin.PE(destin_pe).add_to_input(x);
   }
  while(connections.goto_next());
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void bp_connection_set::set_learning_rate(DATA lrate)
  {
  m_learning_rate=lrate;
  }

/*-----------------------------------------------------------------------*/
/* Back Propagation Perceptron (bp_nn)									 */
/*-----------------------------------------------------------------------*/
// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bp_nn::bp_nn()
	:NN_PARENT_CLASS("Back Propagation")
 {
 set_initialization_mode_to_default();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bp_nn::~bp_nn()
 {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void bp_nn::set_initialization_mode_to_default()
 {
 bp_rnd_min = BP_RND_MIN;			// min random value used when initializing weights and biases.
 bp_rnd_max = BP_RND_MAX;			// max random value used when initializing weights and biases.
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// for experimentation, use before running setup

void bp_nn::set_initialization_mode_to_custom(DATA min_value, DATA max_value)
 {
 bp_rnd_min = min_value;			// min random value used when initializing weights and biases.
 bp_rnd_max = max_value;			// max random value used when initializing weights and biases.
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool bp_nn::setup(int input_dimension,int output_dimension,DATA learning_rate,int hidden_layers,int hidden_layer_size)
{
int i;

parameters.reset();

for(i=0;i<hidden_layers;i++)
{
	parameters.append(hidden_layer_size);
	parameters.append(learning_rate);
}

parameters.append(learning_rate);

return setup(input_dimension, output_dimension);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool bp_nn::setup(int input_dimension, int output_dimension)
 {

 int i,hidden_layers;
// char namebuffer [100];
 bp_layer * source_layer, * destin_layer;
 bp_connection_set * new_connection_set;
 int cparam;

 reset(false);

 if(no_error())
  {
  if(parameters.number_of_items() < 1)
   {
   error (NN_SYSTEM_ERR,"Additional parameters required for BP");
   return false;
   }

  if( ((parameters.number_of_items()-1) % 2) NEQL 0)
   {
   error (NN_SYSTEM_ERR,"Additional parameters required for BP hidden layers");
   return false;
   }

  cparam = 0;
  hidden_layers = (int) (parameters.number_of_items())/2;

  // create input layer...

  source_layer = new bp_input_layer;
  source_layer->set_error_flag(my_error_flag());				// runtime errors in layer affect entire neural net.
  source_layer->setup("Input",input_dimension);
  topology.append(source_layer);

  for(i=0;i<hidden_layers;i++)
   {
  // create connections to next layer(s)...

   new_connection_set = new bp_connection_set;
   new_connection_set->set_error_flag(my_error_flag());
   topology.append(new_connection_set);

  // create hidden layer(s)...

   std::stringstream layer_name;
   layer_name << "Hidden Layer " << i+1 <<  " (Comp.)";
   destin_layer = new bp_comput_layer ();
   destin_layer->set_error_flag(my_error_flag());
   destin_layer->setup(layer_name.str(),(int)(parameters[cparam++]));
   DATA lrate = (DATA)(parameters[cparam++]);
   destin_layer-> set_learning_rate(lrate);
   destin_layer->randomize_biases(bp_rnd_min,bp_rnd_max);
   topology.append(destin_layer);

  // setup and initialize the connections...

   new_connection_set->setup("Connections",source_layer,destin_layer);
   new_connection_set->fully_connect();
   new_connection_set->set_learning_rate(lrate);
   new_connection_set->set_connection_weights_random(bp_rnd_min,bp_rnd_max);

   source_layer = destin_layer;
   }

  // similarly, create and connect output layer:

  new_connection_set = new bp_connection_set;
  new_connection_set->set_error_flag(my_error_flag());
  topology.append(new_connection_set);

  destin_layer = new bp_output_layer ();
  destin_layer->set_error_flag(my_error_flag());
  destin_layer->setup("Output (Comp.)",output_dimension);
  DATA lrate = (DATA)(parameters[cparam++]);
  destin_layer->set_learning_rate(lrate);
  destin_layer->randomize_biases(bp_rnd_min,bp_rnd_max);
  topology.append(destin_layer);

  new_connection_set->setup("Connections",source_layer,destin_layer);
  new_connection_set->fully_connect();
  new_connection_set->set_learning_rate(lrate);
  new_connection_set->set_connection_weights_random(bp_rnd_min,bp_rnd_max);

  if(no_error())
   {
   set_component_for_input(0);								// the first in topology
   set_component_for_output(topology.size()-1);				// the last in topology
   set_is_ready_flag();
   }
  }

 return no_error();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DATA bp_nn::encode_s(	DATA PTR input,
						int input_dim,
						DATA PTR desired_output,
						int output_dim,
						int UNUSED)
 {
 DATA error_level = DATA_MAX;

 if(is_ready())
  {
  recall(input,input_dim);

  error_level = 0;

  DATA d;

  for(int i=0;i<output_dim;i++)
   {

   d = (desired_output[i] - OUTPUT_LAYER.PE(i).output);

   if(display_squared_error)

     error_level = error_level + (d * d); 					// calculate squared error.

   else

     error_level = error_level + fabs(d);					// calculate absolute error.

   }

  if(no_error())
   {
   if(OUTPUT_LAYER.input_data_from_vector(desired_output,output_dim))
   if(topology.goto_last())									// start from output layer and...
   do
    topology.current()->encode();							// ...encode while...
   while(topology.goto_previous());							// ...propagating backwards.
   }
  }
 return error_level;										// Note: error level is calculated before last 'encode' cycle.
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// input it :
// This assumes that the sequence of topology is:
// input_layer->connection_list->hidden_layer->connection_list->...->output_layer

void bp_nn::from_stream ( std::istream REF s )
 {
 int i, number_of_components, hidden_layers;
 string comment;
 bp_layer * source_layer, * destin_layer;
 bp_connection_set * new_connection_set;

 nn::from_stream(s);	                                    // read header (the way it was done in older versions)

 if(no_error())
  {
  if(s.rdstate()) {error(NN_IOFILE_ERR,"Error reading stream (BP)");return;}

  s >> comment >> number_of_components ;

  if(number_of_components<3) {error(NN_IOFILE_ERR,"No BP topology to load");return;}

  hidden_layers = (number_of_components - 3)/2;				// there is at least 2 layers (in-out) and a set of connections.

  // create input layer...

  source_layer = new bp_input_layer;
  source_layer->set_error_flag(my_error_flag());
  topology.append(source_layer);
  source_layer->from_stream(s);								// load input layer

  for(i=0;(i<hidden_layers)AND no_error();i++)
   {
  // create connections to next layer(s)...

   new_connection_set = new bp_connection_set;
   new_connection_set->set_error_flag(my_error_flag());
   topology.append(new_connection_set);
   new_connection_set->from_stream(s);						// load connection set

  // create hidden layer(s)...

   destin_layer = new bp_comput_layer;
   destin_layer->set_error_flag(my_error_flag());
   topology.append(destin_layer);
   destin_layer->from_stream(s);							// load hidden layer

  // fixup connection set (fix pointers ...)

   new_connection_set->setup(source_layer,destin_layer);
   source_layer = destin_layer;
   }

  // similarly, create and connect output layer:

  new_connection_set = new bp_connection_set;
  new_connection_set->set_error_flag(my_error_flag());
  topology.append(new_connection_set);
  new_connection_set->from_stream(s);						// load connection set

  destin_layer = new bp_output_layer;
  destin_layer->set_error_flag(my_error_flag());
  topology.append(destin_layer);
  destin_layer->from_stream(s);								// load output layer

  // fixup connection set (fix pointers ...)

  new_connection_set->setup(source_layer,destin_layer);

  if(no_error())
   {
   set_component_for_input(0);								// the first in topology
   set_component_for_output(topology.size()-1);				// the last in topology
   set_is_ready_flag();
   }
  }
 }

/*-----------------------------------------------------------------------*/
/* Experimental Unsupervised extentions of Back Propagation by VNN		 */
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Experimental Unsupervised extention of Back Propagation by VNN		 */
/*                              WinnerTakes1							 */
/*-----------------------------------------------------------------------*/
/// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bpu1_nn::bpu1_nn()
 :bp_nn()
 {
 m_name="Unsupervised MLP (BPU) WinnerTakes1";
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// recalls a vector and then encodes it (similar to what unsupervised LVQ does).
// do we really want to do this???

DATA bpu1_nn::encode_u(DATA PTR input, int input_dim, int iteration)
 {
 DATA error_level = DATA_MAX;

 if(is_ready())
  {
  recall(input,input_dim);

  error_level = 0;

  DATA d;
  int i;

  // find a way to encourage clustering, here find current winner

  DATA max_output = OUTPUT_LAYER.PE(0).output;
  int  max_output_pe = 0;
  for(i=1;i<output_dimension();i++)
   {
   d = OUTPUT_LAYER.PE(i).output;
   if(d>max_output)
	{
    max_output	  = d;
	max_output_pe = i;
	}
   }

  // create a "desired output" on the fly, based on step above

  DATA * desired_output = new DATA [output_dimension()];
  if(desired_output==NULL) error(NN_MEMORY_ERR,"Cannot create a desired output vector");

  if(no_error())
  for(i=0;i<output_dimension();i++)
   {
   if(i==max_output_pe) 	desired_output[i] = +1;
   else						desired_output[i] = 0;			// BP outputs to [0 1] range
   }

  // now encode like in regular bp (see bp_nn::encode_s)
  if(no_error())
  for(int i=0;i<output_dimension();i++)
   {

   d = (desired_output[i] - OUTPUT_LAYER.PE(i).output);

   if(display_squared_error)

     error_level = error_level + (d * d); 					// calculate squared error.

   else

     error_level = error_level + fabs(d);					// calculate absolute error.

   }


 if(no_error())
  {
  if(OUTPUT_LAYER.input_data_from_vector(desired_output,output_dimension()))
  if(topology.goto_last())									// start from output layer and...
   do
    topology.current()->encode();							// ...encode while...
   while(topology.goto_previous());							// ...propagating backwards.
  }
  // done encoding like in regular bp.

  delete [] desired_output;
  }
 return error_level;										// Note: error level is calculated before last 'encode' cycle.
 }


/*-----------------------------------------------------------------------*/
/* Experimental Unsupervised extention of Back Propagation by VNN		 */
/*                              PunishLoosers							 */
/*-----------------------------------------------------------------------*/
/// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bpu2_nn::bpu2_nn()
 :bp_nn()
 {
 m_name="Unsupervised MLP (BPU) PunishLoosers";
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// recalls a vector and then encodes it (similar to what unsupervised LVQ does).
// do we really want to do this???

DATA bpu2_nn::encode_u(DATA PTR input, int input_dim, int iteration)
 {
 DATA error_level = DATA_MAX;

 if(is_ready())
  {
  recall(input,input_dim);

  error_level = 0;

  DATA d;
  int i;

  // find a way to encourage clustering, here find current winner

  DATA max_output = OUTPUT_LAYER.PE(0).output;
  int  max_output_pe = 0;
  for(i=1;i<output_dimension();i++)
   {
   d = OUTPUT_LAYER.PE(i).output;
   if(d>max_output)
	{
    max_output	  = d;
	max_output_pe = i;
	}
   }

  // create a "desired output" on the fly, based on step above

  DATA * desired_output = new DATA [output_dimension()];
  if(desired_output==NULL) error(NN_MEMORY_ERR,"Cannot create a desired output vector");

  if(no_error())
  for(i=0;i<output_dimension();i++)
   {
   if(i==max_output_pe) 	desired_output[i] = OUTPUT_LAYER.PE(i).output;	// this is the only difference from V0
   else						desired_output[i] = 0;							// this is the only difference from V0
   }

  // now encode like in regular bp (see bp_nn::encode_s)
  if(no_error())
  for(int i=0;i<output_dimension();i++)
   {

   d = (desired_output[i] - OUTPUT_LAYER.PE(i).output);

   if(display_squared_error)

     error_level = error_level + (d * d); 					// calculate squared error.

   else

     error_level = error_level + fabs(d);					// calculate absolute error.

   }


 if(no_error())
  {
  if(OUTPUT_LAYER.input_data_from_vector(desired_output,output_dimension()))
  if(topology.goto_last())									// start from output layer and...
   do
    topology.current()->encode();							// ...encode while...
   while(topology.goto_previous());							// ...propagating backwards.
   }
  // done encoding like in regular bp.

  delete [] desired_output;
  }
 return error_level;										// Note: error level is calculated before last 'encode' cycle.
 }


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -/


/*-----------------------------------------------------------------------*/
/* Experimental Unsupervised extention of Back Propagation by VNN		 */
/*                              HardLimitLike							 */
/*-----------------------------------------------------------------------*/
/// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bpu3_nn::bpu3_nn()
 :bp_nn()
 {
 m_name="Unsupervised MLP (BPU) HardLimitLike";
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// recalls a vector and then encodes it (similar to what unsupervised LVQ does).
// do we really want to do this???

DATA bpu3_nn::encode_u(DATA PTR input, int input_dim, int iteration)
 {
 DATA error_level = DATA_MAX;

 if(is_ready())
  {
  recall(input,input_dim);

  error_level = 0;

  DATA d;
  int i;

  // create a "desired output" on the fly, based on output

  DATA * desired_output = new DATA [output_dimension()];
  if(desired_output==NULL) error(NN_MEMORY_ERR,"Cannot create a desired output vector");

  for(i=0;i<output_dimension();i++)
   {
   d = OUTPUT_LAYER.PE(i).output;
   if(d<0) warning ("Output < 0");
   if(d>1) warning ("Output > 1");
   if(d<=0.5)
	 desired_output[i] = 0;
   else
	 desired_output[i] = 1;
   }

  // now encode like in regular bp (see bp_nn::encode_s)
  if(no_error())
  for(int i=0;i<output_dimension();i++)
   {

   d = (desired_output[i] - OUTPUT_LAYER.PE(i).output);

   if(display_squared_error)

     error_level = error_level + (d * d); 					// calculate squared error.

   else

     error_level = error_level + fabs(d);					// calculate absolute error.

   }

 if(no_error())
  {
  if(OUTPUT_LAYER.input_data_from_vector(desired_output,output_dimension()))
  if(topology.goto_last())									// start from output layer and...
   do
    topology.current()->encode();							// ...encode while...
   while(topology.goto_previous());							// ...propagating backwards.
   }
  // done encoding like in regular bp.

  delete [] desired_output;
  }
 return error_level;										// Note: error level is calculated before last 'encode' cycle.
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -/


/*-----------------------------------------------------------------------*/
/* Experimental Unsupervised extention of Back Propagation by VNN		 */
/*			Autoencoder1 (was: VNN's I Think I Saw A Pussy Cat 1)		 */
/*-----------------------------------------------------------------------*/
/// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Note: in BPs if initial weights are all set to -1, output values are @ around 0 (up to around 0.25)

bpu4_nn::bpu4_nn()
 :bp_nn()
 {
 m_name="Unsupervised MLP (BPU) AutoEncoder1";
 m_global_learning_rate		= 0;
 m_hidden_layers_per_set	= 0;
 m_hidden_layer_size		= 0;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool bpu4_nn::setup(int input_dimension,int output_dimension, DATA learning_rate,int hidden_layers,int hidden_layer_size)
 {
 m_global_learning_rate = learning_rate;
 m_hidden_layers_per_set = hidden_layers;
 m_hidden_layer_size = hidden_layer_size;
 return setup(input_dimension, output_dimension);
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool bpu4_nn::setup(int input_dimension,int output_dimension)
 {

 int i;
 std::stringstream name;

 bp_layer * source_layer, * destin_layer;
 bp_connection_set * new_connection_set;

  // create input layer...

  source_layer = new bp_input_layer;
  source_layer->set_error_flag(my_error_flag());				// runtime errors in layer affect entire neural net.
  source_layer->setup("Input",input_dimension);
  topology.append(source_layer);

  for(i=0;i<m_hidden_layers_per_set;i++)
   {
  // create connections to next layer(s)...

   new_connection_set = new bp_connection_set;
   new_connection_set->set_error_flag(my_error_flag());
   topology.append(new_connection_set);

  // create hidden layer(s)...

   name.clear();
   name << "Hidden %d (Comp.)" << i+1;
   destin_layer = new bp_comput_layer ();
   destin_layer->set_error_flag(my_error_flag());
   destin_layer->setup(name.str(),m_hidden_layer_size);
   destin_layer->set_learning_rate(m_global_learning_rate);
   destin_layer->randomize_biases(bp_rnd_min,bp_rnd_max);
   topology.append(destin_layer);

  // setup and initialize the connections...

   new_connection_set->setup("",source_layer,destin_layer);
   new_connection_set->fully_connect();
   new_connection_set->set_learning_rate(m_global_learning_rate);
   new_connection_set->set_connection_weights_random(bp_rnd_min,bp_rnd_max);

   source_layer = destin_layer;
   }

  // similarly, create and connect internal special layer:

  new_connection_set = new bp_connection_set;
  new_connection_set->set_error_flag(my_error_flag());
  topology.append(new_connection_set);

  destin_layer = new bp_comput_layer ();									// This is the special layer (nothing special in this version, only size(
  destin_layer->set_error_flag(my_error_flag());
  destin_layer->setup("Special",output_dimension);
  destin_layer->set_learning_rate(m_global_learning_rate);
  destin_layer->randomize_biases(bp_rnd_min,bp_rnd_max);
  topology.append(destin_layer);

  new_connection_set->setup("",source_layer,destin_layer);
  new_connection_set->fully_connect();
  new_connection_set->set_learning_rate(m_global_learning_rate);
  new_connection_set->set_connection_weights_random(bp_rnd_min,bp_rnd_max);

  source_layer = destin_layer;

  for(i=0;i<m_hidden_layers_per_set;i++)
   {
  // create connections to next layer(s)...

   new_connection_set = new bp_connection_set;
   new_connection_set->set_error_flag(my_error_flag());
   topology.append(new_connection_set);

  // create hidden layer(s)...

   name.clear();
   name << "Hidden %d (Comp.)" << i+1;
   destin_layer = new bp_comput_layer ();
   destin_layer->set_error_flag(my_error_flag());
   destin_layer->setup(name.str(),m_hidden_layer_size);
   destin_layer->set_learning_rate(m_global_learning_rate);
   destin_layer->randomize_biases(bp_rnd_min,bp_rnd_max);
   topology.append(destin_layer);

  // setup and initialize the connections...

   new_connection_set->setup("",source_layer,destin_layer);
   new_connection_set->fully_connect();
   new_connection_set->set_learning_rate(m_global_learning_rate);
   new_connection_set->set_connection_weights_random(bp_rnd_min,bp_rnd_max);

   source_layer = destin_layer;
   }

  // similarly, create and connect output layer:

  new_connection_set = new bp_connection_set;
  new_connection_set->set_error_flag(my_error_flag());
  topology.append(new_connection_set);

  destin_layer = new bp_output_layer ();
  destin_layer->set_error_flag(my_error_flag());
  destin_layer->setup("Output (Comp.)",input_dimension);
  destin_layer->set_learning_rate(m_global_learning_rate);
  destin_layer->randomize_biases(bp_rnd_min,bp_rnd_max);
  topology.append(destin_layer);

  new_connection_set->setup("",source_layer,destin_layer);
  new_connection_set->fully_connect();
  new_connection_set->set_learning_rate(m_global_learning_rate);
  new_connection_set->set_connection_weights_random(bp_rnd_min,bp_rnd_max);

  source_layer = destin_layer;

  if(no_error())
   {
   set_component_for_input(0);								// the first in topology
   set_component_for_output(topology.size()-1);				// the last in topology (this is still a BP...I guess)
   set_is_ready_flag();
   }

 return no_error();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

DATA bpu4_nn::encode_u(DATA PTR input, int input_dim, int iteration)
 {
 DATA error_level = 0;

 if(is_ready())
  {
  bp_nn::recall(input,input_dim);

  // create a "desired output" on the fly, equal to the input

  DATA * desired_output = new DATA [input_dim];
  if(desired_output==NULL) error(NN_MEMORY_ERR,"Cannot create a desired output vector");

  if(no_error())
  for(int i=0;i<input_dim;i++)	desired_output[i] = input[i];

  // now encode like in regular bp (see bp_nn::encode_s)

  if(no_error())
  for(int i=0;i<input_dim;i++)
   {
   DATA d = (desired_output[i] - OUTPUT_LAYER.PE(i).output);
   if(display_squared_error)
     error_level = error_level + (d * d); 					// calculate squared error.
   else
     error_level = error_level + fabs(d);					// calculate absolute error.
   }

  if(no_error())
   {
   if(OUTPUT_LAYER.input_data_from_vector(desired_output,input_dim))
   if(topology.goto_last())									// start from output layer and...
   do
    topology.current()->encode();							// ...encode while...
   while(topology.goto_previous());							// ...propagating backwards.
   }

  // done encoding like in regular bp.

  delete [] desired_output;
  }
 return error_level;										// Note: error level is calculated before last 'encode' cycle.
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool bpu4_nn::recall(DATA PTR input,int input_dim,DATA PTR output_buffer,int output_dim)
 {
 if(NOT is_ready()) return false;
 bp_comput_layer * p_special_layer = (bp_comput_layer *) topology[1+2*m_hidden_layers_per_set+1+1-1];

 if((p_special_layer!=NULL) AND
    (output_dim EQL p_special_layer->size()) AND
    (bp_nn::recall(input,input_dim)))
   {
   p_special_layer->output_data_to_vector(output_buffer,output_dim);
   return no_error();
   }
  else
   {
   error(NN_INTEGR_ERR,"Current network structure corresponds to different output size");
   }
 return false;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*-----------------------------------------------------------------------*/
/* Experimental extention of Back Propagation by VNN					 */
/* "Heteroencoder" aka AutoEncoder (to use for dimensionality reduction) */
/*-----------------------------------------------------------------------*/
/// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bpu5_nn::bpu5_nn()
 :bp_nn()
 {
 m_name="BPU5 HeteroEncoder (Autoencoder)";
 m_special_layer_component	= -1;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool bpu5_nn::setup(int input_dimension, DATA learning_rate, int hidden_layers_per_set, int hidden_layer_size,int special_layer_size)
 {
 int i;
 std::stringstream name;
 bp_layer * source_layer, * destin_layer;
 bp_connection_set * new_connection_set;

  // create input layer...

  source_layer = new bp_input_layer;
  source_layer->set_error_flag(my_error_flag());				// runtime errors in layer affect entire neural net.
  source_layer->setup("Input",input_dimension);
  topology.append(source_layer);

  for(i=0;i<hidden_layers_per_set;i++)
   {
  // create connections to next layer(s)...

   new_connection_set = new bp_connection_set;
   new_connection_set->set_error_flag(my_error_flag());
   topology.append(new_connection_set);

  // create hidden layer(s)...

   name.clear();
   name << "Hidden %d (Comp.)" << i+1;
   destin_layer = new bp_comput_layer ();
   destin_layer->set_error_flag(my_error_flag());
   destin_layer->setup(name.str(),hidden_layer_size);
   destin_layer->set_learning_rate(learning_rate);
   destin_layer->randomize_biases(bp_rnd_min,bp_rnd_max);
   topology.append(destin_layer);

  // setup and initialize the connections...

   new_connection_set->setup("",source_layer,destin_layer);
   new_connection_set->fully_connect();
   new_connection_set->set_learning_rate(learning_rate);
   new_connection_set->set_connection_weights_random(bp_rnd_min,bp_rnd_max);

   source_layer = destin_layer;
   }

  // similarly, create and connect internal special layer:

  new_connection_set = new bp_connection_set;
  new_connection_set->set_error_flag(my_error_flag());
  topology.append(new_connection_set);

  destin_layer = new bp_comput_layer ();									// This is the special layer (nothing special in this version, only size(
  destin_layer->set_error_flag(my_error_flag());
  destin_layer->setup("Special",special_layer_size);
  destin_layer->set_learning_rate(learning_rate);
  destin_layer->randomize_biases(bp_rnd_min,bp_rnd_max);
  topology.append(destin_layer);
  m_special_layer_component = topology.size()-1;							// the index position of special layer in topology

  new_connection_set->setup("",source_layer,destin_layer);
  new_connection_set->fully_connect();
  new_connection_set->set_learning_rate(learning_rate);
  new_connection_set->set_connection_weights_random(bp_rnd_min,bp_rnd_max);

  source_layer = destin_layer;

  for(i=0;i<hidden_layers_per_set;i++)
   {
  // create connections to next layer(s)...

   new_connection_set = new bp_connection_set;
   new_connection_set->set_error_flag(my_error_flag());
   topology.append(new_connection_set);

  // create hidden layer(s)...

   name.clear();
   name << "Hidden %d (Comp.)" << i + 1;
   destin_layer = new bp_comput_layer ();
   destin_layer->set_error_flag(my_error_flag());
   destin_layer->setup(name.str(),hidden_layer_size);
   destin_layer->set_learning_rate(learning_rate);
   destin_layer->randomize_biases(bp_rnd_min,bp_rnd_max);
   topology.append(destin_layer);

  // setup and initialize the connections...

   new_connection_set->setup("",source_layer,destin_layer);
   new_connection_set->fully_connect();
   new_connection_set->set_learning_rate(learning_rate);
   new_connection_set->set_connection_weights_random(bp_rnd_min,bp_rnd_max);

   source_layer = destin_layer;
   }

  // similarly, create and connect output layer:

  new_connection_set = new bp_connection_set;
  new_connection_set->set_error_flag(my_error_flag());
  topology.append(new_connection_set);

  destin_layer = new bp_output_layer ();
  destin_layer->set_error_flag(my_error_flag());
  destin_layer->setup("Output (Comp.)",input_dimension);		// <--- note this is same as input
  destin_layer->set_learning_rate(learning_rate);
  destin_layer->randomize_biases(bp_rnd_min,bp_rnd_max);
  topology.append(destin_layer);

  new_connection_set->setup("",source_layer,destin_layer);
  new_connection_set->fully_connect();
  new_connection_set->set_learning_rate(learning_rate);
  new_connection_set->set_connection_weights_random(bp_rnd_min,bp_rnd_max);

  source_layer = destin_layer;

  if(no_error())
   {
   set_component_for_input(0);								// the first in topology
   set_component_for_output(topology.size()-1);				// the last in topology (this is still a BP...I guess)
   set_is_ready_flag();
   }

 return no_error();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool bpu5_nn::recall(DATA PTR input,int input_dim,DATA PTR output_buffer,int output_dim)
 {
 if(NOT is_ready()) return false;
 if((bp_nn::recall(input,input_dim)) AND
    (m_special_layer_component>=0))
   {
   bp_comput_layer * p_special_layer = (bp_comput_layer *) topology[m_special_layer_component];
   p_special_layer->output_data_to_vector(output_buffer,output_dim);
   return no_error();
   }
  else
   {
   error(NN_INTEGR_ERR,"Current network structure corresponds to different output size");
   }
 return false;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // end of namespace bp
} // end of namespace nnlib2

