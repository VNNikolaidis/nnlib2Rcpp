//		----------------------------------------------------------
//		(C)2019       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    Rcpp glue code for LVQ NN (lvq_nn, supervised)
//		-----------------------------------------------------------

#include "nnlib2.h"
#include <iostream>
#include <fstream>

#ifdef NNLIB2_FOR_RCPP
using namespace Rcpp;

//--------------------------------------------------------------------------------

#include "nn_lvq.h"
#include "nnlib2_misc.h"                     // for which_max etc.

using namespace nnlib2;
using namespace nnlib2::lvq;

//--------------------------------------------------------------------------------
// Note: below is LVQ implemented as a Module
//--------------------------------------------------------------------------------
// R wrapper class:

class LVQs_NN
{
protected:

  lvq_nn      lvq;

public:

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  LVQs_NN()
  {
  TEXTOUT << "LVQ created, now encode data (or load NN from file).\n";
  lvq.reset();
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // recommended cluster ids should be in 0 to n range.

  void encode(NumericMatrix data,IntegerVector desired_class_ids,int training_epochs)
  {
    int min_class_id = min(desired_class_ids);
    int max_class_id = max(desired_class_ids);
    int input_data_dim = data.cols();
    int output_dim = max_class_id+1;                      // assumes cluster indexing starts with 0

    if((data.rows()<=0) OR
       (data.rows()!=desired_class_ids.size()))
    {
      error(NN_DATAST_ERR,"Cannot setup LVQ for these datasets");
      return;
    }

    if((min_class_id<0) OR (min_class_id>max_class_id) OR (output_dim<1))
    {
      error(NN_DATAST_ERR,"Cannot setup LVQ for these classes");
      return;
    }

    TEXTOUT << "Setting up LVQ for 0 to " << max_class_id << " ids (" << output_dim << " classes).\n";

    if(!lvq.setup(input_data_dim,output_dim))
    {
      error(NN_INTEGR_ERR,"Cannot setup LVQ NN");
      lvq.reset();
      return;
    }

    if(!lvq.no_error()) return;

    TEXTOUT << "LVQ will be trained for " << output_dim << " classes.\n";

    // encode all data

    for(int i=0;i<training_epochs;i++)
    {
      for(int r=0;r<data.rows();r++)
      {
        NumericVector v(data( r , _ ));                              // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
        double * fpdata = REAL(v);                                   // my (lame?) way to interface with R, cont.)
        int desired_class_for_data = desired_class_ids.at(r);

        lvq.encode_s(fpdata,data.cols(),desired_class_for_data,i);   // Encode supervised
      }
      checkUserInterrupt();                                            // (RCpp function to check if user pressed cancel)
    }

    TEXTOUT << "Training Finished.\n";
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  IntegerVector recall(NumericMatrix data_in)
  {
    IntegerVector returned_cluster_ids = rep(-1,data_in.rows());

    if(!lvq.is_ready()) return returned_cluster_ids;

    if(lvq.input_dimension() != data_in.cols())
    {
      TEXTOUT << "Number of variables (columns) differs from trained data, cannot apply LVQ to this data_in\n";
      return returned_cluster_ids;
    }

    int output_dim = lvq.output_dimension();
    DATA * output_vector = new DATA [output_dim];

    for(int r=0;r<data_in.rows();r++)
    {
      NumericVector v(data_in( r , _ ));                              // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
      double * fpdata = REAL(v);                                      // my (lame?) way to interface with R, cont.)

      lvq.recall(fpdata, data_in.cols(), output_vector, output_dim);

      // find which element in the output vector has the smallest value and use it as the winner id.
      returned_cluster_ids[r] = which_min(output_vector,output_dim);
    }

    delete [] output_vector;

    TEXTOUT << "Lvq returned " << unique(returned_cluster_ids).length() << " clusters with ids: " << unique(returned_cluster_ids) << "\n";

    return returned_cluster_ids;
    }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  bool save_to_file(std::string filename)
  {
    std::ofstream outfile;
    outfile.open(filename);
    lvq.to_stream(outfile);
    outfile.close();
    TEXTOUT << "LVQ NN saved to file " << filename << "\n";
    return true;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  bool load_from_file(std::string filename)
  {
    std::ifstream infile;
    infile.open(filename);
    if(!infile) {error(NN_IOFILE_ERR,"File cannot be opened");return false;}
    lvq.from_stream(infile);
    infile.close();
    TEXTOUT << "LVQ NN loaded from file " << filename << "\n";
    return true;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  void print()
  {
    TEXTOUT << "------Network structure (BEGIN)--------\n";
    lvq.to_stream(TEXTOUT);
    TEXTOUT << "--------Network structure (END)--------\n";
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

};

//--------------------------------------------------------------------------------

RCPP_MODULE(class_LVQs_NN) {
  class_<LVQs_NN>( "LVQs_NN" )
  .constructor()
  //.constructor<NumericMatrix,IntegerVector,int>()
  .method( "encode",    &LVQs_NN::encode,        "Encode input and output (classification) for a dataset using LVQ NN" )
  .method( "recall",    &LVQs_NN::recall,        "Get output (classification) for a dataset using LVQ NN" )
  .method( "print",     &LVQs_NN::print,         "Print LVQ NN details" )
  .method( "load",      &LVQs_NN::load_from_file,"Load LVQ" )
  .method( "save",      &LVQs_NN::save_to_file,  "Save LVQ" )
  ;
}

//--------------------------------------------------------------------------------

#endif // NNLIB2_FOR_RCPP
