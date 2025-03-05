# nnlib2Rcpp
An R package providing access to Neural Networks created using nnlib2. 

---

A tool for creating custom neural networks in C++ and using them in R. The Artificial Neural Networks (ANN), also called Artificial Neural Systems (ANS) or simply Neural Networks (NN) are created using the 'nnlib2' C++ library.

Currently also includes predefined versions of BP, Autoencoder, MAM, LVQ (supervised and unsupervised).

All NNs are created using 'nnlib2' (a C++ library of classes for implementing NNs) and interfaced with R via RCpp.

The package also provides the NN R module (Class "NN") which allows creation and control of custom NNs configurations and topologies from R. These NNs consist of components (predefined or user-defined) usually defined via 'nnlib2'. To add new user-defined NN components (layers, nodes, connections, sets of connections etc) to the package see the "NN" component documentation (type ?NN in R). Note: defining components via 'nnlib2' requires some familiarity with C++.

---

To install:

(a) From CRAN Repository (recommended): The CRAN (stable) version of this package can be installed the usual way, i.e. by invoking the following R command:

    install.packages("nnlib2Rcpp") 

(b) From GitHub: To add the GitHub (latest) version of this package to your R installation, use the following R commands:

    library(devtools) 
    install_github("VNNikolaidis/nnlib2Rcpp")

(c) From r-universe: To add the package (corresponding to the latest GitHub release version) to your R installation, use the following R command:

    install.packages('nnlib2Rcpp', repos = 'https://vnnikolaidis.r-universe.dev')

Once installed, for package help (including documentation and examples for each function or class provided by nnlib2Rcpp) use the following R command:

    help(package='nnlib2Rcpp')

while the package vignette (containing information on adding custom components) can be viewed using the following R command:

    vignette("intro", package='nnlib2Rcpp')

The package vignette is also available in PDF format here:

https://github.com/VNNikolaidis/nnlib2Rcpp/blob/master/support/manual.pdf

A reference manual in PDF format (for the last version in CRAN) can be found here:

https://cran.r-project.org/web/packages/nnlib2Rcpp/nnlib2Rcpp.pdf

---

For information on citing this package use the following R command:

    citation("nnlib2Rcpp")

---

For copyright information see LICENSE.md file or DESCRIPTION+LICENSE files (as imposed by package format for CRAN submissions).

---

The ‘nnlib2’ library used (and included) in this package is a collection of C++ base classes and templates for creating NNs. This library is also available as a standalone project, in GitHub repository (https://github.com/VNNikolaidis/nnlib2). For a (simplified) class-diagram of significant nnlib2 classes and templates see: https://github.com/VNNikolaidis/nnlib2/blob/master/misc/diagram%20of%20main%20classes.png

For implementing new NN components and models in nnlib2 that can be used in nnlib2Rcpp, see also: 

https://r-posts.com/creating-custom-neural-networks-with-nnlib2rcpp/ ( permalink: https://wp.me/p8rgs6-sh )

Instructions on using 'nnlib2' and 'nnlib2Rcpp' can also be found in the package vingette, also available in PDF format here:

https://github.com/VNNikolaidis/nnlib2Rcpp/blob/master/support/manual.pdf

Link to related paper in the Journal of Open Source Software:

[![DOI](https://joss.theoj.org/papers/10.21105/joss.02876/status.svg)](https://doi.org/10.21105/joss.02876)

---

Future goals for this project (iF AND WHEN time permits) include:

- implement additional neural network components and models using 'nnlib2' classes and templates (and thus new neural network components compatible with "NN" module in nnlib2Rcpp).
- create a library of 'classic' neural network components and models.
- add parallelism to ‘nnlib2’ component base classes (possibly via OpenMP, CUDA etc.).
- replace custom data structures in 'nnlib2' with STL containers.
- add support for package 'nn2poly' (https://arxiv.org/pdf/2406.01588)
to the package.
- other performance enhancements.

Let us know if interested to contribute.

---

We invite anyone to contribute to this software and/or provide feedback, suggestions, report issues or problems.

To contact us, please use the issues option in GitHub or email (vnnikolaidis AT gmail.com).

---

[![](https://cranlogs.r-pkg.org/badges/nnlib2Rcpp)](https://cran.r-project.org/package=nnlib2Rcpp)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4780957.svg)](https://doi.org/10.5281/zenodo.4780957)
