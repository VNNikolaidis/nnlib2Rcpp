\name{Autoencoder}
\alias{Autoencoder}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
Autoencoder NN
}
\description{
A neural network for autoencoding data, projects data to a new set of variables.
}
\usage{
Autoencoder(
  data_in,
  desired_new_dimension,
  number_of_training_epochs,
  learning_rate,
  num_hidden_layers = 1L,
  hidden_layer_size = 5L,
  show_nn = FALSE,
  error_type = "MAE",
  acceptable_error_level = 0,
  display_rate = 1000)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{data_in}{
data to be autoencoded, a numeric matrix, (2d, cases in rows, variables in columns).  It is recommended to be in [0 1] range.
}
  \item{desired_new_dimension}{
number of new variables to be produced. This is effectively the size (length) of the special hidden layer that outputs the new variable values, thus the dimension of the output vector space.
}
  \item{number_of_training_epochs}{
number of training epochs, aka presentations of all training data to ANN during training.
}
  \item{learning_rate}{
the learning rate parameter of the Back-Propagation (BP) NN.
}
  \item{num_hidden_layers}{
number of hidden layers on each side of the special layer.
}
  \item{hidden_layer_size}{number of nodes (processing elements or PEs) in each of the hidden layers. In this implementation of Autoencoder all hidden layers are of the same length (defined here), except for the special hidden layer (whose size is defined by \code{desired_new_dimension} above).}

  \item{show_nn}{boolean, option to display the (trained) ANN internal structure.}

  \item{error_type}{string, error to display and possibly use to stop training (must be 'MSE' or 'MAE').}

  \item{acceptable_error_level}{stops training when error is below this level.}

  \item{display_rate}{number of epochs that pass before current error level is displayed (0 = never display current error).}
}

\value{
Returns a numeric matrix containing the projected data.
}
\references{
Nikolaidis V.N., Makris I.A, Stavroyiannis S, "ANS-based preprocessing of company performance indicators." Global Business and Economics Review 15.1 (2013): 49-58.
}
\author{
Vasilis N. Nikolaidis <vnnikolaidis@gmail.com>
}
\note{
This Autoencoder NN employs a \code{\link{BP}}-type NN to perform a data pre-processing step baring similarities to PCA since it too can be used for dimensionality reduction (Kramer 1991)(DeMers and Cottrell 1993)(Hinton and Salakhutdinov 2006). Unlike PCA, an autoencoding NN can also expand the feature-space dimensions (as feature expansion methods do). The NN maps input vectors to themselves via a special hidden layer (the coding layer, usually of different size than the input vector length) from which the new data vectors are produced. Note:
The internal BP PEs in computing layers apply the logistic sigmoid threshold function, and their output is in [0 1] range. It is recommended to use this range in your data as well. More for this particular autoencoder implementation can be found in (Nikolaidis, Makris, and Stavroyiannis 2013). The method is not deterministic and the mappings may be non-linear, depending on the NN topology.

(This function uses Rcpp to employ 'bpu_autoencoder_nn' class in nnlib2.)
}

%% ~Make other sections like Warning with \section{Warning }{....} ~
\seealso{\code{\link{BP}}.}
\examples{
iris_s <- as.matrix(scale(iris[1:4]))
output_dim <- 2
epochs <- 100
learning_rate <- 0.73
num_hidden_layers <-2
hidden_layer_size <- 5

out_data <-  Autoencoder( iris_s, output_dim,
                          epochs, learning_rate,
                          num_hidden_layers, hidden_layer_size, FALSE)

plot( out_data,pch=21,
      bg=c("red","green3","blue")[unclass(iris$Species)],
      main="Randomly autoencoded Iris data")
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ neural }% use one of  RShowDoc("KEYWORDS")
