/* File: list.c
 *
 * A doubly linked-list implementation of a circular buffer.
 */

#include "list.h"

/* insert input node between left and right input nodes
 * should check for NULL
 * assumes a proper set of left/right pointers are passed
 */
void list_insert (struct list_head** left, struct list_head** right,
		  struct list_head** node)
{
  (*left)->next = *node;
  (*right)->prev = *node;  
  (*node)->prev = *left;
  (*node)->next = *right;
}

/* insert input node to the left of the list head
 * or node is set as front if list is empty
 */
void list_append (struct list_head** front,
		  struct list_head* node)
{
  if (front && node)
    {
      if (*front)
	{
	  if (LIST_IS_ONLY_NODE(front))
	    {
	      (*front)->next = node;
	      node->prev = *front;
	    }
	  else
	    {
	      (*front)->prev->next = node;
	      node->prev = (*front)->prev;
	    }

	  node->next = *front;
	  (*front)->prev = node;
	}
      else
	{
	  *front = node;
	  node->prev = node;
	  node->next = node;
	}
    }
}

/* left is modified to be the concatenated list
 * right is not guaranteed to point to any part of the final list
 */
void list_concat (struct list_head** left, struct list_head** right)
{
  if (left && right)
    {
      if (*left == NULL)
	{
	  *left = *right;
	}
      else if (*right)
	{
	  (*right)->prev->next = *left;
	  (*left)->prev->next = (*right)->prev;
	  (*right)->prev = (*left)->prev;
	  (*left)->prev = (*left)->prev->next;
	  (*right)->prev->next = *right;
	}
    }
}

void list_remove (struct list_head** front,
		  struct list_head* node)
{
  if (front && *front && node)
    {
      if (!LIST_IS_ONLY_NODE(front))
	{
	  node->prev->next = node->next;
	  node->next->prev = node->prev;

	  /* node->next, node->prev are now undefined */
	}
      else
	*front = NULL;
    }
}

/* Input function operates on the value of every node in the list */
int list_for_all (struct list_head** front, void (*function)(void*))
{
  struct list_head* iter;

  if (front && *front && function)
    {
      iter = (*front)->next;
      function((*front)->value);

      while (iter != *front)
	{
	  function(iter->value);
	  iter = iter->next;
	}
    }
}
