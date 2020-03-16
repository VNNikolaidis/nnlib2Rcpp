\name{LVQ}
\alias{LVQ}
\title{
LVQ
}
\description{
Learning Vector Quantization, supervised-training Artificial Neural Network (ANN)
}
\usage{
LVQ(data, desired_cluster_ids, number_of_training_epochs, test_data, show_nn = FALSE)
}
\arguments{
  \item{data}{
training data, a numeric matrix, (2d, cases in rows, variables in columns). Data should be in 0 to 1 range.
}
  \item{desired_cluster_ids}{
vector of integers containing a desired class id for each training data case (row). Should contain integers in 0 to n-1 range, where n is the number of classes.
}
  \item{number_of_training_epochs}{
number of training epochs, aka presentations of all training data to ANN during training.
}
  \item{test_data}{
numeric matrix (2d) containing  data to be recalled from the trained ANN. Should have same number or columns as the training data.
}
  \item{show_nn}{
boolean, option to print an outline of the (trained) ANN internal structure.
}
}
\value{
Returns a vector of integers containing a class id for each test data case (row).
}
\references{
Kohonen, T (1988). Self-Organization and Associative Memory, Springer-Verlag.; Simpson, P. K. (1991). Artificial neural systems: Foundations, paradigms, applications, and implementations. New York: Pergamon Press.
}
\author{
Vasilis N. Nikolaidis <vnnikolaidis@gmail.com>
}
\note{
The NN used in this function uses supervised training for data classification (described as Supervised Learning LVQ in Simpson (1991)). Data should be scaled in 0 to 1 range.

(This function uses Rcpp to employ lvq_nn class in nnlib2 C++ Artificial Neural Network library.)
}
\seealso{
  \code{\link{LVQ_NN}}
}
\examples{
# LVQ and SOM expect data in 0 to 1 range, so scale...
iris_s<-as.matrix(iris[1:4])
c_min<-apply(iris_s,2,FUN = "min")
c_max<-apply(iris_s,2,FUN = "max")
c_rng<-c_max-c_min
iris_s<-sweep(iris_s,2,FUN="-",c_min)
iris_s<-sweep(iris_s,2,FUN="/",c_rng)

# use cluster indexing starting from 0.
iris_ids<-as.integer(iris$Species)-1;

# now run LVQ:
returned_ids<-LVQ(iris_s,iris_ids,100,iris_s)
print(returned_ids)
plot(iris_s, pch=returned_ids, main="LVQ recalled clusters")
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ classif }% use one of  RShowDoc("KEYWORDS")
\keyword{ neural }% __ONLY ONE__ keyword per line
