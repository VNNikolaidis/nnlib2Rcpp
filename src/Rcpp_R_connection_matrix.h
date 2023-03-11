//		----------------------------------------------------------
//		(C)2020       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    	connection set component for NN module (nnlib2Rcpp)
//		-----------------------------------------------------------

#include "nnlib2.h"

#ifdef NNLIB2_FOR_RCPP

#ifndef RCPP_NN_R_CONNECTION_MATRIX
#define RCPP_NN_R_CONNECTION_MATRIX

#include "connection_matrix.h"

using namespace nnlib2;

//--------------------------------------------------------------------------------

class R_connection_matrix : public generic_connection_matrix
{
protected:

	string m_R_function_encode;				// the R function to be called
	string m_R_function_recall;				// the R function to be called

public:

	R_connection_matrix(string encode_FUN = "", string recall_FUN = "");
	R_connection_matrix(string name,
                    	string encode_FUN = "" ,
                    	string recall_FUN = "",
                    	bool requires_misc = false);

	void encode ();
	void recall ();

private:

	bool collect_data_for_R_call(NumericMatrix REF weights,
                              NumericVector REF v_source_in,
                              NumericVector REF v_source_out,
                              NumericVector REF v_source_misc,
                              NumericVector REF v_destin_in,
                              NumericVector REF v_destin_out,
                              NumericVector REF v_destin_misc,
                              NumericMatrix REF misc);

};

//--------------------------------------------------------------------------------

R_connection_matrix::R_connection_matrix(string encode_FUN, string recall_FUN)
	:generic_connection_matrix()
{
	m_R_function_encode = encode_FUN;
	m_R_function_recall = recall_FUN;
	m_name = "R-connection matrix(" + encode_FUN + "," + recall_FUN + ")";
}

//--------------------------------------------------------------------------------

R_connection_matrix::R_connection_matrix(string name,
                                         string encode_FUN,
                                         string recall_FUN,
                                         bool requires_misc)
	:generic_connection_matrix(name,requires_misc)
{
	m_R_function_encode = encode_FUN;
	m_R_function_recall = recall_FUN;
	m_name = name + "(" + encode_FUN + "," + recall_FUN + ")";
}

//--------------------------------------------------------------------------------
// Collect various pieces of data that may be useful if sent to the R function...
// They will be placed in an Environment

// copy m_weights to local NumericMatrix weights. This is VERY inefficient but safer. Must be improved.
// Notes:
// - a much more efficient way seems possible if m_weights was allocated as a single vector (may be done in the future).
// - even better if m_weights was a NumericMatrix but this would break compatibility on nnlib2 for applications outside Rcpp.
// - there may be other ways to avoid this copying, but this is ok as a starting point for experimenting with small sets
// - The weights matrix is currently a transposed version of the m_weights matrix. As R stores matrices
//   in a column-first order it will be more convenient (on the R code) to perform common operations
//	 (s.a multiply weights by source's output) if stored this way. This also holds for 'misc' (if any).

bool R_connection_matrix::collect_data_for_R_call(NumericMatrix REF weights,
                                                  NumericVector REF v_source_in,
                                                  NumericVector REF v_source_out,
                                                  NumericVector REF v_source_misc,
                                                  NumericVector REF v_destin_in,
                                                  NumericVector REF v_destin_out,
                                                  NumericVector REF v_destin_misc,
                                                  NumericMatrix REF misc)
{
	int source_size = source_layer().size();
	int destin_size = destin_layer().size();
	if(source_size<=0) return false;
	if(destin_size<=0) return false;
	if(!sizes_are_consistent()) return false;

	CharacterVector s_names = CharacterVector(source_size);
	CharacterVector d_names = CharacterVector(destin_size);

	for(int s=0;s<source_size;s++) s_names(s)="S"+std::to_string(s);
	for(int d=0;d<destin_size;d++) d_names(d)="D"+std::to_string(d);

	weights = NumericMatrix(source_size,destin_size);
	for(int d=0;d<destin_size;d++)
		for(int s=0;s<source_size;s++)
			weights(s,d)=m_weights[d][s];		// transposed (see note above).

	rownames(weights)=s_names;
	colnames(weights)=d_names;

	if(uses_misc())
	{
		misc = NumericMatrix(source_size,destin_size);
		for(int d=0;d<destin_size;d++)
			for(int s=0;s<source_size;s++)
				misc(s,d)=m_misc[d][s];			// transposed (see note above).

		rownames(misc)=s_names;
		colnames(misc)=d_names;
	}

	// data from source layer:

	// collect the input of source layer (usually invalid or zeroed but may be useful in user-defined layers)...
	v_source_in   = NumericVector (source_size);
	double * fp_v_source_in = REAL(v_source_in);
	if(!source_layer().get_input(fp_v_source_in,source_size)) return false;

	// collect the output of source layer...
	v_source_out  = NumericVector (source_size);
	double * fp_v_source_out = REAL(v_source_out);
	if(!source_layer().output_data_to_vector(fp_v_source_out,source_size)) return false;

	// collect misc values of source layer...
	v_source_misc = NumericVector (source_size);
	double * fp_v_source_misc = REAL(v_source_misc);
	if(!source_layer().get_misc(fp_v_source_misc,source_size)) return false;

	// data from destination layer:

	// collect the input of destination layer...
	v_destin_in   = NumericVector (destin_size);
	double * fp_v_destin_in = REAL(v_destin_in);
	if(!destin_layer().get_input(fp_v_destin_in,destin_size)) return false;

	// collect the output of destination layer...
	v_destin_out  = NumericVector (destin_size);
	double * fp_v_destin_out = REAL(v_destin_out);
	if(!destin_layer().output_data_to_vector(fp_v_destin_out,destin_size)) return false;

	// collect misc values of destination layer...
	v_destin_misc = NumericVector (destin_size);
	double * fp_v_destin_misc = REAL(v_destin_misc);
	if(!destin_layer().get_misc(fp_v_destin_misc,destin_size)) return false;

	return true;
}

