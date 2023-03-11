//		----------------------------------------------------------
//		(C)2020       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    	layer component for NN module (nnlib2Rcpp)
//		-----------------------------------------------------------

#include "nnlib2.h"

#ifdef NNLIB2_FOR_RCPP

#ifndef RCPP_NN_R_LAYER
#define RCPP_NN_R_LAYER

#include "layer.h"

using namespace nnlib2;

//--------------------------------------------------------------------------------

class R_layer : public pe_layer
{
protected:

	string m_R_function_encode;				// the R function to be called
	string m_R_function_recall;				// the R function to be called

private:

	bool collect_data_for_R_call(NumericVector REF input,
                            	 NumericMatrix REF input_q,
                            	 NumericVector REF bias,
                            	 NumericVector REF misc,
                            	 NumericVector REF output);

public:

	R_layer(string name,
        	int size,
           	string encode_FUN = "" ,
           	string recall_FUN = "");

	void encode ();
	void recall ();
};

//--------------------------------------------------------------------------------

R_layer::R_layer(string name,
                 int size,
                 string encode_FUN,
                 string recall_FUN)
	:pe_layer(name,size)
{
	m_R_function_encode = encode_FUN;
	m_R_function_recall = recall_FUN;
	m_name = m_name + "(" + encode_FUN + "," + recall_FUN + ")";
}


//--------------------------------------------------------------------------------

bool R_layer::collect_data_for_R_call(	NumericVector REF input,
            			            	NumericMatrix REF input_q,
                            			NumericVector REF bias,
                        				NumericVector REF misc,
                            			NumericVector REF output)
{
	if(size()<=0) return false;

	// collect the current value in PE input registers...
	input = NumericVector(size());
	double * fp_v_input = REAL(input);
	for (int i = 0; i < size(); i++) fp_v_input[i] = pes[i].input;

	// determine how many values are waiting to be input via received_values...
	int maxw = 0;
	for (int i = 0; i < size(); i++)
		if(maxw < pes[i].number_of_received_input_values())
			maxw = pes[i].number_of_received_input_values();

	// ... and collect them

	if(maxw>0)
	{
		input_q = NumericMatrix(maxw,size());
		for (int i = 0; i < size(); i++)
			for(int v = 0; v < pes[i].number_of_received_input_values(); v++)
				input_q(v,i) = pes[i].received_input_value(v);
	}


	// collect the current value in PE bias registers...
	bias = NumericVector(size());
	double * fp_v_bias = REAL(bias);
	for (int i = 0; i < size(); i++) fp_v_bias[i] = pes[i].bias;

	// collect the current value in PE misc registers...
	misc = NumericVector(size());
	double * fp_v_misc = REAL(misc);
	for (int i = 0; i < size(); i++) fp_v_misc[i] = pes[i].misc;

	// collect the current value in PE output registers...
	output = NumericVector(size());
	double * fp_v_output = REAL(output);
	for (int i = 0; i < size(); i++) fp_v_output[i] = pes[i].output;

	return true;
}

//--------------------------------------------------------------------------------

void R_layer::encode()
{
	if(m_R_function_encode!="")
	{
		NumericVector input;
		NumericMatrix input_q;
		NumericVector bias;
		NumericVector misc;
		NumericVector output;

		if(NOT collect_data_for_R_call(input,input_q,bias,misc,output))
		{
			warning("Layer cannot encode, preparing R data failed");
			return;
		}

		// Call the R function...

		Function f(m_R_function_encode);

		List ret_vals = f(	Named("INPUT")=input,
	                        Named("INPUT_Q")=input_q,
	                        Named("BIAS")=bias,
	                        Named("MISC")=misc,
	                        Named("OUTPUT")=output );

		// Once finished, set local new_v_destin_in values as the destination input

		if(ret_vals.length()<=0)
		{
		//	warning("No data was returned from "+m_R_function_encode);
			return;
		}

		if(ret_vals.containsElementNamed("INPUT"))	 {warning("Note: Layer will not set INPUT to specific returned values during encode.");}
		if(ret_vals.containsElementNamed("INPUT_Q")) {warning("Note: Layer will not set INPUT_Q (PE received values) to specific returned values during encode.");}

		if(ret_vals.containsElementNamed("BIAS"))
		{
		bias = ret_vals["BIAS"];
		if((bias.length()>0) AND (bias.length()!=size()))
			{
			error(NN_INTEGR_ERR,"Length of returned BIAS values not equal to layer size");
			return;
			}
		for (int i=0; i<bias.length(); i++)
			pes[i].bias = bias(i);
		}

		if(ret_vals.containsElementNamed("MISC"))
		{
			misc = ret_vals["MISC"];
			if((misc.length()>0) AND (misc.length()!=size()))
			{
				error(NN_INTEGR_ERR,"Length of returned MISC values not equal to layer size");
				return;
			}
		for (int i=0; i<misc.length(); i++)
			pes[i].misc = misc(i);
		}

		if(ret_vals.containsElementNamed("OUTPUT"))
		{
			output = ret_vals["OUTPUT"];
			if((output.length()>0) AND (output.length()!=size()))
			{
				error(NN_INTEGR_ERR,"Length of returned OUTPUT not equal to layer size");
				return;
			}
			for (int i=0; i<output.length(); i++)
				pes[i].output = output(i);
		}
	}

	// reset input (it has been processed)

	for (int i=0; i<size(); i++)
	{
		pes[i].input = 0;
		pes[i].reset_received_values();
	}
}


//--------------------------------------------------------------------------------

void R_layer::recall()
{
	if(m_R_function_recall!="")
	{
		NumericVector input;
	    NumericMatrix input_q;
	    NumericVector bias;
	    NumericVector misc;
	    NumericVector output;

	    if(NOT collect_data_for_R_call(input,input_q,bias,misc,output))
	    {
	    	warning("Layer cannot recall, preparing R data failed");
	    	return;
	    }

	    // Call the R function...

	    Function f(m_R_function_recall);

	    NumericVector ret_vals = f(	Named("INPUT")=input,
	                                Named("INPUT_Q")=input_q,
	                                Named("BIAS")=bias,
	                                Named("MISC")=misc,
	                                Named("OUTPUT")=output);

	    // Once finished, set local new_v_destin_in values as the destination input

	    if(ret_vals.length()<=0)
	    {
	    	warning("No data was returned from R as PE output");
	    	return;
	    }

	    if(ret_vals.length()!=size())
	    {
	    	error(NN_INTEGR_ERR,"Returned data not equal layer size");
	    	return;
	    }

	    for(int i=0;i<size();i++)
	    	pes[i].output = ret_vals(i);
	}

	// reset input (it has been processed)

	for (int i=0; i<size(); i++)
	{
		pes[i].input = 0;
		pes[i].reset_received_values();
	}
}

//---------------------------------------------------------------------------------

#endif // RCPP_NN_R_LAYER
#endif // NNLIB2_FOR_RCPP

