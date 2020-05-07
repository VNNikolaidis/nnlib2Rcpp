\name{MAM-class}
\Rdversion{1.1}
\docType{class}
\alias{MAM-class}
\alias{MAM}
\alias{Rcpp_MAM}
\alias{Rcpp_MAM-class}

\title{Class \code{"MAM"}}
\description{
A single Matrix Associative Memory (MAM) implemented as a (supervised) NN.
}
\section{Extends}{
Class \code{"\linkS4class{RcppClass}"}, directly.

All reference classes extend and inherit methods from \code{"\linkS4class{envRefClass}"}.
}
\references{
Pao Y (1989). Adaptive Pattern Recognition and Neural Networks. Reading, MA (US); Addison-Wesley Publishing Co., Inc.
}
\author{
Vasilis N. Nikolaidis <vnnikolaidis@gmail.com>
}
\note{
The NN in this module uses supervised training to store input-output vector pairs.

(This function uses Rcpp to employ 'mam_nn' class in nnlib2.)
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{\code{\link{BP},\link{LVQs}}.}

\examples{
iris_s            <- as.matrix(scale(iris[1:4]))
class_ids         <- as.integer(iris$Species)
num_classes       <- max(class_ids)

# create output dataset to be used for training, Here we encode class as -1s and 1s
iris_data_out <- matrix( data = -1, nrow = nrow(iris_s), ncol = num_classes)

# now for each case, assign a 1 to the column corresponding to its class
for(r in 1:nrow(iris_data_out)) iris_data_out[r,class_ids[r]]=1

# Finally apply MAM:
# Encode train pairs in MAM and then get output dataset by recalling the test data.

mam <- new("MAM")

mam$encode(iris_s,iris_data_out)

# test the encoding by recalling the original input data...
mam_data_out <- mam$recall(iris_s)

# find which MAM output has the largest value and use this as the final cluster tag.
mam_recalled_cluster_ids = apply(mam_data_out,1,which.max)

plot(iris_s, pch=mam_recalled_cluster_ids, main="MAM recalled Iris data classes")

cat("MAM recalled these IDs:\n")
print(mam_recalled_cluster_ids)
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

    \item{\code{encode( data_in, data_out )}:}{ Setup a new MAM NN and encode input-output data pairs. Parameters are:
    \itemize{
    \item{\code{data_in}}{: numeric matrix, input data to be encoded in MAM, a numeric matrix (2d, of n rows). Each row will be paired to the corresponding data_out row, forming an input-output vector pair.}
    \item{\code{data_out}}{: numeric matrix, output data to be encoded in MAM, a numeric matrix (2d, also of n rows). Each row will be paired to the corresponding data_in row, forming an input-output vector pair.}
    }
    Note: to encode additional input-output vector pairs in an existing MAM, use \code{train_single} method (see below).
    }

    \item{\code{recall(data)}:}{ Get output for a dataset (numeric matrix \code{data}) from the (trained) MAM NN. }

    \item{\code{train_single (data_in, data_out)}:}{ Encode an input-output vector pair in the MAM NN. Vector sizes should be compatible to the current NN (as resulted from the \code{encode} method).}

    \item{\code{print()}:}{ print NN structure. }

    \item{\code{load(filename)}:}{ retrieve the NN from specified file. }

    \item{\code{save(filename)}:}{ save the NN to specified file. }
  }

The following methods are inherited (from the corresponding class):
objectPointer ("RcppClass"), initialize ("RcppClass"), show ("RcppClass")
}
