\name{MAM}
\alias{MAM}
\title{
Matrix Associative Memory
}
\description{
A single Matrix Associative Memory implemented as a (supervised) Artificial Neural Network.
}
\usage{
MAM(train_data_in, train_data_out, test_data_in, show_nn)
}
\arguments{
  \item{train_data_in}{
data to be encoded in MAM, a numeric matrix (2d, of n rows). Each row will be paired to the corresponding train_data_out row, forming an input-output vector pair.
}
  \item{train_data_out}{
data to be encoded in MAM, a numeric matrix (2d, also of n rows). Each row will be paired to the corresponding train_data_in row, forming an input-output vector pair.
}
  \item{test_data_in}{
data to be recalled from MAM, a numeric matrix (2d, same number of columns as train_data_in).
}
  \item{show_nn}{
boolean, option to print an outline of the (trained) ANN internal structure.
}
}
\value{
Returns recalled data from MAM, a numeric matrix (2d, same number of rows as test_data_in, same number of columns as train_data_out).
}
\author{
Vasilis N. Nikolaidis <vnnikolaidis@gmail.com>
}
\note{
The NN used in this function uses supervised training to store input-output vector pairs.

(This function uses Rcpp to employ mam_nn class in nnlib2 C++ Artificial Neural Network library.)
}
\examples{
iris_s            <- as.matrix(scale(iris[1:4]))
class_ids         <- as.integer(iris$Species)
num_classes       <- max(class_ids)

# create output dataset to be used for training, Here we encode class as -1s and 1s
iris_data_out <- matrix( data = -1, nrow = nrow(iris_s), ncol = num_classes)

# now for each case, assign a 1 to the column corresponding to its class
for(r in 1:nrow(iris_data_out)) iris_data_out[r,class_ids[r]]=1

# Finally apply MAM:
# Encode train pairs in MAM and then get output dataset by recalling the test data.

test_data_out <- MAM (iris_s,iris_data_out,iris_s,TRUE)

# find which MAM output has the largest value and use this as the final cluster tag.
mam_recalled_cluster_ids = apply(test_data_out,1,which.max)

plot(iris_s, pch=mam_recalled_cluster_ids, main="MAM recalled Iris data classes")

cat("MAM recalled these IDs:\n")
print(mam_recalled_cluster_ids)
}
\keyword{ neural }% use one of  RShowDoc("KEYWORDS")
