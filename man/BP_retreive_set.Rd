\name{BP_recall_set}
\alias{BP_recall_set}
\title{
Apply a (trained) BP ANN to data
}
\description{
Present the data as input to a (trained) BP ANN and produce corresponding output.
}
\usage{
BP_recall_set(data_in)
}
\arguments{
  \item{data_in}{
numeric matrix, containing input vectors as rows (for operation to succeed, they should have length equal to the size of the input layer in the current active BP).
}
}
\details{
This is the main function for using the active BP once it is trained, as it accepts an input dataset and produces corresponding output. It is one of several functions used to handle BP supervised ANNs in this package(*). A BP NN object must be created by invoking the BP_create (or BP_load_from_file) function before it can be used . Any further calls of BP_create would replace the current BP NN by a new one. The BP NN object should also be deleted when no longer needed by invoking the BP_destroy function.

(*) A simpler approach is to use \code{\link{BP_NN}} which includes all the required BP functionality in a module.
}
\value{
numeric matrix, containing corresponding vectors returned from the BP ANS.
}
\references{
Simpson, P. K. (1991). Artificial neural systems: Foundations, paradigms, applications, and implementations. New York: Pergamon Press.
}
\author{
Vasilis N. Nikolaidis <vnnikolaidis@gmail.com>
}
\note{
These functions employ a Back-Propagation (BP) multilayer perceptron NN described in
Simpson (1991) as the vanilla back-propagation algorithm, to store input-output vector pairs.

(This function uses Rcpp to employ bp_nn class in nnlib2 C++ Artificial Neural Network library.)
}
%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
  \code{\link{BP_create}},
  \code{\link{BP_train_single}},
  \code{\link{BP_train_set}},
  \code{\link{BP_recall_set}},
  \code{\link{BP_print}},
  \code{\link{BP_save_to_file}},
  \code{\link{BP_load_from_file}},
  \code{\link{BP_destroy}},
  \code{\link{BP_NN}}.
}
\examples{
# prepare input-output data sets to be used for training
iris_s                  <- as.matrix(scale(iris[1:4]))

# use a randomply picked subset of (scaled) iris data for training
training_cases          <- sample(1:nrow(iris_s), nrow(iris_s)/2,replace=FALSE)
train_set               <- iris_s[training_cases,]
train_class_ids         <- as.integer(iris$Species[training_cases])
train_num_cases         <- nrow(train_set)
train_num_variables     <- ncol(train_set)
train_num_classes       <- max(train_class_ids)

# create output dataset to be used for training, Here we encode class as 0s and 1s
train_set_data_out <- matrix(
	data = 0,
	nrow = train_num_cases,
	ncol = train_num_classes)

# for each case, assign 1 to the column corresponding to its class, keep 0 otherwise
# (note: there must be a better R syntax way to do this)

for(r in 1:train_num_cases) train_set_data_out[r,train_class_ids[r]]=1

# finally create the BP Neural Network

BP_create(
	train_num_variables,
	train_num_classes,
	learning_rate = 0.83,
	num_hidden_layers = 1,
	hidden_layer_size = 5 )

# ...train the BP Neural Network

BP_train_set(
	train_set,
	train_set_data_out,
	training_epochs = 5000 );

# ...and later use the trained BP Neural Network

# BP has been trained. It can be used to recall input-output pairs.
# Let's see how well it recalls the data set it was trained with:

bp_output <- BP_recall_set(train_set)

# show output
cat("\n- Recalling training set returns:\n")
print(bp_output)
cat("- Using this demo's encoding, original class is:\n")
print(train_class_ids)
cat("- Using this demo's encoding, recalled class is:\n")
print(apply(bp_output,1,which.max))
cat("- BP success in recalling correct class is: ",
	sum(apply(bp_output,1,which.max)==train_class_ids),
		" out of ", train_num_cases, "\n")

# Let's see how well it recalls the entire Iris set:

bp_output <- BP_recall_set(iris_s)

# show output
cat("\n- Recalling entire Iris set returns:\n")
print(bp_output)
cat("- Using this demo's encoding, original class is:\n")
print(as.integer(iris$Species))
cat("- Using this demo's encoding, recalled class is:\n")
bp_classification <- apply(bp_output,1,which.max)
print(bp_classification)
cat("- BP success in recalling correct class is: ",
	sum(apply(bp_output,1,which.max)==as.integer(iris$Species)),
		" out of ", nrow(iris_s), "\n")
plot(iris_s, pch=bp_classification, main="Iris classified by a partialy trained BP")

# ...when no longer needed, delete the BP Neural Network

BP_destroy()
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ neural }% use one of  RShowDoc("KEYWORDS")
