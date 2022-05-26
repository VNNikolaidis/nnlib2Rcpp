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
#include "spare_parts.h"
#include "additional_parts.h"		// header for user-defined parts (components etc)

using namespace nnlib2;

class NN
{
protected:

	nn    m_nn;					// the internal NN


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// generate layer for further use later (note: name is also used as type selector)

	layer PTR generate_layer(string name, int size, DATA optional_parameter=0)
		{
		if( name == "pe" ) 				return new pe_layer(name,size);
		if( name == "generic_d" ) 		return new pe_layer(name,size);
		if( name == "generic" )  		return new Layer<pe>(name,size);

		if( name == "pass-through" )	return new pass_through_layer(name,size);

		if( name == "which-max")       	return new which_max_layer(name,size);

		if( name == "MAM" )				return new mam::mam_layer(name,size);

		if( name == "LVQ-input" )		{
										TEXTOUT << "(Note: "<< name << " layer implementation is outdated - expect limited functionality.)\n";
										lvq::lvq_input_layer PTR pl = new lvq::lvq_input_layer;
										pl->setup(name,size);
										return pl;
										}

		if( name == "LVQ-output" )		{
										TEXTOUT << "(Note: "<< name << " layer implementation is outdated - expect limited functionality.)\n";
										lvq::lvq_output_layer PTR pl = new lvq::lvq_output_layer;
										pl->setup(name,size,1);
										return pl;
										}

		 if( name == "BP-hidden" )		{
										 TEXTOUT << "Note: current "<< name << " layer implementation is outdated - expect limited functionality.\n";
		 								bp::bp_comput_layer PTR pl = new bp::bp_comput_layer;
		 								pl->setup(name,size);
		 								pl->randomize_biases(-1,1);
		 								pl->set_learning_rate(0.6);
		 								return pl;
		 								}

		 if( name == "BP-output" )		{
		 								TEXTOUT << "Note: current "<< name << " layer implementation is outdated - expect limited functionality.\n";
		 								bp::bp_output_layer PTR pl = new bp::bp_output_layer;
		 								pl->setup(name,size);
		 								pl->randomize_biases(-1,1);
		 								pl->set_learning_rate(0.6);
		 								return pl;
		 								}

		layer PTR pl = generate_custom_layer(name,size);
		if(pl != NULL) return pl;

		warning("Unknown layer type");
		return NULL;
		}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// generate connection set for further use later (note: name is also used as type)

	connection_set PTR generate_connection_set(string name="", DATA optional_parameter=0)
	    {
		if( name == "generic" )			return new Connection_Set<connection>(name);

		if( name == "pass-through" )	return new Connection_Set<pass_through_connection>(name);

		if( name == "wpass-through" )	return new Connection_Set<weighted_pass_through_connection>(name);

		if( name == "MAM" )				return new mam::mam_connection_set(name);

		if( name == "LVQ")				{
										TEXTOUT << "(Note: "<< name << " connection set implementation is outdated - expect limited functionality.)\n";
										lvq::lvq_connection_set PTR pc = new lvq::lvq_connection_set;
										if(pc!=NULL)
											{
											pc->set_iteration_number(100);
											pc->name() = name;
											}
										return pc;
										}

		 if( name == "BP" )				{
		 								TEXTOUT << "(Note: "<< name << " connection set implementation is outdated - expect limited functionality.)\n";
		 								bp::bp_connection_set PTR pc = new bp::bp_connection_set;
		 								if(pc!=NULL)
		 									{
		 									pc->name() = name;
		 									pc->set_learning_rate(0.6);
		 									}
		 								return pc;
		 								}

		connection_set PTR pc = generate_custom_connection_set(name);
		if(pc!=NULL) return pc;

		warning("Unknown connection set type");
		return NULL;
	    }


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Connect two layers (note: connection_set_name is also used as type)

