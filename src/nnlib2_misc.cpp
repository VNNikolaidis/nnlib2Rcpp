//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		nnlib2_misc.cpp									Version 0.1
//		-----------------------------------------------------------
//		misc routines
//		-----------------------------------------------------------

#include <cstdlib>
#include <time.h>

#include "nnlib2.h"
#include "nnlib2_vector.h"

namespace nnlib2 {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// a simple random number function for typical C++ implentations (not R and RCpp)

#ifdef NNLIB2_FOR_RCPP

DATA random (DATA min, DATA max)
	{
	double r = Rcpp::runif(1,min,max)[0];
	return r;
	}

#else // not NNLIB2_FOR_RCPP

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// a simple random number function for typical C++ implentations (not R and RCpp)

DATA random (DATA min, DATA max)
{
	double r;
	static bool first_time_in = true;

	if(first_time_in)
	{
		srand( (unsigned)time( NULL ) );
		first_time_in = false;
	}

	// NOTE: VC6 defines RAND_MAX=32767

	double r1;
	double randmax=1;
	r=1;

	do
	{
		r1 = (double)rand();					// 0	to	RAND_MAX
		r=r*r1;
		randmax=randmax*RAND_MAX;
	}
	while((r!=0) AND (r<max));

	r =  r / randmax;						// 0.0	to	1.0
	r =  (r * (max - min)) + min;

	return (DATA) r;
}

#endif // NNLIB2_FOR_RCPP
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int winner_takes_all(DATA * vec, int vec_dim, bool find_max)
 {
 if(vec==NULL) return -1;
 if(vec_dim<0) return -1;
 if(vec_dim<1) return 0;

 DATA v=vec[0];
 int  p=0;
 for(int i=1;i<vec_dim;i++)
  {
  if(vec[i]>DATA_MAX) {vec[i]=DATA_MAX;/*warning("Network output value too large");*/}
  if(vec[i]<DATA_MIN) {vec[i]=DATA_MIN;/*warning("Network output value too small");*/}
  if((find_max ? (vec[i]>=v) : (vec[i]<=v)))
   {
   p=i;
   v=vec[i];
   }
  }
 return p;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int which_max(DATA * vec, int vec_dim) {return winner_takes_all(vec,vec_dim,true);}
int which_min(DATA * vec, int vec_dim) {return winner_takes_all(vec,vec_dim,false);}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Remove empty classes, return actual number of classes.

int pack_class_assignments(	int * class_assignments, int number_of_records,
							int requested_number_of_classes)
 {

 vector<int> class_buffer (requested_number_of_classes);
 int c,r,i,numclasses;

 // first, count elements in each class.

 for(c=0;c<requested_number_of_classes;c++) class_buffer[c]=0;
 for(r=0;r<number_of_records;r++) class_buffer[class_assignments[r]]++;

 // next, assign a new class number for non-empty classes.

 i=0;
 for(c=0;c<requested_number_of_classes;c++)
  if (class_buffer[c] NEQL 0)	{ class_buffer[i]=c; i++; }
 numclasses=i;
 for(c=i;c<requested_number_of_classes;c++) class_buffer[c]=-1;

 // re-assign class tags, ignoring empty.

 for(r=0;r<number_of_records;r++)
   class_assignments[r] = class_buffer.first_location_of(class_assignments[r]);

 // return number of non-empty classes
 return numclasses;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// make sure there are no empty classes before calling this function
// (use pack_class_assignments()). Also make sure that
// ret_calculated_class_centers and ret_calculated_class_sizes sets
// are properly initialized and allocated.

bool calculate_class_centers_and_sizes
							(DATA ** data,
							 const int number_of_records,
							 const int dimension,
							 const int number_of_classes,
							 int * class_assignments,
							 DATA ** ret_calculated_class_centers,
							 int *  ret_calculated_class_sizes)

 {
 int r,c;
 int aclass;
 bool ok = true;

 if((number_of_records < 0) OR
	(dimension < 0) OR
	(number_of_classes < 0) OR
	(data == NULL) OR
	(class_assignments == NULL) OR
	(ret_calculated_class_centers == NULL) OR
	(ret_calculated_class_sizes == NULL))
  return false;

 // initialize

 for(r=0;r<number_of_classes;r++)
  {
  ret_calculated_class_sizes[r]=0;
  for(c=0;c<dimension;c++)
   ret_calculated_class_centers[r][c]=0.0;
  }

 // calculate.

 for(r=0;r<number_of_records;r++)
  {
  aclass = class_assignments[r];
  if((aclass<0)OR(aclass>=number_of_classes))
	error(NN_DATAST_ERR,"Invalid class",&ok);
  else
	{
    ret_calculated_class_sizes[aclass]++;
    for(c=0;c<dimension;c++)
     ret_calculated_class_centers[aclass][c]+=data[r][c];
	}
  }

  // divide by size to calculate mean.

 if(ok)
  for(r=0;r<number_of_classes;r++)
   for(c=0;c<dimension;c++)
    ret_calculated_class_centers[r][c] =
	 ret_calculated_class_centers[r][c]/ret_calculated_class_sizes[r];

 return(ok);
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

} // end of namespace nnlib2

