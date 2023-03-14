\name{NN_R_components}
\alias{NN_R_components}

\title{Custom NN components defined using R}

\description{Custom NN components used in \code{\link{NN}} module can be defined using \pkg{nnlib2} C++ classes. Alternatively, NN components can also be defined using R code.}

\section{Introduction}{
In addition to NN components defined using the provided \pkg{nnlib2} C++ classes and class-templates (see Notes in \link{NN}), custom, user-defined NN components also can be created in R code (without any need for C++). Regardless of how they are defined, such components can be added to neural networks created in R via \code{\link{NN}} module and cooperate with each other.}

\section{1. Layers}{Layers of nodes (aka Processing Elements or PEs) whose encode/recall behavior is to be defined using R can be added to the NN via the \code{add_layer} \code{\link{NN}} method. The call to \code{add_layer} should have a single parameter, a \code{list} containing four named elements:
  \itemize{
    \item{\code{name}}{: always equal to \code{"R-layer".}}
    \item{\code{size}}{: the number of nodes in the new layer.}
    \item{\code{encode_FUN}}{: the name of the R function to be called when the layer is encoding data (or \code{""} if none).}
	\item{\code{recall_FUN}}{: the name of the R function to be called when the layer is recalling data (or \code{""} if none).}
	}
For example: \cr\cr \code{p$add_layer(list(name="R-layer", size=100, encode_FUN="", recall_FUN="rfun"))} \cr\cr adds a layer to a NN topology (here the NN is named \code{p}). The new layer will contain 100 nodes, no R function will be used when the layer is encoding, some R function (here named \code{rfun}) will be used when the layer is recalling (mapping) data.\cr
The R functions specified as \code{encode_FUN} and \code{recall_FUN} for layers will have to be defined so that they accept (zero or more) of the following parameters:
  \itemize{
    \item{\code{INPUT}}{: vector of the current incoming numeric values (one per node, length equals size of the layer).}
    \item{\code{INPUT_Q}}{: matrix where each column contains the numeric values that have been sent to the corresponding node.}
    \item{\code{BIAS}}{: vector with the numeric value stored as 'bias' of each node (length equals size of the layer).}
    \item{\code{MISC}}{: vector with the numeric value stored in each node's 'misc' register (length equals size of the layer).}
    \item{\code{OUTPUT}}{: vector with the current output of the layer (length equals size of the layer).}
	}
In particular, for layers the R functions should have the following characteristics:
\itemize{
    \item{Encode function}{: the R function to be called when the layer is encoding data may use any of the parameters listed above and must return a \code{list} containing named items with the new (adjusted) values for \code{BIAS}, \code{MISC} and / or \code{OUTPUT}. If no changes are made by the R function, it may return an empty list.}
    \item{Recall function}{: the R function to be called when the layer is recalling (mapping) data may use any of the parameters listed above and must return a vector containing the layer's new \code{OUTPUT}.}}
Note: The two variations of input (\code{INPUT} and \code{INPUT_Q}) are provided for flexibility in various implementations. Some connection set implementations may only send a single (final) input value to each node. These values are found in \code{INPUT}. Other connection set types may send the individual values from each individual connection, so that they can be processed by the node’s \code{input_function}; these values will be found in \code{INPUT_Q}. Furthermore, there may be designs where a combination of the two is used (not recommended), or several different connection sets are connected and sending data to the same destination layer, etc. Also note that direct access to \code{INPUT} may be removed is future versions.
}

\section{2. Set of connections}{Connection sets whose encode/recall behavior is to be defined using R can be added to the NN via the \code{add_connection_set} or \code{fully_connect_layers_at} \code{\link{NN}} methods. The call to \code{add_connection_set} should have a single parameter, a \code{list} containing named elements:
  \itemize{
    \item{\code{name}}{: always equal to \code{"R-connections".}}
    \item{\code{encode_FUN}}{: the name of the R function to be called when the connection set is encoding data (or \code{""} if none).}
	\item{\code{recall_FUN}}{: the name of the R function to be called when the connection set is recalling data (or \code{""} if none).}
	\item{\code{requires_misc}}{: (optional) logical, if TRUE each connection will be provided with an extra 'misc' data register.}
	}
For example: \cr\cr \code{p$add_connection_set(list(name="R-connections",encode_FUN="ef",recall_FUN="rf"))} \cr\cr adds a set of connections to the NN topology (here the NN is named \code{p}). The new connection set will use some R function (here named \code{ef}) when encoding data and another R function (here named \code{rf}) when recalling (mapping) data.\cr\cr
Note that here each set of connections maintains connection weights and (if required) misc values in matrices. During encode or recall (map) operations, the connection weights matrix, misc values matrix (if any) and other data from the connected layers are sent for processing to the two R functions. No \code{connection_set} or \code{connection} classes are employed (this is significantly less versatile than what these nnlib2’s C++ classes offer). Thus, the R functions specified as \code{encode_FUN} and \code{recall_FUN} for connection sets will have to be defined so that they accept (zero or more) of the following parameters:
  \itemize{
    \item{\code{WEIGHTS}}{: numeric matrix (s rows, d columns). This matrix contains the current connection weights..}
    \item{\code{SOURCE_INPUT}}{: numeric vector (length s) containing the current input values of the nodes in the source layer (note: nodes often reset this values after they have processed them).}
    \item{\code{SOURCE_outPUT}}{: numeric vector (length s) containing the current output values of the nodes in the source layer.}
    \item{\code{SOURCE_MISC}}{: numeric vector (length s) with the numeric value stored in 'misc' registers of each node in the source layer.}
    \item{\code{DESTINATION_INPUT}}{: numeric vector (length d) containing the current input values of the nodes in the destination layer (note: nodes often reset this values after they have processed them).}
    \item{\code{DESTINATION_outPUT}}{: numeric vector (length d) containing the current output values of the nodes in the destination layer.}
    \item{\code{DESTINATION_MISC}}{: numeric vector (length d) with the numeric value stored in 'misc' registers of each node in the destination layer.}
    \item{\code{MISC}}{: numeric matrix (s rows, d columns). If not used, this is a matrix of 0 rows and 0 columns, otherwise it contains the values of  the ‘misc’ register in each connection.}
}
where s is the number of nodes (length) of the source layer and d the number nodes in the destination layer.\cr

The R functions for connection sets should have the following characteristics:

\itemize{
    \item{Encode function}{: the R function to be called when the connection set is encoding data may use any of the parameters listed above, but must return a \code{list} containing the new (adjusted) connection weights (named \code{WEIGHTS}, numeric matrix of s rows, d columns) and possibly the new connection ‘misc’ values (named \code{MISC}, numeric matrix of s rows, d columns). If no changes were made by the R function, it may return an empty list.}
    \item{Recall function}{: the R function to be called when the connection set is recalling or mapping data may use any of the parameters listed above and must return a numeric matrix of d columns. Each column of this matrix should contain the data values to be sent to the corresponding node (PE) in the destination layer. (Note: this matrix is similar to the \code{INPUT_Q} used in layers, see above).}
}
}

\section{3. Control components}{
Other special control or processing components can be added using \code{\link{NN}} module's \code{add_R_forwarding}, \code{add_R_pipelining}, \code{add_R_ignoring}, and \code{add_R_function} methods. See \code{\link{NN}} module.}

\author{
Vasilis N. Nikolaidis <vnnikolaidis@gmail.com>
}

\note{
Defining NN components with custom behavior in R does have a cost in terms of runtime performance. It also, to a certain degree, defies some of the reasons for using C++ classes. However, it may be useful for experimentation, prototyping, education purposes etc.
}

\seealso{\code{\link{NN}}.}

\examples{\dontrun{
#-------------------------------------------------------------------------------
# 1. LAYER EXAMPLE:

# Example R function to be used when the layer is encoding:
# Version for when the final input (a single value per PE) is directly sent to
# the layer (by set_input or some connection set).
# Outputs difference from current bias values, stores current input as new bias:

LAYERenc1 <- function(INPUT,BIAS,...)
{
	i <- INPUT				# get values directly injected as input to the PE.
	o <- i-BIAS				# subtract old bias from input.
	# update layer's output and biases:
	return(list(OUTPUT=o, BIAS=INPUT))
}

# Example R function to be used when the layer is recalling (mapping):
# Version for when the final input (a single value per PE) is directly sent to
# the layer (by set_input or some connection set).
# Outputs difference from current bias values:

LAYERrec1 <- function(INPUT,BIAS,...)
{
	i <- INPUT				# get values directly injected as input to the PE.
	o <- i-BIAS				# subtract old bias from input.
	return(o)				# return this as output.
}

# Example R function to be used when the layer is encoding (same as above):
# Version for cases where a connection set is designed to send multiple
# values (one for each incoming connection) to each PE in the layer so that
# the PE can process them as needed. - typicaly via its 'input_function'.
# (also works when set_input is used)
# INPUT_Q is a matrix where each column contains the values that have been sent
# to the corresponding node (PE).
# Outputs difference from current bias values, stores current input as new bias:

LAYERenc2 <- function(INPUT_Q,BIAS,...)
{
	i <- colSums(INPUT_Q)	# summate incoming values to produce final input.
	o <- i-BIAS				# subtract old bias from that input.
	# update layer's output and biases:
	return(list(OUTPUT=o, BIAS=i))
}

# Example R function to be used when the layer is recalling/mapping (same as above):
# version for cases where a connection set is designed to send multiple
# values (one for each incoming connection) to each PE in the layer so that
# the PE can process them as needed. - typicaly via its 'input_function'.
# (also works when set_input is used)
# INPUT_Q is a matrix where each column contains the values that have been sent
# to the corresponding node (PE).
# Outputs difference from current bias values:

LAYERrec2 <- function(INPUT_Q,BIAS,...)
{
	i <- colSums(INPUT_Q)	# summate incoming values to produce final input.
	o <- i-BIAS				# subtract old bias from that input.
	return(o)				# return this as output.
}

# create and setup a "NN".

n<-new("NN")
n$add_layer(list(name="R-layer", size=4,
				 encode_FUN="LAYERenc1", recall_FUN="LAYERrec1"))

# test the layer:

n$set_input_at(1,c(1,0,5,5))
n$encode_at(1)
print(n$get_biases_at(1))

n$set_input_at(1,c(20,20,20,20))
n$recall_at(1)
print(n$get_output_at(1))
n$set_input_at(1,c(10,0,10,0))
n$recall_at(1)
print(n$get_output_at(1))

#-------------------------------------------------------------------------------
# 2. CONNECTION SET EXAMPLE:

# This simple connection set will encode data by adding to each connection
# weight the output of the source node.

CSenc <- function(WEIGHTS, SOURCE_OUTPUT,...)
{
	x <- WEIGHTS + SOURCE_OUTPUT
	return(list(WEIGHTS=x))
}

# When recalling, this simple connection set multiplies source data by weights.
# this version sends multiple values (the products) to each destination node.
# Typical (s.a. generic) nodes add these values to process them.

CSrec1 <- function(WEIGHTS, SOURCE_OUTPUT,...)
{
	x <- WEIGHTS * SOURCE_OUTPUT
	return(x)
}

# When recalling, this simple connection set multiplies source data by weights.
# this version sends a single value (the sum of the products) to each
# destination node.

CSrec2 <- function(WEIGHTS, SOURCE_OUTPUT,...)
{
	x <-  SOURCE_OUTPUT %*% WEIGHTS
	return(x)
}

# create and setup a "NN".

n<-new("NN")
n$add_layer("generic",4)
n$add_connection_set(list(name="R-connections",encode_FUN="CSenc",recall_FUN="CSrec2"))
n$add_layer("generic",2)
n$create_connections_in_sets(0,0)

# test the NN:

n$set_input_at(1,c(0,1,5,10))
n$encode_all_fwd()
n$set_input_at(1,c(1,1,1,1))
n$encode_all_fwd()

# see if weights were modified:
print(n$get_weights_at(2))

n$set_input_at(1,c(20,20,20,20))
n$recall_all_fwd()
print(n$get_output_at(3))

#-------------------------------------------------------------------------------
# 3. A COMPLETE EXAMPLE (simple single layer perceptron-like NN):

# Function for connections, when recalling/mapping:
# Use any one of the two functions below.
# Each column of the returned matrix contains the data that will be sent to the
# corresponding destination node.

# version 1: sends multiple values (product) for destination nodes to summate.

CSmap1 <- function(WEIGHTS, SOURCE_OUTPUT,...)  WEIGHTS * SOURCE_OUTPUT

# version 2: sends corresponding value (dot product) to destination node.

CSmap2 <- function(WEIGHTS, SOURCE_OUTPUT,...) SOURCE_OUTPUT %*% WEIGHTS

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Function for connections, when encoding data:

learning_rate <- 0.3

CSenc <- function(WEIGHTS, SOURCE_OUTPUT, DESTINATION_MISC, DESTINATION_OUTPUT, ...)
{
  a <- learning_rate *
          (DESTINATION_MISC - DESTINATION_OUTPUT)   # desired output is in misc registers.
  a <- outer( SOURCE_OUTPUT, a , "*" )              # compute weight adjustments.
  w <- WEIGHTS + a                                  # compute adjusted weights.
  return(list(WEIGHTS=w))                           # return new (adjusted) weights.
}

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Function for layer, when recalling/mapping:
# (note: no encode function is used for the layer in this example)

LAmap <- function(INPUT_Q,...)
{
	x <- colSums(INPUT_Q)		# input function is summation.
	x <- ifelse(x>0,1,0)		# threshold function is step.
	return(x)
}

#- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# prepare some data based on iris data set:

data_in <- as.matrix(iris[1:4])
iris_cases <- nrow((data_in))
# make a "one-hot" encoding matrix for iris species
desired_data_out <- matrix(data=0, nrow=iris_cases, ncol=3)
desired_data_out[cbind(1:iris_cases,unclass(iris[,5]))]=1

# create the NN and define its components:
# (first generic layer simply accepts input and transfers it to the connections)

p <- new("NN")

p$add_layer("generic",4)

p$add_connection_set(list(name="R-connections",
                          encode_FUN="CSenc",
                          recall_FUN="CSmap2"))

p$add_layer(list(name="R-layer",
                 size=3,
                 encode_FUN="",
                 recall_FUN="LAmap"))

p$create_connections_in_sets(0,0)

# encode data and desired output (for 50 training epochs):

for(i in 1:50)
	for(c in 1:iris_cases)
	{
		p$input_at(1,data_in[c,])
		p$set_misc_values_at(3,desired_data_out[c,])  # put desired output in misc registers
		p$recall_all_fwd();
		p$encode_at(2)
	}

# Recall the data and show NN's output:

for(c in 1:iris_cases)
{
	p$input_at(1,data_in[c,])
	p$recall_all_fwd()
	cat("iris case ",c,", desired = ", desired_data_out[c,],
		" returned = ", p$get_output_from(3),"\n")
}
}}
