//		----------------------------------------------------------
//		(C)2019       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    Rcpp glue code for BP NN (bp_nn, supervised)
//		-----------------------------------------------------------

#include "nnlib2.h"
#include <iostream>
#include <fstream>

/* uncomment the section below if you need it. Here it is not used and can be deleted.
 //
 // use the BH package
 // [[Rcpp::depends(BH)]]
 //
 // select between boost or std shared_ptr
 // credit: https://www.implementingquantlib.com/2018/04/going-to-11-shared-ptr.html
#if defined(QL_USE_STD_POINTERS)
#include <memory>
 using std::shared_ptr;
 using std::make_shared;
#else
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
 using boost::shared_ptr;
 using boost::make_shared
 // use the BH package
 // [[Rcpp::depends(BH)]];
#endif
 */

#ifdef NNLIB2_FOR_RCPP
using namespace Rcpp;

//--------------------------------------------------------------------------------

#include "nn_bp.h"

using namespace nnlib2;
using namespace nnlib2::bp;

//--------------------------------------------------------------------------------

bp_nn *       p_bp = NULL;

//--------------------------------------------------------------------------------

// [[Rcpp::export]]
void BP_destroy()
{
  if(p_bp==NULL) {    error(NN_NULLPT_ERR,"No nn"); return; }
  delete p_bp;
  p_bp=NULL;
}

//--------------------------------------------------------------------------------

// [[Rcpp::export]]
bool BP_create  (int input_data_dim,
                 int output_dim,
                 double learning_rate,
                 int  num_hidden_layers,
                 int hidden_layer_size)
  {
  if (p_bp!=NULL) BP_destroy();
  p_bp = new bp_nn();
  if(p_bp==NULL) { error(NN_NULLPT_ERR,"No nn"); return false; }
  p_bp->setup(input_data_dim,output_dim,learning_rate,num_hidden_layers,hidden_layer_size);
  if(p_bp->no_error())
   {
   TEXTOUT << "BP NN created. Make sure it is deleted (by calling BP_destroy()) when no longer needed.\n";
   return true;
   }
  return false;
  }

//--------------------------------------------------------------------------------

// [[Rcpp::export]]
void BP_print()
{
if(p_bp==NULL)          {TEXTOUT << "No BP network to print.\n"; return;}
if(!p_bp->no_error())   {TEXTOUT << "NOTE: BP reports errors.\n";}
if(!p_bp->is_ready())   {TEXTOUT << "NOTE: BP reports not ready to use.\n";}
TEXTOUT << "\n------Network structure (BEGIN)--------\n";
p_bp->to_stream(TEXTOUT);
TEXTOUT << "--------Network structure (END)--------\n";
TEXTOUT << "\n";
}

//--------------------------------------------------------------------------------

// [[Rcpp::export]]
double BP_train_single (NumericVector data_in,
                        NumericVector data_out)
  {
    if(p_bp==NULL)          return DATA_MAX;
    if(!p_bp->no_error())   return DATA_MAX;
    if(!p_bp->is_ready())   return DATA_MAX;

    double *fpdata_in   = data_in.begin();             // (interface with R)
    double *fpdata_out  = data_out.begin();            // (interface with R)

    // Encode a case item pair (supervised)
    DATA error_level = p_bp->encode_s(fpdata_in,
                                      data_in.length(),
                                      fpdata_out,
                                      data_out.length()
                                      );
    return(error_level/data_out.length());
  }

//--------------------------------------------------------------------------------

// [[Rcpp::export]]
double BP_train_set (NumericMatrix dataset_in,
                     NumericMatrix dataset_out,
                     int training_epochs)
{
  if(p_bp==NULL) { error(NN_NULLPT_ERR,"No nn"); return DATA_MAX; }

  int num_training_cases=dataset_in.rows();

  if(!p_bp->no_error())   return DATA_MAX;
  if(!p_bp->is_ready())   return DATA_MAX;

  // encode all data input-output pairs
  DATA error_level = DATA_MAX;

  for(int i=0;i<training_epochs && p_bp->no_error();i++)
  {
   for(int r=0;r<num_training_cases;r++)
    {
      NumericVector v_in  = dataset_in( r , _ );         // (interface with R)
      NumericVector v_out = dataset_out( r , _ );        // (interface with R)
      // Encode a case item pair (supervised)
      error_level = BP_train_single (v_in,v_out);
    }
   if(i%1000==0)
    {
    TEXTOUT << "Epoch = "<< i << " , Error level indication = " << error_level << "\n";
    checkUserInterrupt();                           // (RCpp function to check if user pressed cancel)
    }
  }

  TEXTOUT << "Training Finished.\n";
  return(error_level);
}

//--------------------------------------------------------------------------------
// assumes training was preceded

// [[Rcpp::export]]
NumericMatrix BP_recall_set(NumericMatrix data_in)
{
  NumericMatrix data_out;

  if(p_bp==NULL) { error(NN_NULLPT_ERR,"No nn"); return data_out; }
  data_out= NumericMatrix(data_in.rows(),p_bp->output_dimension());

  for(int r=0;r<data_in.rows();r++)
  {
    NumericVector v_in(data_in( r , _ ));               // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
    double * fpdata_in = REAL(v_in);                    // my (lame?) way to interface with R, cont.)
    NumericVector v_out = data_out( r , _ );            // my (lame?) way to interface with R, cont.)
    double *fpdata_out = REAL(v_out);                   // my (lame?) way to interface with R, cont.)

    p_bp->recall(fpdata_in, data_in.cols(), fpdata_out, data_out.cols());

    data_out( r , _ ) = v_out;                          // my (lame?) way to interface with R. Copy result vector back to matrix. Remember, NumericMatrix stores data row-first, as R does.
    }

  return data_out;
}

//--------------------------------------------------------------------------------

// [[Rcpp::export]]
bool BP_save_to_file(std::string filename)
{
  if(p_bp==NULL)  return false;
  std::ofstream outfile;
  outfile.open(filename);
  p_bp->to_stream(outfile);
  outfile.close();
  TEXTOUT << "BP NN saved to file " << filename << "\n";
  return true;
}

//--------------------------------------------------------------------------------

// [[Rcpp::export]]
bool BP_load_from_file(std::string filename)
{
  if(p_bp!=NULL) BP_destroy();
  p_bp = new bp_nn();
  if(p_bp==NULL) return false;
  std::ifstream infile;
  infile.open(filename);
  if(!infile) {error(NN_IOFILE_ERR,"File cannot be opened");return false;}
  p_bp->from_stream(infile);
  infile.close();
  TEXTOUT << "BP NN loaded from file " << filename << "\n";
  return true;
}

//--------------------------------------------------------------------------------

#endif // NNLIB2_FOR_RCPP
