//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		nn_lvq.h		 							Version 0.1
//		-----------------------------------------------------------
//		Definition - implementation of kohonen clustering nets
//		(lvq_nn) SOM (actually LVQ-unsupervised) and LVQ functionality.
//		SOM implementation is described as Unsupervised Learning LVQ
//		in P.K.Simpson's Artificial Neural Systems (1990)";
//		m_neighborhood_size=1 => Single Winner Unsupervised.
//		m_neighborhood_size>1 => Multiple Winner Unsupervised.
//		LVQ:implementation is described as Supervised Learning LVQ
//		in P.K.Simpson's Artificial Neural Systems (1990),p.88.";
//		-----------------------------------------------------------
// 		NOTE: this NN was implemented with a much earlier version of the library
//		and does not take full advantage of its current features and syntax.
//		I plan to rewrite this with newer (clearer) code if I get the time.
//		-----------------------------------------------------------

#include <sstream>
#include "nn_lvq.h"

#include "layer.h"
#include "connection_set.h"

#define LVQ_RND_MIN 0
#define LVQ_RND_MAX +1

#define LVQ_PUNISH_PE	(10)
#define LVQ_DEACTI_PE	(20)
#define LVQ_REWARD_PE	(30)

#define INPUT_LAYER     (*(reinterpret_cast <lvq_input_layer *>    (topology[0])))
#define LVQ_CONNECTIONS (*(reinterpret_cast <lvq_connection_set *> (topology[1])))
#define OUTPUT_LAYER    (*(reinterpret_cast <lvq_output_layer *>   (topology[2])))

