//		----------------------------------------------------------
//		(C)2019       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    Rcpp glue code for Self-Organized Map NN (som_nn,unsupervised)
//		-----------------------------------------------------------

#include "nnlib2.h"

#ifdef NNLIB2_FOR_RCPP
using namespace Rcpp;

//--------------------------------------------------------------------------------

#include "nnlib2_misc.h"                      // for which_max
#include "nn_lvq.h"                         // for som_nn

using namespace nnlib2;
using namespace nnlib2::lvq;

//--------------------------------------------------------------------------------
// Rcpp glue code for LVQ-unsupervised (som_nn)

// [[Rcpp::export]]
IntegerVector LVQu (  NumericMatrix data,
                     int max_number_of_desired_clusters,
                     int number_of_training_epochs,          // (each presents all data)
                     int neighborhood_size =1,               // should be odd.
                     bool show_nn = false )
{
   IntegerVector returned_cluster_ids = rep(-1,data.rows());

   int input_data_dim = data.cols();
   int output_dim = max_number_of_desired_clusters;

   som_nn som(neighborhood_size);                                       // A Self-Organizing-Map NN

   if(som.no_error())   som.setup(input_data_dim,output_dim);
   if(NOT som.no_error())   return returned_cluster_ids;

   // encode all data

   for(int i=0;i<number_of_training_epochs;i++)
   {
   for(int r=0;r<data.rows();r++)
      {
      NumericVector v(data( r , _ ));                                   // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
      double * fp_v = REAL(v);                                          // my (lame?) way to interface with R, cont.)

      som.encode_u(fp_v,data.cols(),i);                                 // Encode a single item, unsupervised
      }
   checkUserInterrupt();                                                // (RCpp function to check if user pressed cancel)
   }

   if(show_nn)
   {
      TEXTOUT << "------Network structure (BEGIN)--------\n";
      som.to_stream(TEXTOUT);
      TEXTOUT << "--------Network structure (END)--------\n";
   }


   // training completed, now recall the data and get output

   DATA * output_vector = new DATA [output_dim];

   for(int r=0;r<data.rows();r++)
     {
     NumericVector v(data( r , _ ));                                   // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
     double * fp_v = REAL(v);                                          // my (lame?) way to interface with R, cont.)

     som.recall(fp_v, data.cols(), output_vector, output_dim);

     // now find which element in the output vector has the smallest value and use it as the winner id.
     returned_cluster_ids[r] = which_min(output_vector,output_dim);
     }

   delete [] output_vector;

 TEXTOUT << "LVQ returned " << unique(returned_cluster_ids).length() << " clusters with ids: " << unique(returned_cluster_ids) << "\n";
 return returned_cluster_ids;
}

//--------------------------------------------------------------------------------


#endif // NNLIB2_FOR_RCPP
