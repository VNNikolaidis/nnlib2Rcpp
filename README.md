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

This R package uses nnlib2 C++ library for NNs.

---

Changes to nnlib2Rcpp version 0.1.1 (from 0.1.0)
- removed redundant unused files.
- minor documentation fixes (Rcpp missing entries added), passes devtools::check() with no related warnings. 
- Since LVQ has two variations (supervised and unsupervised), it was mentioned in comments that the names may be misleading or confusing. So the LVQ related R functions and modules are renamed as follows:

  ‘LVQs’ for supervised LVQ function (was ‘LVQ’ in v.0.1.0)
  
  ‘LVQs_NN’ for supervised LVQ module (was ‘LVQ_NN’ in v.0.1.0)
  
  ‘LVQu’ for unsupervised LVQ function (was ‘SOM’ in v.0.1.0)
  
- minor documentation improvements.
- pdf documentation removed, but can be built by running the following R command: devtools::build_manual()