namespace nnlib2 {
namespace lvq {

/*-----------------------------------------------------------------------*/
/* Layers																 */
/*-----------------------------------------------------------------------*/
// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void lvq_input_layer::recall()
  {
  if(no_error())
  for(int i=0;i<size();i++)
   {
   pes[i].output=pes[i].input;
   pes[i].input=0;
   }
  }

/*-----------------------------------------------------------------------*/
// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool lvq_output_layer::setup(string name, int size)
{
	bool ok = pe_layer::setup(name,size);
	m_neighborhood_size=1;
	return ok;
}

bool lvq_output_layer::setup(string name, int size, int neighborhood)
{
	bool ok = pe_layer::setup(name,size);
	m_neighborhood_size=neighborhood;
	return ok;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void lvq_output_layer::recall()
  {
  int last_winner = -1;
  DATA last_winning_value = DATA_MAX;

  if(no_error())
  for(int i=0;i<size();i++)
   {
   pe REF p = pes[i];

   DATA x = p.input;											// input is already the Euclidian distance squared.
   x = sqrt(x);													// Well, make it the actual Euclidian distance.
   p.output=x;													// output the Euclidian distance
   p.input=0;													// reset input.
   }

  // find winner

  for(int i=0;i<size();i++)
   {
   pe REF p = pes[i];
   if(p.output < last_winning_value)
    {
    last_winning_value = p.output;
    p.bias = LVQ_REWARD_PE;										// set PE to activated.
    if(last_winner >= 0) pes[last_winner].bias = LVQ_DEACTI_PE;	// deactivate any previous winner.
    last_winner = i;
    }
   else
    p.bias = LVQ_DEACTI_PE;										// set PE to not activated.
   }

  // also activate nodes in neighborhood

  if(m_neighborhood_size>1)
	{
	int distance_to_activate = (m_neighborhood_size-1)/2;
	int current_pe_to_activate;
	int i;
	current_pe_to_activate = last_winner;						// activate counterclockwise (left)
	for (i=1;i<=distance_to_activate;i++)
		{
		current_pe_to_activate = current_pe_to_activate-1;
		if(current_pe_to_activate<0)
			current_pe_to_activate=size()-1;					// circular topology
		pes[current_pe_to_activate].bias  = LVQ_REWARD_PE;		// PE is in winner's neighborhood, activate it;
		}
	current_pe_to_activate = last_winner;						// activate counterclockwise (left)
	for (i=1;i<=distance_to_activate;i++)
		{
		current_pe_to_activate = current_pe_to_activate+1;
		if(current_pe_to_activate>size()-1)
			current_pe_to_activate=0;							// circular topology
		pes[current_pe_to_activate].bias  = LVQ_REWARD_PE;		// PE is in winner's neighborhood, activate it;
		}
	}

  }

/*-----------------------------------------------------------------------*/
/* LVQ Connections														 */
/*-----------------------------------------------------------------------*/
// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

lvq_connection_set::lvq_connection_set()
:generic_connection_set()
 {
 m_iteration=0;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void lvq_connection_set::set_iteration_number(int iteration)
	{
	if(iteration<0)
		{
		warning("Attempted to set LVQ iteration number to negative value, setting iteration counter to 0");
		m_iteration=0;
		return;
		}

	if(iteration>LVQ_MAXITERATION)
		{
		std::stringstream m;
		m << "Attempted to set LVQ iteration above maximum limit (" << (int)LVQ_MAXITERATION << ")";
		warning(m.str()); m_iteration=LVQ_MAXITERATION;
		return;
		}

	m_iteration = iteration;
	}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// a variation of encode, imposes iteration number

void lvq_connection_set::encode(int iteration)
{
	set_iteration_number(iteration);
	encode();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// encodes a single data item. does not affect iteration

void lvq_connection_set::encode()
  {
  if(m_iteration < 0)
	{
	warning("Negative iteration (epoch) number.");
	m_iteration = 0;
	}

  if(m_iteration > LVQ_MAXITERATION)
	{
  	warning("Maximum iteration (epoch) exceeded.");
  	m_iteration = LVQ_MAXITERATION;
	}

//DATA a = 1/(DATA) iteration;							    // (SIMPSON 5-110)
  DATA a = 0.2*(1.0-((DATA)m_iteration)/LVQ_MAXITERATION);	// (SIMPSON 5-111) we assume it means "epoch" here

  layer REF destin = destin_layer();

  if(no_error() AND (connections.goto_first()))
  do
   {
   connection REF c = (connections.current());
   pe REF destin_pe = destin.PE(c.destin_pe_id());

   if(destin_pe.bias == LVQ_REWARD_PE) 						// if destination PE is activated...
	{
    DATA d = c.misc;										// get difference, it was already computed during recall (see below)...
    DATA dw = a*d ;
    c.weight() += dw;										// adjust weight (SIMPSON 5-109)
    }

    if(destin_pe.bias == LVQ_PUNISH_PE)						// if destination PE is activated but is to be punished (supervised mode)...
 	{
    DATA d = c.misc;										// get difference, it was already computed during recall (see below)...
    DATA dw = -a*d ;
    c.weight() += dw;										// adjust weight (SIMPSON 5-113)
    }
   }
  while(connections.goto_next());
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void lvq_connection_set::recall()
  {
  int source_pe,destin_pe;
  DATA x,w;

  layer REF source = source_layer();
  layer REF destin = destin_layer();

  if(no_error() AND (connections.goto_first()))
  do
   {
   connection REF c = (connections.current());

   source_pe = c.source_pe_id();
   destin_pe = c.destin_pe_id();

   x = source.PE(source_pe).output;
   w = c.weight();

   c.misc = (x - w);												// keep a copy of the difference in connection's 'misc' register.

   x = (x - w)*(x - w);												// then find the square of the differences and...

   destin.PE(destin_pe).add_to_input(x);  					        // ...do summation, forming the euclidian distance squared.
   }
  while(connections.goto_next());
  }

/*-----------------------------------------------------------------------*/
/* Base class for Kohonen - inspired ANS (currently LVQ or SOM)			 */
/*-----------------------------------------------------------------------*/

kohonen_nn::kohonen_nn()
:NN_PARENT_CLASS("Kohonen-inspired ANS")
{}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

kohonen_nn::~kohonen_nn()
{
reset();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool kohonen_nn::setup(
			int input_dimension,
            int output_dimension,
            int output_neighborhood_size,						// for for unsupervised training (SOM) output_neighborhood_size is how many output nodes are affected,
            DATA ** initial_cluster_centers_matrix)			  	// optional matrix initializes weights; must be sized output_dimension X input_dimension
	{
		lvq_input_layer    * p_input_layer;
		lvq_output_layer   * p_output_layer;
		lvq_connection_set * p_connection_set;

		if((input_dimension<=0) OR (output_dimension<=0)) {error(NN_DATAST_ERR,"Invalid LVQ dims"); return false;}

		if(no_error())
		{
			reset();

			// create input layer...

			p_input_layer = new lvq_input_layer;
			p_input_layer->set_error_flag(my_error_flag());			// runtime errors in layer affect entire neural net.
			p_input_layer->setup("Input",input_dimension);

			p_output_layer = new lvq_output_layer ();
			p_output_layer->set_error_flag(my_error_flag());
			p_output_layer->setup("Output",output_dimension, output_neighborhood_size);

			p_connection_set = new lvq_connection_set;
			p_connection_set->set_error_flag(my_error_flag());

			p_connection_set->setup("",p_input_layer,p_output_layer);
			p_connection_set->fully_connect(false);

			if(initial_cluster_centers_matrix EQL NULL)
			{
				p_connection_set->set_connection_weights_random(LVQ_RND_MIN,LVQ_RND_MAX);
			}
			else
			{
				int s,d;
				for(d=0;d<p_output_layer->size();d++)
					for(s=0;s<p_input_layer->size();s++)
						p_connection_set->set_connection_weight(s,d,initial_cluster_centers_matrix[d][s]);
			}

			topology.append(p_input_layer);
			topology.append(p_connection_set);
			topology.append(p_output_layer);

			if(no_error())
			{
				set_component_for_input(0);
				set_component_for_output(2);
				set_is_ready_flag();
			}
		}

		return no_error();
	}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// input it :

void kohonen_nn::from_stream ( std::istream REF s )
{
	string comment;
	lvq_input_layer    * p_input_layer;
	lvq_output_layer   * p_output_layer;
	lvq_connection_set * p_connection_set;
	int number_of_components;

	nn::from_stream(s);		                                    // read header (the way it was done in older versions)

	if(no_error())
	{
		if(s.rdstate()) {error(NN_IOFILE_ERR,"Error reading stream (LVQ)");return;}

		s >> comment >> number_of_components ;

		if(number_of_components NEQL 3) {error(NN_IOFILE_ERR,"Not a Kohonen-type (LVQ or SOM) neural net");return;}

		// create input layer...

		p_input_layer = new lvq_input_layer;
		p_input_layer->set_error_flag(my_error_flag());				// runtime errors in layer affect entire neural net.
		topology.append(p_input_layer);
		p_input_layer->from_stream(s);								// load input layer

		p_connection_set = new lvq_connection_set;
		p_connection_set->set_error_flag(my_error_flag());
		topology.append(p_connection_set);
		p_connection_set->from_stream(s);							// load connection set

		p_output_layer = new lvq_output_layer ();
		p_output_layer->set_error_flag(my_error_flag());
		topology.append(p_output_layer);
		p_output_layer->from_stream(s);								// load output layer

		// fixup connection set (fix pointers ...)

		p_connection_set->setup("Connections",p_input_layer,p_output_layer);

		if(no_error())
		{
			set_component_for_input(0);
			set_component_for_output(2);
			set_is_ready_flag();
		}
	}
}

/*-----------------------------------------------------------------------*/
/* LVQ ANS (Supervised LVQ)												 */
/*-----------------------------------------------------------------------*/
// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

lvq_nn::lvq_nn()
 {
 m_number_of_output_nodes_per_class = 0;
 set_number_of_output_nodes_per_class(1);
 punish_enable(TRUE);
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

lvq_nn::lvq_nn(int number_of_output_nodes_per_class, bool allow_punish)
 {
 m_number_of_output_nodes_per_class = 0;
 set_number_of_output_nodes_per_class(number_of_output_nodes_per_class);
 punish_enable(allow_punish);
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void lvq_nn::set_number_of_output_nodes_per_class(int number_of_output_nodes_per_class)
{
	if(number_of_output_nodes_per_class==m_number_of_output_nodes_per_class)
		return;

	if(number_of_output_nodes_per_class<=1)
		{
		m_name = "LVQs (Supervised LVQ) ANS";
		m_number_of_output_nodes_per_class = 1;
		return;
		}

	m_name = "LVQs (Supervised LVQ) ANS with multiple output nodes per class";
	m_number_of_output_nodes_per_class = number_of_output_nodes_per_class;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int lvq_nn::get_number_of_output_nodes_per_class()
{
	return m_number_of_output_nodes_per_class;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void lvq_nn::punish_enable(bool enable)
{
	m_punish_enabled = enable;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool lvq_nn::punish_enabled()
{
	return(m_punish_enabled);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// optional matrix initializes weights; must be sized output_dimension X input_dimension

bool lvq_nn::setup(int input_dimension,
                   int number_of_classes,
                   DATA ** initial_cluster_centers_matrix)
 {
	return kohonen_nn::setup(
							input_dimension,
							number_of_classes * m_number_of_output_nodes_per_class,
							1,
							initial_cluster_centers_matrix
							);

 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

DATA lvq_nn::encode_s(DATA PTR input, int input_dim, DATA PTR desired_output, int output_dim, int iteration)
 {
 if(desired_output==NULL)
 	error (NN_NULLPT_ERR,"No desired output defined for LVQ");
 if(output_dim<1)
 	error (NN_DATAST_ERR,"No desired output defined for LVQ");
 if(m_number_of_output_nodes_per_class>1)
 	error (NN_DATAST_ERR,"This operation is not currently implemented for LVQs with multiple outputs per class");

 if(no_error())
  {
  DATA max=desired_output[0];
  int win=0;
  for(int i=1;i<output_dim;i++)
   if(desired_output[i]>max)
    {
    max=desired_output[i];
    win=i;
    }
  return encode_s(input,input_dim,win,iteration);
  }
 return DATA_MAX;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

DATA lvq_nn::encode_s(	DATA PTR input,
						int input_dim,
						int desired_class,
						int iteration)
 {

 if(is_ready())
  {
  // recall the data vector:

  INPUT_LAYER.input_data_from_vector(input,input_dim);
  recall();

  // find which output node is best for input vector (has smallest distance)

  int current_winner_pe	  = 0;
  DATA current_win_output = OUTPUT_LAYER.PE(0).output;	// this should be the distance, see lvq_output_layer::recall.

  for(int i=0;i<output_dimension();i++)
   {
   OUTPUT_LAYER.PE(i).bias = LVQ_DEACTI_PE;
   DATA d = OUTPUT_LAYER.PE(i).output;					// this should be the distance, see lvq_output_layer::recall.
   if(d<=current_win_output)
	{
	current_win_output = d;
    current_winner_pe  = i;
    }
   }

  // translate winning PE number to class id (numbers start at 0):

  int returned_class =
  	(int)(current_winner_pe / m_number_of_output_nodes_per_class);

  // reward if winning PE is one of those assigned to desired class, else punish.

  if(returned_class==desired_class)					// this is based on SIMPSON equation (5-113)
    {
	OUTPUT_LAYER.PE(current_winner_pe).bias = LVQ_REWARD_PE;
  	OUTPUT_LAYER.PE(current_winner_pe).misc = OUTPUT_LAYER.PE(current_winner_pe).misc + 1;  // just a counter of rewards given to the PE (for inspection purposes, not affecting results)
    }
  else
	if(m_punish_enabled)
		OUTPUT_LAYER.PE(current_winner_pe).bias = LVQ_PUNISH_PE;

  if(no_error()) LVQ_CONNECTIONS.encode(iteration);
  }

 return 0;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int lvq_nn::recall_class( DATA PTR input,
                    	  int input_dim )

{
	int returned_class = -1;

	if(is_ready())
	{
		// recall the data vector:

		INPUT_LAYER.input_data_from_vector(input,input_dim);
		recall();

		// find which output node is best for input vector (has smallest distance)

		int current_winner_pe	  = 0;
		DATA current_win_output = OUTPUT_LAYER.PE(0).output;	// this should be the distance, see lvq_output_layer::recall.

		for(int i=0;i<output_dimension();i++)
		{
			OUTPUT_LAYER.PE(i).bias = LVQ_DEACTI_PE;
			DATA d = OUTPUT_LAYER.PE(i).output;					// this should be the distance, see lvq_output_layer::recall.
			if(d<=current_win_output)
			{
				current_win_output = d;
				current_winner_pe  = i;
			}
		}

		// translate winning PE number to class id (numbers start at 0):

		returned_class =
			(int)(current_winner_pe / m_number_of_output_nodes_per_class);
	}

	return returned_class;
}

/*-----------------------------------------------------------------------*/
/* Kononen SOM	ANS	(Unsupervised LVQ)									 */
/*-----------------------------------------------------------------------*/
// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

som_nn::som_nn(int neighborhood_size)
 {
 m_name = "LVQu (SOM or Unsupervised LVQ) ANS";
 m_output_neighborhood_size = neighborhood_size;
 if(m_output_neighborhood_size%2==0) m_output_neighborhood_size=m_output_neighborhood_size-1;
 if(m_output_neighborhood_size<1) m_output_neighborhood_size=1;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool som_nn::setup(int input_dimension,
                   int output_dimension,
                   DATA ** initial_cluster_centers_matrix)   			// optional matrix initializes weights; must be sized output_dimension X input_dimension
{
	return kohonen_nn::setup(	input_dimension,
                    			output_dimension,
                    			m_output_neighborhood_size,
                    			initial_cluster_centers_matrix);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

DATA som_nn::encode_u(DATA PTR input, int input_dim, int iteration)
 {
 if(is_ready())
  {
  INPUT_LAYER.input_data_from_vector(input,input_dim);
  recall();
  if(no_error()) LVQ_CONNECTIONS.encode(iteration);
  }
 return 1;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // end of namespace lvq
} // end of namespace nnlib2
