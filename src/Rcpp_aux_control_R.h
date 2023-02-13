//		----------------------------------------------------------
//		(C)2020       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    	Rcpp glue code for building custom NNs
//		-----------------------------------------------------------

#ifdef NNLIB2_FOR_RCPP

#ifndef RCPP_NN_AUX_CONTROL_R
#define RCPP_NN_AUX_CONTROL_R

#include "nnlib2.h"
#include "additional_parts.h"		// header for user-defined parts (components etc)

#define AUX_CONTROL_R_AUTODETERMINE_PREV (-1000)
#define AUX_CONTROL_R_AUTODETERMINE_NEXT (-2000)

using namespace nnlib2;

//--------------------------------------------------------------------------------

class aux_control_R : public aux_control
{
protected:

	NumericVector m_data;

	nn   * mp_nn;						// the NN for input and output

	int  m_component_index_for_input;	// component on the NN to get input from (if valid)
	int  m_component_index_for_output;	// component on the NN to send output to (if valid)

	string m_R_function;				// the R function to be called

	bool m_active_on_recall;			// perform R when recalling
	bool m_active_on_encode;			// perform R when encoding

	bool m_ignore_result;	// ignores the (possibly invalid) result of the R function and forwards current values

	void do_R_magic();

	void get_source_dest_component_indexes(int REF source_index, int REF dest_index);

public:

	aux_control_R(string R_function,
               nn * p_nn,
               string input_mode,
               int input_nn_component,
               string output_mode,
               int output_nn_component,
               bool active_on_encode,
               bool active_on_recall,
               bool ignore_result );

	int size();															   // size of m_data (input and output). NOTE: this changes dynamically!

	bool input_data_from_vector(DATA * data, int dimension);               // overrides virtual method in data_receiver, sets values for m_data
	bool output_data_to_vector(DATA * buffer, int dimension);              // overrides virtual method in data_provider, gets values from m_data

	bool send_input_to (int index, DATA d);                                // overrides virtual method in data_receiver, sets value to corresponding m_data item
	DATA get_output_from (int index);                                      // overrides virtual method in data_provider, gets value from corresponding m_data item

	void encode ();
	void recall ();

	bool read_data_from_NN_component(int component_index);
	bool write_data_to_NN_component(int component_index);

	string m_input_mode;
	string m_output_mode;
};

//--------------------------------------------------------------------------------

aux_control_R::aux_control_R(string R_function,
                             nn * p_nn,
                             string input_mode,
                             int input_nn_component,
                             string output_mode,
                             int output_nn_component,
                             bool active_on_encode,
                             bool active_on_recall,
                             bool ignore_result)
{
	m_name="R-component";
	m_R_function = "";

	if(p_nn==NULL) {error(NN_INTEGR_ERR,"Cannot create R component"); return;}

	if(
		(input_mode!="none") AND
	(input_mode!="input of") AND
	(input_mode!="output of") AND
	(input_mode!="weights at") AND
	(input_mode!="biases at") AND
	(input_mode!="misc at"))
	{
		warning("Input mode must be 'none', 'input of','output of','weights at','biases at' or 'misc at'. Changed to 'none");
		m_input_mode = "none";
	}
	else m_input_mode = input_mode;

	if(
		(output_mode!="none") AND
	(output_mode!="to input") AND
	(output_mode!="to output") AND
	(output_mode!="to weights") AND
	(output_mode!="to biases") AND
	(output_mode!="to misc"))
	{
		warning("Output mode must be 'none', 'to input','to output','to weights','to biases' or 'to misc'. Changed to 'none");
		m_output_mode = "none";
	}
	else m_output_mode = output_mode;

	mp_nn = p_nn;
	m_R_function = R_function;
	m_name="simple-R-component ("+R_function+")";

	m_component_index_for_input = input_nn_component;
	m_component_index_for_output = output_nn_component;
	m_active_on_encode = active_on_encode;
	m_active_on_recall = active_on_recall;
	m_ignore_result = ignore_result;
}

