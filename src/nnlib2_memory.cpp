//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		nnlib2_memory.cpp					                      Version 0.1
//		simple memory handling
//		-----------------------------------------------------------


#include "nnlib2.h"
#include "nnlib2_error.h"

#include <stdlib.h>

namespace nnlib2 {

/*--------------------------------------------------------------------*/

DATA ** malloc_2d (int r, int c)
{
DATA ** dp;
int i,j;
bool noerror = true;

if((dp=(DATA**)malloc(sizeof(DATA *) * r))==NULL)
 error(NN_MEMORY_ERR,"No memory for pointers to rows.",NULL);
else
 for(i=0;((i<r)&&(noerror));i++)
  {
  if((dp[i]=(DATA*)malloc(sizeof(DATA) * c))==NULL)
   {
   error(NN_MEMORY_ERR,"No memory for rows.",NULL);
   noerror = false;
   }
  else
   for(j=0;j<c;j++)
    dp[i][j]=(DATA)0;
  }
return dp;
}

/*--------------------------------------------------------------------*/

void free_2d (DATA ** dp, int r)
{
int i;

if(dp!=NULL)
 {
 for(i=r-1;i>=0;i--)
 if (dp[i]!=NULL) free(dp[i]);
 else
  error(NN_NULLPT_ERR,"Cannot free null pointer",NULL);
 free(dp);
 }
 else
 error(NN_NULLPT_ERR,"Cannot free null pointer",NULL);
}

/*--------------------------------------------------------------------*/

}   // end of namespace nnlib2
