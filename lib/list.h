/* File: list.h
 *
 * A doubly linked-list implementation of a circular buffer.
 */


#include <stdint.h>

#ifndef LIST_H
#define LIST_H

#ifndef NULL
#define NULL (void*)0
#endif

#define LIST_IS_ONLY_NODE(node) (*node == (*node)->prev)

struct list_head
{
  struct list_head* prev;
  struct list_head* next;
  void* value;
};

/* Insert node in-between left and right nodes */
void list_insert (struct list_head** left, struct list_head** right,
		  struct list_head** node);

/* Append node to end of the list. */
void list_append (struct list_head** front,
		  struct list_head* node);

/* Remove node from the list. */
void list_remove (struct list_head** front,
		  struct list_head* node);

/* Concatenate the 'right' list onto the end of the 'left' list.  */
void list_concat (struct list_head** left, struct list_head** right);

/* Call function on each list_head->value in the list */
int list_for_all (struct list_head** front, void (*function)(void*));


#endif	/* LIST_H */
