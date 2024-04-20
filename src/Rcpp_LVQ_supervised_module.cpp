//		----------------------------------------------------------
//		(C)2019       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    Rcpp glue code for LVQ NN (lvq_nn, supervised)
//		-----------------------------------------------------------

#include "nnlib2.h"

#ifdef NNLIB2_FOR_RCPP

//--------------------------------------------------------------------------------

#include "nn_lvq.h"
#include "nnlib2_misc.h"                     // for which_max etc.
#include <iostream>
#include <fstream>

using namespace nnlib2;
using namespace nnlib2::lvq;

//--------------------------------------------------------------------------------
// Note: below is LVQ implemented as a Module
//--------------------------------------------------------------------------------
// R wrapper class:

class LVQs
{
protected:

  lvq_nn      lvq;

public:

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  LVQs()
  {
  TEXTOUT << "LVQ created, now encode data (or load NN from file).\n";
  lvq.reset();
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // set number of output PEs (nodes) per class

  int set_number_of_nodes_per_class(int n)
  {
  	if(lvq.is_ready())
  	{
  	if(lvq.get_number_of_output_nodes_per_class()!=n)
  		warning("LVQ is already set up. Changing number of nodes per class may lead to erroneous classifications");
  	else
  		{
  		TEXTOUT << "LVQ is already set up for this number of nodes per class\n";
  		return n;
  		}
  	}

  	if(lvq.get_number_of_output_nodes_per_class()!=n)
  	{
  		lvq.set_number_of_output_nodes_per_class(n);
  		TEXTOUT << "LVQ will use " << lvq.get_number_of_output_nodes_per_class()  << " output node(s) per class when encoding or recalling data.\n";
  	}

    return lvq.get_number_of_output_nodes_per_class();
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // set number of output PEs (nodes) per class

  int get_number_of_nodes_per_class()
  {
  	return lvq.get_number_of_output_nodes_per_class();
  }

 // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


 bool set_weight_limits(double min, double max)
 {

 	if(lvq.set_weight_limits(min,max))
 	{
 	TEXTOUT << "LVQ will limit connection weights to [" << min << "," << max << "] when encoding data.\n";
 	return true;
 	}

	return false;
 }

 // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 bool set_encoding_coefficients(double reward, double punish)
	{
	return lvq.set_encoding_coefficients(reward, punish);
	}

 // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 // set number of output PEs (nodes) per class

 bool enable_punishment()
	{
		TEXTOUT << "LVQ will notify winner nodes with incorrect classification when encoding data.\n";
		lvq.punish_enable(TRUE);
		return lvq.punish_enabled();
	}

 bool disable_punishment()
	{
		TEXTOUT << "LVQ will NOT notify winner nodes with incorrect classification when encoding data.\n";
		lvq.punish_enable(FALSE);
		return lvq.punish_enabled();
	}

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Setup an lvq for future use

  bool setup(int input_length, int number_of_classes)
  {
  	return setup_extended(input_length,number_of_classes,lvq.get_number_of_output_nodes_per_class());
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Setup an lvq for future use, also sets number of nodes per class

  bool setup_extended(int input_length, int number_of_classes, int number_of_nodes_per_class)
	{
  	if(lvq.is_ready())
  		{
  		TEXTOUT << "Note: Current LVQ is reset.\n";
  		lvq.reset();
  		}

  	if(set_number_of_nodes_per_class(number_of_nodes_per_class)!=number_of_nodes_per_class)
  		return false;

  	if(!lvq.setup(input_length,number_of_classes))
  	{
  		error(NN_INTEGR_ERR,"Cannot setup LVQ NN");
  		lvq.reset();
  		return false;
  	}

	return lvq.is_ready();;
	}

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // recommended cluster ids should be in 0 to n-1 range (n the number of clusters)

  void encode(NumericMatrix data,IntegerVector desired_class_ids,int training_epochs)
  {
  	if(training_epochs<0)
  	{
  		training_epochs = 0;
  		warning("Number of epochs set to 0");
  	}

  	if(training_epochs>LVQ_MAXITERATION)
  	{
  		training_epochs = LVQ_MAXITERATION;
  		warning("Number of epochs set to maximum allowed");

  	}

    int min_class_id = min(desired_class_ids);
    int max_class_id = max(desired_class_ids);

    int input_data_dim = data.cols();
    int output_dim = max_class_id+1;	// assumes cluster indexing starts with 0

    if((data.rows()<=0) OR
       (data.rows()!=desired_class_ids.size()))
    {
      error(NN_DATAST_ERR,"Cannot encode data on LVQ using these datasets");
      return;
    }

    if((min_class_id<0) OR (min_class_id>max_class_id) OR (output_dim<1))
    {
      error(NN_DATAST_ERR,"Cannot encode data on LVQ using these classes");
      return;
    }


    if(lvq.is_ready() &&
       (lvq.input_length() == input_data_dim) &&
       ((int)(lvq.output_length()/lvq.get_number_of_output_nodes_per_class()) == output_dim))
    {
     	TEXTOUT << "Encoding will be performed on existing LVQ.\n";
    }
    else
    {
    	TEXTOUT << "Setting up LVQ for 0 to " << max_class_id << " ids (" << output_dim << " classes). \n";
    	if(!lvq.setup(input_data_dim,output_dim))
    	{
    		error(NN_INTEGR_ERR,"Cannot setup LVQ NN");
    		lvq.reset();
    		return;
    	}
    }

    if(!lvq.no_error()) return;

    if(lvq.get_reward_coefficient() != 0.2)
    TEXTOUT << "LVQ reward coefficient = " << lvq.get_reward_coefficient() << " .\n";

    if(lvq.punish_enabled())
    {
     if(lvq.get_punish_coefficient() != -0.2)
      TEXTOUT << "LVQ punishment coefficient = " << lvq.get_punish_coefficient() << " .\n";
    }
    else
      TEXTOUT << "LVQ punishment disabled.\n";

    TEXTOUT << "Training LVQ to encode " << output_dim << " classes...\n";

    // encode all data

    for(int i=0;i<training_epochs;i++)
    {
      for(int r=0;r<data.rows();r++)
      {
        NumericVector v(data( r , _ ));								// my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
        double * fpdata = REAL(v);									// my (lame?) way to interface with R, cont.)
        int desired_class_for_data = desired_class_ids.at(r);

        lvq.encode_s(fpdata,data.cols(),desired_class_for_data,i);	// Encode supervised
      }
      checkUserInterrupt();											// (RCpp function to check if user pressed cancel)
    }

    TEXTOUT << "Training Finished.\n";
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  bool train_single (NumericVector data_in,
                       int desired_class_id,
                       int epoch)
	{
		double *fpdata_in   = data_in.begin();             // (interface with R)

		// Encode a case (supervised)

		return (lvq.encode_s( fpdata_in,
                             data_in.length(),
                             desired_class_id,
                             epoch ) != 1 );			  // here 0 indicates no error (success), 1 failure

	}

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  IntegerVector recall(NumericMatrix data_in)
  {
  	return recall_rewarded(data_in,0);
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  IntegerVector recall_rewarded (NumericMatrix data_in, int minimum_number_of_rewards)
  {
    IntegerVector returned_cluster_ids = rep(-1,data_in.rows());

    if(!lvq.is_ready()) return returned_cluster_ids;

    if(lvq.input_dimension() != data_in.cols())
    {
      TEXTOUT << "Number of variables (columns) differs from trained data, cannot apply LVQ to this data_in\n";
      return returned_cluster_ids;
    }

    for(int r=0;r<data_in.rows();r++)
    {
      NumericVector v(data_in( r , _ ));                              // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
      double * fpdata = REAL(v);                                      // my (lame?) way to interface with R, cont.)

      returned_cluster_ids[r] = lvq.recall_class(fpdata, data_in.cols(), minimum_number_of_rewards);
    }

    TEXTOUT << "Lvq returned " << unique(returned_cluster_ids).length() << " classes with ids: " << unique(returned_cluster_ids) << "\n";

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
  // Get weights (connection variable)

  NumericVector get_weights()
  {
  	// using R numbering, 1st is input layer, 2nd connections, 3rd output layer:
  	int pos = 2;

	NumericVector data_out;

	if(lvq.number_of_components_in_topology()!=3)
		{
		warning("The LVQ topology has not been defined yet.");
		return data_out;
		}

  	component PTR pc;
  	pc = lvq.component_from_topology_index(pos-1);
  	if(pc==NULL) return data_out;
  	if(pc->type()!=cmpnt_connection_set)
  	{
  		warning("Not a connection set.");
  		return data_out;
  	}

  	int num_items = pc->size();
  	if(num_items>0)
  	{
  		data_out= NumericVector(num_items);
  		double * fpdata_out = REAL(data_out);                   // my (lame?) way to interface with R, cont.)
  		if(NOT lvq.get_weights_at_component(pos-1,fpdata_out, num_items))
  			warning("Cannot retreive weights from specified component");
  	}

  	return data_out;
  }

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Set weight (connection variable) for connections in given connection set (R to Cpp index converted)

	bool set_weights(NumericVector data_in)
	{
		if(lvq.number_of_components_in_topology()!=3)
		{
			warning("The LVQ topology has not been defined yet.");
			return false;
		}

		// using R numbering, 1st is input layer, 2nd connections, 3rd output layer:
		int pos = 2;

		double * fpdata_in  = REAL(data_in);                    // my (lame?) way to interface with R, cont.)
		if(!lvq.set_weights_at_component(pos-1,fpdata_in,data_in.length()))
		{
			error(NN_INTEGR_ERR,"Cannot change weights at specified NN component, incompatible type or sizes");
			return false;
		}
		return true;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Get weights (connection variable)

	NumericVector get_number_of_rewards()
	{
	// using R numbering, 1st is input layer, 2nd connections, 3rd output layer:
	int pos = 3;

	NumericVector data_out;

	if(lvq.number_of_components_in_topology()!=3)
	{
		warning("The LVQ topology has not been defined yet.");
		return data_out;
	}

	component PTR pc;
	pc = lvq.component_from_topology_index(pos-1);
	if(pc==NULL) return data_out;
	if(pc->type()!=cmpnt_layer)
	{
		warning("Not a layer.");
		return data_out;
	}

	int num_items = pc->size();
	if(num_items>0)
	{
		data_out= NumericVector(num_items);
		double * fpdata_out = REAL(data_out);                   // my (lame?) way to interface with R, cont.)
		if(NOT lvq.get_misc_at_component(pos-1,fpdata_out, num_items))
			warning("Cannot retreive misc values from specified component");
	}

	return data_out;
	}

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  void print()
  {
    TEXTOUT << "------Network structure (BEGIN)--------\n";
    lvq.to_stream(TEXTOUT);
    TEXTOUT << "--------Network structure (END)--------\n";
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  void show()
  {
    TEXTOUT << "Learning Vector Quantizer NN (Class LVQs):\n";
    print();
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

};

//--------------------------------------------------------------------------------

RCPP_MODULE(class_LVQs) {
  class_<LVQs>( "LVQs" )
  .constructor()
  //.constructor<NumericMatrix,IntegerVector,int>()
  .method( "setup",				 (bool (LVQs::*)(int,int))&LVQs::setup,					"Setup an untrained supervised LVQ for given input data vector dimensions and number of classes" )
  .method( "setup",				 (bool (LVQs::*)(int,int,int))&LVQs::setup_extended,	"Setup an untrained supervised LVQ for given input data vector dimensions and number of classes" )
  .method( "encode",    						&LVQs::encode,							"Encode input and output (classification) for a dataset using LVQ NN" )
  .method( "recall", (IntegerVector (LVQs::*)(NumericMatrix))&LVQs::recall,				"Get output (classification) for a dataset using LVQ NN" )
  .method( "recall", (IntegerVector (LVQs::*)(NumericMatrix,int))&LVQs::recall_rewarded,"Get output (classification) for a dataset using LVQ NN" )
  .method( "print",     						&LVQs::print,							"Print LVQ NN details" )
  .method( "show",      						&LVQs::show,							"Print LVQ NN details" )
  .method( "load",  						 	&LVQs::load_from_file,					"Load LVQ" )
  .method( "save",      						&LVQs::save_to_file,					"Save LVQ" )
  .method( "get_weights",						&LVQs::get_weights,						"Get current weight values" )
  .method( "set_weights",						&LVQs::set_weights,						"Set current weight values" )
  .method( "set_number_of_nodes_per_class",		&LVQs::set_number_of_nodes_per_class,	"Set number of output PEs to be used per class" )
  .method( "get_number_of_nodes_per_class",		&LVQs::get_number_of_nodes_per_class,	"Get number of output PEs to be used per class" )
  .method( "get_number_of_rewards",				&LVQs::get_number_of_rewards,			"Get number of times each output PE was positively reinforced during encoding" )
  .method( "enable_punishment",					&LVQs::enable_punishment,				"During encoding incorrect winner nodes will be notified" )
  .method( "disable_punishment",				&LVQs::disable_punishment,				"During encoding incorrect winner nodes will not be notified" )
  .method( "set_weight_limits",					&LVQs::set_weight_limits,				"Define minimum and maximum values allowed in weights" )
  .method( "set_encoding_coefficients",			&LVQs::set_encoding_coefficients,		"Define coefficients used for reward and punishment" )
  .method( "train_single",  					&LVQs::train_single,    				"Encode a single case in current LVQ NN" )
  ;
}

//--------------------------------------------------------------------------------

#endif // NNLIB2_FOR_RCPP
