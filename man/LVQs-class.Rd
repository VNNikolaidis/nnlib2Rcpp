\name{LVQs-class}
\Rdversion{1.1}
\docType{class}
\alias{LVQs-class}
\alias{Rcpp_LVQs-class}
\alias{LVQs}

\title{Class \code{"LVQs"}}
\description{
Supervised Learning Vector Quantization (LVQ) NN module, for data classification.
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
The NN used in this module uses supervised training for data classification (described as Supervised Learning LVQ in Simpson (1991)). Data should be scaled in 0 to 1 range.

(This module uses Rcpp to employ 'lvq_nn' class in nnlib2.)
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
  \code{\link{LVQu}} (unsupervised LVQ function).
}

\examples{
# LVQ expects data in 0 to 1 range, so scale some numeric data...
iris_s<-as.matrix(iris[1:4])
c_min<-apply(iris_s,2,FUN = "min")
c_max<-apply(iris_s,2,FUN = "max")
c_rng<-c_max-c_min
iris_s<-sweep(iris_s,2,FUN="-",c_min)
iris_s<-sweep(iris_s,2,FUN="/",c_rng)

# create a vector of desired class ids (starting from 0):
iris_desired_class_ids<-as.integer(iris$Species)-1;

# now create the NN:
lvq<-new("LVQs")

# and train it:
lvq$encode(iris_s,iris_desired_class_ids,100)

# recall the same data (a simple check of how well the LVQ was trained):
lvq_recalled_class_ids<-lvq$recall(iris_s);
plot(iris_s, pch=lvq_recalled_class_ids, main="LVQ recalled clusters (module)")
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

    \item{\code{encode(data, desired_class_ids, training_epochs)}:}{ Encode input and output (classification) for a dataset using LVQ NN. Parameters are:}
    \itemize{
    \item{\code{data}}{: training data, a numeric matrix, (2d, cases in rows, variables in columns). Data should be in 0 to 1 range.}
    \item{\code{desired_class_ids}}{ : vector of integers containing a desired class id for each training data case (row). Should contain integers in 0 to n-1 range, where n is the number of classes.}
  \item{\code{training_epochs}}{: integer, number of training epochs, aka presentations of all training data to the NN during training.}
  }

    \item{\code{recall(data_in)}:}{ Get output (classification) for a dataset (numeric matrix \code{data_in}) from the (trained) LVQ NN. The \code{data_in} dataset should be 2-d containing  data cases (rows) to be presented to the NN and is expected to have same number or columns as the original training data. Returns a vector of integers containing a class id for each case (row).}

    \item{\code{print()}:}{ print NN structure. }

    \item{\code{load(filename)}:}{ retrieve the NN from specified file. }

    \item{\code{save(filename)}:}{ save the NN to specified file. }
  }

The following methods are inherited (from the corresponding class):
objectPointer ("RcppClass"), initialize ("RcppClass"), show ("RcppClass")
}
