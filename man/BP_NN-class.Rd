\name{BP_NN-class}
\Rdversion{1.1}
\docType{class}
\alias{BP_NN-class}
\alias{BP_NN}
\alias{Rcpp_BP_NN}
\alias{Rcpp_BP_NN-class}
\alias{C++Object-class}
\alias{RcppClass-class}

\title{Class \code{"BP_NN Backpropagation NN"}}
\description{
Back-Propagation NN as a module. This is an alternative (and probably better) functionaly equivalent way to employ a BP supervised NN without using functions (such as \code{\link{BP_create}}, \code{\link{BP_train_single}}, \code{\link{BP_train_set}}, \code{\link{BP_destroy}} etc.)
}
\section{Extends}{
Class \code{"\linkS4class{RcppClass}"}, directly.

All reference classes extend and inherit methods from \code{"\linkS4class{envRefClass}"}.

}
\references{
Simpson, P. K. (1991). Artificial neural systems: Foundations, paradigms, applications, and implementations. New York: Pergamon Press.
}
\author{
Vasilis N. Nikolaidis <vnnikolaidis@gmail.com>
}
\note{
The R object employs a Back-Propagation (BP) multilayer perceptron NN described in
Simpson (1991) as the vanilla back-propagation algorithm, to store input-output vector pairs. Since the PEs in computing layers of this BP implementation apply the logistic sigmoid threshold function, their output is in [0 1] range (and so should the desired output vector values).

(This object uses Rcpp to employ bp_nn class in nnlib2 C++ Artificial Neural Network library.)}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
  \code{\link{BP_create}},
  \code{\link{BP_train_single}},
  \code{\link{BP_train_set}},
  \code{\link{BP_recall_set}},
  \code{\link{BP_print}},
  \code{\link{BP_save_to_file}},
  \code{\link{BP_load_from_file}},
  \code{\link{BP_destroy}}.}
\examples{
# create some data...
iris_s                  <- as.matrix(scale(iris[1:4]))

# use a randomply picked subset of (scaled) iris data for training
training_cases          <- sample(1:nrow(iris_s), nrow(iris_s)/2,replace=FALSE)
train_set               <- iris_s[training_cases,]
train_class_ids         <- as.integer(iris$Species[training_cases])
train_num_cases         <- nrow(train_set)
train_num_variables     <- ncol(train_set)
train_num_classes       <- max(train_class_ids)

# create output dataset to be used for training, Here we encode class as 0s and 1s
train_set_data_out <- matrix(
          data = 0,
          nrow = train_num_cases,
          ncol = train_num_classes)

# now for each case, assign a 1 to the column corresponding to its class, 0 otherwise
# (there must be a better R way to do this)
for(r in 1:train_num_cases) train_set_data_out[r,train_class_ids[r]]=1

# done with data, let's use BP...

bp<-new("BP_NN")

bp$encode(train_set,train_set_data_out,0.8,10000,2,4)

# let's test by recalling the original training set...
bp_output <- bp$recall(train_set)

cat("- Using this demo's encoding, recalled class is:\n")
print(apply(bp_output,1,which.max))
cat("- BP success in recalling correct class is: ",
  sum(apply(bp_output,1,which.max)==train_class_ids)," out of ",
  train_num_cases, "\n")

# Let's see how well it recalls the entire Iris set:
bp_output <- bp$recall(iris_s)
# show output
cat("\n- Recalling entire Iris set returns:\n")
print(bp_output)
cat("- Using this demo's encoding, original class is:\n")
print(as.integer(iris$Species))
cat("- Using this demo's encoding, recalled class is:\n")
bp_classification <- apply(bp_output,1,which.max)
print(bp_classification)
cat("- BP success in recalling correct class is: ",
  sum(apply(bp_output,1,which.max)==as.integer(iris$Species)),
  "out of ", nrow(iris_s), "\n")
plot(iris_s, pch=bp_classification, main="Iris classified by a partialy trained BP (module)")
}
\keyword{classes}
\section{Fields}{
  \describe{
    \item{\code{.CppObject}:}{Object of class \code{C++Object} ~~ }
    \item{\code{.CppClassDef}:}{Object of class \code{activeBindingFunction} ~~ }
    \item{\code{.CppGenerator}:}{Object of class \code{activeBindingFunction} ~~ }
  }
}
\section{Methods}{
  \describe{
    \item{\code{encode( ... )}:}{ Setup BP and encode input-output datasets in the NN. Parameters are: NumericMatrix tr_dataset_in, NumericMatrix tr_dataset_out, double learning_rate, int training_epochs, int hidden_layers, int hidden_layer_size }
    \item{\code{recall(data_in)}:}{ Get output for a dataset (NumericMatrix data_in) using BP NN. }
    \item{\code{train_single (...)}:}{ Encode a single input-output vector pair in current BP NN. }
    \item{\code{print()}:}{ prints NN structure. }
    \item{\code{load(filename)}:}{ retrieves the NN stored in file. }
    \item{\code{save(filename)}:}{ saves the status of the NN to specified file. }
  }

The following methods are inherited (from the corresponding class):
objectPointer ("RcppClass"), initialize ("RcppClass"), show ("RcppClass")
}
