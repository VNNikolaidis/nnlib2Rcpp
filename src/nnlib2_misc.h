//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		nnlib2_misc.h									Version 0.1
//		-----------------------------------------------------------
//		misc routines
//		-----------------------------------------------------------

#ifndef NN_MISC_H
#define NN_MISC_H

#include "nnlib2.h"

namespace nnlib2 {

DATA random (DATA min, DATA max);
int winner_takes_all(DATA * vec, int vec_dim, bool find_max=true);
int which_max(DATA * vec, int vec_dim);
int which_min(DATA * vec, int vec_dim);
int pack_class_assignments(	int * class_assignments, int number_of_records,
							int requested_number_of_classes);
bool calculate_class_centers_and_sizes
							(DATA ** data,
							 const int number_of_records,
							 const int dimension,
							 const int number_of_classes,
							 int * class_assignments,
							 DATA ** ret_calculated_class_centers,
							 int *  ret_calculated_class_sizes);

} // end of namespace nnlib2

#endif // NN_MISC_H
