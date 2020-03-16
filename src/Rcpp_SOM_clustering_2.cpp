//		----------------------------------------------------------
//		(C)2019       Vasilis.N.Nikolaidis     All rights reserved.
//		-----------------------------------------------------------
//    Rcpp glue code for Self-Organized Map NN (som_nn,unsupervised)
//		-----------------------------------------------------------
//    this is same as util_SOM_clustering_1 example except it
//    creates (and deletes) the som NN dynamicaly, so a pointer
//    to the som (p_som) is used.
//		-----------------------------------------------------------

#include "nnlib2.h"

#ifdef NNLIB2_FOR_RCPP
using namespace Rcpp;

//--------------------------------------------------------------------------------

#include "nn_lvq.h"
#include "nnlib2_misc.h"                      // for which_max()

using namespace nnlib2;
using namespace nnlib2::lvq;

//--------------------------------------------------------------------------------

som_nn *       p_som = NULL;

//--------------------------------------------------------------------------------

void SOM_nn_create (int input_data_dim,
                    int output_dim,
                    int neighborhood_size = 1)
{
    p_som = new som_nn(neighborhood_size);
    if(p_som==NULL) {    error(NN_NULLPT_ERR,"No nn"); return; }
    if(NOT p_som->setup(input_data_dim,output_dim))  error(NN_INTEGR_ERR,"Cannot setup SOM");
}

//--------------------------------------------------------------------------------

IntegerVector SOM_nn_train (NumericMatrix data,int number_of_training_epochs)
{
  IntegerVector returned_cluster_ids = rep(-1,data.rows());

  if(p_som==NULL) { error(NN_NULLPT_ERR,"No nn"); return returned_cluster_ids; }
  int output_dim = p_som->output_dimension();

  if(!p_som->no_error())   return(returned_cluster_ids);

  // encode all data

  for(int i=0;i<number_of_training_epochs;i++)
  {
    for(int r=0;r<data.rows();r++)
    {
      NumericVector v(data( r , _ ));                 // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
      double * fpdata = REAL(v);                      // my (lame?) way to interface with R, cont.)

      p_som->encode_u(fpdata,data.cols(),i);          // Encode unsupervised
    }
  checkUserInterrupt();                               // (RCpp function to check if user pressed cancel)
  }

  // training completed, now recall the data and get output

  DATA * output_vector = new DATA [output_dim];

  for(int r=0;r<data.rows();r++)
  {
    NumericVector v(data( r , _ ));                 // my (lame?) way to interface with R. Remember, NumericMatrix stores data row-first, as R does.
    double * fpdata = REAL(v);                      // my (lame?) way to interface with R, cont.)

    p_som->recall(fpdata, data.cols(), output_vector, output_dim);

    // now find which element in the output vector has the smallest value and use it as the winner id.
    returned_cluster_ids[r] = which_min(output_vector,output_dim);
  }

  delete [] output_vector;

  return returned_cluster_ids;
}

//--------------------------------------------------------------------------------

void SOM_nn_delete()
{
  if(p_som==NULL) {    error(NN_NULLPT_ERR,"No nn"); return; }
  delete p_som;
  p_som=NULL;
}

//--------------------------------------------------------------------------------

IntegerVector SOM_2 (NumericMatrix data,
                                int desired_num_clusters,
                                int number_of_training_epochs,          // (each presents all data)
                                bool show_nn)
{
  SOM_nn_create(data.cols(),desired_num_clusters,1);
  IntegerVector returned_cluster_ids = SOM_nn_train(data,number_of_training_epochs);
  if(p_som!=NULL)
    if(show_nn)
      {
        TEXTOUT << "------Network structure (BEGIN)--------\n";
        p_som->to_stream(TEXTOUT);
        TEXTOUT << "--------Network structure (END)--------\n";
      }
  SOM_nn_delete();
  TEXTOUT << "SOM returned " << unique(returned_cluster_ids).length() << " clusters with ids: " << unique(returned_cluster_ids) << "\n";
  return(returned_cluster_ids);
}

//--------------------------------------------------------------------------------

/*** R
#
# demo_nnlib2_example_SOM_2<-function(desired_num_clusters=3)
# {
# iris_s<-as.matrix(scale(iris[1:4]))
# som_cluster_ids<-SOM_2(iris_s,desired_num_clusters,num_epochs,show_nn)
# plot(iris$Sepal.Length, iris$Sepal.Width, pch=21, bg=c("red","green3","blue")[unclass(iris$Species)], main="Iris species")
# plot(iris_s, pch=som_cluster_ids, bg=heat.colors(desired_num_clusters)[som_cluster_ids], main="SOM clustered IRIS data (2)")
# }
#
# demo_nnlib2_example_SOM_2()
*/

#endif // NNLIB2_FOR_RCPP
