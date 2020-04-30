\name{BP_print}
\alias{BP_print}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
Print the current active BP ANN
}
\description{
Prints the current active BP ANN (if any).
}
\usage{
BP_print()
}
\details{
Prints the current active BP ANN created via \code{\link{BP_create}} or \code{\link{BP_load_from_file}}.
This is one of several functions used to handle BP supervised ANNs in this package(*). A BP NN object must be created by invoking the BP_create (or BP_load_from_file) function before it can be used . Any further calls of BP_create would replace the current BP NN by a new one. The BP NN object should also be deleted when no longer needed by invoking the BP_destroy function.

(*) A simpler approach is to use \code{\link{BP_NN}} which includes all the required BP functionality in a module.
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
  \code{\link{BP_destroy}}.
}
\examples{
	BP_create(4,2,0.89,1,4);
	BP_print()
	BP_destroy()

}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ neural }% use one of  RShowDoc("KEYWORDS")
