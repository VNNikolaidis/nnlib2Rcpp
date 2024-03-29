//		----------------------------------------------------------
//		(c)2019  Vasilis.N.Nikolaidis          All rights reserved.
//		-----------------------------------------------------------
//		nnlib2_dllist.h		 							Version 0.2
//		-----------------------------------------------------------
//		custom double linked list functionality.
//      Warning!:
//		   very limited functionality, a single iterator (mp_current)
//         is used for each instance of such list. Only one loop that
//		   uses current() or may change mp_current (using goto_first,
//		   goto_next etc.) should be traversing each instance of such
//		   list. No other such nested loop should be accessing it
//		   before the first one is completed.
//		   In such cases the [] operator may be use to access the list.
//		-----------------------------------------------------------

#ifndef NN_DLLIST_H
#define NN_DLLIST_H

#include <iostream>
#include "nnlib2_string.h"
#include "nnlib2_error.h"

// #define DLLIST_EXTRA_INTEGRITY_CHECKS

namespace nnlib2 {

template <class T>
class dllist : public error_flag_client
 {
 protected:

 struct _T_wrapper;
 typedef struct _T_wrapper {T item ; _T_wrapper PTR  previous; _T_wrapper PTR  next; } T_wrapper;

 T_wrapper PTR	mp_first;
 T_wrapper PTR	mp_last;
 T_wrapper PTR	mp_current;					// warning: a single internal iterator for each instance of a list. use with care!
 int			m_number_of_items;
 T				m_junk;

 public:

