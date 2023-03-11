//		----------------------------------------------------------
//		(C)2020       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    	Rcpp glue code for building custom NNs
//		-----------------------------------------------------------

#include "nnlib2.h"

#ifdef NNLIB2_FOR_RCPP

#include "nn.h"
#include <iostream>
#include <fstream>

#include "nn_lvq.h"
#include "nn_bp.h"
#include "nn_mam.h"

#include "Rcpp_R_layer.h"
#include "Rcpp_R_connection_matrix.h"
#include "Rcpp_R_aux_control.h"

#include "spare_parts.h"
#include "additional_parts.h"		// header for user-defined parts (components etc)

using namespace nnlib2;

class NN
{
protected:

	nn    m_nn;					// the internal NN

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// generate layer for further use later (note: name is also used as type selector)

	layer PTR generate_layer(List parameters)
	{
		// extract incoming parameters from list:

		string name = parameters["name"];
		int size	= parameters["size"];
		DATA optional_parameter = DATA_MIN;

		if(parameters.containsElementNamed("optional_parameter"))
			optional_parameter = parameters["optional_parameter"];

		// create the new layer (NN components already defined in nnlib2 are shown below):

		if( name == "pe" ) 				return new pe_layer(name,size);
		if( name == "generic_d" ) 		return new pe_layer(name,size);
		if( name == "generic" )  		return new Layer<pe>(name,size);

		if( name == "pass-through" )	return new pass_through_layer(name,size);

		if( name == "which-max")       	return new which_max_layer(name,size);

		if( name == "MAM" )				return new mam::mam_layer(name,size);

		if( name == "LVQ-input" )		{
			lvq::lvq_input_layer PTR pl = new lvq::lvq_input_layer;
			pl->setup(name,size);
			return pl;
		}

		if( name == "LVQ-output" )		{
			lvq::lvq_output_layer PTR pl = new lvq::lvq_output_layer;
			pl->setup(name,size,1);
			return pl;
		}

		if( name == "BP-hidden" )		{
			bp::bp_comput_layer PTR pl = new bp::bp_comput_layer;
			pl->setup(name,size);
			pl->randomize_biases(-1,1);
			DATA bp_learnrate = 0.6;
			if(optional_parameter!=DATA_MIN)
				bp_learnrate = optional_parameter;
			pl->set_learning_rate(bp_learnrate);
			TEXTOUT << "(This " << name << " layer uses learning rate = " << bp_learnrate << ")\n";
			return pl;
		}

		if( name == "BP-output" )		{
			bp::bp_output_layer PTR pl = new bp::bp_output_layer;
			pl->setup(name,size);
			pl->randomize_biases(-1,1);
			DATA bp_learnrate = 0.6;
			if(optional_parameter!=DATA_MIN)
				bp_learnrate = optional_parameter;
			pl->set_learning_rate(bp_learnrate);
			TEXTOUT << "(This " << name << " layer uses learning rate = " << bp_learnrate << ")\n";
			return pl;
		}

		if( name == "R-layer" )
		{
			string encode_FUN = parameters["encode_FUN"];
			string recall_FUN = parameters["recall_FUN"];
			R_layer PTR pl = new R_layer(name,size,encode_FUN,recall_FUN);
			return pl;
		}


		layer PTR pl = generate_custom_layer(parameters);
		if(pl != NULL) return pl;

		warning("Unknown layer type");
		return NULL;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// generate connection set for further use later (note: name is also used as type)

	connection_set PTR generate_connection_set(List parameters)
	{
		// extract incoming parameters from list:

		string name = parameters["name"];
		DATA optional_parameter = DATA_MIN;

		if(parameters.containsElementNamed("optional_parameter"))
			optional_parameter = parameters["optional_parameter"];

		// create the new connection set (NN components already defined in nnlib2 are shown below):

		if( name == "generic" )			return new Connection_Set<connection>(name);

		if( name == "pass-through" )	return new Connection_Set<pass_through_connection>(name);

		if( name == "wpass-through" )	return new Connection_Set<weighted_pass_through_connection>(name);

		if( name == "MAM" )				return new mam::mam_connection_set(name);

		if( name == "LVQ")
		{
			lvq::lvq_connection_set PTR pc = new lvq::lvq_connection_set;
			if(pc!=NULL)
			{
				DATA LVQ_iteration = 100;
				if(optional_parameter!=DATA_MIN)
					LVQ_iteration = optional_parameter;
				pc->set_iteration_number(LVQ_iteration);
				TEXTOUT << "(This " << name << " connection set uses iteration parameter = " << LVQ_iteration << ")\n";
				pc->name() = name;
			}
			return pc;
		}

		if( name == "BP" )
		{
			bp::bp_connection_set PTR pc = new bp::bp_connection_set;
			if(pc!=NULL)
			{
				pc->name() = name;
				DATA bp_learnrate = 0.6;
				if(optional_parameter!=DATA_MIN)
					bp_learnrate = optional_parameter;
				pc->set_learning_rate(bp_learnrate);
				TEXTOUT << "(This " << name << " connection set uses learning rate = " << bp_learnrate << ")\n";
			}
			return pc;
		}

		if( name == "R-connections" )
		{
			string encode_FUN = parameters["encode_FUN"];
			string recall_FUN = parameters["recall_FUN"];
			bool requires_misc = false;
			if(parameters.containsElementNamed("requires_misc"))
				requires_misc = parameters["requires_misc"];
			R_connection_matrix PTR pc = new R_connection_matrix(name,encode_FUN,recall_FUN,requires_misc);
			return pc;
		}

		connection_set PTR pc = generate_custom_connection_set(parameters);
		if(pc!=NULL) return pc;

		warning("Unknown connection set type");
		return NULL;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Connect two layers (note: connection_set name is also used as type)

	bool add_connection_set_for(int source_pos,
                             int destin_pos,
                             List parameters,
                             bool fully_connect,
                             DATA min_random_weight,
                             DATA max_random_weight)
	{
		// extract incoming name from list:

		string connection_set_name = parameters["name"];

		// create the set and then connect it if required:

		TEXTOUT << "Adding set of " << connection_set_name << " connections to topology.\n";

		connection_set PTR p = generate_connection_set(parameters);

		if (p == NULL) return false;

		if(m_nn.connect_layers_at_topology_indexes(source_pos-1,destin_pos-1,p,fully_connect,min_random_weight,max_random_weight))
		{
			TEXTOUT << "Topology changed:\n";
			outline();
			return true;
		}

		warning("Deleting orphan (?) connection set");
		delete p;
		return false;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	void warning(string message)
	{
		Rcpp::warning("(NN module) "+message);
	}


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	NN()
	{
		TEXTOUT << "NN module created, now add components.\n";
		m_nn.reset();
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// adds a NN component that calls the R function specified by FUN
	// see also: https://teuder.github.io/rcpp4everyone_en/230_R_function.html

	bool add_R_function
			  ( string trigger,
                string FUN,
                string i_mode,
                int input_from,
                string o_mode,
                int output_to,
                bool ignore_result )
	{
		bool active_on_encode = false;
		bool active_on_recall = false;

		if(trigger=="on encode") active_on_encode = true;
		if(trigger=="on recall") active_on_recall = true;
		if(trigger=="always")  {
			active_on_encode = true;
			active_on_recall = true;
		}

		if(trigger!="never")
			if((!active_on_recall)AND(!active_on_encode))
			{
				warning("Not added, trigger must be 'on encode', 'on recall', 'never' or 'always'");
				return false;
			}

		if(
			(i_mode!="none") AND
			(i_mode!="input of") AND
			(i_mode!="output of") AND
			(i_mode!="weights at") AND
			(i_mode!="biases at") AND
			(i_mode!="misc at"))
			{
				warning("Not added, data to retreive must be 'none', 'input of','output of','weights at','biases at' or 'misc at'");
				return false;
			}

		if(
			(o_mode!="none") AND
			(o_mode!="to input") AND
			(o_mode!="to output") AND
			(o_mode!="to weights") AND
			(o_mode!="to biases") AND
			(o_mode!="to misc"))
			{
				warning("Not added, processed data should be send to 'none', 'to input','to output','to weights','to biases' or 'to misc'");
				return false;
			}

		//  convert R NN component indexes to C++ indexes:
		int i_index = input_from;
		int o_index = output_to;

		if((i_index != AUX_CONTROL_R_AUTODETERMINE_PREV) AND
           (i_index != AUX_CONTROL_R_AUTODETERMINE_NEXT))
			i_index = i_index - 1;

		if((o_index != AUX_CONTROL_R_AUTODETERMINE_PREV) AND
           (o_index != AUX_CONTROL_R_AUTODETERMINE_NEXT))
			o_index = o_index - 1;

		TEXTOUT << "Adding R component to topology.\n";

		aux_control_R PTR paR = new aux_control_R( FUN,
                                             &m_nn,
                                             i_mode,
                                             i_index,
                                             o_mode,
                                             o_index,
                                             active_on_encode,
                                             active_on_recall,
                                             ignore_result);
		if(paR != NULL)
			{
			//	paR->set_auxiliary_parameter(optional_parameter);

				if(m_nn.add_aux_control(paR))
				{
					m_nn.change_is_ready_flag(true);   // patch, not checked, but this component should be ready for processing no matter what.
					TEXTOUT << "Topology changed:\n";
					outline();
					return true;
				}
				warning("Deleting orphan (?) R component");
				delete paR;
			}

		return false;
	}


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// version that does not ignore results

	bool add_R_call ( string trigger,
                      string FUN,
                      string i_mode,
                      int input_from,
                      string o_mode,
                      int output_to)

	{
		return add_R_function(
			trigger,
			FUN,
			i_mode, input_from,
			o_mode, output_to,
			false);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// best for functions such as filters etc.

	bool add_R_pipelining ( string trigger, string FUN, bool fwd )
	{
		if(fwd) return add_R_function(
						trigger,
						FUN,
                		"output of", AUX_CONTROL_R_AUTODETERMINE_PREV,
                		"to input", AUX_CONTROL_R_AUTODETERMINE_NEXT,
                		false);
		else return add_R_function(
						trigger,
						FUN,
						"output of", AUX_CONTROL_R_AUTODETERMINE_NEXT,
						"to input", AUX_CONTROL_R_AUTODETERMINE_PREV,
						false);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// best for functions such as filters etc. reads from previous, results become input of next

	bool add_R_forwarding ( string trigger, string FUN)
	{
	return 	add_R_pipelining(trigger, FUN, true);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // best for functions whose result should be ignored s.a. plot

	bool add_R_ignoring( string trigger, string FUN, string i_mode, int input_from )
	{
		return add_R_function(
						trigger,FUN,
                		i_mode, input_from,
                		"none", AUX_CONTROL_R_AUTODETERMINE_NEXT,
                		true);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// (note: name is also used as type)
	// Some redundancy to add optional parameters:
	// (add_layer ----> add_layer_0xp, add_layer_1xp)
	// see https://lists.r-forge.r-project.org/pipermail/rcpp-devel/2010-November/001326.html

	bool add_layer_1xp(string name, int size, DATA optional_parameter)
	{
		List parameters = List::create(	Named("name")=name,
                                		Named("size")=size,
                                		Named("optional_parameter")=optional_parameter);

		return add_layer_Mxp(parameters);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	bool add_layer_0xp(string name, int size)
	{
		return add_layer_1xp(name,size,DATA_MIN);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	bool add_layer_Mxp(List parameters)
	{
		string name = parameters["name"];
		int size	= parameters["size"];

		m_nn.change_is_ready_flag(true);

		TEXTOUT << "Adding layer of "<< size << " " << name << " PEs to topology.\n";

		layer PTR p = generate_layer(parameters);
		if(p!=NULL)
		{
			if(m_nn.add_layer(p))
			{
				TEXTOUT << "Topology changed:\n";
				outline();
				return true;
			}
			warning("Deleting orphan (?) layer");
			delete p;
		}
		m_nn.change_is_ready_flag(false);
		TEXTOUT << "Note: Adding layer failed.\n";
		return false;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// add connection set to topology (disconnected and empty)
	// (note: name is also used as type)
	// Some redundancy to add optional parameters:
	// (add_connection_set ----> add_connection_set_0xp (now obsolete), add_connection_set_1xp, etc)
	// see https://lists.r-forge.r-project.org/pipermail/rcpp-devel/2010-November/001326.html

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// the following is left for backward compatibility AND for the case a single string is passed as parameter...

	bool add_connection_set_1xp(string name, DATA optional_parameter=DATA_MIN)
	{
		List parameters = List::create(	Named("name")=name,
                                  Named("optional_parameter")=optional_parameter);

		return add_connection_set_Mxp(parameters);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// this is called for a single string (assumed name) or a list of parameters

	bool add_connection_set_Mxp(List parameters)
	{
		if(parameters.length()==1) return add_connection_set_1xp(parameters[0]);			// i.e. name. Takes advantage of how Rcpp handles a a single string...

		string name = parameters["name"];
		DATA optional_parameter = DATA_MIN;

		if(parameters.containsElementNamed("optional_parameter"))
			optional_parameter = parameters["optional_parameter"];

		TEXTOUT << "Adding (empty) set of " << name << " connections to topology.\n";
		TEXTOUT << "(once topology is complete, use create_connections_in_sets to fill it with connections).\n";

		m_nn.change_is_ready_flag(false);

		connection_set PTR p = generate_connection_set(parameters);

		if(p!=NULL)
		{
			if(m_nn.add_connection_set(p))
			{
				TEXTOUT << "Topology changed:\n";
				outline();
				return true;
			}
			warning("Deleting orphan (?) connection set");
			delete p;
		}

		m_nn.change_is_ready_flag(false);
		TEXTOUT << "Note: Adding connection set failed.\n";
		return false;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// connect and create full connections in empty connection sets (between consecutive layers)

	bool create_connections_in_sets(DATA min_random_weight, DATA max_random_weight)
	{
		if(m_nn.connect_consecutive_layers(true,true,min_random_weight,max_random_weight))
		{
			TEXTOUT << "Connections added, you can now encode data.\n";
			return true;
		}
		return false;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Setup connection set for two layers (note: connection set name is also used as type),
	// no connections added
	// Some redundancy to add optional parameters:
	// (connect_layers_at ----> connect_layers_at_0xp (now obsolete), connect_layers_at_1xp etc.)
	// see https://lists.r-forge.r-project.org/pipermail/rcpp-devel/2010-November/001326.html

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// the following is left for backward compatibility AND for the case a single string is passed as parameter...

	bool connect_layers_at_1xp(	int source_pos,
                            	int destin_pos,
                            	string name,
                            	DATA optional_parameter)
	{
		List parameters = List::create(	Named("name")=name,
                                		Named("optional_parameter")=optional_parameter);

		return add_connection_set_for(source_pos, destin_pos, parameters, false, 0, 0);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// this is called for a single string (assumed name) or a list of parameters

	bool connect_layers_at_Mxp(	int source_pos,
                            	int destin_pos,
                            	List parameters)
	{
		string name;

		if(parameters.length()==1) return connect_layers_at_1xp( source_pos,
    															 destin_pos,
    															 parameters[0],			// i.e. name. Takes advantage of how Rcpp handles a a single string...
																 DATA_MIN);

		return add_connection_set_for(source_pos, destin_pos, parameters,false,0,0);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Fully connect two layers (note: connection set  name is also used as type)
	// Some redundancy to add optional parameters:
	// (fully_connect_layers_at ----> fully_connect_layers_at_0xp (now obsolete) , fully_connect_layers_at_1xp)
	// see https://lists.r-forge.r-project.org/pipermail/rcpp-devel/2010-November/001326.html

	bool fully_connect_layers_at_1xp(	int source_pos,
                                		int destin_pos,
                                		string name,
                                		DATA min_random_weight,
                                		DATA max_random_weight,
                                		DATA optional_parameter )
	{
		List parameters = List::create(	Named("name")=name,
                                		Named("optional_parameter")=optional_parameter);

		return add_connection_set_for(source_pos, destin_pos, parameters, true, min_random_weight, max_random_weight);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	bool fully_connect_layers_at_Mxp(	int source_pos,
                                		int destin_pos,
                                		List parameters,
                                		DATA min_random_weight,
                                		DATA max_random_weight )
	{
		string name;

		if(parameters.length()==1) return fully_connect_layers_at_1xp(	source_pos,
    																	destin_pos,
    																	parameters[0],			// i.e. name. Takes advantage of how Rcpp handles a a single string...
            															min_random_weight,
            															max_random_weight,
            															DATA_MIN);

		return add_connection_set_for(source_pos, destin_pos, parameters,true,min_random_weight,max_random_weight);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Add a connection at a set that already connects two layers R to Cpp indexes converted)

	bool add_single_connection(int pos, int source_pe, int destin_pe, DATA weight)
	{
		return m_nn.add_connection(pos-1,source_pe-1,destin_pe-1,weight);
	}


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// remove a connection at a set that already connects two layers R to Cpp indexes converted)

	bool remove_single_connection(int pos, int con)
	{
		return m_nn.remove_connection(pos-1,con);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	int size() {return m_nn.size();}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	IntegerVector component_ids()
	{
		IntegerVector x;
		if(m_nn.size()<=0) return x;
		x = IntegerVector(m_nn.size());
		for(int i = 0;i<m_nn.size(); i++ )
			x[i]=m_nn.component_id_from_topology_index(i);
		return x;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	IntegerVector sizes()
	{
		IntegerVector x;
		if(m_nn.size()<=0) return x;
		x = IntegerVector(m_nn.size());
		for(int i = 0;i<m_nn.size(); i++ )
			x[i]=m_nn.component_from_topology_index(i)->size();
		return x;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Send input (R to Cpp index converted)

	bool input_at(int pos, NumericVector data_in)
	{
		double * fpdata_in  = REAL(data_in);                    // my (lame?) way to interface with R, cont.)

		if(m_nn.set_component_for_input(pos-1))
			return m_nn.input_data_from_vector(fpdata_in,data_in.length());

		return false;
	}

	bool set_input_at(int pos, NumericVector data_in)
	{
		return input_at(pos,data_in);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Trigger encode for component at specified topology index (R to Cpp index converted)

	bool encode_at(int pos)
	{
		return m_nn.call_component_encode(pos-1);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Trigger recall for component at specified topology index (R to Cpp index converted)

	bool recall_at(int pos)
	{
		return m_nn.call_component_recall(pos-1);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Trigger encode for all components, in fwd or bwd direction

	bool encode_all(bool fwd = true)
	{
		return m_nn.call_component_encode_all(fwd);
	}

	bool encode_all_fwd()
	{
		return encode_all(true);
	}

	bool encode_all_bwd()
	{
		return encode_all(false);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Trigger recall for all components, in fwd or bwd direction

	bool recall_all(bool fwd = true)
	{
		return m_nn.call_component_recall_all(fwd);
	}

	bool recall_all_fwd()
	{
		return recall_all(true);
	}

	bool recall_all_bwd()
	{
		return recall_all(false);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Encode multiple input vectors stored in data set

	bool encode_dataset_unsupervised(
			NumericMatrix data,
			int pos,							// input component position
			int epochs = 1000,				// training epochs (presentations of all data)
			bool fwd = true					// processing direction (order) for components in NN
	)
	{
		if(data.rows()<=0)
		{
			error(NN_DATAST_ERR,"Cannot perform unsupervised training, dataset empty");
			return false;
		}

		int num_training_cases=data.rows();

		TEXTOUT << "Encoding (unsupervised)...\n";

		for(int i=0;i<epochs;i++)
		{
			if(NOT m_nn.is_ready())
			{
				error(NN_DATAST_ERR,"Training failed");
				return false;
			}

			for(int r=0;r<num_training_cases;r++)
			{
				if(NOT input_at(pos, data( r , _ ) ))
				{
					error(NN_INTEGR_ERR,"Training failed");
					return false;
				}
				encode_all(fwd);
			}
			if(i%100==0) checkUserInterrupt();					// (RCpp function to check if user pressed cancel)
		}

		TEXTOUT << "Finished.\n";
		return true;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Encode multiple (i,j) vector pairs stored in two corresponding data sets

	bool encode_datasets_supervised	(
			NumericMatrix i_data,				// data set, each row is a vector i of vector-pair (i,j)
			int i_pos,							// position (in topology) of component to receive i.
			NumericMatrix j_data,				// data set, each row is the corresponding vector j of vector-pair (i,j)
			int j_pos,							// position (in topology) of component to receive j.
			int j_destination_selector = 0,		// vector j will be sent to pe internal registers: 'input' if 0, to 'output' if 1, 'misc' if 2.
			int epochs = 1000,					// training epochs (presentations of all data)
			bool fwd = true						// processing direction (order) for components in NN
	)
	{
		if( (i_data.rows()<=0) OR
          (j_data.rows()<=0) OR
          (i_data.rows()!=j_data.rows()) )
		{
			error(NN_DATAST_ERR,"Cannot perform supervised training, invalid dataset size(s)");
			return false;
		}

		int num_training_pairs=i_data.rows();

		TEXTOUT << "Encoding (supervised)...\n";

		for(int e=0;e<epochs;e++)
		{
			if(NOT m_nn.is_ready())
			{
				error(NN_DATAST_ERR,"Training failed");
				return false;
			}

			bool i_data_sent = false;
			bool j_data_sent = false;

			for(int r=0;r<num_training_pairs;r++)
			{
				i_data_sent = input_at(i_pos, i_data( r , _ ));

				if(j_destination_selector==0) j_data_sent = input_at(j_pos, j_data( r , _ ));
				if(j_destination_selector==1) j_data_sent = set_output_at(j_pos, j_data( r , _ ));
				if(j_destination_selector==2) j_data_sent = set_misc_values_at(j_pos, j_data( r , _ ));

				if(NOT(i_data_sent AND j_data_sent))
				{
					error(NN_INTEGR_ERR,"Error sending the data to NN, training failed");
					return false;
				}

				encode_all(fwd);
			}
			if(e%100==0) checkUserInterrupt();					// (RCpp function to check if user pressed cancel)
		}

		TEXTOUT << "Finished.\n";
		return true;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Decode multiple input vectors stored in data set and get output.

	NumericMatrix recall_dataset(NumericMatrix data_in,
                              int input_pos,				// input component position
                              int output_pos,				// output component position
                              bool fwd = true				// processing direction (order) for components in NN
	)
	{
		NumericMatrix data_out;

		if((input_pos<1) OR (input_pos>size()) OR
         (output_pos<1) OR (output_pos>size()))
		{
			error(NN_INTEGR_ERR,"Invalid component position");
			return data_out;
		}

		int in_component_size = sizes()[input_pos-1];
		int out_component_size = sizes()[output_pos-1];
		int num_cases = data_in.rows();

		if((num_cases<=0))
		{
			error(NN_DATAST_ERR,"Cannot recall (decode or map) empty dataset");
			return data_out;
		}

		if((in_component_size!=data_in.cols()) OR
         (out_component_size<=0))
		{
			error(NN_DATAST_ERR,"Invalid or incompatible component sizes");
			return data_out;
		}

		data_out= NumericMatrix(num_cases,out_component_size);

		for(int r=0;r<num_cases;r++)
		{
			if(NOT input_at(input_pos, data_in( r , _ ) ))
			{
				error(NN_INTEGR_ERR,"Recall failed");
				return data_out;
			}
			recall_all(fwd);
			NumericVector v_out = get_output_from(output_pos);
			data_out( r , _ ) = v_out;                          //a lame way to interface with R. Copy result vector back to matrix. Remember, NumericMatrix stores data row-first, as R does.
		}

		return data_out;
	}


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// get output (R to Cpp index converted)

	NumericVector get_output_from(int pos)
	{
		NumericVector data_out;
		if(m_nn.set_component_for_output(pos-1))
			if(m_nn.output_dimension()>0)
			{
				data_out= NumericVector(m_nn.output_dimension());
				double * fpdata_out = REAL(data_out);                   // my (lame?) way to interface with R, cont.)
				if(NOT m_nn.output_data_to_vector(fpdata_out,data_out.length()))
					warning("Cannot retreive output from specified component");
			}
		return data_out;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// get output (same as above)

	NumericVector get_output_at(int pos)
	{
		return get_output_from(pos);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// get input  (R to Cpp index converted)
	/// warning: see implementation details for pe and connections, not a general method

	NumericVector get_input_at(int pos)
	{
		NumericVector data_out;

		component PTR pc;
		pc = m_nn.component_from_topology_index(pos-1);
		if(pc==NULL) return data_out;

		int num_items = pc->size();
		if(num_items>0)
		{
			data_out= NumericVector(num_items);
			double * fpdata_out = REAL(data_out);                   // my (lame?) way to interface with R, cont.)
			if(NOT m_nn.get_input_at_component(pos-1,fpdata_out, num_items))
				warning("Cannot retreive input from specified component");
		}

		return data_out;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Get weights (connection variable)

	NumericVector get_weights_at(int pos)
	{
		NumericVector data_out;

		component PTR pc;
		pc = m_nn.component_from_topology_index(pos-1);
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
			if(NOT m_nn.get_weights_at_component(pos-1,fpdata_out, num_items))
				warning("Cannot retreive weights from specified component");
		}

		return data_out;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Set "bias" register values for PEs in given layer (R to Cpp index converted)

	bool set_weights_at(int pos, NumericVector data_in)
	{
		double * fpdata_in  = REAL(data_in);                    // my (lame?) way to interface with R, cont.)
		return m_nn.set_weights_at_component(pos-1,fpdata_in,data_in.length());
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Get weight for given connection.

	DATA get_weight_at(int pos, int connection)
	{
		return m_nn.get_weight_at_component(pos-1,connection);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Set weight at given connection.

	bool set_weight_at(int pos, int connection, DATA value)
	{
		return m_nn.set_weight_at_component(pos-1,connection,value);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Get "misc" register values for PEs or connections in given component (R to Cpp index converted)

	NumericVector get_misc_values_at(int pos)
	{
		NumericVector data_out;

		component PTR pc;
		pc = m_nn.component_from_topology_index(pos-1);
		if(pc==NULL) return data_out;

		int num_items = pc->size();
		if(num_items>0)
		{
			data_out= NumericVector(num_items);
			double * fpdata_out = REAL(data_out);                   // my (lame?) way to interface with R, cont.)
			if(NOT m_nn.get_misc_at_component(pos-1,fpdata_out, num_items))
				warning("Cannot retreive misc values");
		}

		return data_out;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Set "misc" register values for PEs or connections in given component (R to Cpp index converted)

	bool set_misc_values_at(int pos, NumericVector data_in)
	{
		double * fpdata_in  = REAL(data_in);                    // my (lame?) way to interface with R, cont.)
		return m_nn.set_misc_at_component(pos-1,fpdata_in,data_in.length());
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Set "output" register values for PEs in given layer (R to Cpp index converted)

	bool set_output_at(int pos, NumericVector data_in)
	{
		double * fpdata_in  = REAL(data_in);                    // my (lame?) way to interface with R, cont.)
		return m_nn.set_output_at_component(pos-1,fpdata_in,data_in.length());
	}


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Get "bias" register values (pe variable) from layer

	NumericVector get_biases_at(int pos)
	{
		NumericVector data_out;

		component PTR pc;
		pc = m_nn.component_from_topology_index(pos-1);
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
			if(NOT m_nn.get_biases_at_component(pos-1,fpdata_out, num_items))
				warning("Cannot retreive biases");
		}

		return data_out;
	}


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Get "bias" register for given pe.

	DATA get_bias_at(int pos, int pe)
	{
		return m_nn.get_bias_at_component(pos-1,pe);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Set "bias" register values for PEs in given layer (R to Cpp index converted)

	bool set_biases_at(int pos, NumericVector data_in)
	{
		double * fpdata_in  = REAL(data_in);                    // my (lame?) way to interface with R, cont.)
		return m_nn.set_biases_at_component(pos-1,fpdata_in,data_in.length());
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Set "bias" at given PE.

	bool set_bias_at(int pos, int pe, DATA value)
	{
		return m_nn.set_bias_at_component(pos-1,pe,value);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	void print()
	{
		TEXTOUT << "------Network structure (BEGIN)--------\n";
		m_nn.to_stream(TEXTOUT);
		TEXTOUT << "--------Network structure (END)--------\n";
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	void show()
	{
		TEXTOUT << "User-defined NN type (Class NN):\n\n";
		outline();
		TEXTOUT << "\n";
		print();
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	void outline()
	{
		TEXTOUT << "------Network outline (BEGIN)--------\n";
		TEXTOUT << m_nn.outline(true);
		TEXTOUT << "--------Network outline (END)--------\n";
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	DataFrame get_topology_info()
	{
		DataFrame result;

		if(m_nn.size()<=0)
		{
			warning("The NN is empty");
			return result;
		}

		IntegerVector component_index = IntegerVector (m_nn.size(),NA_INTEGER);
		IntegerVector component_types = IntegerVector (m_nn.size(),NA_INTEGER);
		StringVector  component_descs = StringVector(m_nn.size(),"No Description");
		StringVector  component_names = StringVector(m_nn.size(),"No Name");
		IntegerVector component_sizes = IntegerVector  (m_nn.size(),NA_INTEGER);

		for(int c=0;c<m_nn.size();c++)
		{
			component PTR pc = m_nn.component_from_topology_index(c);
			if(pc!=NULL)
			{
				component_index [c]		= c+1;
				component_types [c]		= pc->type();
				component_descs [c]		= "Unknown";
				if(m_nn.get_connection_set_at(c)!=NULL)
					component_descs[c] = "Connection Set";
				if(m_nn.get_layer_at(c)!=NULL)
					component_descs[c] = "PE Layer";
				if(m_nn.get_aux_control_at(c)!=NULL)
					component_descs[c] = "Control";
				component_names [c]		= pc->name();
				component_sizes [c]		= pc->size();
			}
		}

		result = DataFrame::create(
							  Named("Position")		= clone(component_index),
                              Named("Type")			= clone(component_types),
                              Named("Description")	= clone(component_descs),
                              Named("Name")			= clone(component_names),
                              Named("Size")			= clone(component_sizes) );
		return result;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

};

//--------------------------------------------------------------------------------
// Some redundancy was needed below to allow for optional parameters (the way Rcpp currently supports it):
// (add_layer				----> add_layer_0xp, add_layer_1xp)
// (add_connection_set		----> add_connection_set_0xp, add_connection_set_1xp)
// (fully_connect_layers_at	----> fully_connect_layers_at_0xp, fully_connect_layers_at_1xp)
// (connect_layers_at		----> connect_layers_at_0xp, connect_layers_at_1xp)
// see:
//		https://lists.r-forge.r-project.org/pipermail/rcpp-devel/2010-November/001326.html
//		2.2.4. Exposing methods using Rcpp modules https://cloud.r-project.org/web/packages/Rcpp/vignettes/Rcpp-modules.pdf
//		https://lists.r-forge.r-project.org/pipermail/rcpp-devel/2010-November/001326.html

RCPP_MODULE(class_NN) {
	class_<NN>( "NN" )
	.constructor()
	// 	.constructor<NumericMatrix,IntegerVector,int>()
     .method( "size",         							&NN::size, 		      												"Number of components in NN topology" )
     .method( "sizes",         							&NN::sizes, 		      											"Sizes of components in NN topology" )
     .method( "add_R_function", 						&NN::add_R_function,         										"Append R function component to topology" )
     .method( "add_R_forwarding", 						&NN::add_R_forwarding,         										"Append R function, results will be forwarded to next component in topology" )
     .method( "add_R_pipelining", 						&NN::add_R_pipelining,         										"Append R function, results will be transfered fwd or bwd" )
     .method( "add_R_ignoring", 						&NN::add_R_ignoring,         										"Append R function, ignoring its results" )
     .method( "add_layer",					  (bool (NN::*)(string,int))(&NN::add_layer_0xp),								"Append layer component to topology" )
     .method( "add_layer",				 (bool (NN::*)(string,int,DATA))(&NN::add_layer_1xp),								"Append layer component to topology" )
     .method( "add_layer",							(bool (NN::*)(List))(&NN::add_layer_Mxp),								"Append layer component to topology" )
     .method( "add_connection_set",			 (bool (NN::*)(string,DATA))(&NN::add_connection_set_1xp),  					"Append set of connections to topology (disconnected and empty of connections)" )
     .method( "add_connection_set",					(bool (NN::*)(List))(&NN::add_connection_set_Mxp),  					"Append set of connections to topology (disconnected and empty of connections)" )
     .method( "connect_layers_at",	 (bool (NN::*)(int,int,string,DATA))(&NN::connect_layers_at_1xp),						"Add connection set for two layers, no connections added" )
     .method( "connect_layers_at",		    (bool (NN::*)(int,int,List))(&NN::connect_layers_at_Mxp),						"Add connection set for two layers, no connections added" )
     .method( "fully_connect_layers_at",   (bool (NN::*)(int,int,string,DATA,DATA,DATA))(&NN::fully_connect_layers_at_1xp), "Add connection set that fully connects two layers with connections" )
	 .method( "fully_connect_layers_at",		  (bool (NN::*)(int,int,List,DATA,DATA))(&NN::fully_connect_layers_at_Mxp), "Add connection set that fully connects two layers with connections" )
     .method( "create_connections_in_sets", 			&NN::create_connections_in_sets,									"Create connections to fully connect consequent layers" )
     .method( "add_single_connection",					&NN::add_single_connection,											"Add a connection to a set that already connects two layers" )
     .method( "remove_single_connection",				&NN::remove_single_connection,										"Remove a connection from a set" )
     .method( "component_ids",   						&NN::component_ids, 	      										"Vector of topology component ids" )
     .method( "input_at",        						&NN::input_at,     													"Input vector to specified topology index" )
     .method( "set_input_at",      						&NN::set_input_at,     												"Input vector to specified topology index" )
     .method( "encode_at",       						&NN::encode_at, 	      											"Trigger encode for specified topology index" )
     .method( "recall_at",       						&NN::recall_at, 	      											"Trigger recall for specified topology index" )
     .method( "encode_all",      						&NN::encode_all, 	   												"Trigger encode for entire topology" )
     .method( "encode_all_fwd",    						&NN::encode_all_fwd,	   											"Trigger encode for entire topology, forward direction" )
     .method( "encode_all_bwd",    						&NN::encode_all_bwd,	   											"Trigger encode for entire topology, backward direction" )
	 .method( "recall_all",     						&NN::recall_all,	   												"Trigger recall for entire topology" )
     .method( "recall_all_fwd",    						&NN::recall_all_fwd,	   											"Trigger recall for entire topology, forward direction" )
     .method( "recall_all_bwd",    						&NN::recall_all_bwd,	   											"Trigger recall for entire topology, backward direction" )
     .method( "encode_dataset_unsupervised",     		&NN::encode_dataset_unsupervised,	   								"Encode a data set using unsupervised training" )
     .method( "encode_datasets_supervised",     		&NN::encode_datasets_supervised,	   								"Encode multiple (i,j) vector pairs using supervised training" )
     .method( "recall_dataset",     					&NN::recall_dataset,				   								"Recall (i.e decode,map) a data set" )
     .method( "get_output_from",     					&NN::get_output_from,    											"Output vector from specified topology index" )
     .method( "get_output_at",	     					&NN::get_output_at,    												"Output vector from specified topology index" )
     .method( "get_input_at",     						&NN::get_input_at,		   											"Get input (pe variable value or connection input) at specified topology index" )
     .method( "get_weights_at",  	 					&NN::get_weights_at,	   											"Get connection weights (connection variable values) at specified topology index" )
     .method( "set_weights_at",     					&NN::set_weights_at,	   											"Set connection weights for connection set at specified topology index" )
     .method( "get_weight_at",  	 					&NN::get_weight_at,	   												"Get connection weight for given connection at specified topology index" )
     .method( "set_weight_at",     						&NN::set_weight_at,	   												"Set connection weight for given connection at specified topology index" )
     .method( "get_misc_values_at",     				&NN::get_misc_values_at,	   										"Get misc registers of elements at specified topology index" )
     .method( "set_misc_values_at",     				&NN::set_misc_values_at,	   										"Set misc registers of elements at specified topology index" )
     .method( "set_output_at",     						&NN::set_output_at,	   												"Set output values at specified topology index" )
     .method( "get_biases_at",     						&NN::get_biases_at,	   												"Get bias values in layer at specified topology index" )
     .method( "get_bias_at",     						&NN::get_bias_at,	   												"Get bias value for given PE in layer at specified topology index" )
     .method( "set_biases_at",     						&NN::set_biases_at,	   												"Set bias values in layer at specified topology index" )
     .method( "set_bias_at",     						&NN::set_bias_at,	   												"Set bias value for given PE in layer at specified topology index" )
     .method( "print",     								&NN::print,         												"Print internal NN state" )
     .method( "show",     								&NN::show,         													"Print internal NN state" )
     .method( "outline",     							&NN::outline,         												"Show outline of the NN topology" )
     .method( "get_topology_info", 						&NN::get_topology_info,         									"Get NN topology information" )
	;
}

//--------------------------------------------------------------------------------

#endif // NNLIB2_FOR_RCPP

