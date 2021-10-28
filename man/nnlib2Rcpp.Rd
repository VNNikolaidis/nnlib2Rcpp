\name{nnlib2Rcpp-package}
\Rdversion{1.1}
\docType{package}
\alias{nnlib2Rcpp}

\title{A collection of Neural Networks and tools to create custom models}
\description{
This package contains a small collection of ready-to-use Neural Networks (NN), i.e. versions of Autoencoder, BP, LVQ and MAM NN. It also provides a module (NN module) to define and control neural networks created from custom nnlib2 NN components. More information and examples for each of the above can be found in its help documentation (see below).
}

\section{Ready-to-use Neural Networks:}{
\itemize{
\item{Plain Back-Propagation (BP-supervised) (\code{\link{BP}})}
\item{Learning Vector Quantization (LVQ-supervised) (\code{\link{LVQs}})}
\item{Learning Vector Quantization (LVQ-unsupervised) (\code{\link{LVQu}})}
\item{Matrix-Associative-Memory (MAM-supervised) (\code{\link{MAM}}})
\item{Autoencoder (unsupervised) (\code{\link{Autoencoder}})}
}}

\section{Custom Neural Networks:}{
\itemize{
\item{NN module (\code{\link{NN}})}
}}

\references{
References:
\itemize{
\item{

Nikolaidis, V. N., (2021). The nnlib2 library and nnlib2Rcpp R package for implementing neural networks. Journal of Open Source Software, 6(61), 2876, url:\url{https://doi.org/10.21105/joss.02876}
}
\item{
Nikolaidis V. N., Makris I. A., Stavroyiannis S. (2013). ANS-based preprocessing
of company performance indicators. Global Business and Economics
Review, 15 (1), 49-58. doi: 10.1504/GBER.2013.050667, url:\url{https://doi.org/10.1504/GBER.2013.050667}}
}
}

\seealso{
Detailed information on using the package can be found in the package vignette:

\code{vignette("manual", package='nnlib2Rcpp')}

(also available in PDF format here:
\url{https://github.com/VNNikolaidis/nnlib2Rcpp/blob/master/doc/manual.pdf})

Related links:
\itemize{
  \item \url{https://github.com/VNNikolaidis/nnlib2Rcpp}
  \item Report bugs, issues and suggestions at \url{https://github.com/VNNikolaidis/nnlib2Rcpp/issues}
}

}
\author{
Author/Maintainer:
\itemize{
  \item{Vasilis Nikolaidis \email{vnnikolaidis@gmail.com}}
}

Other contributors:
\itemize{
  \item Arfon Smith [contributor]
  \item Dirk Eddelbuettel [contributor]
}
}
