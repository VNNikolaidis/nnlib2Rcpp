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
Simpson, P. K. (1991). Artificial neural systems: Foundations, paradigms, applications, and implementations. New York: Pergamon Press. p.88.
}
\author{
Vasilis N. Nikolaidis <vnnikolaidis@gmail.com>
}
\note{
This module uses Rcpp to employ 'lvq_nn' class in nnlib2.
The NN used in this module uses supervised training for data classification (described as Supervised Learning LVQ in Simpson (1991)). Initial weights are random (uniform distribution) in 0 to 1 range. As these weights represent vector coordinates (forming the class reference, prototype or codebook vector(s)), it is important that input data is also scaled to 0 to 1 range.
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
  \code{\link{LVQu}} (unsupervised LVQ function).
}

\examples{
# Example 1:

# LVQ expects data in 0 to 1 range, so scale some numeric data...
DATA <- as.matrix(iris[1:4])
c_min <- apply(DATA, 2, FUN = "min")
c_max <- apply(DATA, 2, FUN = "max")
c_rng <- c_max - c_min
DATA <- sweep(DATA, 2, FUN = "-", c_min)
DATA <- sweep(DATA, 2, FUN = "/", c_rng)

# create a vector of desired class ids (starting from 0):
CLASS <- as.integer(iris$Species) - 1
NUM_CLASSES <- length(unique(CLASS))

# now create the NN:
lvq <- new("LVQs")

# Optionally, the output layer could be expanded, e.g. use 2 nodes per each class:
lvq$set_number_of_nodes_per_class(2)

# Optionally, for experimentation negative reinforcement can be disabled:
# lvq$disable_punishment()

# train it:
lvq$encode(DATA, CLASS, 100)

# recall the same data (a simple check of how well the LVQ was trained):
lvq_recalled_class_ids <- lvq$recall(DATA)

# show results:
cat("Correct: ", sum(lvq_recalled_class_ids == CLASS), "\n")

# Done. Optional part for further examining results of training:

# collect the connection weights (codebook vector coordinates), number
# of rewards per node and corresponding class:

lvq_codebook_vector_info <-
	cbind(
		matrix(lvq$get_weights(),
			   ncol = ncol(DATA),
			   byrow = TRUE),
		lvq$get_number_of_rewards(),
		rep(
			0:(NUM_CLASSES - 1),
			rep(lvq$get_number_of_nodes_per_class(),
				NUM_CLASSES)
		)
	)

colnames(lvq_codebook_vector_info) <-
	c(colnames(DATA), "Rewarded", "Class")

print(lvq_codebook_vector_info)

# plot recalled classification:

plot(
	DATA,
	pch = lvq_recalled_class_ids,
	main = "LVQ recalled clusters (LVQs module)",
	xlim = c(-0.2, 1.2),
	ylim = c(-0.2, 1.2)
)

# plot connection weights (a.k.a codebook vectors):
# the big circles are codebook vectors, (crossed-out if they were never used
# to assign a training data point to the correct class, i.e. never rewarded)

points(
	lvq_codebook_vector_info[, 1:2],
	cex = 4,
	pch = ifelse(lvq_codebook_vector_info[, "Rewarded"] > 0,	1, 13),
	col  = lvq_codebook_vector_info[, "Class"] + 10
)

# Example 2: (to demonstrate using setup method)
# Note: using the same data (DATA,CLASS,NUM_CLASSES variables) as Example 1 above

# create the NN:
lvq2 <- new("LVQs")

# setup the LVQ:
lvq2$setup(length(DATA[1, ]), NUM_CLASSES, 2)
lvq2$set_weight_limits(-0.5 , 1.5)
lvq2$set_encoding_coefficients(0.2,-sum(CLASS == 0) / length(CLASS))

# train it:
lvq2$encode(DATA, CLASS, 100)

# recall the same data (a simple check of how well the LVQ was trained):
lvq2_recalled_class_ids <- lvq2$recall(DATA, 1)

# show results:
cat("Correct: ", sum(lvq2_recalled_class_ids == CLASS), "\n")
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

    \item{\code{encode(data, desired_class_ids, training_epochs)}:}{ Encode input and output (classification) for a dataset using a LVQ NN (which sets up accordingly if required). Parameters are:}
    \itemize{
    \item\code{data}: training data, a numeric matrix, (2d, cases in rows, variables in columns). Data should be in 0 to 1 range.
    \item\code{desired_class_ids} : vector of integers containing a desired class id for each training data case (row). Should contain integers in 0 to n-1 range, where n is the number of classes.
  \item\code{training_epochs}: integer, number of training epochs, aka presentations of all training data to the NN during training.
  }

    \item{\code{recall(data_in, min_rewards)}:}{ Get output (classification) for a dataset (numeric matrix \code{data_in}) from the (trained) LVQ NN. The \code{data_in} dataset should be 2-d containing  data cases (rows) to be presented to the NN and is expected to have same number or columns as the original training data. Returns a vector of integers containing a class id for each case (row).Parameters are:
    \itemize{
    \item\code{data_in}: numeric 2-d matrix containing  data cases (as rows).
    \item\code{min_rewards}: (optional) integer, ignore output nodes that (during encoding/training) were rewarded less times that this number (default is 0, i.e. use all nodes).
    }
    }

\item{\code{setup( input_length, int number_of_classes, number_of_nodes_per_class )}:}{Setup an untrained supervised LVQ for given input data vector dimension and number of classes. Parameters are:
    \itemize{
    \item\code{input_length}: integer, dimension (length) of input data vectors.
    \item\code{number_of_classes}: integer, number of classes in data (including empty ones).
    \item\code{number_of_nodes_per_class}: (optional) integer, number of output nodes (PE) to be used per class. Default is 1.
    }
    }

 \item{\code{train_single (data_in, class_id, epoch)}:}{ Encode a single [input vector,class] pair in the LVQ NN. Only performs a single training iteration (multiple may be required for proper encoding). Vector length and class id should be compatible to the current NN (as resulted from the \code{encode}, \code{setup} or \code{load} methods). Returns \code{TRUE} if succesfull, \code{FALSE} otherwise. Parameters are:
    \itemize{
    \item\code{data_in}: numeric, data vector to be encoded.
    \item\code{class_id}: integer, id of class corresponding to the data vector.(ids start from 0).
    \item\code{epoch}: integer, presumed epoch during which this encoding occurs (learning rate decreases with epochs in supervised LVQ).
    }
    }

 \item{\code{get_weights()}:}{Get the current weights (codebook vector coordinates) of the 2nd component (\code{connection_set}). If successful, returns NumericVector of connection weights (otherwise vector of zero length).
    }

 \item{\code{set_weights( data_in )}:}{Set the weights of the 2nd component (\code{connection_set}), i.e. directly define the LVQ's codebook vectors. If successful, returns TRUE. Parameters are:
    \itemize{
    \item\code{data_in}: NumericVector, data to be used for new values in \code{weight} registers of connections (sizes must match).
    }
    }

 \item{\code{set_number_of_nodes_per_class( n )}:}{Set the number of nodes in the output layer (and thus incoming connections whose weights form codebook vectors) that will be used per class. Default is 1, i.e. each class in the data to be encoded in the NN corresponds to a single node (PE) in it's output layer. This method affects how the new NN topology will be created, therefore this method should be used before the NN has been set up (either by \code{encode} or \code{setup}) or after a NN topology (and NN state) has been loaded from file via \code{load}). Returns number of nodes to be used per class. Parameters are:
    \itemize{
    \item\code{n}: integer, number of nodes to be used per each class.
    }
    }

 \item{\code{get_number_of_nodes_per_class( )}:}{Get the number of nodes in the output layer that are used per class.}


 \item{\code{enable_punishment( )}:}{ Enables negative reinfoncement. During encoding incorrect winner nodes will be notified and incoming weights will be adjusted accordingly. Returns TRUE if punishment is enabled, FALSE otherwise. }

 \item{\code{disable_punishment( )}:}{ Disables negative reinfoncement. During encoding incorrect winner nodes will not be notified, thus incoming weights will not be adjusted accordingly. Adjustments will only occur in correct winning nodes. Returns TRUE if punishment is enabled, FALSE otherwise. }


 \item{\code{get_number_of_rewards( )}:}{ Get the number of times an output node was positively reinforced during data encoding. Returns NumericVector containing results per output node. }

\item{\code{set_weight_limits(  min, max )}:}{Define the minimum and maximum values that will be allowed in connection weights during encoding (limiting results of punishment). The NN must have been set up before using this method (either by \code{encode}, \code{setup} or \code{load}). Parameters are:
    \itemize{
    \item\code{min}: numeric, minimum weight allowed.
    \item\code{max}: numeric, maximum weight allowed.
    }
    }

\item{\code{set_encoding_coefficients( reward, punish )}:}{Define coefficients used for reward and punishment during encoding. In this version, the actual learning rate a(t) also depends on the epoch t, i.e. a(t) = coefficient * (1 - (t/10000)). The NN must have been set up before using this method (either by \code{encode}, \code{setup} or \code{load}). Parameters are:
    \itemize{
    \item\code{reward}: numeric, coefficient used to reward a node that classified data correctly (usually positive, e.g. 0.2).
    \item\code{punish}: numeric, coefficient used to punish a node that classified data incorrectly (usually negative, e.g. -0.2).
    }
    }

    \item{\code{print()}:}{ print NN structure. }

    \item{\code{show()}:}{ print NN structure. }

    \item{\code{load(filename)}:}{ Retrieve the state of the NN from specified file. Note: parameters such as number of nodes per class or reward/punish coefficients are not retrieved. }

    \item{\code{save(filename)}:}{ Store the state of the current NN to specified file. Note: parameters such as number of nodes per class or reward/punish coefficients are not stored.}
  }

The following methods are inherited (from the corresponding class):
objectPointer ("RcppClass"), initialize ("RcppClass"), show ("RcppClass")
}
