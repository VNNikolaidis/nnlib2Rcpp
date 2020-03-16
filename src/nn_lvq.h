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
/* Kohonen LVQ	ANS	(Supervised LVQ)									 */
/*-----------------------------------------------------------------------*/

class lvq_nn : public NN_PARENT_CLASS
 {
 protected:

 int	 m_output_neighborhood_size;

 public:

 lvq_nn();
 ~lvq_nn();
 bool setup(int input_dimension,int output_dimension, DATA ** initial_cluster_centers_matrix = NULL);   			// optional matrix initializes weights; must be sized output_dimension X input_dimension


 DATA encode_s(DATA PTR input, int input_dim, DATA PTR desired_output, int output_dim, int iteration);
 DATA encode_s(DATA PTR input, int input_dim, int desired_winner, int iteration);

 void from_stream ( std::istream REF s );
 };

/*-----------------------------------------------------------------------*/
/* Kohonen Self-Organized Map (SOM) ANS	(Unsupervised LVQ NN)		 */
/*-----------------------------------------------------------------------*/

class som_nn : public lvq_nn
 {
 public:

 som_nn(int neighborhood_size);
 DATA encode_u(DATA PTR input, int input_dim, int iteration);
 };

} // namespace lvq
} // namespace nnlib2

#else
#error Header file included twice : NN_LVQ_H
#endif // NN_LVQ_H
