//		----------------------------------------------------------
//		(C)2019       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    Rcpp glue code for LVQ NN (lvq_nn, supervised)
//		-----------------------------------------------------------

#include "nnlib2.h"

#ifdef NNLIB2_FOR_RCPP
using namespace Rcpp;

//--------------------------------------------------------------------------------

#include "nn_lvq.h"                          // for som_nn
#include "nnlib2_misc.h"                     // for which_max etc.

using namespace nnlib2;
using namespace nnlib2::lvq;

//--------------------------------------------------------------------------------

// [[Rcpp::export]]
IntegerVector LVQ (  NumericMatrix data,
                     IntegerVector desired_cluster_ids,              // 0 to max cluster id
                     int number_of_training_epochs,
                     NumericMatrix test_data,
                     bool show_nn = false)
   {
   IntegerVector returned_cluster_ids = rep(-1,test_data.rows());

   int train_data_dim  = data.cols();
   int test_data_dim   = test_data.cols();

   int output_dim = max(desired_cluster_ids)+1;                      // assumes cluster indexing starts with 0

   TEXTOUT << "LVQ will be trained for " << output_dim << " classes.\n";

   lvq_nn lvq;                                   					      // A Learning Vector Quantizer NN

   if( lvq.no_error())   lvq.setup(train_data_dim,output_dim);
   if(!lvq.no_error())   return(returned_cluster_ids);

   // encode all data

   for(int i=0;i<number_of_training_epochs;i++)
   {
   for(int r=0;r<data.rows();r++)
      {
        NumericVector v(data( r , _ ));                              // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
        double * fpdata = REAL(v);                                   // my (lame?) way to interface with R, cont.)
        int desired_class_for_data = desired_cluster_ids.at(r);

        lvq.encode_s(fpdata,data.cols(),desired_class_for_data,i);   // Encode supervised
      }
   checkUserInterrupt();                                             // (RCpp function to check if user pressed cancel)
   }

   if(show_nn)
   {
      TEXTOUT << "------Network structure (BEGIN)--------\n";
      lvq.to_stream(TEXTOUT);
      TEXTOUT << "--------Network structure (END)--------\n";
   }

    // training completed, now recall the test_data and get output

    if(train_data_dim != test_data_dim)
    {
       TEXTOUT << "Number of variables (columns) differs from trained data, cannot apply LVQ to this test_data\n";
       return returned_cluster_ids;
    }

   DATA * output_vector = new DATA [output_dim];

   for(int r=0;r<test_data.rows();r++)
     {
     NumericVector v(test_data( r , _ ));                            // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
     double * fpdata = REAL(v);                                      // my (lame?) way to interface with R, cont.)

     lvq.recall(fpdata, test_data.cols(), output_vector, output_dim);

     // find which element in the output vector has the smallest value and use it as the winner id.
     returned_cluster_ids[r] = which_min(output_vector,output_dim);
     }

   delete [] output_vector;

   TEXTOUT << "Lvq returned " << unique(returned_cluster_ids).length() << " clusters with ids: " << unique(returned_cluster_ids) << "\n";

   return returned_cluster_ids;
   }

//--------------------------------------------------------------------------------

#endif // NNLIB2_FOR_RCPP
