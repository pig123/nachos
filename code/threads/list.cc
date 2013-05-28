// list.cc 
//
//     	Routines to manage a singly-linked list of "things".
//
// 	A "ListElement" is allocated for each item to be put on the
//	list; it is de-allocated when the item is removed. This means
//      we don't need to keep a "next" pointer in every object we
//      want to put on a list.
// 
//     	NOTE: Mutual exclusion must be provided by the caller.
//  	If you want a synchronized list, you must use the routines 
//	in synchlist.cc.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "list.h"

//----------------------------------------------------------------------
// ListElement::ListElement
// 	Initialize a list element, so it can be added somewhere on a list.
//
//	"itemPtr" is the item to be put on the list.  It can be a pointer
//		to anything.
//	"sortKey" is the priority of the item, if any.
//----------------------------------------------------------------------

ListElement::ListElement(void *itemPtr, int sortKey)
{
     item = itemPtr;
     key = sortKey;
     next = NULL;	// assume we'll put it at the end of the list
     prev = NULL;
}

//----------------------------------------------------------------------
// List::List
//	Initialize a list, empty to start with.
//	Elements can now be added to the list.
//----------------------------------------------------------------------

List::List()
{ 
    first = last = NULL;
    count = 0;
}

//----------------------------------------------------------------------
// List::~List
//	Prepare a list for deallocation.  If the list still contains any 
//	ListElements, de-allocate them.  However, note that we do *not*
//	de-allocate the "items" on the list -- this module allocates
//	and de-allocates the ListElements to keep track of each item,
//	but a given item may be on multiple lists, so we can't
//	de-allocate them here.
//----------------------------------------------------------------------

List::~List()
{ 
    while (Remove() != NULL)
	;	 // delete all the list elements
    count =0;
}

//----------------------------------------------------------------------
// List::Append
//      Append an "item" to the end of the list.
//      
//	Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, put it at the end.
//
//	"item" is the thing to put on the list, it can be a pointer to 
//		anything.
//----------------------------------------------------------------------

void
List::Append(void *item)
{
    ListElement *element = new ListElement(item, 0);

    if (IsEmpty()) {		// list is empty
	first = element;
	last = element;
    } else {			// else put it after last
	last->next = element;
	element->prev = last;
	last = element;
    }
    count ++;
}

//----------------------------------------------------------------------
// List::Prepend
//      Put an "item" on the front of the list.
//      
//	Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, put it at the beginning.
//
//	"item" is the thing to put on the list, it can be a pointer to 
//		anything.
//----------------------------------------------------------------------

void
List::Prepend(void *item)
{
    ListElement *element = new ListElement(item, 0);

    if (IsEmpty()) {		// list is empty
	first = element;
	last = element;
    } else {			// else put it before first
	element->next = first;
	first->prev = element;
	first = element;
    }
    count ++;
}

//----------------------------------------------------------------------
// List::Remove
//      Remove the first "item" from the front of the list.
// 
// Returns:
//	Pointer to removed item, NULL if nothing on the list.
//----------------------------------------------------------------------

void *
List::Remove()
{
    return SortedRemove(NULL);  // Same as SortedRemove, but ignore the key
}

//----------------------------------------------------------------------
// List::Mapcar
//	Apply a function to each item on the list, by walking through  
//	the list, one element at a time.
//
//	Unlike LISP, this mapcar does not return anything!
//
//	"func" is the procedure to apply to each element of the list.
//----------------------------------------------------------------------

void
List::Mapcar(VoidFunctionPtr func)
{
    for (ListElement *ptr = first; ptr != NULL; ptr = ptr->next) {
       DEBUG('l', "In mapcar, about to invoke %x(%x)\n", func, ptr->item);
       (*func)((int)ptr->item);
    }
}

//----------------------------------------------------------------------
// List::IsEmpty
//      Returns TRUE if the list is empty (has no items).
//----------------------------------------------------------------------

bool
List::IsEmpty() 
{ 
    if (first == NULL)
        return TRUE;
    else
	return FALSE; 
}

