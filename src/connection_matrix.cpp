//		----------------------------------------------------------
//		(c)2023  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		connection_matrix.cpp							Version 0.1
//		-----------------------------------------------------------
//		Definition/implementation a parent class for blocks ("sets")
//		of connections represented as a matrix. A very quick draft
//		for the implementation, many needed features are missing,
//		Note:
//		This does not use 'connection' class, so all encoding
//		and recall (map) code should be withing the class that
//		inherits this one.
//		-----------------------------------------------------------

#include "connection_matrix.h"
#include "nnlib2_memory.h"

#include <sstream>

namespace nnlib2 {

/*-----------------------------------------------------------------------*/
/* (Type B) Base-class for a simple matrix-based connection set  		 */
/*-----------------------------------------------------------------------*/

static pe dummy_pe;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This (quick) implementation associates 1 row of the matrix per
// destination layer PEs and 1 column per source layer PE. Thus the output
// of the source layer is related with a single row for each destination
// node. In many cases the transpose of this is more useful, but such option
// is not yet implemented.

generic_connection_matrix::generic_connection_matrix()
{
	mp_source_layer = NULL;
	mp_destin_layer = NULL;

	m_allocated_rows_destin_layer_size = 0;
	m_allocated_cols_source_layer_size = 0;

	m_requires_misc = false;

	m_weights = NULL;
	m_misc = NULL;

	m_type = cmpnt_connection_set;
	m_name = "Connection matrix";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

generic_connection_matrix::generic_connection_matrix(string name, bool requires_misc)
	:generic_connection_matrix()
{
	m_name = name;
	m_requires_misc = requires_misc;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

generic_connection_matrix::~generic_connection_matrix()
{
	reset_matrices();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// just being extra cautious...

bool generic_connection_matrix::sizes_are_consistent()
{
	if(((m_allocated_rows_destin_layer_size>0)  AND (m_allocated_cols_source_layer_size<=0)) OR
        ((m_allocated_rows_destin_layer_size<=0) AND (m_allocated_cols_source_layer_size>0)))
		warning("Invalid allocated sizes");

	if(m_allocated_rows_destin_layer_size>0)
		if(m_allocated_cols_source_layer_size>0)
		{
			if(mp_destin_layer==NULL)		{error(NN_INTEGR_ERR,"Invalid destination layer");return false;}
			if(mp_destin_layer->size()<=0)	{error(NN_INTEGR_ERR,"Invalid destination layer size");return false;}

			if(mp_destin_layer!=NULL)
				if(mp_destin_layer->size()!=m_allocated_rows_destin_layer_size)
				{error(NN_INTEGR_ERR,"Invalid source layer size");return false;}

			if(mp_source_layer==NULL)		{error(NN_INTEGR_ERR,"Invalid source layer");return false;}
			if(mp_source_layer->size()<=0)	{error(NN_INTEGR_ERR,"Invalid source layer size");return false;}

			if(mp_source_layer!=NULL)
				if(mp_source_layer->size()!=m_allocated_cols_source_layer_size)
				{error(NN_INTEGR_ERR,"Invalid source layer size");return false;}

			if(m_weights==NULL) {error(NN_NULLPT_ERR,"Invalid weights matrix");return false;}

			if(m_requires_misc)
			 if(m_misc==NULL)   {error(NN_NULLPT_ERR,"Invalid misc matrix");return false;}
		}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int generic_connection_matrix::size()
{

	if(((m_allocated_rows_destin_layer_size>0)  AND (m_allocated_cols_source_layer_size<=0)) OR
        ((m_allocated_rows_destin_layer_size<=0) AND (m_allocated_cols_source_layer_size>0)))
		{warning("Invalid allocated sizes"); return 0;}

	if( (m_allocated_rows_destin_layer_size<=0) OR
        (m_allocated_cols_source_layer_size<=0))
		{
		if(m_weights!=NULL)
			{warning("Invalid allocated matrices"); return 0;}
		if(m_requires_misc AND (m_misc!=NULL))
        	{warning("Invalid allocated matrices"); return 0;}
		}

	return m_allocated_cols_source_layer_size*m_allocated_rows_destin_layer_size;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void generic_connection_matrix::reset_matrices()
{
	if(m_weights!=NULL)
	{
		if(m_allocated_rows_destin_layer_size<=0) warning("Inconsistent  sizes");
		free_2d(m_weights,m_allocated_rows_destin_layer_size);
		m_weights=NULL;
	}

	if(m_misc!=NULL)
	{
		if(m_allocated_rows_destin_layer_size<=0) warning("Inconsistent  sizes");
		free_2d(m_misc,m_allocated_rows_destin_layer_size);
		m_misc=NULL;
	}

	m_allocated_rows_destin_layer_size = 0;
	m_allocated_cols_source_layer_size = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// returns mp_destin_layer as a reference to layer (or to dummy_layer if error)

layer REF generic_connection_matrix::source_layer()
{
	if(mp_source_layer!=NULL)
		if(mp_source_layer->type() EQL cmpnt_layer)
			return (ATPTR mp_source_layer);
	error(NN_INTEGR_ERR,"Source component is not a layer");
	return dummy_layer;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// returns mp_destin_layer as a reference to layer (or to dummy_layer if error)

layer REF generic_connection_matrix::destin_layer()
{
	if(mp_destin_layer!=NULL)
		if(mp_destin_layer->type() EQL cmpnt_layer)
			return (ATPTR mp_destin_layer);
	error(NN_INTEGR_ERR,"Destination component is not a layer");
	return dummy_layer;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// this useful to 'connection' objects, so here it is not essential.
// here, index is used to identify a pe...
// trying to be similar with dllist-based connections...
// Note: PE indexes start from 0.
// Note: this function is not used or tested!

pe REF generic_connection_matrix::source_pe(int c)
{
	if(mp_source_layer!=NULL)
		if(m_allocated_cols_source_layer_size==mp_source_layer->size())
		{
			int column_for_item_c = c % m_allocated_cols_source_layer_size;
			return mp_source_layer->PE(column_for_item_c);
		}
	error(NN_INTEGR_ERR,"Inconsistent  sizes");
	return dummy_pe;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// this useful to 'connection' objects, so here it is not essential.
// here, index is used to identify a pe...
// trying to be similar with dllist-based connections...
// Note: PE indexes start from 0.
// Note: this function is not used or tested!

pe REF generic_connection_matrix::destin_pe(int c)
{
	if(mp_destin_layer!=NULL)
		if(m_allocated_rows_destin_layer_size==mp_destin_layer->size())
		{
			int row_for_item_c = (int)c/m_allocated_rows_destin_layer_size;
			return mp_destin_layer->PE(row_for_item_c);
		}
	error(NN_INTEGR_ERR,"Inconsistent  sizes");
	return dummy_pe;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool generic_connection_matrix::has_source_layer()
{
	return (mp_source_layer != NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool generic_connection_matrix::has_destin_layer()
{
	return (mp_destin_layer != NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

DATA generic_connection_matrix::get_connection_weight(int source_pe, int destin_pe)
{
	if(m_weights!=NULL)
		if(destin_pe>=0)
			if(destin_pe<m_allocated_rows_destin_layer_size)
				if(source_pe>=0)
					if(source_pe<m_allocated_cols_source_layer_size)
						return m_weights[destin_pe][source_pe];

error(NN_INTEGR_ERR,"Cannot retreive connection weight from matrix");
return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

DATA generic_connection_matrix::get_connection_weight(int connection)
{
	if((connection>=0) AND (connection<size()))
	{
		int con_row = (int)connection/m_allocated_cols_source_layer_size;
		int con_col = connection % m_allocated_cols_source_layer_size;
		return get_connection_weight(con_col,con_row);
	}
	error(NN_INTEGR_ERR,"Cannot retreive connection weight from matrix");
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool generic_connection_matrix::set_connection_weight(int source_pe, int destin_pe, DATA value)
{
	if(m_weights!=NULL)
		if(destin_pe>=0)
			if(destin_pe<m_allocated_rows_destin_layer_size)
				if(source_pe>=0)
					if(source_pe<m_allocated_cols_source_layer_size)
					{
						m_weights[destin_pe][source_pe] = value;
						return true;
					}
	error(NN_INTEGR_ERR,"Cannot set connection weight in matrix");
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool generic_connection_matrix::set_connection_weight(int connection, DATA value)
{
	if((connection>=0) AND (connection<size()))
	{
		int con_row = (int)connection/m_allocated_cols_source_layer_size;
		int con_col = connection % m_allocated_cols_source_layer_size;
		return set_connection_weight(con_col,con_row,value);
	}
	error(NN_INTEGR_ERR,"Cannot set connection weight in matrix");
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool generic_connection_matrix::get_misc(DATA * buffer, int dimension)
{
	if(!sizes_are_consistent()) return false;

	if(buffer==NULL) return false;

	if(!m_requires_misc)
	{
		error(NN_INTEGR_ERR,"This connection matrix is not set up to use misc values");
		return false;
	}

	if(dimension!=size())
	{
		error(NN_INTEGR_ERR,"Inconsistent sizes for getting misc values");
		return false;
	}

	for(int i=0;i<size();i++)
	{
		int con_row = (int)i/m_allocated_cols_source_layer_size;
		int con_col = i % m_allocated_cols_source_layer_size;
		buffer[i]=m_misc[con_row][con_col];
	}

	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool generic_connection_matrix::set_misc(DATA * data, int dimension)
{
	if(!sizes_are_consistent()) return false;

	if(data==NULL) return false;

	if(!m_requires_misc)
	{
		error(NN_INTEGR_ERR,"This connection matrix is not set up to use misc values");
		return false;
	}

	if(dimension!=size())
	{
		error(NN_INTEGR_ERR,"Inconsistent sizes for setting misc values");
		return false;
	}

	for(int i=0;i<size();i++)
	{
		int con_row = (int)i/m_allocated_cols_source_layer_size;
		int con_col = i % m_allocated_cols_source_layer_size;
		m_misc[con_row][con_col]=data[i];
	}

	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// to be similar with Connection_Set<CONNECTION_TYPE> the matrices are
// NOT allocated or deleted here, they are created when fully_connect is called.

bool generic_connection_matrix::setup (layer PTR source_layer, layer PTR destin_layer)
{
	if(source_layer==NULL)		{error(NN_INTEGR_ERR,"Invalid source layer");return false;}
	if(destin_layer==NULL)		{error(NN_INTEGR_ERR,"Invalid destination layer");return false;}

	mp_source_layer = source_layer;
	mp_destin_layer = destin_layer;

	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool generic_connection_matrix::setup (string name, layer PTR source_layer, layer PTR destin_layer)
{
	m_name = name;
	return setup(source_layer,destin_layer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool generic_connection_matrix::setup (layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers)
{

	if(!setup(source_layer,destin_layer)) return false;
	set_error_flag(error_flag_to_use);
	if(fully_connect_layers) return fully_connect();
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool generic_connection_matrix::setup (string name, layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers)
{
	m_name = name;
    return setup(source_layer, destin_layer, error_flag_to_use, fully_connect_layers);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool generic_connection_matrix::setup (string name, layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers, DATA min_random_weight, DATA max_random_weight)
{
	if (setup(name, source_layer, destin_layer, error_flag_to_use, fully_connect_layers))
	{
		set_connection_weights_random(min_random_weight, max_random_weight);
		return true;
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// already fully connected...
// to be consistent with Connection_Set, this is where m_weights and m_misc are be created.

bool generic_connection_matrix::fully_connect (bool group_by_source)
{
	if(group_by_source==true)
	{
		error(NN_INTEGR_ERR,"Connection matrices currently only support connections that are grouped by destination PE");
		return false;
	}

	if(mp_source_layer==NULL)		{error(NN_INTEGR_ERR,"Invalid source layer");return false;}
	if(mp_source_layer->size()<=0)	{error(NN_INTEGR_ERR,"Invalid source layer size");return false;}
	if(mp_destin_layer==NULL)		{error(NN_INTEGR_ERR,"Invalid destination layer");return false;}
	if(mp_destin_layer->size()<=0)	{error(NN_INTEGR_ERR,"Invalid destination layer size");return false;}

	reset_matrices();

	// create new matrices

	int source_layer_size = mp_source_layer->size();
	int destin_layer_size = mp_destin_layer->size();

	m_weights = malloc_2d(destin_layer_size,source_layer_size);
	if(m_weights==NULL)
	{
		error(NN_INTEGR_ERR,"Cannot allocate memory for connections matrix");
		return false;
	}

	if(m_requires_misc)
	{
		m_misc = malloc_2d(destin_layer_size,source_layer_size);
		if(m_misc==NULL)
		{
			free_2d(m_weights,destin_layer_size);
			m_weights = NULL;
			error(NN_INTEGR_ERR,"Cannot allocate memory for connections matrix");
			return false;
		}
	}

	m_allocated_rows_destin_layer_size = destin_layer_size;
	m_allocated_cols_source_layer_size = source_layer_size;

	m_name = m_name + " (Fully Connected)";

	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void generic_connection_matrix::set_connection_weights_random(DATA min_random_value, DATA max_random_value)
{
	if(!no_error()) return;

	if(!sizes_are_consistent())
	 {error(NN_INTEGR_ERR,"Cannot initialize weights to random");return;}

	DATA rmin = min_random_value;
	DATA rmax = max_random_value;

	if (rmin > rmax)
	{
		warning("Invalid weight initialization");
		rmin = rmax;
	}

	if (rmin == rmax)
	{
		for(int r=0;r<m_allocated_rows_destin_layer_size;r++)
			for(int c=0;c<m_allocated_cols_source_layer_size;c++)
				m_weights[r][c]=rmax;
		return;
	}

	for(int r=0;r<m_allocated_rows_destin_layer_size;r++)
		for(int c=0;c<m_allocated_cols_source_layer_size;c++)
			m_weights[r][c]= random(rmin, rmax);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool generic_connection_matrix::add_connection(const int source_pe, const int destin_pe, const DATA initial_weight)
{
	error(NN_INTEGR_ERR,"Cannot add an single connection to a matrix-based connection set (it can only be empty or fully connected)");
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool generic_connection_matrix::remove_connection(int connection_number)
{
	error(NN_INTEGR_ERR,"Cannot remove an individual connection from a matrix-based connection set (it can only be empty or fully connected)");
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Note: this function is not used or tested!

bool generic_connection_matrix::connection_properties( int connection,
                                               int REF source_component_id,
                                               int REF source_item,
                                               int REF destin_component_id,
                                               int REF destin_item,
                                               DATA REF weight)
{
	if(sizes_are_consistent())
		if((connection>=0) AND (connection<size()))
		{
			int con_row = (int)connection/m_allocated_cols_source_layer_size;
			int con_col = connection % m_allocated_cols_source_layer_size;

			source_component_id=source_layer().id();
			destin_component_id=destin_layer().id();

			if(con_row>=0)
				if(con_row<m_allocated_rows_destin_layer_size)
					if(con_col>=0)
						if(con_col<m_allocated_cols_source_layer_size)
						{
							source_item = con_col;
							destin_item = con_row;
							weight = m_weights[con_row][con_col];
							return true;
						}
		}
		warning("Cannot retreive connection properties from matrix");
		return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// input it:
// very inefficient but somewhat compatible with Connection_Set<CONNECTION_TYPE>
// Note: The from_stream for classes derived from connection_set (incl.
// generic_connection_matrix) must be improved so that the source and destination
// layer ids are used to automatically connect the connection_set to them...
//
// In case of connection matrices, this also means allocating the matrix so
// source and destination layer sizes must be known.
//
// The old (simpler) approach assumed whichever nn class was retrieving
// connection data would also setup the connection_set. As these components
// are now used more interactively in nnlib2Rcpp (via NN module), this
// has to be improved.

void generic_connection_matrix::from_stream (std::istream REF s)
{
	string comment;

	if(no_error())
	{
		component::from_stream(s);
		s >> comment >> comment;		// original_source_layer_id;
		s >> comment >> comment;		// original_destin_layer_id;

		// This is a patch, attempting to be similar to current Connection_Set.

		dllist <connection> stored_connections;

		if(no_error()) stored_connections.from_stream(s);		// changed for VC7 port,was	s >> connections;

		int max_stored_source_pe_id = -1;
		int max_stored_destin_pe_id = -1;

		// read store info as usual and TRY to determine proper matrix sizes

		if(no_error())
		for(int i=0;i<stored_connections.size();i++)
		{
			connection a = stored_connections[i];
			if(a.source_pe_id()>max_stored_source_pe_id) max_stored_source_pe_id=a.source_pe_id();
			if(a.destin_pe_id()>max_stored_destin_pe_id) max_stored_destin_pe_id=a.destin_pe_id();
		}

		int stored_source_layer_size = max_stored_source_pe_id + 1;
		int stored_destin_layer_size = max_stored_destin_pe_id + 1;

		if((stored_source_layer_size<=0) OR (stored_destin_layer_size<=0))
			{
			error(NN_IOFILE_ERR,"Error loading connections");
			return;
			}

		// now try to allocate matrices using stored sizes (and hope for the best) <- sad I wrote this a few days before the terrible 'pame kai opou vgei' accident [or it would be if Greece was run half-decently at any point in the past 200 years; now you can call it a crime]

		reset_matrices();

		m_weights = malloc_2d(stored_destin_layer_size,stored_source_layer_size);
		if(m_weights==NULL)
		{
			error(NN_INTEGR_ERR,"Cannot allocate memory for connections matrix");
			return;
		}

		if(m_requires_misc)
		{
			m_misc = malloc_2d(stored_destin_layer_size,stored_source_layer_size);
			if(m_misc==NULL)
			{
				free_2d(m_weights,stored_destin_layer_size);
				m_weights = NULL;
				error(NN_INTEGR_ERR,"Cannot allocate memory for connections matrix");
				return;
			}
		}

		m_allocated_rows_destin_layer_size = stored_destin_layer_size;
		m_allocated_cols_source_layer_size = stored_source_layer_size;

		int r=0;
		int c=0;
		int num_items = 0;

		if(stored_connections.size()==size())
			num_items = stored_connections.size();

		for(int i=0;i<num_items;i++)
		{
			connection a = stored_connections[i];
			r = a.destin_pe_id();
			c = a.source_pe_id();
			m_weights[r][c]=a.weight();
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// output it :
// very inefficient but somewhat compatible with Connection_Set<CONNECTION_TYPE>

void generic_connection_matrix::to_stream (std::ostream REF s)
{
	if(no_error())
	{
		component::to_stream(s);
		if((mp_source_layer==NULL)OR(mp_destin_layer==NULL)) return;
		s << "SourceCom: " << mp_source_layer->id() << "\n";		// this is the id, not the original pointer.
		s << "DestinCom: " << mp_destin_layer->id() << "\n";		// this is the id, not the original pointer.

		dllist <connection> connections_to_store;

		connection temp_connection;									// there is a much simpler and efficient way to do this, but lets follow what Connection_Set does.

		for(int r=0;r<m_allocated_rows_destin_layer_size;r++)
			for(int c=0;c<m_allocated_cols_source_layer_size;c++)
			{
				temp_connection.setup(this, c, r, m_weights[r][c]);
				connections_to_store.append(temp_connection);
			}

		connections_to_store.to_stream(s);
	}
}


} // end of namespace nnlib2


