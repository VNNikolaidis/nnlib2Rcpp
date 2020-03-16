//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		nnlib2_string.h					Version 0.1
//		-----------------------------------------------------------
//		custom string functionality.
//		-----------------------------------------------------------

#ifndef NN_STRING_H
#define NN_STRING_H

#include "nnlib2.h"

#include <iostream>
#include <string>

#define MAX_STRING_LENGTH 100
#define STRING_SPACE_REPLACE '_'

namespace nnlib2 {

using std::string;

class streamable_string : public string
 {
 public:
 streamable_string () : string () {};
 streamable_string (const char * source) : string (source) {};
 streamable_string (std::string source)  : string (source) {};
 friend std::istream& operator>> ( std::istream REF s, streamable_string REF it );
 friend std::ostream& operator<< ( std::ostream REF s, streamable_string REF it );
 };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // end of namespace nnlib2

#endif // NN_STRING_H