	bool add_connection_set_for(int source_pos, int destin_pos, string connection_set_name="", bool fully_connect=true, DATA min_random_weight = 0, DATA max_random_weight = 0)
	{
		TEXTOUT << "Adding set of " << connection_set_name << " connections to topology.\n";

		connection_set PTR p = generate_connection_set(connection_set_name, 0);

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

public:

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	NN()
	{
		TEXTOUT << "NN module created, now add components.\n";
		m_nn.reset();
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// (note: name is also used as type)

	bool add_layer(string name,int size)
	{
		m_nn.change_is_ready_flag(true);
		TEXTOUT << "Adding layer of " << name << " PEs to topology.\n";

		layer PTR p = generate_layer(name, size, 0);
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
	// add connection set to topology (disconnected and empty) (note: type is also used as name)

	bool add_connection_set(string type)
	{
		m_nn.change_is_ready_flag(false);

		TEXTOUT << "Adding (empty) set of " << type << " connections to topology.\n";
		TEXTOUT << "(once topology is complete, use create_connections() to fill it).\n";

        connection_set PTR p = generate_connection_set(type, 0);
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

	bool create_connections_in_sets(DATA min_random_weight=0, DATA max_random_weight=0)
	{
		if(m_nn.connect_consecutive_layers(true,true,min_random_weight,max_random_weight))
		{
			TEXTOUT << "Connections added, now can now encode data.\n";
			return true;
		}
		return false;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Setup connection set for two layers (note: connection_set_name is also used as type),
	// no connections added

	bool connect_layers_at(int source_pos, int destin_pos, string name)
	{
		return add_connection_set_for(source_pos,destin_pos,name,false,0,0);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Fully connect two layers (note: connection_set_name is also used as type)

	bool fully_connect_layers_at(int source_pos, int destin_pos, string name, DATA min_random_weight = 0, DATA max_random_weight = 0)
	{
		return add_connection_set_for(source_pos,destin_pos,name,true,min_random_weight,max_random_weight);
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
		return m_nn.remove_connection(pos-1,con-1);
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Trigger recall for all components, in fwd or bwd direction

	bool recall_all(bool fwd = true)
	{
		return m_nn.call_component_recall_all(fwd);
	}


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Encode multiple input vectors stored in data set

	bool encode_dataset_unsupervised
						  (NumericMatrix data,
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

	bool encode_datasets_supervised
	(NumericMatrix i_data,				// data set, each row is a vector i of vector-pair (i,j)
	 int i_pos,							// position (in topology) of component to receive i.
	 NumericMatrix j_data,				// data set, each row is the corresponding vector j of vector-pair (i,j)
	 int j_pos,							// position (in topology) of component to receive j.
	 int j_destination_selector = 0,	// vector j will be sent to pe internal registers: 'input' if 0, to 'output' if 1, 'misc' if 2.
	 int epochs = 1000,					// training epochs (presentations of all data)
     bool fwd = true					// processing direction (order) for components in NN
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
				warning("Cannot retreive output");
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
	// get input (pe variable or connection input) (R to Cpp index converted)

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
			if(NOT m_nn.get_input_data_at_component(pos-1,fpdata_out, num_items))
				warning("Cannot retreive input");
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
			warning("Not a connection set");
			return data_out;
			}

		int num_items = pc->size();
		if(num_items>0)
		{
			data_out= NumericVector(num_items);
			double * fpdata_out = REAL(data_out);                   // my (lame?) way to interface with R, cont.)
			if(NOT m_nn.get_weights_at_component(pos-1,fpdata_out, num_items))
				warning("Cannot retreive weights");
		}

		return data_out;
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Get weight for given connection.

	DATA get_weight_at(int pos, int connection)
	{
		return m_nn.get_weight_at_component(pos-1,connection-1);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Get weight for given connection.

	bool set_weight_at(int pos, int connection, DATA value)
	{
		return m_nn.set_weight_at_component(pos-1,connection-1,value);
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Set "misc" register values for PEs in given component (R to Cpp index converted)

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

};

//--------------------------------------------------------------------------------

RCPP_MODULE(class_NN) {
	class_<NN>( "NN" )
	.constructor()
// 	.constructor<NumericMatrix,IntegerVector,int>()
    .method( "size",         							&NN::size, 		      							"Number of components in NN topology" )
    .method( "sizes",         							&NN::sizes, 		      						"Sizes of components in NN topology" )
    .method( "add_layer", 								&NN::add_layer,     							"Append layer component to topology" )
    .method( "add_connection_set", 						&NN::add_connection_set,  						"Append set of connections to topology (disconnected and empty of connections)" )
    .method( "create_connections_in_sets", 				&NN::create_connections_in_sets,				"Create connections to fully connect consequent layers" )
    .method( "connect_layers_at",						&NN::connect_layers_at,							"Add connection set for two layers, no connections added" )
    .method( "fully_connect_layers_at", 				&NN::fully_connect_layers_at,					"Add connection set that fully connects two layers with connections" )
    .method( "add_single_connection",					&NN::add_single_connection,						"Add a connection to a set that already connects two layers" )
    .method( "remove_single_connection",				&NN::remove_single_connection,					"Remove a connection from a set" )
    .method( "component_ids",   						&NN::component_ids, 	      					"Vector of topology component ids" )
    .method( "input_at",        						&NN::input_at,     								"Input vector to specified topology index" )
    .method( "encode_at",       						&NN::encode_at, 	      						"Trigger encode for specified topology index" )
    .method( "recall_at",       						&NN::recall_at, 	      						"Trigger recall for specified topology index" )
    .method( "encode_all",      						&NN::encode_all, 	   							"Trigger encode for entire topology" )
    .method( "recall_all",     							&NN::recall_all,	   							"Trigger recall for entire topology" )
    .method( "encode_dataset_unsupervised",     		&NN::encode_dataset_unsupervised,	   			"Encode a data set using unsupervised training" )
    .method( "encode_datasets_supervised",     			&NN::encode_datasets_supervised,	   			"Encode multiple (i,j) vector pairs using supervised training" )
    .method( "recall_dataset",     						&NN::recall_dataset,				   			"Recall (i.e decode,map) a data set" )
    .method( "get_output_from",     					&NN::get_output_from,    						"Output vector from specified topology index" )
    .method( "get_output_at",	     					&NN::get_output_at,    							"Output vector from specified topology index" )
    .method( "get_input_at",     						&NN::get_input_at,		   						"Get input (pe variable value or connection input) in specified topology index" )
    .method( "get_weights_at",     						&NN::get_weights_at,	   						"Get connection weights (connection variable value) in specified topology index" )
    .method( "get_weight_at",     						&NN::get_weight_at,	   							"Get connection weight for given connection in specified topology index" )
    .method( "set_weight_at",     						&NN::set_weight_at,	   							"Set connection weight for given connection in specified topology index" )
	.method( "set_misc_values_at",     					&NN::set_misc_values_at,	   					"Set misc registers of elements in specified topology index" )
	.method( "set_output_at",     						&NN::set_output_at,	   							"Set output values in specified topology index" )
	.method( "print",     								&NN::print,         							"Print internal NN state" )
	.method( "show",     								&NN::show,         							"Print internal NN state" )
	.method( "outline",     							&NN::outline,         							"Outline of the NN topology" )
;
}

//--------------------------------------------------------------------------------

#endif // NNLIB2_FOR_RCPP

