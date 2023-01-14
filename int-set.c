//CHRISTOPHER PAINTER
#include "int-set.h"

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/** Abstract data type for set of int's.  Note that sets do not allow
 *  duplicates.
 */
typedef struct NodeStruct { //linked-list node for int
  int value;               //stored value
  struct NodeStruct *succ; //point to successor node; NULL if none.
} Node;

typedef struct { //header for sorted-list
  int nElements; //# of elements currently in list
  Node dummy;    //dummy Node; value never used
                 //dummy facilitates adding elements to list.
} Header;


/** Return a new empty int-set.  Returns NULL on error with errno set.
 */
void *newIntSet(){
  //by using calloc() we set nElements to 0 and succ to NULL.
  return calloc(1, sizeof(Header));
}

/** Return # of elements in intSet */
int nElementsIntSet(void *intSet){
  const Header *header = (Header *)intSet;
  return header->nElements;
}

/** Return non-zero iff intSet contains element. */
int isInIntSet(void *intSet, int element){
  Header *header = (Header *)intSet;
  Node *p0; //lagging pointer: will insert after p0.
  for (p0 = &header->dummy; p0->succ != NULL && p0->succ->value <= element;
       p0 = p0->succ) {
    //by using <= in test, we guarantee that later equal elements are
    //inserted after earlier equal elements
    if (element == p0->succ->value){
      return 1;
    }
  }
  return 0;
}

/** Change intSet by adding element to it.  Returns # of elements
 *  in intSet after addition.  Returns < 0 on error with errno
 *  set.
 */
int addIntSet(void *intSet, int element){
  Header *header = (Header *)intSet;
  Node *p0; //lagging pointer: will insert after p0.
  for (p0 = &header->dummy; p0->succ != NULL && p0->succ->value <= element;
       p0 = p0->succ) {
    //by using <= in test, we guarantee that later equal elements are
    //inserted after earlier equal elements
    if (element == p0->succ->value){
		 return header->nElements;
    }
  }
  //assert(p0->succ == NULL || p0->succ->value > element);
  //create and insert new node after p0
  Node *p = malloc(sizeof(Node));   //create a new Node to hold value
  if (!p) return -1;              //malloc failure
  p->value = element;
  p->succ = p0->succ; p0->succ = p; //link new node into list
  return ++header->nElements;
}

/** Change intSet by adding all elements in array elements[nElements] to
 *  it.  Returns # of elements in intSet after addition.  Returns
 *  < 0 on error with errno set.
 */
int addMultipleIntSet(void *intSet, const int elements[], int nElements){
  int ret = -1;
  for (int i = 0; i < nElements; i++) {
    if ((ret = addIntSet(intSet, elements[i])) < 0) break;
  }
  return ret;
}

/** Set intSetA to the union of intSetA and intSetB.  Return # of
 *  elements in the updated intSetA.  Returns < 0 on error.
 */
int unionIntSet(void *intSetA, void *intSetB){
  Header *header = (Header *)intSetA;
  Header *header2 = (Header *)intSetB;
  Node *pA0; //lagging pointer: will insert after p0.
  Node *pB=&header2->dummy;
  for (pA0 = &header->dummy, pB = pB->succ; pA0->succ!=NULL && pB!=NULL;) {
    if(pA0->succ->value < pB->value){
      pA0 = pA0->succ;  
      continue;  
    }
    if (pA0->succ->value == pB->value){
      pA0 = pA0->succ;
      pB = pB->succ;    
      continue;
    }
    if (pA0->succ->value > pB->value){
      Node *p = malloc(sizeof(Node));   //create a new Node to hold value
  		if (!p) return -1;              //malloc failure
  		p->value = pB->value;
  		p->succ = pA0->succ; pA0->succ = p; //link new node into list
      pB = pB->succ;
      ++header->nElements;
      continue;
    }
  }
  while (pB!=NULL){
    Node *p = malloc(sizeof(Node));   //create a new Node to hold value
    if (!p) return -1;              //malloc failure
  	 p->value = pB->value;
    p->succ = pA0->succ; pA0->succ = p; //link new node into list
    ++header->nElements;
    pA0 = pA0->succ;
    pB = pB->succ;
  }
  return nElementsIntSet(intSetA);
}

/** Set intSetA to the intersection of intSetA and intSetB.  Return #
 *  of elements in the updated intSetA.  Returns < 0 on error.
 */
int intersectionIntSet(void *intSetA, void *intSetB){
  Header *header = (Header *)intSetA;
  Header *header2 = (Header *)intSetB;
  Node *pA0; //lagging pointer: will insert after p0.
  Node *pB=&header2->dummy;
  for (pA0 = &header->dummy, pB = pB->succ; pA0->succ!=NULL && pB!=NULL;) {
    if(pA0->succ->value < pB->value){
      Node *temp = pA0->succ;
      pA0->succ = temp->succ;
      free(temp);
      --header->nElements;
      continue;  
    }
    if (pA0->succ->value == pB->value){
      pA0 = pA0->succ;
      pB = pB->succ;    
      continue;
    }
    if (pA0->succ->value > pB->value){
      pB = pB->succ;
      continue;
    }
  }
  while (pA0->succ!=NULL){
	 Node *temp = pA0->succ;
    pA0->succ = temp->succ;
    free(temp);
    --header->nElements;
  }
  return nElementsIntSet(intSetA);
}

/** Free all resources used by previously created intSet. */
void freeIntSet(void *intSet){
  Header *header = (Header *)intSet;
  Node *p1;
  for (Node *p = header->dummy.succ; p != NULL; p = p1) {
    p1 = p->succ;  //must complete accesses to p before freeing p
    free(p);
  }
  free(header);
}

/** Return a new iterator for intSet.  Returns NULL if intSet
 *  is empty.
 */
const void *newIntSetIterator(const void *intSet){
  const Header *header = (Header *)intSet;
  return header->dummy.succ;
}

/** Return current element for intSetIterator. */
int intSetIteratorElement(const void *intSetIterator){
  const Node *p = (Node *)intSetIterator;
  return p->value;
}

/** Step intSetIterator and return stepped iterator.  Return
 *  NULL if no more iterations are possible.
 */
const void *stepIntSetIterator(const void *intSetIterator){
  const Node *p = (Node *)intSetIterator;
  return p->succ;
}
