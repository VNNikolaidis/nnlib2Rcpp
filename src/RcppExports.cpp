// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// Autoencoder
NumericMatrix Autoencoder(NumericMatrix data_in, int desired_new_dimension, int number_of_training_epochs, double learning_rate, int num_hidden_layers, int hidden_layer_size, bool show_nn);
RcppExport SEXP _nnlib2Rcpp_Autoencoder(SEXP data_inSEXP, SEXP desired_new_dimensionSEXP, SEXP number_of_training_epochsSEXP, SEXP learning_rateSEXP, SEXP num_hidden_layersSEXP, SEXP hidden_layer_sizeSEXP, SEXP show_nnSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericMatrix >::type data_in(data_inSEXP);
    Rcpp::traits::input_parameter< int >::type desired_new_dimension(desired_new_dimensionSEXP);
    Rcpp::traits::input_parameter< int >::type number_of_training_epochs(number_of_training_epochsSEXP);
    Rcpp::traits::input_parameter< double >::type learning_rate(learning_rateSEXP);
    Rcpp::traits::input_parameter< int >::type num_hidden_layers(num_hidden_layersSEXP);
    Rcpp::traits::input_parameter< int >::type hidden_layer_size(hidden_layer_sizeSEXP);
    Rcpp::traits::input_parameter< bool >::type show_nn(show_nnSEXP);
    rcpp_result_gen = Rcpp::wrap(Autoencoder(data_in, desired_new_dimension, number_of_training_epochs, learning_rate, num_hidden_layers, hidden_layer_size, show_nn));
    return rcpp_result_gen;
END_RCPP
}
// BP_destroy
void BP_destroy();
RcppExport SEXP _nnlib2Rcpp_BP_destroy() {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    BP_destroy();
    return R_NilValue;
END_RCPP
}
// BP_create
bool BP_create(int input_data_dim, int output_dim, double learning_rate, int num_hidden_layers, int hidden_layer_size);
RcppExport SEXP _nnlib2Rcpp_BP_create(SEXP input_data_dimSEXP, SEXP output_dimSEXP, SEXP learning_rateSEXP, SEXP num_hidden_layersSEXP, SEXP hidden_layer_sizeSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< int >::type input_data_dim(input_data_dimSEXP);
    Rcpp::traits::input_parameter< int >::type output_dim(output_dimSEXP);
    Rcpp::traits::input_parameter< double >::type learning_rate(learning_rateSEXP);
    Rcpp::traits::input_parameter< int >::type num_hidden_layers(num_hidden_layersSEXP);
    Rcpp::traits::input_parameter< int >::type hidden_layer_size(hidden_layer_sizeSEXP);
    rcpp_result_gen = Rcpp::wrap(BP_create(input_data_dim, output_dim, learning_rate, num_hidden_layers, hidden_layer_size));
    return rcpp_result_gen;
END_RCPP
}
// BP_print
void BP_print();
RcppExport SEXP _nnlib2Rcpp_BP_print() {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    BP_print();
    return R_NilValue;
END_RCPP
}
// BP_train_single
double BP_train_single(NumericVector data_in, NumericVector data_out);
RcppExport SEXP _nnlib2Rcpp_BP_train_single(SEXP data_inSEXP, SEXP data_outSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericVector >::type data_in(data_inSEXP);
    Rcpp::traits::input_parameter< NumericVector >::type data_out(data_outSEXP);
    rcpp_result_gen = Rcpp::wrap(BP_train_single(data_in, data_out));
    return rcpp_result_gen;
END_RCPP
}
// BP_train_set
double BP_train_set(NumericMatrix dataset_in, NumericMatrix dataset_out, int training_epochs);
RcppExport SEXP _nnlib2Rcpp_BP_train_set(SEXP dataset_inSEXP, SEXP dataset_outSEXP, SEXP training_epochsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericMatrix >::type dataset_in(dataset_inSEXP);
    Rcpp::traits::input_parameter< NumericMatrix >::type dataset_out(dataset_outSEXP);
    Rcpp::traits::input_parameter< int >::type training_epochs(training_epochsSEXP);
    rcpp_result_gen = Rcpp::wrap(BP_train_set(dataset_in, dataset_out, training_epochs));
    return rcpp_result_gen;
END_RCPP
}
// BP_recall_set
NumericMatrix BP_recall_set(NumericMatrix data_in);
RcppExport SEXP _nnlib2Rcpp_BP_recall_set(SEXP data_inSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericMatrix >::type data_in(data_inSEXP);
    rcpp_result_gen = Rcpp::wrap(BP_recall_set(data_in));
    return rcpp_result_gen;
END_RCPP
}
// BP_save_to_file
bool BP_save_to_file(std::string filename);
RcppExport SEXP _nnlib2Rcpp_BP_save_to_file(SEXP filenameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type filename(filenameSEXP);
    rcpp_result_gen = Rcpp::wrap(BP_save_to_file(filename));
    return rcpp_result_gen;
END_RCPP
}
// BP_load_from_file
bool BP_load_from_file(std::string filename);
RcppExport SEXP _nnlib2Rcpp_BP_load_from_file(SEXP filenameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type filename(filenameSEXP);
    rcpp_result_gen = Rcpp::wrap(BP_load_from_file(filename));
    return rcpp_result_gen;
END_RCPP
}
// LVQ
IntegerVector LVQ(NumericMatrix data, IntegerVector desired_cluster_ids, int number_of_training_epochs, NumericMatrix test_data, bool show_nn);
RcppExport SEXP _nnlib2Rcpp_LVQ(SEXP dataSEXP, SEXP desired_cluster_idsSEXP, SEXP number_of_training_epochsSEXP, SEXP test_dataSEXP, SEXP show_nnSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericMatrix >::type data(dataSEXP);
    Rcpp::traits::input_parameter< IntegerVector >::type desired_cluster_ids(desired_cluster_idsSEXP);
    Rcpp::traits::input_parameter< int >::type number_of_training_epochs(number_of_training_epochsSEXP);
    Rcpp::traits::input_parameter< NumericMatrix >::type test_data(test_dataSEXP);
    Rcpp::traits::input_parameter< bool >::type show_nn(show_nnSEXP);
    rcpp_result_gen = Rcpp::wrap(LVQ(data, desired_cluster_ids, number_of_training_epochs, test_data, show_nn));
    return rcpp_result_gen;
END_RCPP
}
// MAM
NumericMatrix MAM(NumericMatrix train_data_in, NumericMatrix train_data_out, NumericMatrix test_data_in, bool show_nn);
RcppExport SEXP _nnlib2Rcpp_MAM(SEXP train_data_inSEXP, SEXP train_data_outSEXP, SEXP test_data_inSEXP, SEXP show_nnSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericMatrix >::type train_data_in(train_data_inSEXP);
    Rcpp::traits::input_parameter< NumericMatrix >::type train_data_out(train_data_outSEXP);
    Rcpp::traits::input_parameter< NumericMatrix >::type test_data_in(test_data_inSEXP);
    Rcpp::traits::input_parameter< bool >::type show_nn(show_nnSEXP);
    rcpp_result_gen = Rcpp::wrap(MAM(train_data_in, train_data_out, test_data_in, show_nn));
    return rcpp_result_gen;
END_RCPP
}
// SOM
IntegerVector SOM(NumericMatrix data, int max_number_of_desired_clusters, int number_of_training_epochs, int neighborhood_size, bool show_nn);
RcppExport SEXP _nnlib2Rcpp_SOM(SEXP dataSEXP, SEXP max_number_of_desired_clustersSEXP, SEXP number_of_training_epochsSEXP, SEXP neighborhood_sizeSEXP, SEXP show_nnSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericMatrix >::type data(dataSEXP);
    Rcpp::traits::input_parameter< int >::type max_number_of_desired_clusters(max_number_of_desired_clustersSEXP);
    Rcpp::traits::input_parameter< int >::type number_of_training_epochs(number_of_training_epochsSEXP);
    Rcpp::traits::input_parameter< int >::type neighborhood_size(neighborhood_sizeSEXP);
    Rcpp::traits::input_parameter< bool >::type show_nn(show_nnSEXP);
    rcpp_result_gen = Rcpp::wrap(SOM(data, max_number_of_desired_clusters, number_of_training_epochs, neighborhood_size, show_nn));
    return rcpp_result_gen;
END_RCPP
}

