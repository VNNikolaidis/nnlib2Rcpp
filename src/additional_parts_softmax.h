#ifndef NNLIB2_ADDITIONAL_PARTS_SOFTMAX_H
#define NNLIB2_ADDITIONAL_PARTS_SOFTMAX_H

#include "nn.h"
using namespace nnlib2;

//--------------------------------------------------------------------------------------------
// a layer that performs softmax when recalling data.
// it contains generic "dumb" pes, so most processing is done in layer code
// Note:
// when encoding it simply passes input to output.
// when recalling it outputs the components of the softmax vector of input values.

class softmax_layer: public pe_layer
{
public:

	softmax_layer(string name, int size):pe_layer(name,size){}

	void recall()
	{
		if(no_error())
		{
			DATA denom=0;
			for(int i=0;i<size();i++)
			{
				pe REF p = pes[i];
				DATA x = p.input;											// input is already summated;
				denom=denom+exp(x);
			}

			if(denom==0)													// actually, this can never happen if size>0, but just to be sure
				warning("Sum is zero, cannot compute softmax.");
			else
			for(int i=0;i<size();i++)
			{
				pe REF p = pes[i];
				DATA x = p.input;											// input is already summated;
				p.output=(DATA)1*exp(x)/denom;
				p.input=0;													// reset input.
			}
		}
	}
};

//--------------------------------------------------------------------------------------------
// a BP hidden layer that performs softmax when recalling data.
// Based on code (and issue raised) by Thom Quinn (see https://github.com/VNNikolaidis/nnlib2Rcpp/issues/14)

#include "nn_bp.h"

class bp_comput_softmax_layer: public bp::bp_comput_layer
{
public:

	void recall()
	{
		if(no_error())
		{
			DATA denom=0;
			for(int i=0;i<size();i++)
			{
				pe REF p = pes[i];
				DATA x = p.input;											// input is already summated;
				x = x + p.bias;												// add bias,
				denom=denom+exp(x);
			}

			if(denom==0)													// actually, this can never happen if size>0, but just to be sure
				warning("Sum is zero, cannot compute softmax.");
			else
				for(int i=0;i<size();i++)
				{
					pe REF p = pes[i];
					DATA x = p.input;											// input is already summated;
					x = x + p.bias;												// add bias,
					p.output=(DATA)1*exp(x)/denom;
					p.input=0;													// reset input.
				}
		}
	}
};

//--------------------------------------------------------------------------------------------
// a BP output layer that performs softmax when recalling data.
// Based on code (and issue raised) by Thom Quinn (see https://github.com/VNNikolaidis/nnlib2Rcpp/issues/14)

class bp_output_softmax_layer : public bp::bp_output_layer
{
public:

	void recall()
	{
		if(no_error())
		{
			DATA denom=0;
			for(int i=0;i<size();i++)
			{
				pe REF p = pes[i];
				DATA x = p.input;											// input is already summated;
				x = x + p.bias;												// add bias,
				denom=denom+exp(x);
			}

			if(denom==0)													// actually, this can never happen, but just to be sure
				warning("Sum is zero, cannot compute softmax.");
			else
				for(int i=0;i<size();i++)
				{
					pe REF p = pes[i];
					DATA x = p.input;											// input is already summated;
					x = x + p.bias;												// add bias,
					p.output=(DATA)1*exp(x)/denom;//
					p.input=0;													// reset input.
				}
		}
	}
};

//------------------------------------------------------------------------------

#endif // NNLIB2_ADDITIONAL_PARTS_SOFTMAX_H
