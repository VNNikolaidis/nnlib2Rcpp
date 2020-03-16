\name{BP_load_from_file}
\alias{BP_load_from_file}
\title{
Retrieve a BP ANN from file.
}
\description{
Loads a BP ANN (created via \code{\link{BP_save_to_file}}) from specified file, replacing the currently active BP (if any).
}
\usage{
BP_load_from_file(filename)
}
\arguments{
  \item{filename}{
name of file containing the BP ANN (created via \code{\link{BP_save_to_file}}).
}
}
\details{
This is one of several functions used to handle BP supervised ANNs in this package. A BP NN object must be created by invoking the BP_create (or BP_load_from_file) function before it can be used . Any further calls of BP_create would replace the current BP NN by a new one. The BP NN object should also be deleted when no longer needed by invoking the BP_destroy function.
}
\value{
boolean, TRUE if BP is succesfully loaded.
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
	BP_save_to_file("TEST")
	BP_load_from_file("TEST")
	BP_destroy()
	# remove the TEST file if you dont need it.
	file.remove("TEST")
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ neural }% use one of  RShowDoc("KEYWORDS")
