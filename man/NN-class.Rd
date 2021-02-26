\name{NN-class}
\Rdversion{1.1}
\docType{class}
\alias{NN-class}
\alias{Rcpp_NN}
\alias{Rcpp_NN-class}
\alias{NN}
\alias{nn-class}
\alias{C++Object-class}
\alias{RcppClass-class}

\title{Class \code{"NN"}}
\description{
NN module, for defining and manipulating custom neural networks.
}
\section{Extends}{
Class \code{"\linkS4class{RcppClass}"}, directly.

All reference classes extend and inherit methods from \code{"\linkS4class{envRefClass}"}.

}
\author{
Vasilis N. Nikolaidis <vnnikolaidis@gmail.com>
}
\note{
This R module maintains a generic neural network that can be manipulated using the provided methods. In addition to predefined, new neural network components can be defined and then employed by the \code{"NN"} module. Currently, definition of new components must be done in C++, requires the package source code (which includes the \pkg{nnlib2} C++ library of neural network base classes) and the ability to compile it.  In particular:
    \itemize{
  \item Any new component type or class definition can be added to a single header file called "\code{additional_parts.h}" (which is included in the package source). All new components to be employed by the \code{NN} module must be defined in this file (or be accessible from functions in this file).
  \item New \code{layer}, \code{connection_set}, \code{pe} or \code{connection} definitions must comply (at least loosely) to the \pkg{nnlib2} base class hierarchy and structure and follow the related guidelines. Note: some minimal examples of class and type definitions can be found in the "\code{additional_parts.h}" file itself.
  \item A textual name must be assigned to any new \code{layer} or \code{connection_set}, to be used as parameter in \code{NN} module methods that require a name to create a component. This can be as simple as a single line of code where given the textual name the corresponding component object is created and returned. This code must be added (as appropriate) to either \code{generate_custom_layer()} or \code{generate_custom_connection_set()} functions found in the same "\code{additional_parts.h}" header file. Note: example entries can be found in these functions at the "\code{additional_parts.h}" file.
}
More information on expanding the library with new types of NN components (nodes, layers, connections etc) and models, can be found in the package's  vignette as well as the related \href{https://github.com/VNNikolaidis/nnlib2Rcpp}{repository on Github}). Please consider submitting any useful components you create, to enrich  future versions of the package.
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{\code{\link{BP}}, \code{\link{LVQs}}, \code{\link{MAM}}.}

\examples{
# Example 1:

# (1.A) create new 'NN' object:

n <- new("NN")

# (1.B) Add topology components:

# 1. add a layer of 4 generic nodes:
n$add_layer("generic",4)
# 2. add a set for connections that pass data unmodified:
n$add_connection_set("pass-through")
# 3. add another layer of 2 generic nodes:
n$add_layer("generic",2)
# 4. add a set for connections that pass data x weight:
n$add_connection_set("wpass-through")
# 5. add a layer of 1 generic node:
n$add_layer("generic",1)
# Create actual full connections in sets, random initial weights in [0,1]:
n$create_connections_in_sets(0,1)
# Optionaly, show an outline of the topology:
n$outline()

# (1.C) use the network.

# input some data, and create output for it:
n$input_at(1,c(10,20,30,40))
n$recall_all(TRUE)
# the final output:
n$get_output_from(5)

# (1.D) optionally, examine the network:

# the input at first layer at position 1:
n$get_input_at(1)
# Data is passed unmodified through connections at position 2,
# and (by default) summed together at each node of layer at position 3.
# Final output from layer in position 3:
n$get_output_from(3)
# Data is then passed multiplied by the random weights through
# connections at position 4. The weights of these connections:
n$get_weights_at(4)
# Data is finally summed together at the node of layer at position 5,
# producing the final output, which (again) is:
n$get_output_from(5)

# Example 2: A simple MAM NN

# (2.A) Preparation:

# Create data pairs

iris_data    <- as.matrix( scale( iris[1:4] ) )
iris_species <- matrix(data=-1, nrow=nrow(iris_data), ncol=3)
for(r in 1:nrow(iris_data))
 iris_species[r ,as.integer( iris$Species )[r]]=1

# Create the NN and its components:

m <- new( "NN" )
m$add_layer( "generic" , 4 )
m$add_layer( "generic" , 3 )
m$fully_connect_layers_at(1, 2, "MAM", 0, 0)

# (2.B) Use the NN to store iris (data,species) pair:

# encode pairs in NN:

m$encode_datasets_supervised(
	iris_data,1,
	iris_species,3,0,
	1,TRUE)

# (2.C) Recall iris species from NN:

recalled_data <- m$recall_dataset(iris_data,1,3,TRUE)

# (2.D) Convert recalled data to ids and plot results:

recalled_ids <- apply(recalled_data, 1, which.max)
plot(iris_data, pch=recalled_ids)
}

\keyword{classes}
\section{Fields}{
  \describe{
    \item{\code{.CppObject}:}{Object of class \code{C++Object} ~~ }
    \item{\code{.CppClassDef}:}{Object of class \code{activeBindingFunction} ~~ }
    \item{\code{.CppGenerator}:}{Object of class \code{activeBindingFunction} ~~ }
  }
}
\section{Methods}{
  \describe{

    \item{\code{add_layer( name, size )}:}{Setup a new \code{layer} component (a layer of processing nodes) and append it to the NN topology. Parameters are:
    \itemize{
    \item{\code{name}}{: string, containing name (that also Specifies type) of new layer. Names of predefined layers currently include \code{'pe'}(same as \code{'generic'}), \code{'pass-through'}, \code{'which-max'}, \code{'MAM'}, \code{'LVQ-input'}, \code{'LVQ-output'}, \code{'BP-hidden'}, \code{'BP-output'}, \code{'perceptron'} (additional names for user-defined components may be used, see note below.)}
    \item{\code{size}}{: integer, layer size i.e. number of \code{pe} (Processing Elements or nodes) to create in the layer.}
    }
    }

    \item{\code{add_connection_set( name )}:}{Create a new empty \code{connection_set} component (a set of connections between two layers). It does not connect any layers nor contain any connections between specific layer nodes. The set is appended to the NN topology. Parameters are:
    \itemize{
    \item{\code{name}}{: string, containing name (that also specifies type) of new empty connection set. Names of predefined connection sets currently include \code{'generic', 'pass-through'}(which does not multiply weights), \code{'wpass-through'}(which does multiply weights), \code{'MAM'}, \code{'LVQ'}, \code{'BP'}, \code{'perceptron'} (additional names for user-defined components may be used, see note below).}
    }
    }

    \item{\code{create_connections_in_sets( min_random_weight, max_random_weight )}:}{Find empty, unconnected \code{connection_set} components that are between two  \code{layer}s in the topology, and set them up to connect the adjacent layers, adding connections to fully connect their nodes  (n x m connections are created, with n and m the number of nodes at each layer respectively). Parameters are:
    \itemize{
    \item{\code{min_random_weight}}{: double, minimum value for random initial connection weights.}
    \item{\code{max_random_weight}}{: double, maximum value for random initial connection weights.}
    }
    }

    \item{\code{connect_layers_at( source_pos, destin_pos, name )}:}{Insert a new empty \code{connection_set} component (a set of connections between two layers) between the layers at specified topology positions, and prepare it to connect them. No actual connections between any layer nodes are created. Parameters are:
    \itemize{
    \item{\code{source_pos}}{: integer, position in topology of source layer.}
    \item{\code{destin_pos}}{: integer, position in topology of destination layer.}
    \item{\code{name}}{: string, containing name (that also specifies type) of new connection set (see above).}
    }
    }

    \item{\code{fully_connect_layers_at( source_pos, destin_pos, name, min_random_weight, max_random_weight )}:}{Same as \code{connect_layers_at} but also fills the new \code{connection_set} with connections between the nodes of the two layers, fully connecting the layers (n x m connections are created, with n and m the number of nodes at each layer respectively). Parameters are:
    \itemize{
    \item{\code{source_pos}}{: integer, position in topology of source layer.}
    \item{\code{destin_pos}}{: integer, position in topology of destination layer.}
    \item{\code{name}}{: string, containing name (that also specifies type) of new connection set (see above).}
    \item{\code{min_random_weight}}{: double, minimum value for random initial connection weights.}
    \item{\code{max_random_weight}}{: double, maximum value for random initial connection weights.}
    }
    }

   \item{\code{add_single_connection( pos, source_pe, destin_pe, weight )}:}{
   Add a connection to a \code{connection_set} that already connects two layers. Parameters are:
    \itemize{
    \item{\code{pos}}{: integer, position in topology of \code{connection_set} to which the new connection will be added.}
    \item{\code{source_pe}}{: integer, \code{pe} in source layer to connect.}
    \item{\code{destin_pe}}{: integer, \code{pe} in destination layer to connect.}
    \item{\code{weight}}{: double, value for initial connection weight.}
    }
    }

   \item{\code{remove_single_connection( pos, con )}:}{
   Remove a connection from a \code{connection_set}. Parameters are:
    \itemize{
    \item{\code{pos}}{: integer, position in topology of \code{connection_set}.}
	\item{\code{con}}{: integer, connection to remove.}
    }
    }

   \item{\code{size()}:}{Returns neural network size, i.e. the number of components its topology.}

   \item{\code{sizes()}:}{Returns sizes of components in topology.}

  \item{\code{component_ids()}:}{Returns an integer vector containing the ids of the components in topology (these ids are created at run-time and identify each NN component).}

  \item{\code{input_at( pos, data_in )}:}{Input a data vector to the component (layer) at specified topology index. Returns TRUE if successful. Parameters are:
    \itemize{
    \item{\code{pos}}{: integer, position in topology of component to receive input.}
    \item{\code{data_in}}{: NumericVector, data to be sent as input to component (sizes must match).}
    }
    }

 \item{\code{encode_at( pos )}:}{Trigger the encoding operation of the component at specified topology index (note: depending on implementation, an 'encode' operation usually collects inputs, processes the data, adjusts internal state variables and/or weights, and possibly produces output). Returns TRUE if successful. Parameters are:
    \itemize{
    \item{\code{pos}}{: integer, position in topology of component to perform encoding.}
    }
    }

 \item{\code{recall_at( pos )}:}{Trigger the recall (mapping, data retrieval) operation of the component at specified topology index (note: depending on implementation, a 'recall' operation usually collects inputs, processes the data, and produces output). Returns TRUE if successful. Parameters are:
    \itemize{
    \item{\code{pos}}{: integer, position in topology of component to perform recall.}
    }
    }

 \item{\code{encode_all( fwd )}:}{Trigger the encoding operation of all the components in the NN topology. Returns TRUE if successful. Parameters are:
    \itemize{
    \item{\code{fwd}}{: logical, set to TRUE to encode forwards (first-to-last component), FALSE to encode backwards (last-to-first component).}
    }
    }

 \item{\code{encode_dataset_unsupervised( data, pos, epochs, fwd )}:}{Encode a dataset using unsupervised training. A faster method to encode a data set. All the components in the NN topology will perform 'encode' in specified direction. Returns TRUE if successful. Parameters are:
    \itemize{
    \item{\code{data}}{: numeric matrix, containing input vectors as rows.}
    \item{\code{pos}}{: integer, position in topology of component to receive input vectors.}
    \item{\code{epochs}}{: integer, number of training epochs (encoding repetitions of the entire dataset).}
    \item{\code{fwd}}{: logical, indicates direction, TRUE to trigger encoding forwards (first-to-last component), FALSE to encode backwards (last-to-first component).}
    }
    }

 \item{\code{encode_datasets_supervised( i_data, i_pos, j_data, j_pos, j_destination_register, epochs, fwd )}:}{Encode multiple (i,j) vector pairs stored in two corresponding data sets, using supervised training. A faster method to encode the data. All the components in the NN topology will perform 'encode' in specified direction. Returns TRUE if successful. Parameters are:
    \itemize{
    \item{\code{i_data}}{: numeric matrix, data set, each row is a vector i of vector-pair (i,j).}
    \item{\code{i_pos}}{: integer, position in topology of component to receive i vectors.}
    \item{\code{j_data}}{: numeric matrix, data set, each row is a corresponding vector j of vector-pair (i,j).}
    \item{\code{j_pos}}{: integer, position in topology of component to receive j vectors.}
    \item{\code{j_destination_selector}}{: integer, selects which internal node (pe) registers will receive vector j, i.e. if 0 internal node register '\code{input}' will be used (j will become the layer's input), if 1 register '\code{output}' will be used (j will become the layer's output), if 2 register '\code{misc}' will be used (implementations may use this as an alternative way to transfer data to nodes without altering current input or output).}
    \item{\code{epochs}}{: integer, number of training epochs (encoding repetitions of the entire data).}
    \item{\code{fwd}}{: logical, indicates direction, TRUE to trigger encoding forwards (first-to-last component), FALSE to encode backwards (last-to-first component).}
    }
    }

 \item{\code{recall_dataset( data_in, input_pos, output_pos, fwd )}:}{Recall (map, retrieve output for) a dataset. A faster method to recall an entire data set. All the components in the NN topology will perform 'recall' in specified direction. Returns numeric matrix containing corresponding output. Parameters are:
    \itemize{
    \item{\code{data_in}}{: numeric matrix, containing input vectors as rows.}
    \item{\code{input_pos}}{: integer, position in topology of component to receive input vectors.}
    \item{\code{output_pos}}{: integer, position in topology of component to produce output.}
    \item{\code{fwd}}{: logical, indicates direction, TRUE to trigger 'recall' (mapping) forwards (first-to-last component), FALSE to recall backwards (last-to-first component).}
    }
    }

 \item{\code{recall_all( fwd )}:}{Trigger the recall (mapping, data retrieval) operation of all the components in the NN topology. Returns TRUE if successful. Parameters are:
    \itemize{
    \item{\code{fwd}}{: logical, set to TRUE to recall forwards (first-to-last component), FALSE to recall backwards (last-to-first component).}
    }
    }

 \item{\code{get_output_from( pos )}:}{Get the current output of the component at specified topology index. If successful, returns NumericVector of output values. Parameters are:
    \itemize{
    \item{\code{pos}}{: integer, position in topology of component to use.}
    }
    }

\item{\code{get_output_at( pos )}:}{Same as \code{get_output_from}, see above.}

 \item{\code{get_input_at( pos )}:}{Get the current input of the component at specified topology index (depends on the implementation: for layers, this may be valid after the \code{pe}s (nodes) have performed their \code{input_function} on incoming values; \code{pe}s have an overridable \code{input_function} that collects all incoming values and produces a single value for further processing which is stored at an internal \code{input} register (whose value is retrieved here); by default \code{input_function} performs summation. If successful, returns NumericVector of final \code{input} values. Parameters are:
    \itemize{
    \item{\code{pos}}{: integer, position in topology of component to use.}
    }
    }

 \item{\code{get_weights_at( pos )}:}{Get the current weights of the component (\code{connection_set}) at specified topology index. If successful, returns NumericVector of connection weights. Parameters are:
    \itemize{
    \item{\code{pos}}{: integer, position in topology of component to use.}
    }
    }

 \item{\code{get_weight_at( pos, connection )}:}{Get the current weight of a connection in component (\code{connection_set}) at specified topology index. If successful, returns weight, otherwise 0. Parameters are:
    \itemize{
    \item{\code{pos}}{: integer, position in topology of component to use.}
    \item{\code{connection}}{: connection to use.}
    }
    }

 \item{\code{set_weight_at( pos, connection, value )}:}{Set the weight of a connection in component (\code{connection_set}) at specified topology index. If successful, returns TRUE. Parameters are:
    \itemize{
    \item{\code{pos}}{: integer, position in topology of component to use.}
    \item{\code{connection}}{: connection to use.}
    \item{\code{value}}{: new weight for connection.}
    }
    }

 \item{\code{set_misc_values_at( pos, data_in )}:}{Set the values in the \code{misc} data register that \code{pe} and \code{connection} objects maintain, for objects at specified topology index. If successful, returns TRUE. Parameters are:
    \itemize{
    \item{\code{pos}}{: integer, position in topology of component to use.}
    \item{\code{data_in}}{: NumericVector, data to be used for new values in \code{misc} registers (sizes must match).}
    }
    }

 \item{\code{set_output_at( pos, data_in )}:}{Set the values in the \code{output} data register that \code{pe} objects maintain, for \code{layer} at specified topology index (currenly only \code{layer} components are supported). If successful, returns TRUE. Parameters are:
    \itemize{
    \item{\code{pos}}{: integer, position in topology of component to use.}
    \item{\code{data_in}}{: NumericVector, data to be used for new values in \code{misc} registers (sizes must match).}
    }
    }

\item{\code{print( )}:}{Print internal NN state, including all components in topology.}

\item{\code{outline( )}:}{Print a summary description of all components in topology.}

 }

The following methods are inherited (from the corresponding class):
objectPointer ("RcppClass"), initialize ("RcppClass"), show ("RcppClass")
}


