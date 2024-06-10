\name{LVQs_recall}
\alias{LVQs_recall}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
Classify Data Using LVQs Code-Book Vectors
}
\description{
This function simplifies applying a supervised LVQ to data (as compared to using the \code{\link{LVQs}} module directly). It uses the codebook vector information returned by \code{\link{LVQs_train}} to assign data to classes.
}
\usage{
LVQs_recall(codebook_info,
			data,
			k = 1,
			recall_rewards_limit = 1,
			verbose = FALSE,
			...)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{codebook_info}{
LVQ codebook vector information (as returned by \code{\link{LVQs_train}}).
}
  \item{data}{
data to be classified, numeric matrix (2d, cases in rows, variables in columns).
}
  \item{k}{
number of neighbours (codebook vectors) considered. See \code{help("knn",package = class)}.
}

  \item{recall_rewards_limit}{
Do not use codebook vectors that were rewarded less that this limit during training.
}
  \item{verbose}{
Show extra information and plots.
}
  \item{\dots}{
Additional parameters for k-Nearest Neighbour Classification function (\code{class::knn}), see \code{help("knn",package = class)}.
}
}
\details{
This is a k-Nearest Neighbor Classifier (employs \code{class::knn}) customized for LVQs codebook vectors.
}
\value{
Factor of classifications ids for \code{data} (as returned by function \code{class::knn}, see \code{help("knn",package = class)}).
}
\references{
Simpson, P. K. (1991). Artificial neural systems: Foundations, paradigms, applications, and implementations. New York: Pergamon Press. p.88.

Venables, W. N. & Ripley, B. D. (2002) Modern Applied Statistics with S.
  Fourth Edition. Springer, New York. ISBN 0-387-95457-0
}
\author{
Vasilis N. Nikolaidis <vnnikolaidis@gmail.com>
}
\seealso{
\code{\link{LVQs_train}}, \code{\link{LVQs}}.
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