 dllist();
 dllist(int number_of_items);
 dllist(const dllist<T> REF list);
 ~dllist();
 bool goto_first ();						// move mp_current to first item, false if list is empty
 bool goto_last ();							// move mp_current to last item, false if list is empty
 bool goto_next ();							// move mp_current to next item, false if at last of list, or list is empty
 bool goto_previous ();						// move mp_current to previous item, false if at first of list or list is empty
 bool goto_item(int i);						// move mp_current to  specified item
 bool is_at_first_item ();					// check if mp_current is at the first item
 bool is_at_last_item ();					// check if mp_current is at the last item
 bool ended ();								// check if mp_current is NULL
 T REF first();								// moves mp_current at first, then return item pointed by mp_current
 T REF next();								// moves mp_current to its next, then return item pointed by mp_current. Invalid data if past end.
 T REF last();								// moves mp_current at last,  then return item pointed by mp_current
 T REF current();							// returns item pointed by mp_current
 bool append();
 virtual bool append(const T REF item);
 virtual bool insert(int at_index, const T REF item);
 bool reset();
 bool remove_last();
 bool remove_current();
 int number_of_items();						// number of items in list
 int size();								// same as above
 int length();								// same as above
 bool is_empty();
 bool contains (const T REF item);
 bool find (const T REF item);				// same as 'contains' but moves mp_current to target.
 T REF operator [] (int i);					// returns item at i. does NOT change mp_current to that item.
 bool append_from(const dllist<T> REF list);
 void from_stream (std::istream REF s);
 void to_stream   (std::ostream REF s);
 friend std::istream REF operator >> ( std::istream REF is, dllist REF it ) {it.from_stream(is);return is;};
 friend std::ostream REF operator << ( std::ostream REF os, dllist REF it ) {it.to_stream(os);return os;};
 bool check();
 };

// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 template <class T>
 dllist<T>::dllist()
  {
  mp_first=mp_last=mp_current=NULL;
  m_number_of_items = 0;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 template <class T>
 dllist<T>::dllist(int number_of_items)
  {
  mp_first=mp_last=mp_current=NULL;
  m_number_of_items = 0;

  for (int i = 0; (no_error() AND (i<number_of_items)); i++) append();
  }


 //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 template <class T>
 dllist<T>::dllist(const dllist<T> REF list)
 {
 	mp_first=mp_last=mp_current=NULL;
 	m_number_of_items = 0;
 	set_error_flag(list.mp_error_flag);

 	if(!no_error()) return;
 	append_from(list);
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 template <class T>
 dllist<T>::~dllist()
  {
  check();
  reset();
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


 template <class T>
 bool dllist<T>::goto_first ()						// false if list is empty
  {
  mp_current = mp_first;
  return (mp_current NEQL NULL);
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


 template <class T>
 bool dllist<T>::goto_last ()						// false if list is empty
  {
  mp_current = mp_last;
  return (mp_current NEQL NULL);
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


 template <class T>
 bool dllist<T>::goto_next ()						// false if at last of list or list is empty
  {
  if(mp_current EQL NULL) return false;
  if(mp_current->next EQL NULL) return false;
  mp_current = mp_current->next;
  return true;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


 template <class T>
 bool dllist<T>::goto_previous ()					// false if at first of list or list is empty
  {
  if(mp_current EQL NULL) return false;
  if(mp_current->previous EQL NULL) return false;
  mp_current = mp_current->previous;
  return true;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// check if mp_current is at the first item

 template <class T>
 bool dllist<T>::is_at_first_item ()
  {
  if(mp_current->previous EQL NULL) return false;
  return true;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// check if mp_current is at the last item

 template <class T>
 bool dllist<T>::is_at_last_item ()
 {
 	if(mp_current->next EQL NULL) return false;
 	return true;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// check if mp_current is NULL

 template <class T>
 bool dllist<T>::ended()
 {
 return (mp_current==NULL);
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// a few checks, just to be on the safe side. Hmmm, I should have used STL...

 template <class T>
 bool dllist<T>::check()
  {
  #ifdef DLLIST_EXTRA_INTEGRITY_CHECKS

  bool ok = true;
  T_wrapper PTR	p;

  int c1 = 0;
  p = mp_first;
  while (p != NULL)
   {
   c1++;
   if(p->next==NULL) if(p!=mp_last) ok=false;
   p=p->next;
   }

  int c2 = 0;
  p = mp_last;
  while (p != NULL)
   {
   c2++;
   if(p->previous==NULL) if(p!=mp_first) ok=false;
   p=p->previous;
   }

  if (c1 NEQL m_number_of_items) ok = false;

  if (c2 NEQL m_number_of_items) ok = false;

  if( (mp_first EQL NULL)  OR
      (mp_last  EQL NULL) OR
	  (mp_current EQL NULL)  OR
      (m_number_of_items EQL 0) )
  	 if( NOT ((mp_first EQL NULL) AND
              (mp_last  EQL NULL) AND
              (mp_current EQL NULL) AND
              (m_number_of_items EQL 0) ))
       ok = false;

  if(NOT ok) error (NN_SYSTEM_ERR,"dllist:does not pass checks");
  return ok;

  #else

  return true;

  #endif
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 template <class T>
 T REF dllist<T>::current()
  {
  if(mp_current EQL NULL)
   {
   error(NN_SYSTEM_ERR,"dllist, attempt to access non-existant item");
   return m_junk;
   }

  return mp_current->item;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// appends an item and changes mp_current to it.

 template <class T>
 bool dllist<T>::append()
  {
  bool ok = no_error();
  if(ok)
   {
   T_wrapper PTR p_new_wrapper;

   ok=((p_new_wrapper = new T_wrapper) NEQL NULL);

   if(ok)
    {
    if(mp_first EQL NULL)							// List empty?
     {
     check();
     p_new_wrapper->previous=NULL;
     p_new_wrapper->next=NULL;
     mp_first = p_new_wrapper;
     mp_last  = mp_first;
     }
    else
     {
     mp_last->next=p_new_wrapper;
     p_new_wrapper->previous=mp_last;
     p_new_wrapper->next=NULL;
     mp_last = p_new_wrapper;
     }
    mp_current = mp_last;
    m_number_of_items++;
    }
   }
  return ok;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// appends an item and changes mp_current to it.

 template <class T>
 bool dllist<T>::append(const T REF item)
  {
  bool ok = append();
  if(ok) mp_last->item = item;
  return ok;
  }

 //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 template <class T>
 bool dllist<T>::insert(int at_index, const T REF item)
  {
  if(NOT check()) return false;

  T_wrapper PTR p_new;
  p_new = new T_wrapper;
  if(p_new == NULL) return false;

  p_new->item = item;
  p_new->previous=NULL;
  p_new->next=NULL;

  if(mp_first EQL NULL)						  	// list empty.
 	{
 	mp_first 			= p_new;
 	mp_last  			= p_new;
 	mp_current			= p_new;
 	m_number_of_items++;
 	return(check());
 	}

   if(at_index <= 0)  							// insert at first index position, list not empty, new becomes first
 	{
   	p_new->next = mp_first;
   	mp_first->previous	= p_new;
   	mp_first 			= p_new;
   	m_number_of_items++;
   	return(check());
 	}

  if(at_index >= m_number_of_items)  			// insert at index last position, list not empty, new becomes last
   	{
   	p_new->previous 	= mp_last;
   	mp_last->next 		= p_new;
   	mp_last 			= p_new;
   	m_number_of_items++;
   	return(check());
   	}

  // insert at arbitrary internal index position, in non empty list

  T_wrapper PTR p_existing = mp_first;
  int c=0;
  while ((p_existing!=NULL) AND (c<at_index))
  	{
  	p_existing=p_existing->next;
  	c++;
  	}

  if(p_existing==NULL) return false;

  p_new->next 					= p_existing;
  p_new->previous 				= p_existing->previous;
  p_existing->previous->next 	= p_new;
  p_existing->previous			= p_new;
  m_number_of_items++;

  return(check());
  }

 //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 template <class T>
 bool dllist<T>::reset()
  {
  while(goto_last())
   {
   remove_last();
   }

  check();

  mp_first = mp_current = mp_last = NULL;
  m_number_of_items = 0;

  return true;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// removes last item (also changes mp_current)

 template <class T>
 bool dllist<T>::remove_last()
  {
  bool ok = true;

  if(NOT goto_last())							// take mp_current at last item and check if list is not empty...
   {
   error(NN_NULLPT_ERR,"dllist, can not remove, list is empty");
   return false;
   }

  if(mp_last->previous EQL NULL)				// is this the first item ?
   {
   if( (mp_first EQL mp_last) AND (m_number_of_items EQL 1) )
     {
     delete(mp_last);
     mp_first = mp_last = mp_current = NULL;
     m_number_of_items--;
     }
   else
     {
     error(NN_SYSTEM_ERR,"dllist: inconsistent");
     ok = false;
     }
   }
  else										// this is not the first item...
   {
   goto_previous();
   mp_current->next=NULL;

   delete mp_last;
   mp_last = mp_current;
   m_number_of_items--;
   }

  return ok;
  }

 //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 template <class T>
 bool dllist<T>::remove_current()
 {
 	if(mp_current==NULL)
 	{
 		error(NN_NULLPT_ERR,"dllist, can not remove current");
 		return false;
 	}

 	if(((mp_current->previous EQL NULL) AND					// is this the first item ?
         (mp_first NEQL mp_current)) OR
       ((mp_current->next EQL NULL) AND						// is this the last item ?
        (mp_last NEQL mp_current)))
 	{
 		error(NN_SYSTEM_ERR,"dllist: inconsistent, cannot delete current item.");
 		return false;
 	}

  	if(mp_current->previous EQL NULL) 						// is this the first item ?
 		mp_first = mp_current->next;

  	if(mp_current->previous NEQL NULL)						// this ΝΟΤ the first item.
  		mp_current->previous->next = mp_current->next;

 	if(mp_current->next EQL NULL)							// is this the last item ?
 		mp_last = mp_current->previous;

 	if(mp_current->next NEQL NULL)							// this ΝΟΤ the last item.
 		mp_current->next->previous = mp_current->previous;

 	delete(mp_current);
 	m_number_of_items--;
 	mp_current = mp_first;

 	return check();
 }

 //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 // also changes mp_current.

 template <class T>
 T REF dllist<T>::first()
  {
  if(mp_first EQL NULL)
   {
   error(NN_SYSTEM_ERR,"dllist, can not access last item, list empty");
   return m_junk;
   }
  mp_current=mp_first;
  return mp_current->item;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// also changes mp_current.

 template <class T>
 T REF dllist<T>::last()
  {
  if(mp_last EQL NULL)
   {
   error(NN_SYSTEM_ERR,"dllist, can not access last item, list empty");
   return m_junk;
   }
  mp_current=mp_last;
  return mp_current->item;
  }

 //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 // changes mp_current.

 template <class T>
 T REF dllist<T>::next()
 {
 	if(mp_last EQL NULL)
 	{
 		error(NN_SYSTEM_ERR,"dllist, can not access last item, list empty");
 		return m_junk;
 	}
 	if(NOT goto_next())
 	{
 		error(NN_SYSTEM_ERR,"dllist, can not access next item.");
 		return m_junk;
 	}
 	return mp_current->item;
 }

 //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 // size_t would be better here, but would confuse novices

 template <class T>
 int dllist<T>::number_of_items() {return m_number_of_items;}

 template <class T>
 int dllist<T>::size() {return m_number_of_items;}

 template <class T>
 int dllist<T>::length() {return m_number_of_items;}

 //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 template <class T>
 bool dllist<T>::is_empty()
  {
  return (m_number_of_items<=0);
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

 template <class T>
 bool dllist<T>::contains (const T REF item)
  {
  T_wrapper PTR p = mp_first;
  bool found = false;
  while( (p NEQL NULL) AND (NOT found))
   {
   found = (p->item==item);
   p = p->next;
   }
  return found;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// same as 'contains' but moves mp_current to target.

 template <class T>
 bool dllist<T>::find (const T REF item)
  {
  bool found = false;

  if(goto_first())
   {
   do
    {
   	found = (mp_current->item==item);
    if(found) break;
    }
   while(goto_next());
   }
  return found;
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// move mp_current to specified item

template <class T>
bool dllist<T>::goto_item(int i)
 {
	int c=0;

	if((i<0) OR
        (i>=m_number_of_items) OR
        (NOT goto_first()) )
	{
		error(NN_SYSTEM_ERR,"dllist, empty list or attempt to access non-existant item");
		return false;
	}

	while (i NEQL c)
	{
		if(NOT goto_next())
		{
			error(NN_SYSTEM_ERR,"dllist, attempt to access non-existant item");
			return false;
		}
		c++;
	}

 return true;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class T>
bool dllist<T>::append_from(const dllist<T> REF list)
{
	if(!no_error()) return false;

	T_wrapper PTR mp_item_to_copy;

	mp_item_to_copy = list.mp_first;

	while(mp_item_to_copy!=NULL)
	{
		if(append(mp_item_to_copy->item))
			mp_item_to_copy=mp_item_to_copy->next;
		else
			mp_item_to_copy = NULL;
	}
	return(no_error());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// old version, changes mp_current.

 // template <class T>
 // T REF dllist<T>::operator [] (int i)
 //  {
 //  if(NOT goto_item(i)) return m_junk;
 //  return current();
 //  }

 //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 // newer version, does NOT change mp_current. Use it to access elements
 // without disturbing other loops that may use mp_current, current,
 // goto_first, goto_last, etc.

 template <class T>
 T REF dllist<T>::operator [] (int i)
 {
 	if((i<0) OR (i>=m_number_of_items) OR is_empty() OR (mp_first == NULL))
 	{
 		error(NN_SYSTEM_ERR,"dllist, empty list or attempt to access non-existant item");
 		return m_junk;
 	}

 	int c = 0;
 	T_wrapper PTR p = mp_first;
 	while (p != NULL)
 	{
 		if(c==i) return p->item;
 		p=p->next;
 		c++;
 	}

 	error(NN_SYSTEM_ERR,"dllist, attempt to access non-existant item");
 	return m_junk;
 }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// input it :

 template <class T> void dllist<T>::from_stream (std::istream REF s)
  {
  int i,stored_items;
  string comment;

  s >> comment >> stored_items ;
  reset();
  for(i=0;((i<stored_items)AND no_error());i++)
   {
   append();
   s >> comment >> (current());
   }
  }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// output it :

 template <class T> void dllist<T>::to_stream (std::ostream REF s)
  {
  int i=0;
  if(no_error())
   {
   s << "ListSize(elements): " << m_number_of_items << "\n";
   if(goto_first())
    do
     {
     s << i << ": " << current();
     i++;
     }
    while (goto_next());
   }
  }

//-----------------------------------------------------------------------
// pointer_list : same as dllist, slightly modified to store pointers.
//-----------------------------------------------------------------------

template<class T>
class pointer_dllist : public dllist <T>
{
public:
	bool append(const T ADR item);
};

// implementation follows:
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class T>
bool pointer_dllist<T>::append(const T ADR item)
 {
	if(item EQL NULL)
	{
		error (NN_NULLPT_ERR,"Attempting to place null pointer item in list");
		return false;
	}

	if(dllist<T>::append()) dllist<T>::mp_last->item = item;
	return true;
 }

//-----------------------------------------------------------------------

}   // end of namespace nnlib2

#endif // NN_DLLIST_H
