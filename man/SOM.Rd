\name{SOM}
\alias{SOM}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
Self-Organizing Map
}
\description{
Unsupervised clustering ANN, based on the Self-Organizing Map (SOM) introduced by Kohonen.
}
\usage{
SOM(
  data,
  max_number_of_desired_clusters,
  number_of_training_epochs,
  neighborhood_size,
  show_nn )
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{data}{
data to be clustered, a numeric matrix, (2d, cases in rows, variables in columns). Data should be in 0 to 1 range.
}
  \item{max_number_of_desired_clusters}{
clusters to be produced (at most)
}
  \item{number_of_training_epochs}{
number of training epochs, aka presentations of all training data to ANN during training.
}
  \item{neighborhood_size}{
integer >=1, specifies affected neighbor output nodes during training. if 1 (Single Winner) the ANN is somewhat similar to k-means.
}
  \item{show_nn}{
boolean, option to print an outline of the (trained) ANN internal structure.
}
}
\value{
Returns a vector of integers containing a cluster id for each data case (row).
}
\references{
Kohonen, T (1988). Self-Organization and Associative Memory, Springer-Verlag.; Simpson, P. K. (1991). Artificial neural systems: Foundations, paradigms, applications, and implementations. New York: Pergamon Press.
}
\author{
Vasilis N. Nikolaidis <vnnikolaidis@gmail.com>
}
\note{
Function SOM employs a simplified 1-D version of the Self-Organizing-Map NN for clustering data (Kohonen 1988). This SOM variant is described as Unsupervised Learning LVQ in Simpson (1991). Its parameter neighborhood_size controls the encoding mode (where neighborhood_size=1 is Single-Winner Unsupervised encoding, similar to k-means, while an odd valued neighborhood_size > 1 means Multiple-Winner Unsupervised encoding mode). Initial weights are random (uniform distribution) in 0 to 1 range. Since these weights represent cluster center coordinates (the class reference vector), it is important that input data is also scaled to this range.

(This function uses Rcpp to employ som_nn class in nnlib2 C++ Artificial Neural Network library.)
}
\examples{
# LVQ and SOM expect data in 0 to 1 range, so scale...
iris_s<-as.matrix(iris[1:4])
c_min<-apply(iris_s,2,FUN = "min")
c_max<-apply(iris_s,2,FUN = "max")
c_rng<-c_max-c_min
iris_s<-sweep(iris_s,2,FUN="-",c_min)
iris_s<-sweep(iris_s,2,FUN="/",c_rng)

som_cluster_ids<-SOM(iris_s,5,100)
plot(iris_s, pch=som_cluster_ids, main="SOM clustered Iris data")
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ classif }% use one of  RShowDoc("KEYWORDS")
\keyword{ neural }% __ONLY ONE__ keyword per line
