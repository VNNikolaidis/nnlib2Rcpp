\name{BP_create}
\alias{BP_create}
\title{
Create a Back-Propagation (BP) ANN
}
\description{
Create a Back-Propagation (BP) ANN so it can be trained and used. Once created, this will be the current active BP ANN.
}
\usage{
BP_create(input_data_dim, output_dim, learning_rate, num_hidden_layers, hidden_layer_size)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{input_data_dim}{
Length of input vectors (thus dimension of input vector space and size of input layer)
}
  \item{output_dim}{
Length of output vectors (thus dimension of output vector space and size of output layer)
}
  \item{learning_rate}{
a number (preferably greater than 0 and less than 1) used in training.
}
  \item{num_hidden_layers}{
number of hidden layers to be created between input and output layers.
}
  \item{hidden_layer_size}{
number of nodes in the hidden layers (all hidden layers are of the same length in this model).
}
}
\details{
This is one of several functions used to handle BP supervised ANNs in this package. A BP NN object must be created by invoking the BP_create (or BP_load_from_file) function before it can be used . Any further calls of BP_create would replace the current BP NN by a new one. The BP NN object should also be deleted when no longer needed by invoking the BP_destroy function.
}
\value{
boolean, TRUE if BP is succesfully created.
}
\references{
Simpson, P. K. (1991). Artificial neural systems: Foundations, paradigms, applications, and implementations. New York: Pergamon Press.
}
\author{
Vasilis N. Nikolaidis <vnnikolaidis@gmail.com>
}
\note{
These functions employ a Back-Propagation (BP) multilayer perceptron NN described in
Simpson (1991) as the vanilla back-propagation algorithm, to store input-output vector pairs.

(This function uses Rcpp to employ bp_nn class in nnlib2 C++ Artificial Neural Network library.)
}
%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
  \code{\link{BP_create}},
  \code{\link{BP_train_single}},
  \code{\link{BP_train_set}},
  \code{\link{BP_recall_set}},
  \code{\link{BP_print}},
  \code{\link{BP_save_to_file}},
  \code{\link{BP_load_from_file}},
  \code{\link{BP_destroy}},
  \code{\link{BP_NN}}.
}
\examples{
	BP_create(4,2,0.89,1,4);
	# ...train and use the BP Neural Network (not shown here).
	BP_destroy()

}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ neural }% use one of  RShowDoc("KEYWORDS")
