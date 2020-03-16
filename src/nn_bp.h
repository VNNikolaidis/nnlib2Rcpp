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

#ifndef NN_BP_H
#define NN_BP_H

#include <cmath>

#include "nn.h"

namespace nnlib2 {

namespace bp {

/*-----------------------------------------------------------------------*/
/* Back Propagation Perceptron (bp_nn)					 */
/*-----------------------------------------------------------------------*/

class bp_nn : public NN_PARENT_CLASS
 {
 protected:

 bool setup(int input_dimension,int output_dimension);

 public:
 static bool display_squared_error;		// true = display squared error when encoding, false = display absolute error when encoding

 DATA bp_rnd_min;
 DATA bp_rnd_max;

 bp_nn();
 ~bp_nn();
 bool setup(int input_dimension,int output_dimension,DATA learning_rate,int hidden_layers,int hidden_layer_size);
 void set_initialization_mode_to_default();
 void set_initialization_mode_to_custom(DATA min_value, DATA max_value);
 DATA encode_s(DATA PTR input,int input_dim,DATA PTR desired_output,int output_dim,int UNUSED=0);
 void from_stream ( std::istream REF s );
 };


/*-----------------------------------------------------------------------*/
/* Experimental Unsupervised extentions of Back Propagation by VNN	 */
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Experimental Unsupervised extention of Back Propagation by VNN	 */
/*                              WinnerTakes1				 */
/*-----------------------------------------------------------------------*/

class bpu1_nn : public bp_nn
 {
 public:
 bpu1_nn();
 DATA encode_u(DATA PTR input, int input_dim, int iteration=0);
 };

/*-----------------------------------------------------------------------*/
/* Experimental Unsupervised extention of Back Propagation by VNN	 */
/*                              PunishLoosers				 */
/*-----------------------------------------------------------------------*/

class bpu2_nn : public bp_nn
 {
 public:
 bpu2_nn();
 DATA encode_u(DATA PTR input, int input_dim, int iteration=0);
 };

/*-----------------------------------------------------------------------*/
/* Experimental Unsupervised extention of Back Propagation by VNN	 */
/*                              HardLimitLike			         */
/*-----------------------------------------------------------------------*/

class bpu3_nn : public bp_nn
 {
 public:
 bpu3_nn();
 DATA encode_u(DATA PTR input, int input_dim, int iteration=0);
 };

/*-----------------------------------------------------------------------*/
/* Experimental Unsupervised extention of Back Propagation by VNN	 */
/*	Autoencoder1 (was: VNN's I Think I Saw A Pussy Cat 1)		 */
/*-----------------------------------------------------------------------*/

class bpu4_nn : public bp_nn
 {
 private:

 int m_hidden_layers_per_set;
 int m_hidden_layer_size;

 protected:
 DATA m_global_learning_rate;
 bool setup(int input_dimension,int output_dimension);

 public:
 bpu4_nn();
 bool setup(int input_dimension,int output_dimension, DATA learning_rate,int hidden_layers,int hidden_layer_size);
 DATA encode_u(DATA PTR input, int input_dim, int iteration=0);
 bool recall(DATA PTR input,int input_dim,DATA PTR output_buffer,int output_dim);
 };

/*-----------------------------------------------------------------------*/
/* Experimental extention of Back Propagation by VNN			 */
/* I called it 	Heteroencoder (to use for dimensionality reduction)...	 */
/*-----------------------------------------------------------------------*/
/* ...later I realized, such NN are called AutoEncoders in bibliography	 */
/*-----------------------------------------------------------------------*/

#define bpu_autoencoder_nn bpu5_nn

class bpu5_nn : public bp_nn
 {
 protected:
 int m_special_layer_component;

 public:
 bpu5_nn();
 bool setup(int input_dimension,DATA learning_rate,int hidden_layers_per_set,int hidden_layer_size,int special_layer_size);
 bool recall(DATA PTR input,int input_dim,DATA PTR output_buffer,int output_dim);
 };

} // end of namespace bp
} // end of namespace nnlib2

#else
// #error Header file included twice : NN_BP_H
#endif // NN_BP_H
