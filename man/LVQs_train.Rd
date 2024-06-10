\name{LVQs_train}
\alias{LVQs_train}

\title{
Train a Supervised LVQ
}
\description{
This function simplifies training a supervised LVQ (as compared to using the \code{\link{LVQs}} module directly).
}
\usage{
LVQs_train(	train_data,
			train_class,
			iterations = 1000,
			number_of_output_nodes_per_class = 1,
			reward_coef = +0.2,
			punish_coef = -0.2,
			training_order = "reorder",
			initialization_method = "sample",
			recall_train_data = FALSE)
}

\arguments{
  \item{train_data}{
training data, numeric matrix (2d, cases in rows, variables in columns).
}
  \item{train_class}{
vector of integers or factor containing the desired class id for each training data case (row). Expected ids start from 1.
}
  \item{iterations}{
integer, number of training epochs, i.e. number of times the entire training data set will be presented to the NN during training.
}
  \item{number_of_output_nodes_per_class}{
integer, number of output nodes (and thus codebook vectors) to be used per class. A single value is expected, all classes are assigned the same number of output nodes.
}
  \item{reward_coef}{
coefficient used when a output node (and thus codebook vector) is rewarded (has been correctly selected when a training data vector is encoded) and is adjusted closer to the data. For more, see \code{set_encoding_coefficients} method of \code{\link{LVQs}}
}
  \item{punish_coef}{
coefficient used when a output node (and thus codebook vector) is punished (has been incorrectly selected when a training data vector is encoded) and is adjusted away from the data. For more, see \code{set_encoding_coefficients} method of \code{\link{LVQs}}
}
  \item{training_order}{
Order by which the data set vectors will be presented to LVQs for encoding during each training iteration (epoch). Options are: \code{'original'} (vectors are presented in the order in which they are stored, i.e. first row to last), \code{'reorder_once'} (vectors are randomly reordered once, then presented in this same order in all iterations), and \code{'reorder'} (vectors are randomly reordered before each iteration).
}

  \item{initialization_method}{
Defines how the connections weights (codebook vectors) will be initialized. Options are: \code{'0to1'} (random values in [0 1] range, note: internal training data will also be scaled to the same range), \code{'means'} codebook vectors will be the corresponding class's mean vector), \code{'first'} (the first vectors of each class will be used as initial codebook vectors, randomly re-selected if if not enough are available), and \code{'sample'} randomly selected vectors of each class will be used as initial codebook vectors (with replacement if not enough are available).
}
  \item{recall_train_data}{
Once training completes, recall the training data and show accuracy and confusion matrix.
}
}
\details{
This is a wrapper function which internally employs an instance of the \code{\link{LVQs}} module. For more details, see  \code{\link{LVQs}}.
}
\value{
A numeric matrix containing the codebook vector coordinates, the number of times each vector was rewarded during encoding (second from last column named \code{'Rewards'}, ) and the class it corresponds to  (last column, named \code{'Class'}). This matrix can be used by \code{\link{LVQs_recall}} function to classify data.
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

cv<- LVQs_train(train_data,
				train_class,
				number_of_output_nodes_per_class = 4)

# recall (classify) test data:

cl <- LVQs_recall(cv, test_data)

# Compare known and returned test data classifications:

print(table(test_class, cl))
}

