\name{BP_train_set}
\alias{BP_train_set}
\title{
Train the active BP
}
\description{
Presents an entire dataset (and corresponding outputs) to the active BO for training.
}
\usage{
BP_train_set(dataset_in, dataset_out, training_epochs)
}
\arguments{
  \item{dataset_in}{
numeric matrix, containing input vectors as rows (for training to succeed, they should have length equal to the size of the input layer in the current active BP).
}
  \item{dataset_out}{
numeric matrix, containing corresponding (desired) output vectors as rows (for training to succeed, they should have length equal to the size of the output layer in the current active BP). It is recommended that these values fall in range 0 to 1.
}
  \item{training_epochs}{
number of training epochs, aka presentations of all training data to ANN during training.
}
}
\details{
This is the main function for training the active BP, as it accepts entire input-output datasets. Since the PEs in computing layers of this BP implementation apply the logistic sigmoid threshold function, their output is in [0 1] range (and so should the desired output vector values). This function is one of several used to handle BP supervised ANNs in this package. A BP NN object must be created by invoking the BP_create (or BP_load_from_file) function before it can be used . Any further calls of BP_create would replace the current BP NN by a new one. The BP NN object should also be deleted when no longer needed by invoking the BP_destroy function.
}
\value{
Returns a real number, indicator of training error.
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
