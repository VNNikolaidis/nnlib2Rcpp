# nnlib2Rcpp
An R package providing access to Neural Networks created using nnlib2. 

---

A collection of Artificial Neural Networks (NNs or ANNs or ANSs) created using the 'nnlib2' C++ library.

Currently includes predefined versions of BP, Autoencoder, MAM, LVQ (supervised and unsupervised). 
All NNs are created using 'nnlib2' (a C++ library of classes for implementing NNs) and interfaced with R via RCpp.

The package also provides the NN R module (Class "NN") which allows creation and control of custom NNs configurations from R using NN components (predefined or user-defined) created using 'nnlib2'. To add new user-defined NN components (layers, nodes, connections, sets of connections etc) see the "NN" component documentation (type ?NN in R). Note: this process requires some familiarity with C++.

---

To install:

(a) From CRAN Repository: The CRAN (stable) version of this package can be installed the usual way, i.e. by invoking the following R command:

    install.packages("nnlib2Rcpp") 

(b) From GitHub: To add the GitHub (latest) version of this package to your R installation, use the following R commands:

    library(devtools) 
    install_github("VNNikolaidis/nnlib2Rcpp")

Once installed, for package help (including documentation and examples for each function or class provided by nnlib2Rcpp) use the following R command:

    help(package='nnlib2Rcpp')

More instructions on using 'nnlib2' and 'nnlib2Rcpp' can be found in file "manual.pdf" of the repository, i.e. here:

https://github.com/VNNikolaidis/nnlib2Rcpp/blob/master/manual.pdf

A reference manual of the package commands (PDF format, for the package version on CRAN), can be found here:

https://cran.r-project.org/web/packages/nnlib2Rcpp/nnlib2Rcpp.pdf

A reference manual in PDF format (for the last version in CRAN) can be found here:

https://cran.r-project.org/web/packages/nnlib2Rcpp/nnlib2Rcpp.pdf

For information on citing this package use the following R command for information:

    citation("nnlib2Rcpp")

---

For copyright information see LICENSE.md file or DESCRIPTION+LICENSE files (as imposed by package format for CRAN submissions).

---

The ‘nnlib2’ library used (and included) in this package is a collection of C++ classes and templates for creating NNs. This library is also available as a standalone project, in GitHub repository (https://github.com/VNNikolaidis/nnlib2). For a (simplified) class-diagram of significant nnlib2 classes and templates see: https://github.com/VNNikolaidis/nnlib2/blob/master/misc/diagram%20of%20main%20classes.png

For implementing new NN components and models in nnlib2 that can be used in nnlib2Rcpp, see also: 

https://r-posts.com/creating-custom-neural-networks-with-nnlib2rcpp/

( https://wp.me/p8rgs6-sh )

The included Matrix Associative Memory (MAM) related code is also a good example due to the model's simplicity.

---

My future goals for this project (iF AND WHEN time permits):

- to add more classic neural network component and model implementations in nnlib2 (and thus nnlib2Rcpp).
- to parallelize component base classes.

Let me know if interested to contribute, or want to add your neural network components to the package. Or, as stated below:

---

We invite anyone to provide feedback, suggestions, report issues or problems, and/or contribute to this software. Possible improvements and contributions include (but are not limited to) implementation of additional neural network models using nnlib2 classes and templates (and thus new neural network components compatible with "NN" module in nnlib2Rcpp), parallelism (possibly via OpenMP), replacement of custom data structures with STL containers, performance enhancements etc.

Please use the issues option in GitHub or email (vnnikolaidis AT gmail.com) if interested to contribute.
