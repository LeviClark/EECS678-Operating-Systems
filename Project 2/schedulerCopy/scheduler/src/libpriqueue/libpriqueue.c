/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.

  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
	q->size = 0;
	q->ptrHead = NULL;
	q->ptrTail = NULL;
	q->compareFunction = comparer;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{

	if(q->size == 0){ //is empty
		node_t *newNode = malloc(sizeof(node_t));//allocate space
		q->ptrHead = newNode;

		q->ptrTail = q->ptrHead;

		//add data to node
		q->ptrHead->data = ptr;
		q->ptrHead->next = NULL;

		q->size++; //increase size after we've added data


	}
	else{
		node_t *temp = q->ptrHead;
		node_t *prev = NULL;


		int ins = 0;
		//void *tempPointer;
		//q->size++;

		//compares our two nodes. Lowest will take Priority
		while(temp != NULL && q->compareFunction(temp->data, ptr) <= 0){
			prev = temp;
			temp = temp->next;
			ins++;

		}
		//at the end
		if(temp == NULL){
			temp = malloc(sizeof(node_t));
			q->ptrTail->next = temp;
			temp->next = NULL; //cause we're at the end
			temp->data = ptr;
			q->size++;
			q->ptrTail = temp;
		}
		else{
			node_t *insertTemp = malloc(sizeof(node_t));
			insertTemp->data = ptr;
			if(q->ptrHead == temp){
				q->ptrHead = insertTemp;
				insertTemp->next = temp;

			}
			else{
				insertTemp->next = temp;
				prev->next = insertTemp;

			}
			q->size++;

		}
		return ins;
	}
	return 0;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
	if(q->ptrHead == NULL){ //if nothing exists, return null
		return NULL;
	}

	return q->ptrHead;//else return the head
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
	return priqueue_remove_at(q, 0);
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
	if(index < 0 || index > q->size){ //tests for valid index
		return NULL;
	}
	else{
		node_t* temp = q->ptrHead;


		int i = 0;
		while( temp!= NULL && i < index){
			temp = temp->next;
			i++;

		}

		if(temp != NULL){
			return(void *)temp->data;
		}
	}
	return NULL;

}


/**
  Removes all instances of ptr from the queue.

  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
	if(q->size < 1){//if nothing to remove
		return 0;
	}

	int iter = 0;

	node_t *myNode = q->ptrHead, *prev = q->ptrHead;

	while(myNode != NULL){
		if(myNode->data == ptr){

		if(myNode->data == ptr){
			q->ptrHead = prev->next;
			prev = q->ptrHead;
		}
		else{
			prev->next = myNode->next;
		}
		free(myNode);
		iter++;
	}
	myNode = myNode->next;

	}

	q->size -= iter;

	return iter;


}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
	//check if queue is empty
	if(index < 0 || index >= q->size){
		return NULL;
	}

	else{
		node_t *prevPtr, *nextPtr, *elementRemove;

		elementRemove = q->ptrHead;

		if(index == 0){
			q->ptrHead = q->ptrHead->next;

			q->size--;

			void *temp = elementRemove->data;

			free(elementRemove);
			return temp;
		}

		prevPtr = elementRemove;
		elementRemove = elementRemove->next;

		//as long as we arent at the end
		if(elementRemove->next != NULL){
			nextPtr = elementRemove->next;
		}

		for(int i = 1; i < index; i++){
			if(nextPtr->next != NULL){
			nextPtr = nextPtr->next;
			}

			else{
				nextPtr = NULL;
			}

			elementRemove = elementRemove->next;
			prevPtr = prevPtr->next;
		}
		prevPtr->next = nextPtr;
		q->size--;

		void *temp = elementRemove->data;
		free(elementRemove);
		return temp;

		}
		return 0;
}


/**
  Returns the number of elements in the queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->size;
}


/**
  Destroys and frees all the memory associated with q.

  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
	node_t *temp = q->ptrHead, *next;
	q->ptrHead = NULL;

	while(temp!= NULL){
		next = temp->next;
		free(temp);
		temp = next;
		q->size--;
	}
}

void priqueue_print(priqueue_t *q){
	return;
}
