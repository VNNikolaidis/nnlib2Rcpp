//		----------------------------------------------------------
//		(c)2023  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		connection_matrix.h								Version 0.1
//		-----------------------------------------------------------
//		Definition/implementation a parent class for blocks ("sets")
//		of connections represented as a matrix. A very quick draft
//		for the implementation, many needed features are missing,
//		Note:
//		This does not use 'connection' class, so all encoding
//		and recall (map) code should be withing the class that
//		inherits this one.
//		-----------------------------------------------------------

#ifndef NN_CONNECTION_MATRIX_H
#define NN_CONNECTION_MATRIX_H

#include "connection_set.h"

namespace nnlib2 {

/*-----------------------------------------------------------------------*/
/* (Type B) Base-class for a simple matrix-based connection set  		 */
/*-----------------------------------------------------------------------*/

class generic_connection_matrix : public connection_set
{
private:

	layer PTR mp_source_layer;                                     // note: this is not PE_TYPE specific (layer)
	layer PTR mp_destin_layer;                                     // note: this is not PE_TYPE specific (layer)

	int m_allocated_rows_destin_layer_size;
	int m_allocated_cols_source_layer_size;

	bool m_requires_misc;										   // misc (stored in m_misc) is an optional extra value (besides weight) stored per connection (and associated to it) for its own temporary use (not saved)

protected:

	DATA ** m_weights;											   // the weight of each connection
	DATA ** m_misc;												   // an optional extra value (besides weight) stored per connection (and associated to it) for its own temporary use (not saved)

	bool uses_misc() {return m_requires_misc;}

	bool sizes_are_consistent();
	void reset_matrices();

public:

	generic_connection_matrix();
	generic_connection_matrix(string name, bool requires_misc = false);

	~generic_connection_matrix();
;
	int size ();

	layer REF source_layer();
	layer REF destin_layer();
	bool has_source_layer();
	bool has_destin_layer();
	pe REF source_pe(int c);
	pe REF destin_pe(int c);
	DATA get_connection_weight(int connection);
	DATA get_connection_weight(int source_pe, int destin_pe);
	bool set_connection_weight(int connection, DATA value);
	bool set_connection_weight(int source_pe, int destin_pe, DATA value);
	bool set_misc(DATA * data, int dimension);
	bool get_misc(DATA * buffer, int dimension);
	bool setup (layer PTR source_layer, layer PTR destin_layer);
	bool setup (string name, layer PTR source_layer, layer PTR destin_layer);
	bool setup (layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers = false);
	bool setup (string name, layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers = false);
	bool setup (string name, layer PTR source_layer, layer PTR destin_layer, bool PTR error_flag_to_use, bool fully_connect_layers, DATA min_random_weight, DATA max_random_weight);
	bool add_connection(const int source_pe, const int destin_pe, const DATA initial_weight);
	bool remove_connection(int connection_number);
	bool connection_properties( int connection,int REF source_component_id,int REF source_item,int REF destin_component_id,int REF destin_item, DATA REF weight);
	void set_connection_weights_random(DATA min_random_value, DATA max_random_value);
	bool fully_connect (bool group_by_source = false);
	void from_stream (std::istream REF s);
	void to_stream (std::ostream REF s);
};

} // end of namespace nnlib2

#endif // NN_CONNECTION_MATRIX_H
