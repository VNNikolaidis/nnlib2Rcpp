# nnlib2Rcpp
An R package with Neural Nets created using nnlib2. 

---

This collection of artificial neural networks currently includes versions of BP, Autoencoder, MAM, LVQ, SOM (simplified version, 1-d unsupervised LVQ). All NNs were created using nnlib2 C++ NN library and interfaced to R via RCpp.

---

To install in your R installation, use the following R commands:

library(devtools) 

install_github("VNNikolaidis/nnlib2Rcpp")

To cite this software (once installed) use the following R command for information:

citation("nnlib2Rcpp")


---

This R package uses nnlib2 C++ library (latest version is at https://github.com/VNNikolaidis/nnlib2) [1][2].

----
References:

[1] Nikolaidis VN, Makris IA, Stavroyiannis S (2013). “ANS-based Preprocessing of Company Performance Indicators.” Global Business and Economics Review, 15(1), 49–58.

[2] Nikolaidis V (1999). Non-Destructive Inspection (NDI) Techniques for Composite Materials using Unconventional Pattern Regognition Methods (in Greek). Ph.D. thesis, University of Patras. URL https://thesis.ekt.gr/thesisBookReader/id/11158.
