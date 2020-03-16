//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis         All rights reserved.
//		-----------------------------------------------------------
//		aux_control.h		 							Version 0.1
//		-----------------------------------------------------------
//		component for misc functionality
//		-----------------------------------------------------------
//      This is meant to fit within NN's topology, & provide encode/
//      recall functions that can be used to implement additional,
//      user-defined functionality. This may be control code or
//      functionality such as output, user-break, communication w/
//      other components, creation/deletion of components, data
//      operations (which_max,feature extraction) etc.
//		-----------------------------------------------------------

#include "aux_control.h"

namespace nnlib2 {

/*-----------------------------------------------------------------------*/
//		component for misc functionality
/*-----------------------------------------------------------------------*/

aux_control::aux_control()
    {
    m_type = cmpnt_aux_control;
    }

/*-----------------------------------------------------------------------*/
// example of	component for misc functionality
/*-----------------------------------------------------------------------*/

aux_txt_printer::aux_txt_printer(bool PTR error_flag_to_use)
{
	mp_layer=NULL;
	m_name="Print component";
	set_error_flag(error_flag_to_use);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

aux_txt_printer::aux_txt_printer(layer PTR p_layer, bool PTR error_flag_to_use)
		{
		mp_layer=NULL;
		m_name="Print component";
		setup(p_layer,error_flag_to_use);
		}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void aux_txt_printer::setup(layer PTR p_layer, bool PTR error_flag_to_use)
{
	mp_layer=p_layer;
	set_error_flag(error_flag_to_use);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void aux_txt_printer::print()
	{
		if(no_error())
			if(mp_layer!=NULL)
			{
				int s=mp_layer->size();
				TEXTOUT << mp_layer->name() << " layer returns these output values: ";
				for (int i=0;i<s;i++) TEXTOUT << mp_layer->PE(i).output << " ";
				TEXTOUT << "\n";
			}
	}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void aux_txt_printer::encode()
{
print();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void aux_txt_printer::recall()
{
print();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}  // end of namespace nnlib2

