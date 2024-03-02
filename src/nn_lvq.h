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


#ifndef NN_LVQ_H
#define NN_LVQ_H

#include <cmath>

#include "nn.h"

#define LVQ_MAXITERATION (10000)

namespace nnlib2 {
namespace lvq {

/*-----------------------------------------------------------------------*/
/* Base class for Kohonen - inspired ANS (currently LVQ or SOM)			 */
/*-----------------------------------------------------------------------*/

class kohonen_nn : public NN_PARENT_CLASS
{
protected:

bool setup(int input_dimension,
           int output_dimension,
           int output_neighborhood_size,						// for for unsupervised training (SOM) output_neighborhood_size is how many output nodes are affected,
		   DATA ** initial_cluster_centers_matrix = NULL);  	// optional matrix initializes weights; must be sized output_dimension X input_dimension

public:

kohonen_nn();
~kohonen_nn();

void from_stream ( std::istream REF s );
};

/*-----------------------------------------------------------------------*/
/* LVQ ANS (Supervised LVQ)												 */
/*-----------------------------------------------------------------------*/

class lvq_nn : public kohonen_nn
 {
 protected:

 int m_number_of_output_nodes_per_class;	 // for supervised training (LVQ) this is number of output PEs (and codebook vectors) per class.

 public:

 lvq_nn();
 lvq_nn(int number_of_output_nodes_per_class);

 void set_number_of_output_nodes_per_class(int number_of_output_nodes_per_class);
 int  get_number_of_output_nodes_per_class();

 bool setup(int input_dimension, int number_of_classes, DATA ** initial_cluster_centers_matrix = NULL);   			// optional matrix initializes weights; must be sized output_dimension X input_dimension

 DATA encode_s(DATA PTR input, int input_dim, DATA PTR desired_output, int output_dim, int iteration);
 DATA encode_s(DATA PTR input, int input_dim, int desired_class, int iteration);

 int recall_class (DATA PTR input, int input_dim);
 };

/*-----------------------------------------------------------------------*/
/* Self-Organized Map (SOM) ANS	(Unsupervised LVQ NN)					 */
/*-----------------------------------------------------------------------*/

class som_nn : public kohonen_nn
 {

 int	 m_output_neighborhood_size;		 // for unsupervised training (SOM) this is number of output nodes (near the winning one) which will be affected.

 public:

 som_nn(int neighborhood_size);

 bool setup(int input_dimension,int output_dimension, DATA ** initial_cluster_centers_matrix = NULL);   			// optional matrix initializes weights; must be sized output_dimension X input_dimension

 DATA encode_u(DATA PTR input, int input_dim, int iteration);
 };

/*-----------------------------------------------------------------------*/

class lvq_input_layer : public pe_layer
{
public:
        void recall();
};

/*-----------------------------------------------------------------------*/

class lvq_output_layer : public pe_layer
{
private:
        int m_neighborhood_size;	// must be 1 in Single Winner Unsupervised, 3,5,7... in Multiple Winner Unsupervised.
public:
		bool setup(string name, int size);
        bool setup(string name, int size, int neighborhood);
        void recall();
};

/*-----------------------------------------------------------------------*/

class lvq_connection_set : public generic_connection_set
{
private:
        int m_iteration;

public:

        lvq_connection_set();

        void set_iteration_number(int iteration);

        void recall();						// virtual, defined in component
        void encode();						// virtual, defined in component
        void encode(int iteration);		                // a variation of above, imposes iteration
};

/*-----------------------------------------------------------------------*/

} // namespace lvq
} // namespace nnlib2



#else
#error Header file included twice : NN_LVQ_H
#endif // NN_LVQ_H
