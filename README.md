# nnlib2Rcpp
An R package providing access to Neural Nets created using nnlib2. 

---

A collection of artificial neural networks (NNs or ANNs or ANSs).

Currently includes versions of BP, Autoencoder, MAM, LVQ (supervised and unsupervised). 
All NNs are created using nnlib2 (a C++ library for implementing NNs) and interfaced with R via RCpp.

Since v.1.4, the package also provides the NN R module (Class "NN") which allows creation and control of custom NNs configurations from R using predefined components; it also allows the addition of new user-defined NN components (layers, nodes, connection and sets of connections) using a predifined procedure (requires some familiarity with C++).

---

To add the package to your R installation, use the following R commands:

library(devtools) 

install_github("VNNikolaidis/nnlib2Rcpp")

---

For information on citing this package use the following R command for information:

citation("nnlib2Rcpp")

---

The nnlib2 used in this package is a collection of C++ classes and templates for creating NNs. For an example of NN implementation see the MAM related code. 

We would like to add more classic NNs implementations in nnlib2 (and thus nnlib2Rcpp), or "NN" module-compatible NN components. Please let us know if interested to contribute.  
