#--------------------------------------------------------------------------
# Train a LVQ (supervised LVQ) and return the codebook vector information.
# This function uses id numbers from 1 to (number of classes) (R-style).

LVQs_train <- function( train_data,
						train_class,
						iterations = 1000,
						number_of_output_nodes_per_class = 1,
						reward_coef = +0.2,
						punish_coef = -0.2,
						training_order = "reorder_once",
						initialization_method = 'sample',
						recall_train_data = FALSE)
{
	if (training_order != 'original' &
		training_order != 'reorder_once' &
		training_order != 'reorder')
		stop("Unknown training order method")

	if (initialization_method != '0to1' &
		initialization_method != 'means' &
		initialization_method != 'first' &
		initialization_method != 'sample')
		stop("Unknown initialization method")

	if ((sum(is.na(train_data))  > 0) |
		(sum(is.na(train_class)) > 0))
		stop("NAs found")

	if(!is.null(dim(train_class)))
		stop("Training class ids should 1-dimensional")

	if(!is.vector(train_class)&!is.factor(train_class))
		stop("Training class ids should be a vector or factor")

	if(number_of_output_nodes_per_class<=0)
		stop("Output nodes per class must be at least 1")

	if(nrow(train_data)<=0)
		stop("Not enough vectors (rows) to train LVQs.")

	if(nrow(train_data)!=length(train_class))
		stop("Number of (rows) in training data does not equal length of class ids.")

	#--------------------------------------------------------------------------
	if(!is.matrix(train_data))
		train_data <- as.matrix(train_data)

	if(!is.integer(train_class))
		train_class <- as.integer(train_class)

	if(min(train_class<1))  stop("Invalid class id (<1) found")
	if(min(train_class!=1)) warning("Minimum class id not equal to 1")

	num_variables <- ncol(train_data)
	num_classes <- max(train_class)

	variables_scaled_to_0_1_range = FALSE

	#--------------------------------------------------------------------------
	# if initial weights are set to default (i.e. 0 to 1 random values),
	# training data set will be scaled to 0 to 1 range:

	if (initialization_method == '0to1')
	{
		# scale to 0 to 1 range (ONLY TRAINING SET min, max, range USED)

		cat("Note: internal training data variables scaled to [0 1].\n")
		training_min_values   <- apply(train_data, 2, FUN = "min")
		training_max_values   <- apply(train_data, 2, FUN = "max")
		training_range_values <- training_max_values - training_min_values
		train_data <- sweep(train_data, 2, FUN = "-", training_min_values)
		train_data <- sweep(train_data, 2, FUN = "/", training_range_values)

		variables_scaled_to_0_1_range = TRUE
	}

	#--------------------------------------------------------------------------
	# set initial weights (if not default 0 to 1 random values):

	initial_weights <- NULL

	# below, weights are set to the mean vector of the training set data for the
	# class the output node corresponds to:
	# THIS VERSION IS MODIFIED TO HANDLE POSSIBLE EMPTY CLASSES

	if (initialization_method == 'means')
	{
		cat("Note: initializing weights to mean class vectors.\n")

		class_means <- NULL
		for (i in 1:num_classes)
		{
			if (sum(train_class == i) <= 0)
			{
				warning(paste("Note: found empty training class (with id", i, ")"))
				class_mean <- rep(x = 0, num_variables)
			}
			else
				class_mean <- apply(train_data[train_class == i, , drop=FALSE], MARGIN = 2, FUN = mean)
			class_means <- rbind(class_means, class_mean)
		}

		for (i in 1:number_of_output_nodes_per_class)
			initial_weights <- cbind(initial_weights, class_means)

		rm(class_means)
	}

	# below, weights are set to the first points of the training set data for the
	# class the output node corresponds to:

	if (initialization_method == 'sample')
	{
		cat("Note: initializing weights to randomly selected vectors in data (with replacement if not enough are available).\n")

		for (i in 1:num_classes)
		{
			i_class_size <- sum(train_class == i)

			if (i_class_size <= 0)
			{
				warning(paste("Note: data contains no points of class",i,". Setting corresponding initial weights to zeros."))

				for (r in 1:number_of_output_nodes_per_class)
					initial_weights <- rbind(initial_weights, rep(x = 0, num_variables))
			}

			if (i_class_size > 0)
			{
				initial_weights <- rbind(initial_weights,
										 train_data[train_class == i,,drop = FALSE][sample(1:i_class_size,
										 												  min(i_class_size, number_of_output_nodes_per_class),
										 												  replace = FALSE),,drop = FALSE])
				if (i_class_size < number_of_output_nodes_per_class)
				{
					warning(paste("Note: data contains less than",number_of_output_nodes_per_class,"points of class",i,". Remaining randomly selected initial vector(s) will be duplicates of others."))
					initial_weights <- rbind(initial_weights,
											 train_data[train_class == i,,drop = FALSE][sample(1:i_class_size,
											 												  number_of_output_nodes_per_class - i_class_size,
											 												  replace = TRUE),,drop = FALSE])
				}
			}
		}
	}

	if (initialization_method == 'first')
	{
		cat("Note: initializing weights to first class vectors in data.\n")

		for (i in 1:num_classes)
		{
			i_class_size <- sum(train_class == i)

			if (i_class_size <= 0)
			{
				warning(paste("Note: data contains no points of class",i,". Setting corresponding initial weights to zeros."))

				for (r in 1:number_of_output_nodes_per_class)
					initial_weights <- rbind(initial_weights, rep(x = 0, num_variables))
			}

			if (i_class_size > 0)
			{
				initial_weights <- rbind(initial_weights,
										 train_data[train_class == i,,drop = FALSE][1:min(i_class_size, number_of_output_nodes_per_class),,drop = FALSE])

				if (i_class_size < number_of_output_nodes_per_class)
				{
					warning(paste("Note: data contains less than",number_of_output_nodes_per_class,"points of class",i,". Remaining initial vector(s) will be randomly selected and will be duplicates of others."))
					initial_weights <- rbind(initial_weights,
											 train_data[train_class == i,,drop = FALSE][sample(1:i_class_size,
											 												  number_of_output_nodes_per_class - i_class_size,
											 												  replace = TRUE),,drop = FALSE])
				}
			}
		}
	}

	# internal class ids (for LVQs) should be 0 to n-1 (where n the number of classes)

	train_class <- train_class - 1

	# now create the NN:

	lvq <- new("LVQs")
	lvq$set_number_of_nodes_per_class(number_of_output_nodes_per_class)
	lvq$setup(num_variables, num_classes)
	lvq$set_encoding_coefficients(reward_coef, punish_coef)

	if (variables_scaled_to_0_1_range)
		lvq$set_weight_limits(-0.5, 1.5)

	if(initialization_method != "0to1")
		lvq$set_weights(as.vector(t(initial_weights)))

	# train it:

	cat("Training supervised LVQ (LVQs)...\n")

	if (training_order == 'original')
	{
		cat("Note: internal training data not reordered.\n")

		capture.output(lvq$encode(train_data,
								  train_class,
								  iterations),type="output")
		cat("Training completed.\n")
	}


	if (training_order == 'reorder_once')
	{
		# reorder data:

		cat("Note: internal training data randomly reordered.\n")

		newpos <- sample(nrow(train_data), replace = FALSE)

		train_data  <- train_data[newpos, ]
		train_class <- train_class[newpos]

		capture.output(lvq$encode(train_data,
								  train_class,
								  iterations),type="output")
		cat("Training completed.\n")
	}

	if (training_order == 'reorder')
	{
		cat("Note: internal training data randomly reordered at each iteration (epoch).\n")

		if(iterations>10000)
		{
			warning("Number of epochs set to maximum allowed")
			iterations <- 10000
		}

		for(i in 0:(iterations-1))
		{
			newpos <- sample(nrow(train_data), replace = FALSE)

			train_data  <- train_data[newpos, ]
			train_class <- train_class[newpos]

			capture.output(
				{
				lvq$encode(train_data,
				    	   train_class,
						    1)
				lvq$set_encoding_coefficients(reward_coef * (1 - (i/10000)),
											  punish_coef * (1 - (i/10000)))
				},type="output")
		}
		cat("Training completed.\n")
	}

	# get the connection weights (codebook vector coordinates).

	num_rewards <- lvq$get_number_of_rewards()

	lvq_codebook_vector_info <-
		cbind(
			matrix(lvq$get_weights(), ncol = num_variables, byrow = TRUE),
			lvq$get_number_of_rewards(),
			rep(
				0:(num_classes - 1),
				rep(lvq$get_number_of_nodes_per_class(), num_classes)
			)
		)

	colnames(lvq_codebook_vector_info) <-
		c(colnames(train_data), "Rewards", "Class")

	lvq_codebook_vector_info_returned <- lvq_codebook_vector_info

	if (variables_scaled_to_0_1_range)
	{
		lvq_codebook_vector_info_returned[, 1:num_variables] <-
			sweep(lvq_codebook_vector_info_returned[, 1:num_variables],
				  2,
				  FUN = "*",
				  training_range_values)

		lvq_codebook_vector_info_returned[, 1:num_variables] <-
			sweep(lvq_codebook_vector_info_returned[, 1:num_variables],
				  2,
				  FUN = "+",
				  training_min_values)
	}

	# Add one to make it 1..num classes (R style)
	# note: to handle case where there is a "Class" in actual data, reference to last column
	# lvq_codebook_vector_info_returned[,"Class"] was replaced by lvq_codebook_vector_info_returned[,ncol(lvq_codebook_vector_info_returned)]

	lvq_codebook_vector_info_returned[,ncol(lvq_codebook_vector_info_returned)] <-
		lvq_codebook_vector_info_returned[,ncol(lvq_codebook_vector_info_returned)] + 1

	if (recall_train_data)
	{
		cat("Recalling training data:\n")
		capture.output(
			lvq_recalled_class_ids <- lvq$recall(train_data, 0),
			type="output")
		# note: train_data may have been reordered:
		# print(lvq_recalled_class_ids+1)
		correct = sum(lvq_recalled_class_ids == train_class)
		cat(  "Correct when recalling training data:", correct, "out of",
			  length(train_class), "(", round(100 * correct / length(train_class),2),"%)\n")
		cat("Confusion Matrix:")
		print(table((train_class+1), (lvq_recalled_class_ids+1)))
	}

	return(lvq_codebook_vector_info_returned)
}

