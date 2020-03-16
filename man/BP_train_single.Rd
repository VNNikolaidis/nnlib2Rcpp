\name{BP_train_single}
\alias{BP_train_single}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
Train the active BP with an input-output vector pair.
}
\description{
Trains the active BP with by presenting the given input-output vector pair (data_in and data_out) once, i.e. performs a single iteration (note that several such iterations are usually needed for training to be sufficient).
}
\usage{
BP_train_single(data_in, data_out)
}
\arguments{
  \item{data_in}{
a numeric vector containing input data (for training to succeed, it should have length equal to the size of the input layer in the current active BP). It is recommended that these values fall in 0 to 1 range.
}
  \item{data_out}{
a numeric vector containing output data (for training to succeed, it should have length equal to the size of the output layer in the current active BP)
}
}
\details{
Since the PEs in computing layers of this BP implementation apply the logistic sigmoid threshold function, their output is in [0 1] range (and so should the desired output vector values). This function is one of several used to handle BP supervised ANNs in this package. A BP NN object must be created by invoking the BP_create (or BP_load_from_file) function before it can be used . Any further calls of BP_create would replace the current BP NN by a new one. The BP NN object should also be deleted when no longer needed by invoking the BP_destroy function.
}
\value{
Returns a real number, indicator of training error.
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
in1 = c(.1,0,.4,.5)
ou1 = c(.3,.4)
BP_train_single(in1,ou1)
BP_destroy()

}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ neural }% use one of  RShowDoc("KEYWORDS")