//--------------------------------------------------------------------------------

void R_connection_matrix::encode()
{
	if(m_R_function_encode=="")	return;

	int source_size = source_layer().size();
	int destin_size = destin_layer().size();

	NumericMatrix weights;
	NumericVector v_source_in;
	NumericVector v_source_out;
	NumericVector v_source_misc;
	NumericVector v_destin_in;
	NumericVector v_destin_out;
	NumericVector v_destin_misc;
	NumericMatrix misc;

	if(NOT collect_data_for_R_call(weights, v_source_in, v_source_out, v_source_misc,
                                			v_destin_in, v_destin_out, v_destin_misc, misc))
	{
	warning("Connections cannot encode, preparing R data failed");
	return;
	}

	// Call the R function...

	Function f(m_R_function_encode);
	List ret_vals = f(	Named("WEIGHTS")=weights,
            			Named("SOURCE_INPUT")=v_source_in,
            			Named("SOURCE_OUTPUT")=v_source_out,
            			Named("SOURCE_MISC")=v_source_misc,
            			Named("DESTINATION_INPUT")=v_destin_in,
            			Named("DESTINATION_OUTPUT")=v_destin_out,
            			Named("DESTINATION_MISC")=v_destin_misc,
            			Named("MISC_VALUES")=misc );

	if(ret_vals.length()<=0)
	{
		//	warning("No data was returned from "+m_R_function_encode);
		return;
	}

	if(ret_vals.containsElementNamed("WEIGHTS"))
	{
		NumericMatrix weights = ret_vals["WEIGHTS"];
		if(weights.nrow()!=source_size) {warning("Unexpected weight matrix sizes (number of rows)");return;}
		if(weights.ncol()!=destin_size) {warning("Unexpected weight matrix sizes (number of columns)");return;}

		// Copy local NumericMatrix weights to m_weights. This is VERY inefficient, see notes above.
		// Important Note:
		// only connection set weights (and possibly misc) are modified during a connection set "encode".

		for(int d=0;d<destin_size;d++)
			for(int s=0;s<source_size;s++)
				m_weights[d][s]=weights(s,d);	// transposed (see note above).
	}

	if(uses_misc())
		if(ret_vals.containsElementNamed("MISC"))
		{
			NumericMatrix misc = ret_vals["MISC"];
			if(misc.nrow()!=source_size) {warning("Unexpected misc matrix sizes (number of rows)");return;}
			if(misc.ncol()!=destin_size) {warning("Unexpected misc matrix sizes (number of columns)");return;}

			// Copy local NumericMatrix misc to m_misc. This is VERY inefficient, see notes above.
			// Important Note:
			// only connection set weights (and possibly misc) are modified during a connection set "encode".

			for(int d=0;d<destin_size;d++)
				for(int s=0;s<source_size;s++)
					m_misc[d][s]=misc(s,d);	// transposed (see note above).
		}

}

//--------------------------------------------------------------------------------

void R_connection_matrix::recall()
{
	if(m_R_function_recall=="")	return;

	NumericMatrix weights;
	NumericVector v_source_in;
	NumericVector v_source_out;
	NumericVector v_source_misc;
	NumericVector v_destin_in;
	NumericVector v_destin_out;
	NumericVector v_destin_misc;
	NumericMatrix misc;

	if(NOT collect_data_for_R_call(weights, v_source_in, v_source_out, v_source_misc,
                                   v_destin_in, v_destin_out, v_destin_misc, misc))
	{
		warning("Connections cannot recall, preparing R data failed");
		return;
	}

	// Call the R function...

	Function f(m_R_function_recall);

	NumericMatrix out_vals = f(	Named("WEIGHTS")=weights,
            					Named("SOURCE_INPUT")=v_source_in,
            					Named("SOURCE_OUTPUT")=v_source_out,
            					Named("SOURCE_MISC")=v_source_misc,
            					Named("DESTINATION_INPUT")=v_destin_in,
            					Named("DESTINATION_OUTPUT")=v_destin_out,
            					Named("DESTINATION_MISC")=v_destin_misc,
            					Named("MISC_VALUES")=misc);

	// Once finished, set local new_v_destin_in values as the destination input

	int destin_size = destin_layer().size();

	if(out_vals.nrow()<=0)
	{
		warning("No data will be sent to destination layer");
		return;
	}

	if(out_vals.ncol()!=destin_size)
	{
		error(NN_INTEGR_ERR,"Number of columns in returned data not equal to destination layer size (cannot send column values as input to corresponding PEs)");
		return;
	}

	for(int pe=0;pe<destin_size;pe++)
		for(int row=0;row<out_vals.nrow();row++)
			destin_layer().PE(pe).receive_input_value(out_vals(row,pe));
}

//---------------------------------------------------------------------------------

#endif // RCPP_NN_R_CONNECTION_MATRIX
#endif // NNLIB2_FOR_RCPP

