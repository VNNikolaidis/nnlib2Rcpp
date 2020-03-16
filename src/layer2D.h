//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		layer2D.h		 							Version 0.1
//		-----------------------------------------------------------
//		Definition - implementation of general neural 2D layer
//		functionality. Will be replaced by xD in future versions.
//		-----------------------------------------------------------


#include "layer.h"

template <class PE_TYPE>
class Layer2D : public Layer<PE_TYPE>
{
protected:
	int m_dim1, m_dim2;

public:
	Layer2D(string name, int dim1, int dim2);
	Layer2D(string name, int dim1, int dim2, bool PTR error_flag_to_use);
	pe REF PE(int c1, int c2) { return PE(coords2PEid(c1,c2)); }
	int dim1() { return m_dim1; };
	int dim2() { return m_dim2; };
	int  coords2PEid(int c1, int c2);						// Note: PE id is PE's position in vector of PEs.
	void PEid2coords(int id, int REF c1, int REF c2);		// Note: PE id is PE's position in vector of PEs
	bool coords_are_valid(int c1, int c2);
};

/*-----------------------------------------------------------------------*/

template <class PE_TYPE>
Layer2D<PE_TYPE>::Layer2D(string name, int dim1, int dim2)
	:Layer2D(name, dim1, dim2, NULL)
{}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class PE_TYPE>
Layer2D<PE_TYPE>::Layer2D(string name, int dim1, int dim2, bool PTR error_flag_to_use)
	:Layer<PE_TYPE>(name,dim1*dim2,error_flag_to_use)
{
m_dim1 = dim1;
m_dim2 = dim2;
if((dim1<=0)OR(dim2<=0))
	{
	error(NN_INTEGR_ERR,"Invalid layer dimensions");
	m_dim1 = 1;
	m_dim2 = 0;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class PE_TYPE>
bool Layer2D<PE_TYPE>::coords_are_valid(int c1, int c2)
{
if( (c1>=0) AND (c1<m_dim1) AND
    (c2>=0) AND (c2<m_dim2) ) return true;
return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Note: PE id is position in vector of PEs

template <class PE_TYPE>
int Layer2D<PE_TYPE>::coords2PEid(int c1, int c2)
{
if(NOT coords_are_valid(c1,c2)) {error(NN_INTEGR_ERR,"Invalid PE coordinates"); return -1;}
int id = (c1*m_dim2)+c2;
if( (id<0) OR (id>=Layer<PE_TYPE>::size())){error(NN_INTEGR_ERR,"Invalid PE coordinates"); return -1;}
return id;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Note: PE id is position in vector of PEs

template <class PE_TYPE>
void Layer2D<PE_TYPE>::PEid2coords(int id, int REF c1, int REF c2)
{
if( (id<0) OR (id>=Layer<PE_TYPE>::size())){error(NN_INTEGR_ERR,"Invalid PE id");}
c1 = (int)(id/m_dim2);
c2 = id%m_dim2;
if(NOT coords_are_valid(c1,c2)) {error(NN_INTEGR_ERR,"Invalid PE coordinates");}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
