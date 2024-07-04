\name{LVQs_train}
\alias{LVQs_train}

\title{
LVQs Helper Function: Train a Supervised LVQ
}
\description{
This function simplifies using a Supervised Learning Vector Quantizer Neural Network on data (as compared to using the \code{\link{LVQs}} module directly). It trains a supervised Learning Vector Quantizer Neural Network (\code{\link{LVQs}}). Once the NN is trained, the function returns a matrix containing codebook vectors and related information. This matrix can then be used by \code{\link{LVQs_recall}} to classify data.
}
\usage{
LVQs_train(	train_data,
			train_class,
			iterations = 1000,
			number_of_output_nodes_per_class = 1,
			reward_coef = +0.2,
			punish_coef = -0.2,
			training_order = "reorder_once",
			initialization_method = "sample",
			recall_train_data = FALSE,
			initial_codebook_vectors = NULL
			)
}

\arguments{
  \item{train_data}{
training data, numeric matrix (2d, cases in rows, variables in columns).
}
  \item{train_class}{
vector of integers or factor containing the desired class id for each training data case (row). Expected ids start from 1. Number of classes is assumed to be equal to the maximum class id found here.
}
  \item{iterations}{
integer, number of training epochs, i.e. number of times the entire training data set will be presented to the NN during training. Maximum allowed is 10000.
}
  \item{number_of_output_nodes_per_class}{
integer, number of output nodes (and thus codebook vectors) to be used per class. A single value is expected, all classes are assigned this (same) number of output nodes.
}
  \item{reward_coef}{
coefficient used when a output node (and thus codebook vector) is rewarded (has been correctly selected when a training data vector is encoded) and is adjusted closer to the data. For more, see \code{set_encoding_coefficients} method of \code{\link{LVQs}}
}
  \item{punish_coef}{
coefficient used when a output node (and thus codebook vector) is punished (has been incorrectly selected when a training data vector is encoded) and is adjusted away from the data. For more, see \code{set_encoding_coefficients} method of \code{\link{LVQs}}
}
  \item{training_order}{
order by which the data set vectors will be presented to LVQs for encoding during each training iteration (epoch). Options are: \code{'original'} (vectors are presented in the order in which they are stored, i.e. first row to last), \code{'reorder_once'} (vectors are randomly reordered once, then presented in this same order in all iterations), and \code{'reorder'} (vectors are randomly reordered before each iteration).
}

  \item{initialization_method}{
defines how the connections weights (codebook vectors) will be initialized. Options are: \code{'0to1'} (random values in [0 1] range, note: internal training data will also be scaled to the same range), \code{'means'} codebook vectors will be the corresponding class's mean vector), \code{'first'} (the first data vector(s) of each class will be used as initial codebook vector(s), randomly re-selected if not enough are available), \code{'sample'} (randomly selected data vectors of each class will be used as initial codebook vectors, with replacement if not enough are available), and \code{'user-defined'} (weights specified in parameter \code{initial_codebook_vectors} are used to initialize the LVQ).
}

  \item{recall_train_data}{
once training completes, recall the training data and show accuracy and confusion matrix.
}

 \item{initial_codebook_vectors}{
a matrix of codebook vectors to be used as initial weight values when \code{initialization_method} parameter is set to \code{'user-defined'} (see above). Must have the same number of columns as \code{train_data} and sufficient codebook vectors (rows) to initialize all connections (i.e. number of classes found in \code{train_class} * \code{number_of_output_nodes_per_class}). Note: the matrix returned by a previous invocation of \code{LVQs_train} can be used here, excluding its last two columns (those named \code{'Rewards'} and \code{'Class'}).
}
}

\details{
This is a wrapper function which internally employs an instance of the \code{\link{LVQs}} module. For more details, see  \code{\link{LVQs}}.
}
\value{
A numeric matrix containing the codebook vector coordinates, the number of times each vector was rewarded during encoding (second from last column named \code{'Rewards'}, ) and the class it corresponds to  (last column, named \code{'Class'}). This matrix can be used by \code{\link{LVQs_recall}} function to classify other data.
}
\references{
Simpson, P. K. (1991). Artificial neural systems: Foundations, paradigms, applications, and implementations. New York: Pergamon Press. p.88.
}
\author{
Vasilis N. Nikolaidis <vnnikolaidis@gmail.com>
}
\seealso{
\code{\link{LVQs_recall}}, \code{\link{LVQs}}.
}
\examples{
# start with the well-know iris dataset:

DATA <- iris[,1:4]
CLASS <- as.factor(iris$Species)

# Randomly split the data into training and testing sets:

indices <- sample(1:nrow(DATA), size = .5 * nrow(DATA))

train_data  <- DATA[indices, ]
train_class <- CLASS[indices]

test_data  <- DATA[-indices, ]
test_class <- CLASS[-indices]

# train LVQ using train data and class:

cvi <- LVQs_train(train_data,
				  train_class,
				  number_of_output_nodes_per_class = 4)

# recall (classify) test data:

cl <- LVQs_recall(cvi, test_data)

# Compare known and returned test data classifications:

print(table(test_class, cl))
}

