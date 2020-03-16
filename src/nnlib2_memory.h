//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		nnlib2_memory.h		 							Version 0.1
//		simple memory handling
//		-----------------------------------------------------------


#ifndef NN_MEMORY_H
#define NN_MEMORY_H

#include "nnlib2.h"

namespace nnlib2 {

DATA ** malloc_2d (int r, int c);
void free_2d (DATA ** dp, int r);

} // end of namespace nnlib2

#endif // NN_MEMORY_H
