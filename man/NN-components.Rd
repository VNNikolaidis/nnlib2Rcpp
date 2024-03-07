\name{NN_component_names}
\alias{NN_component_names}

\title{Names of available NN components}

\description{A quick summary of names that can be used for adding NN components in a \code{\link{NN}} module. These names are available in the current package version. More components can be defined by the user or may be added in future versions.}

\section{Current names for layers:}{
Layer names currently available include:
  \itemize{
    \item\code{generic}: a layer of generic Processing Elements (PEs).
    \item\code{generic_d}: same as above.
    \item\code{pe}: same as above.
    \item\code{pass-through}: a layer with PEs that simply pass input to output.
    \item\code{which-max}: a layer with PEs that return the index of one of their inputs whose value is maximum.
    \item\code{MAM}: a layer with PEs for Matrix-Associative-Memory NNs (see vignette).
    \item\code{LVQ-input}: LVQ input layer (see vignette).
    \item\code{LVQ-output}: LVQ output layer (see vignette).
    \item\code{BP-hidden}: Back-Propagation hidden layer (see vignette).
    \item\code{BP-output}: Back-Propagation output layer (see vignette).
    \item\code{R-layer}: A layer whose encode and recall (map) functionality is defined in R (see \link{NN_R_components}).
}
Additional (user-defined) layers currently available include:
  \itemize{
    \item\code{JustAdd10}: a layer where PEs output the sum of their inputs plus 10 (created for use as example in vingnette).
    \item\code{perceptron}: a classic perceptron layer (created for use as example in \href{https://www.r-bloggers.com/2020/07/creating-custom-neural-networks-with-nnlib2rcpp/}{in this post}).
    \item\code{MEX}: a layer created for use as example in vingnette.
    \item\code{example_layer_0}: a layer created to be used as a simple code example for users creating custom layers.
    \item\code{example_layer_1}: as above.
    \item\code{example_layer_2}: as above.
    \item\code{BP-hidden-softmax}: Back-Propagation hidden layer that performs softmax on its output (when recalling).
    \item\code{BP-output-softmax}: Back-Propagation output layer that performs softmax on its output (when recalling).
    \item\code{softmax}: a layer that (during recall) sums its inputs and outputs the softmax values.
    \item\code{R-connections}: A set of connections whose encode and recall (map) functionality is defined in R (see \link{NN_R_components}).
}
}

\section{Current names for sets of connections:}{
Names for connection sets that are currently available include:
  \itemize{
    \item\code{generic}: a set of generic connections.
    \item\code{pass-through}: connections that pass data through with no modification.
    \item\code{wpass-through}: connections that pass data multiplied by weight.
    \item\code{MAM}: connections for Matrix-Associative-Memory NNs (see vignette).
    \item\code{LVQ}: connections for LVQ NNs (see vignette).
    \item\code{BP}: connections for Back-Propagation (see vignette).
}
Additional (user-defined) connection sets currently available include:
  \itemize{
    \item\code{perceptron}: connections for perceptron (created for use as example in \href{https://www.r-bloggers.com/2020/07/creating-custom-neural-networks-with-nnlib2rcpp/}{in this post}).
    \item\code{MEX}: a connection set created for use as example in vingnette.
    \item\code{example_connection_set_0}: a connection set created to be used as a simple code example for users creating custom types of connection sets.
    \item\code{example_connection_set_1}: as above.
    \item\code{example_connection_set_2}: as above.
}
}

\author{
Vasilis N. Nikolaidis <vnnikolaidis@gmail.com>
}

\note{
These are component names that can be currently used to add components to a NN  using the methods provided by \code{\link{NN}} module. Such methods include \code{add_layer}, \code{add_connection_set}, \code{connect_layers_at}, \code{fully_connect_layers_at} etc. Some of these components may be experimental or created for use in examples and may change or be removed in future versions, while other components may be added.

More information on expanding the library with new, user-defined types of NN components (nodes, layers, connections etc) and models, can be found in the package's vignette as well as the related \href{https://github.com/VNNikolaidis/nnlib2Rcpp}{repository on Github}). A quick example can also be found \href{https://www.r-bloggers.com/2020/07/creating-custom-neural-networks-with-nnlib2rcpp/}{in this post}. Please consider submitting any useful components you create, to enrich  future versions of the package.
}

\seealso{\code{\link{NN}}, \link{NN_R_components}.}


