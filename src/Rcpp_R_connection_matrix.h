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

class connection_matrix_R : public generic_connection_matrix
{
protected:

	string m_R_function_encode;				// the R function to be called
	string m_R_function_recall;				// the R function to be called

public:

	connection_matrix_R(string encode_FUN = "", string recall_FUN = "");
	connection_matrix_R(string name,
                    	string encode_FUN = "" ,
                    	string recall_FUN = "",
                    	bool requires_misc = false);

	void encode ();
	void recall ();
};

//--------------------------------------------------------------------------------

connection_matrix_R::connection_matrix_R(string encode_FUN, string recall_FUN)
	:generic_connection_matrix()
{
	m_name = "R Connection matrix";
	m_R_function_encode = encode_FUN;
	m_R_function_recall = recall_FUN;
}

//--------------------------------------------------------------------------------

connection_matrix_R::connection_matrix_R(string name, string encode_FUN, string recall_FUN,bool requires_misc)
	:generic_connection_matrix(name,requires_misc)
{
	m_name = name;
	m_R_function_encode = encode_FUN;
	m_R_function_recall = recall_FUN;
}


#endif // RCPP_NN_R_CONNECTION_MATRIX
#endif // NNLIB2_FOR_RCPP
