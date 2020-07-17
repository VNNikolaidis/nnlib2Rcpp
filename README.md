# nnlib2Rcpp
An R package providing access to Neural Nets created using nnlib2. 

---

A collection of artificial neural networks (NNs or ANNs or ANSs).

Currently includes versions of BP, Autoencoder, MAM, LVQ (supervised and unsupervised). 
All NNs are created using nnlib2 (a C++ library for implementing NNs) and interfaced with R via RCpp.

Since v.0.1.4, the package also provides the NN R module (Class "NN") which allows creation and control of custom NNs configurations from R using predefined components; it also allows the addition of new user-defined NN components (layers, nodes, connection and sets of connections) using a predifined procedure (requires some familiarity with C++).

---

To add the package to your R installation, use the following R commands:

library(devtools) 

install_github("VNNikolaidis/nnlib2Rcpp")

---

For information on citing this package use the following R command for information:

citation("nnlib2Rcpp")

---

The nnlib2 used in this package is a collection of C++ classes and templates for creating NNs. For an example of NN implementation see the MAM related code. 

---

For creating new NN components and models see also: 
https://r-posts.com/creating-custom-neural-networks-with-nnlib2rcpp/   ( https://wp.me/p8rgs6-sh )

---

Future goals:

- to add more classic neural network implementations in nnlib2 (and thus nnlib2Rcpp).
- to add "NN"-compatible neural network components. 
- to parallelize components.

Let me know if interested to contribute, or want to add your neural network components to the package.
