//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis   	  All rights reserved.
//		-----------------------------------------------------------
//		nnlib2.h								Version (see below)
//		Definitions  		    	loosely based on nnlib (C) 1998
//		-----------------------------------------------------------
//		this is the main definitions file for any nnlib2 project
//		-----------------------------------------------------------

// Note  : editor tab size set to 4
// spaces:  |   |   |   |   |   |
// tabs__:	|	|	|	|	|	|
// (The above should be aligned.)

#ifndef VNN_H
#define VNN_H

#define NN_VERSION "nnlib2 v.0.2.4 (c)2019 Vasilis N. Nikolaidis"

/*-----------------------------------------------------------------------*/

//#define NNLIB2_FOR_GCC				// for gcc (minor mods)
//#define NNLIB2_FOR_RCPP               // for R (Rcpp) package of nnlib2 NNs
//#define NNLIB2_FOR_MFC_UI				// for MS-Windows (MFC and VS6) support and GUI to nnlib2

// define ONLY ONE of the above (or none for generic, unspecified target, C++ compilation):
#define NNLIB2_FOR_RCPP

/*-----------------------------------------------------------------------*/

#ifdef NNLIB2_FOR_RCPP
#define STRICT_R_HEADERS
#include <float.h>
#include <Rcpp.h>
// [[Rcpp::plugins("cpp11")]]
using namespace Rcpp;
#define TEXTOUT Rcpp::Rcout
#endif

/*-----------------------------------------------------------------------*/

#ifdef NNLIB2_FOR_GCC
#include <climits>
#include <cfloat>
#endif

/*-----------------------------------------------------------------------*/

#ifdef NNLIB2_FOR_MFC_UI
#ifndef DATA
#include <limits>
#define DATA float
#define DATA_MIN -FLT_MAX
#define DATA_MAX  FLT_MAX
//#define DATAFORMAT "%f
#define NNLIB2_WITH_GUI
#endif
#endif

/*-----------------------------------------------------------------------*/

#include <string>
using std::string;

#ifndef DATA
#include <limits>
#define DATA double
#define DATA_MIN  DBL_MIN
#define DATA_MAX  DBL_MAX
#endif

/*-----------------------------------------------------------------------*/

#ifndef TEXTOUT
#include <iostream>
#define TEXTOUT std::cout
#endif
/*-----------------------------------------------------------------------*/

#define PTR   *
#define ATPTR *
#define REF   &
#define ADR   &
#define NEQL  !=
#define EQL   ==
#define AND   &&
#define OR    ||
#define NOT   !

/*-----------------------------------------------------------------------*/

#define NN_VERBOSE						// more messages (for debugging)

/*-----------------------------------------------------------------------*/

#define UNTESTED_FUNCTION_WARNING		warning("Using untested code")

/*-----------------------------------------------------------------------*/

#endif // VNN_H
