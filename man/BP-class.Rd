\name{BP-class}
\Rdversion{1.1}
\docType{class}
\alias{BP-class}
\alias{BP}
\alias{Rcpp_BP}
\alias{Rcpp_BP-class}

\title{Class \code{"BP"}}
\description{
Supervised Back-Propagation (BP) NN module, for encoding input-output mappings.
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
This R module maintains an internal Back-Propagation (BP) multilayer perceptron NN (described in Simpson (1991) as the vanilla back-propagation algorithm), which can be used to store input-output vector pairs. Since the nodes (PEs) in computing layers of this BP implementation apply the logistic sigmoid threshold function, their output is in [0 1] range (and so should the desired output vector values).

(This object uses Rcpp to employ 'bp_nn' class in nnlib2.)}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{\code{\link{Autoencoder}}.}

\examples{
# create some data...
iris_s                  <- as.matrix(scale(iris[1:4]))

# use a randomly picked subset of (scaled) iris data for training.
training_cases          <- sample(1:nrow(iris_s), nrow(iris_s)/2,replace=FALSE)
train_set               <- iris_s[training_cases,]
train_class_ids         <- as.integer(iris$Species[training_cases])
train_num_cases         <- nrow(train_set)
train_num_variables     <- ncol(train_set)
train_num_classes       <- max(train_class_ids)

# create output dataset to be used for training.
# Here we encode class as 0s and 1s (one-hot encoding).

train_set_data_out <- matrix(
          data = 0,
          nrow = train_num_cases,
          ncol = train_num_classes)

# now for each case, assign a 1 to the column corresponding to its class, 0 otherwise
# (note: there are better R ways to do this in R)
for(r in 1:train_num_cases) train_set_data_out[r,train_class_ids[r]]=1

# done with data, let's use BP...
bp<-new("BP")

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

    \item{\code{encode( data_in, data_out, learning_rate, training_epochs, hidden_layers, hidden_layer_size )}:}{ Setup a new BP NN and encode input-output data pairs. Parameters are:
    \itemize{
    \item{\code{data_in}}{: numeric matrix, containing input vectors as rows. . It is recommended that these values are in 0 to 1 range.}
    \item{\code{data_out}}{: numeric matrix, containing corresponding (desired) output vectors. It is recommended that these values are in 0 to 1 range.}
    \item{\code{learning_rate}}{: a number (preferably greater than 0 and less than 1) used in training.}
    \item{\code{training_epochs}}{: number of training epochs, aka single presentation iterations of all training data pairs to the NN during training.}
    \item{\code{hidden_layers}}{: number of hidden layers to be created between input and output layers.}
    \item{\code{hidden_layer_size}}{: number of nodes (processing elements or PEs) in each of the hidden layers (all hidden layers are of the same length in this implementation of BP).}
    }
    Note: to encode additional input-output vector pairs in an existing BP, use \code{train_single} or \code{train_multiple} methods (see below).
    }

    \item{\code{recall(data_in)}:}{ Get output for a dataset (numeric matrix \code{data_in}) from the (trained) BP NN. }

    \item{\code{setup(input_dim, output_dim, learning_rate, hidden_layers, hidden_layer_size)}:}{ Setup the BP NN so it can be trained and used. Note: this is not needed if using \code{encode}. Parameters are:
    \itemize{
    \item{\code{input_dim}}{: integer length of input vectors.}
    \item{\code{output_dim}}{: integer length of output vectors.}
    \item{\code{learning_rate}}{: a number (preferably greater than 0 and less than 1) used in training.}
    \item{\code{hidden_layers}}{: number of hidden layers to be created between input and output layers.}
    \item{\code{hidden_layer_size}}{: number of nodes (processing elements or PEs) in each of the hidden layers (all hidden layers are of the same length in this implementation of BP).}
    }
    }

    \item{\code{train_single (data_in, data_out)}:}{ Encode an input-output vector pair in the BP NN. Only performs a single training iteration (multiple may be required for proper encoding). Vector sizes should be compatible to the current NN (as resulted from the \code{encode} or \code{setup} methods). Returns error level indicator value.}

    \item{\code{train_multiple (data_in, data_out, training_epochs)}:}{ Encode multiple input-output vector pairs stored in corresponding datasets. Performs multiple iterations in epochs (see \code{encode}). Vector sizes should be compatible to the current NN (as resulted from the \code{encode} or \code{setup} methods). Returns error level indicator value.}


    \item{\code{set_error_level(error_type, acceptable_error_level)}:}{ Set options that stop training when an acceptable error level has been reached (when a subsequent \code{encode} or \code{train_multiple} is performed). Parameters are:
    \itemize{
  \item{\code{error_type}}{: string, error type to display and use to stop training (must be 'MSE' or 'MAE').}
  \item{\code{acceptable_error_level}}{: training stops when error is below this level.}
    }
    }

    \item{\code{mute(on)}:}{ Disable output of current error level when training (if parameter \code{on} is TRUE). }

    \item{\code{print()}:}{ Print NN structure. }

    \item{\code{show()}:}{ Print NN structure. }

    \item{\code{load(filename)}:}{ Retrieve the NN from specified file. }

    \item{\code{save(filename)}:}{ Save the NN to specified file. }
  }

The following methods are inherited (from the corresponding class):
objectPointer ("RcppClass"), initialize ("RcppClass"), show ("RcppClass")
}
