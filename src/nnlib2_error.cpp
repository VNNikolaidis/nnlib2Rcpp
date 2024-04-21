//		----------------------------------------------------------
//		nnlib2_error.cpp								Version 0.1
//		-----------------------------------------------------------
//		simple error handling - status
//		-----------------------------------------------------------
//		Author: Vasilis N. Nikolaidis
//		-----------------------------------------------------------


#include "nnlib2.h"
#include <stdio.h>
#include <string.h>
#include "nnlib2_error.h"

#ifdef NNLIB2_FOR_MFC_UI
#include <afx.h>
#include <afxwin.h>
#endif


namespace nnlib2 {

using std::string;

 /*-----------------------------------------------------------------------*/

#ifdef NNLIB2_FOR_MFC_UI
static CWnd * get_active_window()
 {
 CWnd * pwd = AfxGetApp()->GetMainWnd();
 if(pwd!=NULL)
  if(pwd->GetActiveWindow()!=NULL)
   return pwd->GetActiveWindow();
 return pwd;
 }

static HWND get_active_window_handle()
 {
 CWnd * pwd = get_active_window();
 if(pwd!=NULL) return pwd->m_hWnd;
 return NULL;
 }

#endif

/*-----------------------------------------------------------------------*/

bool error(unsigned i, string message,bool * p_error_flag)
{
int severity;

string m1;

switch(i)
 {
 case NN_MEMORY_ERR :
		  m1 = "memory allocation error";
		  severity=5;
		  break;

 case NN_IOFILE_ERR :
		  m1 ="file read/write error";
		  severity=2;
		  break;

 case NN_SYSTEM_ERR :
		  m1 ="system-related error";
		  severity=5;
		  break;

 case NN_INTEGR_ERR :
		  m1 = "NN integrity violation error";
		  severity=4;
		  break;

 case NN_NULLPT_ERR :
		  m1 ="null pointer error";
		  severity=4;
		  break;

 case NN_DATAST_ERR :
		  m1 = "dataset error";
		  severity=2;
		  break;

 case NN_ARITHM_ERR :
		  m1 ="arithmetic error";
		  severity=6;
		  break;

 case NN_USRABR_ERR :
		  m1 ="user-abort error";
		  severity=1;
		  break;

 case NN_METHOD_ERR:
		 m1 ="method error";
		 severity=1;
		 break;

 default: m1 = "Unknown error";
		  severity=1;
		  break;
 }

m1 = message + " (" + m1 + ")";

bool displayed = false;

#ifdef NNLIB2_FOR_MFC_UI
const char * msg_str = m1.c_str();
MessageBox(get_active_window_handle(),msg_str,"NN-Library Error",MB_OK|MB_ICONERROR|MB_APPLMODAL);
displayed = true;
#endif

#ifdef NNLIB2_FOR_RCPP
Rcpp::stop("nnlib2: "+m1);
displayed = true;
#endif

if(!displayed) TEXTOUT << "* ERROR: "<< m1 << "\n";

if (p_error_flag NEQL NULL)
 {
 if (severity>1) *p_error_flag = true;
 return *p_error_flag;
 }

#ifdef _DEBUG
//ASSERT(false);
#endif

if (severity>1) return true;
return false;
}

/*-----------------------------------------------------------------------*/

void warning(string message)
{
#ifdef NNLIB2_FOR_MFC_UI
	const char * msg_str = message.c_str();
    MessageBox(get_active_window_handle(),msg_str,"Neural Network Library Warning",MB_APPLMODAL|MB_OK|MB_ICONEXCLAMATION );
	return;
#endif

#ifdef NNLIB2_FOR_RCPP
	TEXTOUT << "Warning: "<< message << "\n";
	Rcpp::warning("nnlib2: "+message);
	return;
#endif

TEXTOUT << "* WARNING: "<< message << "\n";
}

void warning_modal(string message)
{
#ifdef NNLIB2_FOR_MFC_UI
	const char * msg_str = message.c_str();
    MessageBox(get_active_window_handle(),msg_str,"Neural Network Library Warning",MB_SYSTEMMODAL|MB_OK|MB_ICONEXCLAMATION );
#else
    warning(message);
#endif
}

}   // end of namespace nnlib2
