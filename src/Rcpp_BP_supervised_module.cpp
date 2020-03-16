//		----------------------------------------------------------
//		(C)2019       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    Rcpp glue code for BP NN (bp_nn, supervised)
//		-----------------------------------------------------------

#include "nnlib2.h"
#include <iostream>
#include <fstream>

#ifdef NNLIB2_FOR_RCPP
using namespace Rcpp;

//--------------------------------------------------------------------------------

#include "nn_bp.h"

using namespace nnlib2;
using namespace nnlib2::bp;

//--------------------------------------------------------------------------------
// Note: below is BP implemented as a Module
//--------------------------------------------------------------------------------
// R wrapper class:

class BP_NN
{
protected:

  bp_nn      bp;

public:

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  BP_NN()
  {
  TEXTOUT << "BP NN created, now encode data (or load NN from file).\n";
  bp.reset();
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  void encode(NumericMatrix tr_dataset_in,
         NumericMatrix tr_dataset_out,
         double learning_rate,
         int training_epochs,
         int hidden_layers,
         int hidden_layer_size)
  {
    if((tr_dataset_in.rows()<=0) OR
         (tr_dataset_in.rows()!=tr_dataset_out.rows()))
    {
      error(NN_DATAST_ERR,"Cannot setup BP for these datasets");
      return;
    }

    int input_data_dim = tr_dataset_in.cols();
    int output_dim = tr_dataset_out.cols();

    if(!bp.setup(input_data_dim,output_dim,learning_rate,hidden_layers,hidden_layer_size))
    {
      error(NN_INTEGR_ERR,"Cannot setup BP NN");
      bp.reset();
      return;
    }

    int num_training_cases=tr_dataset_in.rows();

    // encode all data input-output pairs
    DATA error_level = DATA_MAX;

    for(int i=0;i<training_epochs && bp.no_error();i++)
    {
      for(int r=0;r<num_training_cases;r++)
      {
        NumericVector v_in  = tr_dataset_in( r , _ );         // (interface with R)
        NumericVector v_out = tr_dataset_out( r , _ );        // (interface with R)
        // Encode a case item pair (supervised)
        error_level = train_single (v_in,v_out);
      }
      if(i%1000==0)
      {
        TEXTOUT << "Epoch = "<< i << " , Error level indication = " << error_level << "\n";
        checkUserInterrupt();                           // (RCpp function to check if user pressed cancel)
      }
    }

    TEXTOUT << "Training Finished, error level indicator is " << error_level << " .\n";
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


  double train_single (NumericVector data_in,
                       NumericVector data_out)
  {
    if(!bp.is_ready())   return DATA_MAX;

    double *fpdata_in   = data_in.begin();             // (interface with R)
    double *fpdata_out  = data_out.begin();            // (interface with R)

    // Encode a case item pair (supervised)
    DATA error_level = bp.encode_s( fpdata_in,
                                    data_in.length(),
                                    fpdata_out,
                                    data_out.length() );
    return(error_level/data_out.length());
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  NumericMatrix recall(NumericMatrix data_in)
  {
    NumericMatrix data_out;

    data_out= NumericMatrix(data_in.rows(),bp.output_dimension());

    for(int r=0;r<data_in.rows();r++)
    {
      NumericVector v_in(data_in( r , _ ));               // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
      double * fpdata_in = REAL(v_in);                    // my (lame?) way to interface with R, cont.)
      NumericVector v_out = data_out( r , _ );            // my (lame?) way to interface with R, cont.)
      double *fpdata_out = REAL(v_out);                   // my (lame?) way to interface with R, cont.)

      bp.recall(fpdata_in, data_in.cols(), fpdata_out, data_out.cols());

      data_out( r , _ ) = v_out;                          // my (lame?) way to interface with R. Copy result vector back to matrix. Remember, NumericMatrix stores data row-first, as R does.
    }

    return data_out;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  bool save_to_file(std::string filename)
  {
    std::ofstream outfile;
    outfile.open(filename);
    bp.to_stream(outfile);
    outfile.close();
    TEXTOUT << "BP NN saved to file " << filename << "\n";
    return true;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  bool load_from_file(std::string filename)
  {
    std::ifstream infile;
    infile.open(filename);
    if(!infile) {error(NN_IOFILE_ERR,"File cannot be opened");return false;}
    bp.from_stream(infile);
    infile.close();
    TEXTOUT << "BP NN loaded from file " << filename << "\n";
    return true;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  void print()
  {
    TEXTOUT << "------Network structure (BEGIN)--------\n";
    bp.to_stream(TEXTOUT);
    TEXTOUT << "--------Network structure (END)--------\n";
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

};

//--------------------------------------------------------------------------------

RCPP_MODULE(class_BP_NN) {
  class_<BP_NN>( "BP_NN" )
  .constructor()
//.constructor<NumericMatrix,NumericMatrix,double,int,int,int>()
  .method( "encode",      &BP_NN::encode,         "Setup BP and encode input-output datasets in the NN" )
  .method( "recall",      &BP_NN::recall,         "Get output for a dataset using BP NN" )
  .method( "train_single",&BP_NN::train_single,   "Encode a single input-output vector pair in current BP NN" )
  .method( "print",       &BP_NN::print,          "Print BP NN details" )
  .method( "load",        &BP_NN::load_from_file, "Load BP" )
  .method( "save",        &BP_NN::save_to_file,   "Save BP" )
  ;
}

//--------------------------------------------------------------------------------

#endif // NNLIB2_FOR_RCPP
