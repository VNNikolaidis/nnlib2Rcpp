//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		spare_parts.h									Version 0.1
//		-----------------------------------------------------------
//		Definition-implementation of various NN parts and components
//		-----------------------------------------------------------

#ifndef NNLIB2_SPARE_PARTS_H
#define NNLIB2_SPARE_PARTS_H

#include "nn.h"

namespace nnlib2 {

//--------------------------------------------------------------------------------------------
// a pe that returns which input had the largest value
// (written in the least efficient (since received_values is a linked list) but most readable way)

class which_max_pe : public pe
{
	DATA input_function()
	{
		if (received_values.size() <= 0) return -1;   // no received inputs

		int current_max_position = 0;
		DATA current_max_value = received_values[0];

		for(int i=1;i<received_values.size();i++)
		{
			DATA new_value = received_values[i];
			if(new_value>current_max_value)
			{
				current_max_value = new_value;
				current_max_position = i;
			}
		}

		reset_received_values();                      // inputs were processed, delete them
		input = current_max_position;				  // it is recommended to store value at internal input variable
		return input;
	}
};

//--------------------------------------------------------------------------------------------
// a layer of which_max_pes

typedef Layer<which_max_pe> which_max_layer;

//--------------------------------------------------------------------------------------------

} // namespace nnlib2
#endif // NNLIB2_SPARE_PARTS_H
