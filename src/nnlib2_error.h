//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		nnlib2_error.h							                		Version 0.1
//		-----------------------------------------------------------
//		simple error handling - status
//		-----------------------------------------------------------

#ifndef NN_ERROR_H
#define NN_ERROR_H

#include "nnlib2.h"

namespace nnlib2 {

#define NN_MEMORY_ERR	1								/* memory allocation      */
#define NN_IOFILE_ERR	2								/* read/write             */
#define NN_SYSTEM_ERR	3								/* system                 */
#define NN_INTEGR_ERR	4								/* NN integrity			      */
#define NN_NULLPT_ERR	5								/* null pointer           */
#define NN_DATAST_ERR	6								/* data (test/train) set  */
#define NN_ARITHM_ERR	7								/* arithmetic             */
#define NN_USRABR_ERR	8								/* user aborted           */

extern bool error(unsigned id, string message,bool * p_error_flag=NULL);
extern void warning(string message);
extern void warning_modal(string message);

/*-----------------------------------------------------------------------*/
// true in m_error_flag means there is a runtime error.
//
// (Note: this usualy ties components with a nn. Consider replacing with
// a pointer to the "parent" nn (for more functionality but also more
// dependance beetween nn and components. Q. Is this dependance wanted?)

class error_flag_server
 {
 public:
 bool m_error_flag;
 bool * my_error_flag() { return &m_error_flag;}
 error_flag_server()    { reset_error(); };
 void reset_error()     { m_error_flag=false; };
 bool no_error()        { return NOT m_error_flag; };
 };

/*-----------------------------------------------------------------------*/

class error_flag_client
 {
 private :

 bool m_local_error_flag;					// local error flag, used when no error dependancy exists.
 bool * mp_error_flag;						// actual flag is in (related) error_flag_server

 public:

 error_flag_client(bool * p_error_flag=NULL)
  {
  m_local_error_flag = false;
  set_error_flag(p_error_flag);
  };

 void set_error_flag(bool * p_error_flag)
  {
  if(p_error_flag==NULL)
   mp_error_flag=&m_local_error_flag;
  else
   mp_error_flag=p_error_flag;
  };

 bool error(unsigned i, string message)
  {
  bool retval = nnlib2::error(i,message, mp_error_flag);
  if(mp_error_flag == &m_local_error_flag)
   warning("Note:'error_flag_client' uses local error flag.");
  return retval;
  };

 void reset_error()
  {
  if (mp_error_flag != NULL) *mp_error_flag=false;
  };

 bool no_error() { return ((* mp_error_flag) EQL false); };

 bool * my_error_flag () {return mp_error_flag;};

 };

/*-----------------------------------------------------------------------*/

}   // end of namespace nnlib2

#endif // NN_ERROR_H

