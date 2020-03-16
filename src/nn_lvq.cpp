//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		nn_lvq.h		 							Version 0.1
//		-----------------------------------------------------------
//		Definition - implementation of kohonen clustering nets
//		(lvq_nn) SOM and LVQ functionality.
//		SOM implementation is described as Unsupervised Learning LVQ
//		in P.K.Simpson's Artificial Neural Systems (1990)";
//		m_neighborhood_size=1 => Single Winner Unsupervised.
//		m_neighborhood_size>1 => Multiple Winner Unsupervised.
//		LVQ:implementation is described as Supervised Learning LVQ
//		in P.K.Simpson's Artificial Neural Systems (1990)";
//		-----------------------------------------------------------
// 		NOTE: this NN was implemented with a much earlier version of the library
//		and does not take full advantage of its current features and syntax.
//		I plan to rewrite this with newer (clearer) code if I get the time.
//		-----------------------------------------------------------

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

class lvq_input_layer : public pe_layer
 {
 public:
 void recall();
 };

// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void lvq_input_layer::recall()
  {
  if(no_error()) for(int i=0;i<size();i++)
   {
   pes[i].output=pes[i].input;
   pes[i].input=0;
   }
  }

/*-----------------------------------------------------------------------*/

class lvq_output_layer : public pe_layer
 {
 private:
 int m_neighborhood_size;									// must be 1 in Single Winner Unsupervised, 3,5,7... in Multiple Winner Unsupervised.

 public:
 void setup(string name, int size, int neighborhood);
 void recall();
 };

// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void lvq_output_layer::setup(string name, int size, int neighborhood)
  {
  pe_layer::setup(name,size);
  m_neighborhood_size=neighborhood;
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
    p.misc = LVQ_REWARD_PE;										// set PE to activated.
    if(last_winner >= 0) pes[last_winner].misc = LVQ_DEACTI_PE;	// deactivate any previous winner.
    last_winner = i;
    }
   else
    p.misc = LVQ_DEACTI_PE;									// set PE to not activated.
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
		pes[current_pe_to_activate].misc  = LVQ_REWARD_PE;		// PE is in winner's neighborhood, activate it;
		}
	current_pe_to_activate = last_winner;						// activate counterclockwise (left)
	for (i=1;i<=distance_to_activate;i++)
		{
		current_pe_to_activate = current_pe_to_activate+1;
		if(current_pe_to_activate>size()-1)
			current_pe_to_activate=0;							// circular topology
		pes[current_pe_to_activate].misc  = LVQ_REWARD_PE;		// PE is in winner's neighborhood, activate it;
		}
	}

  }

/*-----------------------------------------------------------------------*/
/* Connections															 */
/*-----------------------------------------------------------------------*/

class lvq_connection_set : public generic_connection_set
 {
 private:
 int m_iteration;

 public:

 lvq_connection_set();

 void recall();						// virtual, defined in component
 void encode();						// virtual, defined in component
 void encode(int iteration);		// a useful variation, of above, imposes iteration
 };

// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

