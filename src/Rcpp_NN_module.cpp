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
			m_nn.add_layer(p);
			TEXTOUT << "Topology changed:\n";
			outline();
			return true;
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
         m_nn.add_connection_set(p);
         TEXTOUT << "Topology changed:\n";
         outline();
         return true;
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
	// Set "misc" register values for PEs in given layer (R to Cpp index converted)

	bool set_misc_values_at(int pos, NumericVector data_in)
	{
		double * fpdata_in  = REAL(data_in);                    // my (lame?) way to interface with R, cont.)
		return m_nn.set_misc_at_component(pos-1,fpdata_in,data_in.length());
	}

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	void print()
	{
		TEXTOUT << "------Network structure (BEGIN)--------\n";
		m_nn.to_stream(TEXTOUT);
		TEXTOUT << "--------Network structure (END)--------\n";
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
    .method( "get_output_from",     					&NN::get_output_from,    						"Output vector from specified topology index" )
    .method( "get_input_at",     						&NN::get_input_at,		   						"Get input (pe variable value or connection input) in specified topology index" )
    .method( "get_weights_at",     						&NN::get_weights_at,	   						"Get connection weights (connection variable value) in specified topology index" )
    .method( "get_weight_at",     						&NN::get_weight_at,	   							"Get connection weight for given connection in specified topology index" )
    .method( "set_weight_at",     						&NN::set_weight_at,	   							"Set connection weight for given connection in specified topology index" )
	.method( "set_misc_values_at",     					&NN::set_misc_values_at,	   					"Set misc register values in elements in specified topology index" )
	.method( "print",     								&NN::print,         							"Print internal NN state" )
    .method( "outline",     							&NN::outline,         							"Print an outline of the NN" )
;
}

//--------------------------------------------------------------------------------

#endif // NNLIB2_FOR_RCPP

