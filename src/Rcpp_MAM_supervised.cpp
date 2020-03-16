//		----------------------------------------------------------
//		(C)2019       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    Rcpp glue code for MAM NN (mam_nn,supervised)
//		-----------------------------------------------------------

#include "nnlib2.h"

#ifdef NNLIB2_FOR_RCPP
using namespace Rcpp;

//--------------------------------------------------------------------------------

#include "nn_mam.h"                       // for mam_nn
#include "nnlib2_misc.h"                  // for which_max

using namespace nnlib2;
using namespace nnlib2::mam;

//--------------------------------------------------------------------------------
//	Note: to present MAM as example in the accompaning paper, several functionaly-
// equivalent MAM variants (implementations) were created (see MAM_VARIANT)

// [[Rcpp::export]]
NumericMatrix MAM (NumericMatrix train_data_in,
                   NumericMatrix train_data_out,
                   NumericMatrix test_data_in,
                   bool show_nn)
   {
   NumericMatrix test_data_out;

   int num_train_items  = train_data_in.rows();
   if (num_train_items != train_data_out.rows()) return test_data_out;

   MAM_VARIANT mam (train_data_in.cols(),train_data_out.cols());        // A MAM NN

// if( mam.no_error())   mam.setup();

   for(int r=0;r<num_train_items;r++)
      {
      NumericVector v1(train_data_in( r , _ ));                         // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
      double * fptrain_data_in = REAL(v1);                              // my (lame?) way to interface with R, cont.)
      NumericVector v2(train_data_out( r , _ ));                        // my (lame?) way to interface with R, cont.)
      double *fptrain_data_out = v2.begin();                            // my (lame?) way to interface with R, cont.)

      mam.encode_s(fptrain_data_in,train_data_in.cols(),fptrain_data_out,train_data_out.cols());
      }

   if(show_nn)
   {
      TEXTOUT << "------Network structure (BEGIN)--------\n";
      mam.to_stream(TEXTOUT);
      TEXTOUT << "--------Network structure (END)--------\n";
   }

   int num_test_items  = test_data_in.rows();
//   test_data_out = NumericMatrix(num_test_items,train_data_out.cols());
   test_data_out = NumericMatrix(num_test_items,mam.output_dimension());

   for(int r=0;r<num_test_items;r++)
      {
      NumericVector v1(test_data_in( r , _ ));                          // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
      double * fptest_data_in = REAL(v1);                               // my (lame?) way to interface with R, cont.)
      NumericVector v2(test_data_out( r , _ ));                         // my (lame?) way to interface with R, cont.)
      double *fptest_data_out = v2.begin();                             // my (lame?) way to interface with R, cont.)

      mam.recall(fptest_data_in,test_data_in.cols(),fptest_data_out,test_data_out.cols());

      test_data_out( r , _ ) = v2;                                      // my (lame?) way to interface with R. Copy result vector back to matrix. Remember, NumericMatrix stores data row-first, as R does.
      }

   return (test_data_out);
   }

//--------------------------------------------------------------------------------


#endif // NNLIB2_FOR_RCPP
