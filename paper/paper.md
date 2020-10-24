---
title: 'The nnlib2 library and nnlib2Rcpp R package for implementing neural networks'
tags:
  - neural networks
  - R
  - Cpp
authors:
 - name: Vasilis N Nikolaidis
   orcid: 0000-0003-1471-8788
   affiliation: 1
affiliations:
 - name: University of Peloponnese
   index: 1
date: 15 September 2020
bibliography: paper.bib
---

# Summary

Artificial neural networks are computing models used in various data-driven applications. Such systems typically consist of a large number of processing elements, usually organized in layers, which transfer data to other nodes via weighted connections. An ever-increasing number of different neural network models has been proposed and used. Among the several factors differentiating each model are the network topology, the processing functions in nodes and connections, the training method used to modify internal state of the network components, the processing sequences utilized for presenting and transferring data to, within and from the model during the training and mapping stages etc. The software presented here is a C++ library of classes and templates for implementing neural network components and models and an R package that allows users to instantiate and use such components directly from the R programming language.

# Statement of need

The `nnlib2` library is a collection of C++ base classes and templates for building neural network (NN) components and entire models. While a significant number of very capable, flexible, high performance tools for NN are available today, including frameworks s.a. Keras [@keras1], PyTorch [@PyTorch1] etc, the `nnlib2` library may appeal to NN students and experimenters who seek a basis for implementing various such models in C++ and take advantage of the direct control over the implemented model that this approach allows. Furthermore, the library lets new NN definitions be written in code that is comparatively simple and self-explanatory; most of the provided base classes and templates correspond to typical NN components (processing nodes, layers of nodes, connections, sets of connections between layers, entire neural networks etc) and closely follow the conceptual model often found in related bibliography such as [@SimpsonANS], [@McCord91], [@pao1989adaptive], [@TheoPR] and elsewhere. Typical component functionality and features are provided in these base definitions, while diverse, model-specific components are to be implemented in custom sub-classes. Finally, the library presents a high degree of target and compiler independence, with early versions used in various solutions targeting different operating systems, mainly for research purposes, for example in [@philippidis1999unsupervised], [@Nikolaidis199Phd] and [@nikolaidis2013ans]. To take advantage of the R ecosystem, improve its usability and widen its audience, the `nnlib2` library has also been integrated into an R package (a process supported by `Rcpp` [@Rcpp1]). The package, named `nnlib2Rcpp` (available on CRAN [@nnlib2RcppCRAN] and GitHub [@nnlib2RcppGitHub]) also includes additional R-specific classes and methods that allow its users to instantiate the NN components they define using `nnlib2`, combine them in custom NN configurations and models and generally control, monitor and employ them from R. Finally, the package provides a small collection of predefined components and ready-to-use models (for versions of Back Propagation, autoencoder, supervised/unsupervised Learning Vector Quantization and Matrix Associative Memory neural networks).

# References