//--------------------------------------------------------------------------------

void aux_control_R::do_R_magic()
{
	Function caller_of_R_function(m_R_function);

	int input_comp_index  = -1;
	int output_comp_index = -1;

	get_source_dest_component_indexes(input_comp_index, output_comp_index);

	read_data_from_NN_component(input_comp_index);

	TEXTOUT << "@@@@@@@ id="<< m_id << " i do R magic ( " << m_R_function << " ) \n";
	TEXTOUT << "@@@@@@@ topology index is " << mp_nn->component_topology_index_from_id(m_id) << "\n";
	TEXTOUT << "@@@@@@@ id="<< m_id << " i read " << input_comp_index << " and (maybe) write to " << output_comp_index << " \n";

	TEXTOUT << "current data = " <<  m_data << " changed to ";

	//	if(m_data.length()>0)
	{
		if(m_ignore_result) caller_of_R_function(m_data);
		else m_data = caller_of_R_function(m_data);
	}

	write_data_to_NN_component(output_comp_index);

	TEXTOUT << "new data = " <<  m_data << "\n";
}


//--------------------------------------------------------------------------------
// unlike other NN components, size is dynamic and depends on R function

int aux_control_R::size() {return m_data.length();}

//--------------------------------------------------------------------------------

void aux_control_R::encode ()
{
	if(m_active_on_encode) do_R_magic();
}

//--------------------------------------------------------------------------------

void aux_control_R::recall ()
{
	if(m_active_on_recall) do_R_magic();
}

//--------------------------------------------------------------------------------
// overrides virtual method in data_receiver, sets values for m_data

bool aux_control_R::input_data_from_vector(DATA * data, int dimension)
{
	if(dimension<0) return false;
	if(data==NULL) return false;
	m_data = NumericVector(dimension);
	for(int i=0;i<dimension;i++) m_data[i]=data[i];
	return true;
}

//--------------------------------------------------------------------------------
// overrides virtual method in data_provider, gets values from m_data

bool aux_control_R::output_data_to_vector(DATA * buffer, int dimension)
{
	if(dimension<0) return false;
	if(buffer==NULL) return false;
	if(dimension!=m_data.length()) {warning("Incompatible vector sizes");return false;}   // is this too strict?
	for(int i=0;i<dimension;i++) buffer[i]=m_data[i];
	return true;
}

//--------------------------------------------------------------------------------
// overrides virtual method in data_receiver, sets value to corresponding m_data index

bool aux_control_R::send_input_to(int index, DATA d)
{
	if(index>=m_data.length())
	{ error(NN_INTEGR_ERR,"R control cannot change specified value"); return 0; }
	m_data[index]=d; return true;
}


//--------------------------------------------------------------------------------
// overrides virtual method in data_provider, gets value from corresponding m_data index

DATA aux_control_R::get_output_from (int index)
{
	if(index>=m_data.length())
	{ error(NN_INTEGR_ERR,"R control cannot return specified value"); return 0; }
	return m_data[index];
}

//--------------------------------------------------------------------------------
// true if some data was read

