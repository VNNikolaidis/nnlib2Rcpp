# nnlib2Rcpp
An R package with Neural Nets created using nnlib2. 

---

This collection of artificial neural networks (NNs or ANNs or ANSs).
Currently includes versions of BP, Autoencoder, MAM, LVQ (supervised and unsupervised). 
All NNs were created using nnlib2 (a C++ library for implementing NNs) and interfaced with R via RCpp.

---

To install in your R installation, use the following R commands:

library(devtools) 

install_github("VNNikolaidis/nnlib2Rcpp")

---

For information on citing this package use the following R command for information:

citation("nnlib2Rcpp")

---

The nnlib2 used in this package is a collection of C++ classes and templates for creating NNs. For an exemple of NN implementation see the MAM related code. We would like to add more classic NNs implementations in nnlib2 (and thus nnlib2Rcpp). Please let us know if interested to contribute.  
