# nnlib2Rcpp
An R package with Neural Nets created using nnlib2. 

---

This collection of artificial neural networks (NNs or ANNs or ANSs).
Currently includes versions of BP, Autoencoder, MAM, LVQ (supervised and unsupervised). 
All NNs were created using nnlib2 C++ library for implementing NNs and interfaced with R via RCpp.

---

To install in your R installation, use the following R commands:

library(devtools) 

install_github("VNNikolaidis/nnlib2Rcpp")

---

To cite this software (once installed) use the following R command for information:

citation("nnlib2Rcpp")

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

---

Changes to nnlib2Rcpp version 0.1.2 (from 0.1.1)
-	Checked with R 3.6.3 and RTools35.
-	Checked with R 4.0.0 and RTools40 (produces 1 extra NOTE).
-	Removed redundant BP functions. All BP functionality is now implemented in BP module class (safer and more convenient for supervised methods).
-	Removed MAM functions. All MAM functionality is now implemented in BP module class (safer and more convenient for supervised methods).
-	Minor changes in nnlib2 (now uses nnlib2 v.0.1.5)
-	Fixed LICENCE file (was incompatible with CRAN)


