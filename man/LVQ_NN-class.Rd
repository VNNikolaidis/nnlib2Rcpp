\name{LVQ_NN-class}
\Rdversion{1.1}
\docType{class}
\alias{LVQ_NN-class}
\alias{LVQ_NN}
\alias{Rcpp_LVQ_NN}

\title{Class \code{"LVQ_NN"}}
\description{
Learning Vector Quantization NN as a module. This is an alternative (and probably more versatile) functionaly equivalent way to employ an LVQ supervised NN without using the function \code{\link{LVQ}}.
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
The NN used in this function uses supervised training for data classification (described as Supervised Learning LVQ in Simpson (1991)). Data should be scaled in 0 to 1 range.

(This module uses Rcpp to employ lvq_nn class in nnlib2 C++ Artificial Neural Network library.)
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
  \code{\link{LVQ}}
}
\examples{
showClass("LVQ_NN")

# LVQ and SOM expect data in 0 to 1 range, so scale some data...
iris_s<-as.matrix(iris[1:4])
c_min<-apply(iris_s,2,FUN = "min")
c_max<-apply(iris_s,2,FUN = "max")
c_rng<-c_max-c_min
iris_s<-sweep(iris_s,2,FUN="-",c_min)
iris_s<-sweep(iris_s,2,FUN="/",c_rng)

iris_desired_cluster_ids<-as.integer(iris$Species)-1;  # use cluster indexing starting from 0.

# now create and use the NN
lvq<-new("LVQ_NN")

lvq$encode(iris_s,iris_desired_cluster_ids,100)

# just recalling the same data to check how well LVQ was trained...
lvq_recalled_cluster_ids<-lvq$recall(iris_s);
plot(iris_s, pch=lvq_recalled_cluster_ids, main="LVQ recalled clusters (module)")
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
    \item{\code{encode(...)}:}{ Encode input and output (classification) for a dataset using LVQ NN. Parameters are: NumericMatrix data,IntegerVector desired_class_ids,int training_epochs. }
    \item{\code{recall(data_in)}:}{ Get output (classification) for a dataset (NumericMatrix data_in) using LVQ NN. }
    \item{\code{print()}:}{ prints NN structure. }
    \item{\code{load(filename)}:}{ retrieves the NN stored in file. }
    \item{\code{save(filename)}:}{ saves the status of the NN to specified file. }
  }

The following methods are inherited (from the corresponding class):
objectPointer ("RcppClass"), initialize ("RcppClass"), show ("RcppClass")
}
