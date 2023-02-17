//		----------------------------------------------------------
//		(C)2019       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    Rcpp glue code for BP NN (bp_nn, supervised)
//		-----------------------------------------------------------

#include "nnlib2.h"

#ifdef NNLIB2_FOR_RCPP

//--------------------------------------------------------------------------------

#include "nn_bp.h"
#include <iostream>
#include <fstream>

using namespace nnlib2;
using namespace nnlib2::bp;

//--------------------------------------------------------------------------------
// Note: below is BP implemented as a Module
//--------------------------------------------------------------------------------
// R wrapper class:

class BP
{
protected:

  bp_nn      bp;

  double m_acceptable_error_level;

public:

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  BP()
  {
  TEXTOUT << "BP NN created, now encode data (or load NN from file).\n";
  bp.reset();
  set_error_level("MSE",0);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Setup BP and encode input-output datasets in the NN

  void encode(NumericMatrix data_in,
              NumericMatrix data_out,
              double learning_rate,
              int training_epochs,
              int hidden_layers,
              int hidden_layer_size)
  {
    int input_dim  = data_in.cols();
    int output_dim = data_out.cols();

    if(setup(input_dim,output_dim,learning_rate,hidden_layers,hidden_layer_size))
     train_multiple(data_in,data_out,training_epochs);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Setup a new BP

  bool setup(int input_dim, int output_dim, double learning_rate, int hidden_layers, int hidden_layer_size)
  {
    if(!bp.setup(input_dim, output_dim, learning_rate, hidden_layers, hidden_layer_size))
      {
        error(NN_INTEGR_ERR,"Cannot setup BP NN");
        bp.reset();
        return false;
      }
    return true;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Encode multiple input-output vector pairs stored in corresponding datasets

  double train_multiple (NumericMatrix data_in,
                         NumericMatrix data_out,
                         int training_epochs)
  {
    if((data_in.rows()<=0) OR
         (data_in.rows()!=data_out.rows()))
    {
      error(NN_DATAST_ERR,"Cannot train BP with these datasets");
      return DATA_MAX;
    }

    int num_training_cases=data_in.rows();
    // encode all data input-output pairs
    DATA error_level = DATA_MAX;

    for(int i=0;i<training_epochs && bp.is_ready();i++)
    {

      DATA mean_error_for_dataset = 0;

      for(int r=0;r<num_training_cases;r++)
      {
        NumericVector v_in  = data_in( r , _ );         // (interface with R)
        NumericVector v_out = data_out( r , _ );        // (interface with R)
        // Encode a case item pair (supervised)
        error_level = train_single (v_in,v_out);

        mean_error_for_dataset = mean_error_for_dataset +  error_level;
      }

      mean_error_for_dataset = mean_error_for_dataset / num_training_cases;

      if(i%1000==0)
      {
        TEXTOUT << "Epoch = "<< i << " , Error level indication = " << mean_error_for_dataset << "\n";
        checkUserInterrupt();                           // (RCpp function to check if user pressed cancel)
      }

      if(mean_error_for_dataset<=m_acceptable_error_level)
      {
      	TEXTOUT << "Training reached acceptable error lever (" << mean_error_for_dataset  << ")\n";
      	break;
      }

    }

    TEXTOUT << "Training Finished, error level indicator is " << error_level << " .\n";
    return error_level;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Encode a single input-output vector pair in current BP NN

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

    return(error_level);								// note: was return(error_level/data_out.length());
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Get output for a dataset using BP NN

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

  bool set_error_level(std::string error_type, DATA acceptable_error_level)
  {
  	if(error_type=="MSE")
  	{
  		bp.m_use_squared_error = true;
  		TEXTOUT << "Note: Using and displaying Mean Squared Error (MSE), ";
  	}
  	else
  	{
  		bp.m_use_squared_error = false;
  		TEXTOUT << "Note: Using and displaying Mean Absolute Error (MAE), ";
  	}

  	m_acceptable_error_level = acceptable_error_level;
  	if(m_acceptable_error_level<0) m_acceptable_error_level=0;

  	TEXTOUT << "acceptable " << error_type << " = " << m_acceptable_error_level << "\n";

  return true;
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

  void show()
  {
	TEXTOUT << "Plain Backpropagation NN (Class BP):\n";
  	print();
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

};

//--------------------------------------------------------------------------------

RCPP_MODULE(class_BP) {
  class_<BP>( "BP" )
  .constructor()
//.constructor<NumericMatrix,NumericMatrix,double,int,int,int>()
  .method( "encode",          &BP::encode,          "Setup BP and encode input-output datasets in the NN" )
  .method( "train_multiple",  &BP::train_single,    "Encode multiple input-output vector pairs stored in corresponding datasets" )
  .method( "train_single",    &BP::train_single,    "Encode a single input-output vector pair in current BP NN" )
  .method( "setup",           &BP::setup,           "Setup the BP NN" )
  .method( "recall",          &BP::recall,          "Get output for a dataset using BP NN" )
  .method( "print",           &BP::print,           "Print BP NN details" )
  .method( "show",            &BP::show,            "Print BP NN details" )
  .method( "load",            &BP::load_from_file,  "Load BP" )
  .method( "save",            &BP::save_to_file,    "Save BP" )
  .method( "set_error_level" ,&BP::set_error_level, "Set parameters for acceptable error when training." )

  ;
}

//--------------------------------------------------------------------------------

#endif // NNLIB2_FOR_RCPP