#--------------------------------------------------------------------------
# Use the codebook vector information produced by LVQs_train to classify data.
# Essentially this runs a k-NNC classifier (class::knn) with some additional
# features. The function returns id numbers from 1 to (number of classes) (R-style).

LVQs_recall <- function(codebook_info,  data, k=1, recall_rewards_limit=1, verbose = FALSE, ...)
{
	num_variables = ncol(codebook_info)-2

	if(!is.matrix(data))
		data <- as.matrix(data)

	if(ncol(data)!=num_variables)
		stop("Data and codebook sizes are not compatible")

	# note: to handle case where there is a "Rewards" in actual data, reference to second from last column
	# codebook_info[,"Rewards"] was replaced by codebook_info[,ncol(codebook_info)-1]

	selected_codebook_info <- codebook_info[codebook_info[,ncol(codebook_info)-1]>=recall_rewards_limit,]

	if (verbose)
	{
		cat("\nNot used in recall:\n")
		print(round(codebook_info[codebook_info[,ncol(codebook_info)-1]<recall_rewards_limit,], digits = 4))

		cat("\nUsed in recall:\n")
		print(round(selected_codebook_info, digits = 4))
	}

	class_prototypes <- selected_codebook_info[,1:num_variables]

	# note: to handle case where there is a "Class" in actual data, reference to last column
	# codebook_info[,"Class"] was replaced by codebook_info[,ncol(codebook_info)]

	class_ids <- selected_codebook_info[,ncol(codebook_info)]

	lvq_recalled_class_ids <-
		knn(train=class_prototypes,
			test = data,
			cl = class_ids,
			k=k,
			...)

	if (verbose)
	if (ncol(data)>=2)
	{
		# the big circles are codebook vectors, (crossed-out if they were not used during
		# recall to assign data to a correct class, i.e. below reward limit)

		plot(
			data[ ,1:2],
			pch = (as.integer(lvq_recalled_class_ids) %% 10) + 1,
			col = as.integer(lvq_recalled_class_ids) + 10,
			main = "LVQs recalled clusters" )

		points(
			codebook_info[, 1:2],
			cex = 4,
			pch = ifelse(codebook_info[,ncol(codebook_info)-1] > recall_rewards_limit, 1, 13),
			col  = as.integer(codebook_info[,ncol(codebook_info)]) + 10
		)
	}

	return(lvq_recalled_class_ids)
}

#--------------------------------------------------------------------------