lvq_connection_set::lvq_connection_set()
:generic_connection_set()
 {
 m_iteration=0;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void lvq_connection_set::encode(int iteration)
  {
  if(iteration>LVQ_MAXITERATION) { warning("Max LVQ iteration reached"); m_iteration=LVQ_MAXITERATION;return;}
  m_iteration = iteration;
  encode();
  }

void lvq_connection_set::encode()
  {
//DATA a = 1/(DATA) iteration;							    // (SIMPSON 5-110)
  DATA a = 0.2*(1.0-((DATA)m_iteration)/LVQ_MAXITERATION);	// (SIMPSON 5-111) we assume it means "epoch" here

  layer REF destin = destin_layer();

  if(no_error() AND (connections.goto_first()))
  do
   {
   connection REF c = (connections.current());
   pe REF destin_pe = destin.PE(c.destin_pe_id());

   if(destin_pe.misc == LVQ_REWARD_PE) 						// if destination PE is activated...
	{
    DATA d = c.misc;										// get difference, it was already computed during recall (see below)...
    DATA x = a*d ;
    c.weight() += x;										// adjust weight (SIMPSON 5-109)
    }

   if(destin_pe.misc == LVQ_PUNISH_PE)						// if destination PE is activated but is to be punished (supervised mode)...
	{
    DATA d = c.misc;										// get difference, it was already computed during recall (see below)...
    DATA x = a*d ;
    c.weight() -= x;										// adjust weight (SIMPSON 5-113)
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

   c.misc = (x - w);												// keep a copy of the difference in connection's misc.

   x = (x - w)*(x - w);												// then find the square of the differences and...

   destin.PE(destin_pe).add_to_input(x);  					        // ...do summation, forming the euclidian distance squared.
   }
  while(connections.goto_next());
  }

/*-----------------------------------------------------------------------*/
/* Kohonen LVQ	ANS	(Supervised LVQ)									 */
/*-----------------------------------------------------------------------*/
// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

lvq_nn::lvq_nn()
 :NN_PARENT_CLASS("Kohonen LVQ")
 {
 m_output_neighborhood_size=1;								// no neighborhood in supervised LVQ mode.
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

lvq_nn::~lvq_nn() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// optional matrix initializes weights; must be sized output_dimension X input_dimension

bool lvq_nn::setup(int input_dimension, int output_dimension,DATA ** initial_cluster_centers_matrix) 
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
  p_output_layer->setup("Output",output_dimension,m_output_neighborhood_size);


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
   set_ready();
   }
  }

 return no_error();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

DATA lvq_nn::encode_s(DATA PTR input, int input_dim, DATA PTR desired_output, int output_dim, int iteration)
 {
 if(desired_output==NULL) error (NN_NULLPT_ERR,"No desired output defined for LVQ");
 if(output_dim<1)		  error (NN_DATAST_ERR,"No desired output defined for LVQ");
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
						int desired_winner,
						int iteration)
 {

 if(is_ready())
  {
  INPUT_LAYER.input_data_from_vector(input,input_dim);
  recall();

  // find current winner class (with smallest distance)...

  int current_winner_pe	  = 0;
  DATA current_win_output = OUTPUT_LAYER.PE(0).output;	// this should be the distance, see lvq_output_layer::recall.

  for(int i=0;i<output_dimension();i++)
   {
   OUTPUT_LAYER.PE(i).misc = LVQ_DEACTI_PE;
   DATA d = OUTPUT_LAYER.PE(i).output;					// this should be the distance, see lvq_output_layer::recall.
   if(d<=current_win_output)
	{
	current_win_output = d;
    current_winner_pe  = i;
    }
   }

  if(current_winner_pe==desired_winner)					// this is based on SIMPSON equation (5-113)
	OUTPUT_LAYER.PE(current_winner_pe).misc = LVQ_REWARD_PE;
  else
	OUTPUT_LAYER.PE(current_winner_pe).misc = LVQ_PUNISH_PE;

  if(no_error()) LVQ_CONNECTIONS.encode(iteration);
  }

 return 0;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// input it :

void lvq_nn::from_stream ( std::istream REF s )
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

  if(number_of_components NEQL 3) {error(NN_IOFILE_ERR,"Not an LVQ neural net");return;}

  // create input layer...

  p_input_layer = new lvq_input_layer;
  p_input_layer->set_error_flag(my_error_flag());				// runtime errors in layer affect entire neural net.
  topology.append(p_input_layer);
  p_input_layer->from_stream(s);								// load input layer

  p_connection_set = new lvq_connection_set;
  p_connection_set->set_error_flag(my_error_flag());
  topology.append(p_connection_set);
  p_connection_set->from_stream(s);								// load connection set

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
  	set_ready();
    }
  }
 }

/*-----------------------------------------------------------------------*/
/* Kononen SOM	ANS	(Unsupervised LVQ)									 */
/*-----------------------------------------------------------------------*/
// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

som_nn::som_nn(int neighborhood_size)
 :lvq_nn()
 {
 m_name = "SOM ANS";
 m_output_neighborhood_size = neighborhood_size;
 if(m_output_neighborhood_size%2==0) m_output_neighborhood_size=m_output_neighborhood_size-1;
 if(m_output_neighborhood_size<1) m_output_neighborhood_size=1;
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