//----------------------------------------------------------------------
// List::SortedInsert
//      Insert an "item" into a list, so that the list elements are
//	sorted in increasing order by "sortKey".
//      
//	Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, walk through the list, one element at a time,
//	to find where the new item should be placed.
//
//	"item" is the thing to put on the list, it can be a pointer to 
//		anything.
//	"sortKey" is the priority of the item.
//----------------------------------------------------------------------

void
List::SortedInsert(void *item, int sortKey)
{
    ListElement *element = new ListElement(item, sortKey);
    ListElement *ptr;		// keep track

    if (IsEmpty()) {	// if list is empty, put
        first = element;
        last = element;
    } else if (sortKey < first->key) {	
		// item goes on front of list
	element->next = first;
	first->prev = element;
	first = element;
    } else {		// look for first elt in list bigger than item
      //printf("first_key %d last_key %d sortKey %d\n",first->key,last->key,sortKey);
	for (ptr = first; ptr->next != NULL; ptr = ptr->next) {
            if (sortKey < ptr->next->key) {
		element->next = ptr->next;
		ptr->next->prev = element;
	        ptr->next = element;
		element->prev = ptr;
		return;
	    }
	}
	last->next = element;		// item goes at end of list
	element->prev = last;
	last = element;
    }
    count ++;
}

//----------------------------------------------------------------------
// List::SortedRemove
//      Remove the first "item" from the front of a sorted list.
// 
// Returns:
//	Pointer to removed item, NULL if nothing on the list.
//	Sets *keyPtr to the priority value of the removed item
//	(this is needed by interrupt.cc, for instance).
//
//	"keyPtr" is a pointer to the location in which to store the 
//		priority of the removed item.
//----------------------------------------------------------------------

void *
List::SortedRemove(int *keyPtr)
{
    ListElement *element = first;
    void *thing;

    if (IsEmpty()) 
	return NULL;

    thing = first->item;
    if (first == last) {	// list had one item, now has none 
        first = NULL;
	last = NULL;
    } else {
        first = element->next;
    }
    if (keyPtr != NULL)
        *keyPtr = element->key;
    delete element;
    count --;
    return thing;
}

void *List::RemoveLast() {
  void *thing;
  ListElement *tmp1 = last;
  ListElement *tmp2 = NULL;


  if (IsEmpty())
    return NULL;
  thing = last->item;
  if (first == last) {
    first = last = NULL;
    
  } else {
    tmp2 = last->prev;
    tmp2->next = NULL;
    last = tmp2;
    delete tmp1;
  }
  count --;
  return thing;
}

/*
 * getElementByKey
*/
ListElement *List::getElementByKey (int key) {
  ListElement *tmp = first;
  while (tmp->key != key) tmp = tmp->next;
  return tmp;
}

ListElement *List::getElementByName(void *name) {
  ListElement *tmp = first;
  while(tmp !=NULL && tmp->item != name) tmp = tmp->next;
  return tmp;
}

/*
 * RemoveByKey
 * First find the element, then prior element->next = current->next
 * delete the current element.
*/
void List::RemoveByKey(int key) {
  //first we output the list info
  /*
  ListElement *ri = first;
  while(ri!=NULL) {
    printf("%d  ",ri->key);
    ri = ri->next;
  }
  */
  ListElement *tmp = first;
  ListElement *tmp2 = NULL;
  //printf("first key is %d removekey %d\n",tmp->key,key);
  if (tmp!=NULL && tmp->key == key) {
    printf("helloworld\n");
    first = tmp->next;
    delete tmp;
  
  } else {
    while(tmp!=NULL && tmp->key != key) {
      tmp = tmp->next;
    }
    if (tmp->key == key) {
      if (tmp->prev!=NULL) {
	tmp->prev->next = tmp->next;
      } else {
	first = tmp->next;
      }
      if (tmp->next!=NULL) {
	tmp->next->prev = tmp->prev;
      } else {
	last = tmp->prev;
      }
      delete tmp;
    }
  }
  count --;
}
