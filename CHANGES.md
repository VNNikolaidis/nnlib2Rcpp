Changes to nnlib2Rcpp version 0.1.1 (from 0.1.0)
- removed redundant unused files.
- minor documentation fixes (Rcpp missing entries added), passes devtools::check() with no related warnings. 
- since LVQ has two variations (supervised and unsupervised), it was mentioned in comments that the names may be misleading or confusing. So the LVQ related R functions and modules are renamed as follows:
  ‘LVQs’ for supervised LVQ function (was ‘LVQ’ in v.0.1.0)
  ‘LVQs_NN’ for supervised LVQ module (was ‘LVQ_NN’ in v.0.1.0)
  ‘LVQu’ for unsupervised LVQ function (was ‘SOM’ in v.0.1.0)
- minor documentation improvements.
- pdf documentation removed, but can be built by running the following R command: devtools::build_manual()

---

Changes to nnlib2Rcpp version 0.1.2 (from 0.1.1)
-	(checked with R 3.6.3 and RTools35.)
-	(checked with R 4.0.0 and RTools40 (produces 1 extra NOTE).
-	removed redundant BP functions. All BP functionality is now implemented in BP module class (safer and more convenient for supervised methods).
-	removed redundant Supervised-LVQ functions. All Supervised-LVQ functionality is now implemented in LVQs module class (safer and more convenient for supervised methods). Unsupervised-LVQ is still implemented as a function (LVQu).
-	removed MAM functions. All MAM functionality is now implemented in MAM module class (safer and more convenient for supervised methods).
-	Minor changes in nnlib2 (now uses nnlib2 v.0.1.5)
-	Fixed LICENCE file (was incompatible with CRAN)

---

Changes to nnlib2Rcpp version 0.1.3 (from 0.1.2)
-	includes nnlib2 v.0.1.7.

---

Changes to nnlib2Rcpp version 0.1.4 (from 0.1.3)
-	(checked on Linux Mint 19.3 with R 3.6.3)
-	(checked with R 4.0.0 and RTools40 (produces 1 extra NOTE).
-	includes nnlib2 v.0.2.0.
-	added module "NN" which allows creation and control of custom NNs from R using predefined components. It also provides fixed method for adding user-defined NN components which can be used in the module.

---

Changes to nnlib2Rcpp version 0.1.5 (from 0.1.4)
-	added some methods to module "NN" (get_weight_at, set_weight_at)
-	added some methods to module "NN" (set_misc_values_at)

---

Changes to nnlib2Rcpp version 0.1.6 (from 0.1.5)
-	added method to module "NN" (recall_dataset) to decode/map/retrieve output for entire input dataset.
-	added method to module "NN" (encode_dataset_unsupervised) for faster unsupervised training.
-	added method to module "NN" (encode_datasets_supervised) for faster supervised training.
-	includes nnlib2 v.0.2.4
-	added method to module "NN" (get_output_at).
-	added method to module "NN" (set_output_at).
-	added package vingette
-	other minor changes

---

Changes to nnlib2Rcpp version 0.1.7 (from 0.1.6)
-	Minor source changes (only affect source).
