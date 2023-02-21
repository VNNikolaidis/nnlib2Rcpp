\name{nnlib2Rcpp-package}
\Rdversion{1.1}
\docType{package}
\alias{nnlib2Rcpp}

\title{A collection of Neural Networks and tools to create custom models}
\description{
This package provides a module (NN module) to define and control custom neural networks created from user-defined nnlib2 NN components (written in C++).

It also contains a small collection of ready-to-use Neural Networks (NN), i.e. versions of Autoencoder, Back-Propagation, Learning Vector Quantization and Matrix Associative Memory NN. More information and examples for each of the above can be found in its documentation (see below).
}

\section{Ready-to-use Neural Networks:}{
\itemize{
\item{Plain Back-Propagation (BP-supervised) (\code{\link{BP}})}
\item{Learning Vector Quantization (LVQ-supervised) (\code{\link{LVQs}})}
\item{Learning Vector Quantization (LVQ-unsupervised) (\code{\link{LVQu}})}
\item{Matrix Associative Memory (MAM-supervised) (\code{\link{MAM}}})
\item{Autoencoder (unsupervised) (\code{\link{Autoencoder}})}
}}

\section{Custom Neural Networks:}{
\itemize{
\item{NN module (\code{\link{NN}})}
}}

\references{
\itemize{
\item{
Nikolaidis, V. N., (2021). The nnlib2 library and nnlib2Rcpp R package for implementing neural networks. Journal of Open Source Software, 6(61), 2876, \doi{10.21105/joss.02876}.}

References for the ready-to-use NN models (can also be found in related documentation):
\itemize{
\item{
Kohonen, T (1988). Self-Organization and Associative Memory, Springer-Verlag.; Simpson, P. K. (1991). Artificial neural systems: Foundations, paradigms, applications, and implementations. New York: Pergamon Press.}

\item{
Pao Y (1989). Adaptive Pattern Recognition and Neural Networks. Reading, MA (US); Addison-Wesley Publishing Co., Inc.}

\item{
Simpson, P. K. (1991). Artificial neural systems: Foundations, paradigms, applications, and implementations. New York: Pergamon Press.}

\item{
Philippidis, TP & Nikolaidis, VN & Kolaxis, JG. (1999). Unsupervised pattern recognition techniques for the prediction of composite failure. Journal of acoustic emission. 17. 69-81.}

\item{
Nikolaidis V.N., Makris I.A, Stavroyiannis S, "ANS-based preprocessing of company performance indicators." Global Business and Economics Review 15.1 (2013): 49-58, \doi{10.1504/GBER.2013.050667}.}
}
}
}
\seealso{
More information and examples on using the package can be found in the following vignette:

\code{vignette("manual", package='nnlib2Rcpp')}

Related links:
\itemize{
  \item \url{https://github.com/VNNikolaidis/nnlib2Rcpp}
  \item Package manual in PDF format at \url{https://github.com/VNNikolaidis/nnlib2Rcpp/blob/master/support/manual.pdf})
  \item Report bugs, issues and suggestions at \url{https://github.com/VNNikolaidis/nnlib2Rcpp/issues}
}

}
\author{
Author/Maintainer:
\itemize{
  \item{Vasilis Nikolaidis \email{vnnikolaidis@gmail.com}}
}

Contributors:
\itemize{
  \item Arfon Smith [contributor]
  \item Dirk Eddelbuettel [contributor]
}
}
