//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		nnlib2_string.h		 							Version 0.1
//		-----------------------------------------------------------
//		custom string functionality.
//		-----------------------------------------------------------

#include "nnlib2_string.h"
#include "string.h"

#define MAX_STRING_LENGTH 100
#define STRING_SPACE_REPLACE '_'

namespace nnlib2
{
using std::string;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// read :

std::istream& operator>> (std::istream REF s, streamable_string REF it)
{
    string buffer;
    s >> reinterpret_cast<string REF>(it);
    buffer = it;
    for (unsigned i = 0; i < buffer.length(); i++)
        if (buffer[i] == STRING_SPACE_REPLACE) buffer[i] = ' ';
    it.assign(buffer);
    return s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// write :

std::ostream& operator<< (std::ostream REF s, streamable_string REF it)
{
    string buffer;
    buffer = it;
    for (unsigned i = 0; i < buffer.length(); i++)
        if (buffer[i] == ' ') buffer[i] = STRING_SPACE_REPLACE;
    s << buffer;
    return s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


/* old versions:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// read :

std::istream& operator>> ( std::istream REF s, streamable_string REF it )
 {
 char name_buffer [MAX_STRING_LENGTH];
 s >> reinterpret_cast<string REF>(it);
 strncpy(name_buffer,it.c_str(),MAX_STRING_LENGTH);
 name_buffer[MAX_STRING_LENGTH-1]='\0';
 for(int i=0;i<MAX_STRING_LENGTH;i++)
  if(name_buffer[i]==STRING_SPACE_REPLACE) name_buffer[i]=' ';
 it.assign(name_buffer);
 return s;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// write :

std::ostream& operator<< ( std::ostream REF s, streamable_string REF it )
 {
 char name_buffer [MAX_STRING_LENGTH];
 strncpy(name_buffer,it.c_str(),MAX_STRING_LENGTH);
 name_buffer[MAX_STRING_LENGTH-1]='\0';
 for(int i=0;i<MAX_STRING_LENGTH;i++)
  if(name_buffer[i]==' ')name_buffer[i]=STRING_SPACE_REPLACE;
 s << name_buffer;
 return s;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

} // end of namespace nnlib2

