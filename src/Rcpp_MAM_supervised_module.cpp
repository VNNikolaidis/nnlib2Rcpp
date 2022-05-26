//		----------------------------------------------------------
//		(C)2019       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    Rcpp glue code for MAM NN (mam_nn, supervised)
//		-----------------------------------------------------------

#include "nnlib2.h"

#ifdef NNLIB2_FOR_RCPP

//--------------------------------------------------------------------------------

#include "nn_mam.h"
#include <iostream>
#include <fstream>

using namespace nnlib2;
using namespace nnlib2::mam;

//--------------------------------------------------------------------------------
// Note: below is MAM implemented as a Module
//--------------------------------------------------------------------------------
// R wrapper class:

class MAM
{
protected:

  mam_nn      mam;

public:

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  MAM()
  {
  TEXTOUT << "MAM created, now encode data (or load NN from file).\n";
  mam.reset();
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  void encode(NumericMatrix data_in,
              NumericMatrix data_out)
  {
    int num_train_items  = data_in.rows();
    if (num_train_items != data_out.rows())
     {
     TEXTOUT << "Cannot train, input and output datasets do not have matching number of rows.\n";
     return;
     }

    //  setup the MAM NN...

    mam.setup(data_in.cols(),data_out.cols());

    // ... and encode data:

    if(mam.is_ready())
    for(int r=0;r<num_train_items;r++)
    {
      NumericVector v1(data_in( r , _ ));                         // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
      double * fpdata_in = REAL(v1);                              // my (lame?) way to interface with R, cont.)
      NumericVector v2(data_out( r , _ ));                        // my (lame?) way to interface with R, cont.)
      double *fpdata_out = v2.begin();                            // my (lame?) way to interface with R, cont.)

      mam.encode_s(fpdata_in,data_in.cols(),fpdata_out,data_out.cols());
    }
  TEXTOUT << "Training Finished.\n";
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Encode a single input-output vector pair in current BP NN

  bool train_single (NumericVector data_in,
                     NumericVector data_out)
  {
    if(!mam.is_ready()) return false;

    double *fpdata_in   = data_in.begin();             // (interface with R)
    double *fpdata_out  = data_out.begin();            // (interface with R)

    // Encode a case item pair (supervised)
    mam.encode_s( fpdata_in,
                  data_in.length(),
                  fpdata_out,
                  data_out.length() );

    return mam.no_error();
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  NumericMatrix recall(NumericMatrix data)
  {
  NumericMatrix data_out;
  if(!mam.is_ready()) return data_out;

  int num_test_items  = data.rows();
  data_out = NumericMatrix(num_test_items,mam.output_dimension());

  for(int r=0;r<num_test_items;r++)
    {
      NumericVector v1(data( r , _ ));                          // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
      double * fpdata = REAL(v1);                               // my (lame?) way to interface with R, cont.)
      NumericVector v2(data_out( r , _ ));                      // my (lame?) way to interface with R, cont.)
      double *fpdata_out = v2.begin();                          // my (lame?) way to interface with R, cont.)

      mam.recall(fpdata,data.cols(),fpdata_out,data_out.cols());

      data_out( r , _ ) = v2;                                   // my (lame?) way to interface with R. Copy result vector back to matrix. Remember, NumericMatrix stores data row-first, as R does.
    }
  return (data_out);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  bool save_to_file(std::string filename)
  {
    std::ofstream outfile;
    outfile.open(filename);
    mam.to_stream(outfile);
    outfile.close();
    TEXTOUT << "MAM NN saved to file " << filename << "\n";
    return true;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  bool load_from_file(std::string filename)
  {
    std::ifstream infile;
    infile.open(filename);
    if(!infile) {error(NN_IOFILE_ERR,"File cannot be opened");return false;}
    mam.from_stream(infile);
    infile.close();
    TEXTOUT << "MAM NN loaded from file " << filename << "\n";
    return true;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  void print()
  {
    TEXTOUT << "------Network structure (BEGIN)--------\n";
    mam.to_stream(TEXTOUT);
    TEXTOUT << "--------Network structure (END)--------\n";
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  void show()
  {
	TEXTOUT << "Matrix Associative Memory NN (Class MAM):\n";
	print();
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

};

//--------------------------------------------------------------------------------

RCPP_MODULE(class_MAM) {
  class_<MAM>( "MAM" )
  .constructor()
  //.constructor<NumericMatrix,IntegerVector,int>()
  .method( "encode",      &MAM::encode,        "Encode input and corresponding output" )
  .method( "train_single",&MAM::train_single,  "Encode a single input-output vector pair in current MAM NN" )
  .method( "recall",      &MAM::recall,        "Get output for a dataset using MAM NN" )
  .method( "print",       &MAM::print,         "Print MAM NN details" )
  .method( "show",        &MAM::show,          "Print MAM NN details" )
  .method( "load",        &MAM::load_from_file,"Load MAM" )
  .method( "save",        &MAM::save_to_file,  "Save MAM" )
  ;
}

//--------------------------------------------------------------------------------

#endif // NNLIB2_FOR_RCPP
