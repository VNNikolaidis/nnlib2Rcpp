//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		nnlib2_vector.h		 							              Version 0.2
//		-----------------------------------------------------------
//		custom vector functionality.
//		-----------------------------------------------------------


#ifndef NN_VECTOR_H
#define NN_VECTOR_H

#include "nnlib2.h"
#include "nnlib2_error.h"

#include <new>
#include <iostream>

namespace nnlib2 {

/*-----------------------------------------------------------------------*/
/* vector																 */
/*-----------------------------------------------------------------------*/

template <class T>
class vector : public error_flag_client
 {
 private:

 /* variables */

 T PTR		      m_storage;
 int		        m_number_of_elements;
 T			        m_junk;

 /* methods */

 public:

 /* variables */

 /* methods */

 vector();
 vector(const int number_of_elements);
 vector(const int number_of_elements, T initial_value);
 ~vector();
 T REF operator [] (const int item);
 T REF at (const int item);
 bool setup (const int new_number_of_items);
 void reset ();
 int number_of_items ();
 bool contains (T REF item);
 int first_location_of(T REF item);
 void from_stream (std::istream REF s);
 void to_stream   (std::ostream REF s);
 friend std::istream REF operator >> ( std::istream REF is, vector REF it ) {it.from_stream(is);return is;};
 friend std::ostream REF operator << ( std::ostream REF os, vector REF it ) {it.to_stream(os);return os;};
 };

// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class T> vector<T>::vector ()
 {
 m_storage = NULL;
 m_number_of_elements = 0;
 }


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class T> vector<T>::vector (const int number_of_elements)
 {
 m_storage = NULL;
 m_number_of_elements = 0;
 setup(number_of_elements);
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class T> vector<T>::vector (const int number_of_elements, T initial_value)
 {
 m_storage = NULL;
 m_number_of_elements = 0;
 setup(number_of_elements);
 if(m_storage!=NULL)
  for(int i=0;i<number_of_elements;i++)
   m_storage[i]=initial_value;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class T> vector<T>::~vector()
 {
 reset();
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class T>
T REF vector<T>::operator [] (const int item)
 {
 return at(item);
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class T>
T REF vector<T>::at (const int item)
 {
 if(m_storage EQL NULL)
  {
  error(NN_SYSTEM_ERR,"vector, attempt to access empty storage");
  return m_junk;
  }

 if((item<0) OR (item>=m_number_of_elements))
  {
  error(NN_SYSTEM_ERR,"vector, attempt to access non-existant item");
  return m_junk;
  }

 return m_storage[item];
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// initialize and allocate..

template <class T> bool vector<T>::setup(const int new_number_of_items)
 {
 if(new_number_of_items EQL 0)
  warning("vector: Warning. Setup called for new item size = 0.");

 reset();

  try												// added for future compatibility with C9X standard.
   {
   m_storage = new T [new_number_of_items];
   }
  catch (std::bad_alloc&)
   {
   error(NN_MEMORY_ERR,"Cannot allocate memory for vector");
   m_storage = NULL;
   return false;
   }

 if(m_storage EQL NULL)
  {
  error(NN_MEMORY_ERR,"Cannot allocate memory for vector");
  return false;
  }
 else
  {
  m_number_of_elements = new_number_of_items;
  return true;
  }
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Reset to empty...

template <class T> void vector<T>::reset ()
 {
 bool is_null = (m_storage EQL NULL);
 bool is_zero = (m_number_of_elements EQL 0);

 if (NOT ((is_null AND is_zero) OR (NOT is_null AND NOT is_zero)))
  error(NN_SYSTEM_ERR,"vector: check code");

 if (m_storage NEQL NULL)
 {
	 delete [] m_storage;
//	 delete  m_storage;
 }

 m_storage = NULL;
 m_number_of_elements = 0;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class T> int vector<T>::number_of_items ()
 {return (int)m_number_of_elements;}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class T> bool vector<T>::contains (T REF item)
 {
 int i=0;
 bool found = false;
 while( (i<m_number_of_elements) AND (NOT found) )
  {
  found=(m_storage[i] == item);
  i++;
  }
 return found;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// (returns -1 if not found)

template <class T> int vector<T>::first_location_of (T REF item)
 {
 int i=0;
 bool found = false;
 while( (i<m_number_of_elements) AND (NOT found) )
  {
  found=(m_storage[i] == item);
  i++;
  }
 if(NOT found)
  {
  error(NN_SYSTEM_ERR,"vector, cannot locate element");
  return -1;
  }
 return i-1;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// input it :

template <class T> void vector<T>::from_stream (std::istream REF s)
 {
  int i,stored_items;
  std::string comment;

  s >> comment >> stored_items ;
  reset();
  if(setup(stored_items))
   for(i=0;((i<stored_items)AND no_error());i++)
    {
    s >> comment;
    s >> at(i);
    }
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// output it :

template <class T> void vector<T>::to_stream (std::ostream REF s)
  {
  if(no_error())
   {
   s << "VectSize(elements): " << m_number_of_elements << "\n";
   for(int i=0;i<m_number_of_elements;i++)
    {
    s << i << ": ";
    s << at(i) ;
    }
   }
  }

//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -

} // end of namespace nnlib2

#endif // NN_VECTOR_H

