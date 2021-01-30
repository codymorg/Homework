/******************************************************************************/
/*!
\file   cachelist.c
\author Cody Morgan
\par    email: cody.morgan\@digipen.edu
\par    DigiPen login: cody.morgan
\par    Course: CS180
\par    Section A
\par    Assignment #1
\date   19 SEP 2018
\brief  
  this is a single linked list of nodes containing a value and a label.

  operations include:
  - cl_add_end       : add item to the end 
  - cl_add_front     : add item to the front
  - cl_remove        : remove a give value from the list
  - cl_insert_before : insert a new value before this value
  - cl_insert_after  : insert a new value after this value
  - cl_find          : find this value
  - cl_destroy       : destroy this value (first)
  - cl_dump          : print this list
*/
/******************************************************************************/

#include <stdio.h>     /* printf          */
#include <stdlib.h>    /* malloc, exit    */
#include <string.h>    /* strncpy         */
#include "cachelist.h" /* cachelist stuff */

/*!****************************************************************************
  Makes a new node for a single linked list
            
  \param value
    the new node's new value

  \param label
    the new node's new label

  \return 
    the head of the list
******************************************************************************/
static cl_node *make_node(int value, const char *label)
{
  cl_node *node = (cl_node *)malloc(sizeof(cl_node)); /*! a new list node */

  if (!node)
  {
    printf("Can't allocate new node.\n");
    exit(1);
  }

  node->value = value;
  node->next = NULL;

    /* Be sure not to overwrite memory */
  strncpy(node->label, label, LABEL_SIZE - 1);
  node->label[LABEL_SIZE - 1] = 0;
  
  return node; /* the new node */
}

/*!****************************************************************************
  Dumps all of the list info to the screen for your viewing pleasure
            
  \param list
    the list to dump
******************************************************************************/
void cl_dump(const cl_node *list)
{
  printf("==================\n");
  while (list)
  {
    printf("%4i: %s\n", list->value, list->label);
    list = list->next;    
  }
}

/*!****************************************************************************
  Adds a new node to the end of the list
            
  \param list
    the list on to which you want to add

  \param value
    the new value for that node

  \param label
    the new label for that node 
  
  \return 
    head of the list
******************************************************************************/
cl_node *cl_add_end(cl_node *list, int value, const char *label)
{
  cl_node* newNode = make_node(value, label); /*! a new list node */
  cl_node* endOfList = list;                  /*! the end of the new list */

  /* find the end of the list */
  if(list == NULL)
  {
    list = newNode;
  }
  else
  {
    while(endOfList->next != NULL)
    {
      endOfList = endOfList->next;
    }
    endOfList->next = newNode;
  }

  return list; /* the head of the list */
}

/*!****************************************************************************
  Adds a new node to the front of the list

  \param list
    the list on to which you want to add

  \param value
    the new value for that node

  \param label
    the new label for that node 

  \return 
    head of the list
******************************************************************************/
cl_node *cl_add_front(cl_node *list, int value, const char *label)
{
  cl_node* newNode = make_node(value, label); /*! the new list node */

  /* connect the old list */
  newNode->next = list;

  return newNode; /* the head of the new list */
}

/*!****************************************************************************
  Removes the first node with a matching value

  \param list
    the list in which you are searching

  \param search_value
    the sought after value

  \return 
    head of the list
******************************************************************************/
cl_node *cl_remove(cl_node *list, int search_value)
{
  cl_node* node = list;     /*! remove this node */
  cl_node* previous = list; /*! node prior to the one marked for deleting */

  /* look for the specified value */
  while(node != NULL)
  {
    /* found value, exterminate! */
    if(node->value == search_value)
    {
      /* if we're dealing with the head of the list */
      if(node == previous)
      {
        list = list->next;
        free(node);
      }
      /* all other members of the list */
      else
      {
        previous->next = node->next;
        free(node);
      }
        return list;
    }
    /* no value found, keep moving */
    else
    {
      previous = node;
      node = node->next;
    }
  }

  return list; /* the head of the list */
}

/*!****************************************************************************
  Insert a new node before a given value
            
  \param list
    the list on to which you want to add

  \param search_value
    the value we're looking for

  \param value
    the new value for that node

  \param label
    the new label for that node

  \return 
    head of the list
******************************************************************************/
cl_node *cl_insert_before(cl_node *list, int search_value, int value, 
                          const char *label)
{
  cl_node* node = list;     /*! contains the sought after value */
  cl_node* previous = list; /*! the node prior to the found value */
  cl_node* head = list;     /*! the head of the list */
  cl_node* newNode = NULL;  /*! new node to insert into the list */

  /* look for the specified value */
  while(node != NULL)
  {
    if(node->value == search_value)
    {
      break;
    }
    else
    {
      previous = node;
      node = node->next;
    }
  }

  /* if nothing is found insert nothing */
  if(node == NULL)
  {
    return list;
  }

  /* only make a node if we're going to insert it */
  newNode = make_node(value, label);
  
  /* this only happens with 2+ nodes */
  if(previous != node)
  {
    previous->next = newNode;
  }

  /* reset the head if we're inserting at the beginning */
  else
  {
    head = newNode;
  }
  newNode->next = node;

  return head; /* the head of the list */
}

/*!****************************************************************************
  Insert a node after the sought after value

  \param list        
    the list on to which you want to add

  \param search_value
    the value we're looking for

  \param value       
    the new value for that node

  \param label       
    the new label for that node
******************************************************************************/
void cl_insert_after(cl_node *list, int search_value, int value, 
                     const char *label)
{
  cl_node* node = list; /*! contains the sought after value */

  /* look for the value */
  while(node != NULL)
  {
    if(node->value == search_value)
    {
      break;
    }
    else
    {
      node = node->next;
    }
  }

  /* the value was found */
  if(node != NULL)
  {
    cl_node* newNode = make_node(value, label);
    newNode->next = node->next;
    node->next = newNode;
  }

}

/*!****************************************************************************
  Find a given value, 
    count the number of compares it took to find,
    and finally cache that value if enabled

  \param list       
    the linked list

  \param search_value
    the sought after value

  \param cache
    bool to determine if the found value will be cached or not

  \param compares    
    how many compares it took to find that value

  \return 
    the head of the list
******************************************************************************/
cl_node *cl_find(cl_node *list, int search_value, int cache, int *compares)
{
  cl_node* node = list;      /*! current node while walking the list */
  cl_node* head = list;      /*! head of the list */ 
  cl_node* foundNode = NULL; /*! contains the sought after value */ 
  cl_node* previous = list;  /*! node previous to the found node */ 
  int compares_ = 0;         /*! number of compares it took to find the node */

  /* look for a matching value */
  while(node != NULL)
  {
    compares_++;
    if(node->value == search_value)
    {
      foundNode = node;
      break;
    }
    else
    {
      previous = node;
      node = node->next;
    }
  }

  /* found a matching value */
  if(foundNode != NULL)
  {
    *compares = compares_;

    /* cache the node if enabled
     * ensure that the found node isn't the head already */
    if(cache && foundNode != list)
    {
      cl_node* tempNode = foundNode->next;
      foundNode->next = list;
      previous->next = tempNode;
      head = foundNode;
    }
  }

  return head; /*! the head of the list */
}

/*!****************************************************************************
  Frees all the memory used for the given list
            
  \param list
    the list you want to set free
******************************************************************************/
void cl_destroy(cl_node *list)
{
  cl_node* nextNode; /* keep destroying until this is NULL */

  while(list != NULL)
  {
    nextNode = list->next;
    free(list);
    list = nextNode;
  }
}
