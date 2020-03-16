//		----------------------------------------------------------
//		(C)2019       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    Rcpp glue code for Autoencoder NN (bpu_autoencoder_nn, supervised)
//		-----------------------------------------------------------

#include "nnlib2.h"

#ifdef NNLIB2_FOR_RCPP
using namespace Rcpp;

//--------------------------------------------------------------------------------

#include "nn_bp.h"

using namespace nnlib2;
using namespace nnlib2::bp;

//--------------------------------------------------------------------------------

// [[Rcpp::export]]
NumericMatrix Autoencoder  (
                           NumericMatrix data_in,
                           int desired_new_dimension,
                           int number_of_training_epochs,           // (each presents all data)
                           double learning_rate,
                           int num_hidden_layers = 1,               // number of hidden layers on each side of special layer
                           int hidden_layer_size = 5,               // number of nodes in each hidden layer
                           bool show_nn = false
                           )
 {

 int input_dimension    = data_in.cols();
 int num_training_cases = data_in.rows();

 NumericMatrix data_out;

 if (input_dimension    <= 0) return data_out;
 if (num_training_cases <= 0) return data_out;

 data_out=NumericMatrix(num_training_cases,desired_new_dimension);

 bpu_autoencoder_nn ae;
 if( ae.no_error()) ae.setup(input_dimension, learning_rate, num_hidden_layers, hidden_layer_size, desired_new_dimension);
 if(NOT ae.no_error()) return(data_out);

 TEXTOUT << "Max number of epochs = " << number_of_training_epochs << "\n";

 for(int i=0;(i<number_of_training_epochs) && ae.no_error();i++)
  {
    DATA error_level = 0;
    for(int r=0;r<num_training_cases;r++)
      {
      NumericVector v(data_in( r , _ ));                            // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
      double * fp_v = REAL(v);                                      // my (lame?) way to interface with R, cont.)

      error_level += ae.encode_s(fp_v, v.size(), fp_v, v.size());
      }
    error_level = error_level/(num_training_cases);
    if(i%100==0)
      {
      checkUserInterrupt();                                       // (RCpp function to check if user pressed cancel)
      TEXTOUT << "Epoch = "<< i << " , Error level indicator = " << error_level << "\n";
      }
  }
 TEXTOUT << "Training ended.\n\n";

 if(show_nn)
 {
   TEXTOUT << "------Network structure (BEGIN)--------\n";
   ae.to_stream(TEXTOUT);
   TEXTOUT << "--------Network structure (END)--------\n";
 }

 NumericVector v_out(desired_new_dimension);                        // buffer for output
 double * fp_out = REAL(v_out);                                     // buffer for output (as double *)

 for(int r=0;r<num_training_cases;r++)
 {
   NumericVector v(data_in( r , _ ));                               // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
   double * fp_v = REAL(v);                                         // my (lame?) way to interface with R, cont.)

   ae.recall(fp_v, v.size(), fp_out, desired_new_dimension);

   data_out( r , _ ) = v_out;                                       // my (lame?) way to interface with R, cont.)
 }

 return data_out;
 }


#endif // NNLIB2_FOR_RCPP