RcppExport SEXP _rcpp_module_boot_class_BP_NN();
RcppExport SEXP _rcpp_module_boot_class_LVQ_NN();

static const R_CallMethodDef CallEntries[] = {
    {"_nnlib2Rcpp_Autoencoder", (DL_FUNC) &_nnlib2Rcpp_Autoencoder, 7},
    {"_nnlib2Rcpp_BP_destroy", (DL_FUNC) &_nnlib2Rcpp_BP_destroy, 0},
    {"_nnlib2Rcpp_BP_create", (DL_FUNC) &_nnlib2Rcpp_BP_create, 5},
    {"_nnlib2Rcpp_BP_print", (DL_FUNC) &_nnlib2Rcpp_BP_print, 0},
    {"_nnlib2Rcpp_BP_train_single", (DL_FUNC) &_nnlib2Rcpp_BP_train_single, 2},
    {"_nnlib2Rcpp_BP_train_set", (DL_FUNC) &_nnlib2Rcpp_BP_train_set, 3},
    {"_nnlib2Rcpp_BP_recall_set", (DL_FUNC) &_nnlib2Rcpp_BP_recall_set, 1},
    {"_nnlib2Rcpp_BP_save_to_file", (DL_FUNC) &_nnlib2Rcpp_BP_save_to_file, 1},
    {"_nnlib2Rcpp_BP_load_from_file", (DL_FUNC) &_nnlib2Rcpp_BP_load_from_file, 1},
    {"_nnlib2Rcpp_LVQ", (DL_FUNC) &_nnlib2Rcpp_LVQ, 5},
    {"_nnlib2Rcpp_MAM", (DL_FUNC) &_nnlib2Rcpp_MAM, 4},
    {"_nnlib2Rcpp_SOM", (DL_FUNC) &_nnlib2Rcpp_SOM, 5},
    {"_rcpp_module_boot_class_BP_NN", (DL_FUNC) &_rcpp_module_boot_class_BP_NN, 0},
    {"_rcpp_module_boot_class_LVQ_NN", (DL_FUNC) &_rcpp_module_boot_class_LVQ_NN, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_nnlib2Rcpp(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}