bool aux_control_R::read_data_from_NN_component(int component_index)
{
	if(component_index<0) return false;

	if(mp_nn==NULL)
	{ error(NN_INTEGR_ERR,"R control not connected to NN"); return false; }

	component * pc = mp_nn->component_from_topology_index(component_index);
	if(pc==NULL)
	{ error(NN_INTEGR_ERR,"R control cannot locate NN component for input"); return false; }

	if (m_input_mode=="none") return true;

	int input_size = pc->size();
	if(input_size <= 0)	return false;

	// note: this component gets data and adapts size dynamically

	m_data = NumericVector(input_size);
	DATA * fp_data = REAL(m_data);                  // my (lame?) way to interface with R

	if (m_input_mode == "input of")
	{
		if(!mp_nn->get_input_at_component(component_index,fp_data,input_size))
		{error(NN_INTEGR_ERR,"R control cannot get input of specified NN component"); return false;
		return true;
		}
	}

	if(m_input_mode == "output of")
	{
		if(!mp_nn->get_output_from_component(component_index,fp_data,input_size))
		{error(NN_INTEGR_ERR,"R control cannot get output of specified NN component"); return false; }
		return true;
	}

	if(m_input_mode == "weights at")
	{
		if(!mp_nn->get_weights_at_component(component_index,fp_data,input_size))
		{error(NN_INTEGR_ERR,"R control cannot get weights at specified NN component"); return false; }
		return true;
	}

	if(m_input_mode == "biases at")
	{
		if(!mp_nn->get_biases_at_component(component_index,fp_data,input_size))
		{error(NN_INTEGR_ERR,"R control cannot get biases at specified NN component"); return false; }
		return true;
	}

	if(m_input_mode == "misc at")
	{
		if(!mp_nn->get_misc_at_component(component_index,fp_data,input_size))
		{error(NN_INTEGR_ERR,"R control cannot get misc values at specified NN component"); return false; }
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------
// true if some data was written

bool aux_control_R::write_data_to_NN_component(int component_index)
{
	if(m_data.length()<=0) return true;		// no data to write...

	if(component_index<0) return false;

	if(mp_nn==NULL)
	{ error(NN_INTEGR_ERR,"R control not connected to NN"); return false; }

	if (m_output_mode=="none") return true;

	component * pc = mp_nn->component_from_topology_index(component_index);
	if(pc==NULL)
	{ error(NN_INTEGR_ERR,"R control cannot locate NN component for output"); return false; }

	DATA * fp_data = REAL(m_data);                  // my (lame?) way to interface with R

	if(m_output_mode == "to input")
	{
		if(!mp_nn->set_input_at_component(component_index,fp_data,m_data.length()))
		{error(NN_INTEGR_ERR,"R control cannot change input of specified NN component"); return false; }
		return true;
	}

	if(m_output_mode == "to output")
	{
		if(!mp_nn->set_output_at_component(component_index,fp_data,m_data.length()))
		{error(NN_INTEGR_ERR,"R control cannot change output of specified NN component"); return false; }
		return true;
	}

	if(m_output_mode == "to weights")
	{
		if(!mp_nn->set_weights_at_component(component_index,fp_data,m_data.length()))
		{error(NN_INTEGR_ERR,"R control cannot change weights at specified NN component"); return false; }
		return true;
	}

	if(m_output_mode == "to biases")
	{
		if(!mp_nn->set_biases_at_component(component_index,fp_data,m_data.length()))
		{error(NN_INTEGR_ERR,"R control cannot change biases at specified NN component"); return false; }
		return true;
	}

	if(m_output_mode == "to misc")
	{
		if(!mp_nn->set_misc_at_component(component_index,fp_data,m_data.length()))
		{error(NN_INTEGR_ERR,"R control cannot change biases at specified NN component"); return false; }
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------

void aux_control_R::get_source_dest_component_indexes(int REF source_index, int REF dest_index)
{
	if(mp_nn==NULL) return;
	int my_index = mp_nn->component_topology_index_from_id(m_id);

	source_index = m_component_index_for_input;
	if(source_index == AUX_CONTROL_R_AUTODETERMINE_PREV)
		source_index = my_index - 1;
	if(source_index == AUX_CONTROL_R_AUTODETERMINE_NEXT)
		source_index = my_index + 1;

	dest_index = m_component_index_for_output;
	if(dest_index == AUX_CONTROL_R_AUTODETERMINE_PREV)
		dest_index = my_index - 1;
	if(dest_index == AUX_CONTROL_R_AUTODETERMINE_NEXT)
		dest_index = my_index + 1;
}

//--------------------------------------------------------------------------------

#endif // RCPP_NN_AUX_CONTROL_R
#endif // NNLIB2_FOR_RCPP
