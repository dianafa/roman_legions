#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>

struct elem
{
 int id;
 int clock;
 elem *next;
} KolejkaN[K+1], element; //kazdy trakt ma swoja kolejke

elem head = NULL;

int insert(elem legion)
{
  elem *tmp = new elem;
  tmp-> id = legion.id;
  tmp-> clock = legion.clock;
  
  if((head = NULL) || (legion.clock < head.clock)) //first element
  {
    tmp-> next = NULL;
    head = tmp;
  }
  else
  {
    elem *i = head;
    while(i-> next!=NULL && i->next->clock < legion.clock)
    {
      i = i-> next;
    }
    tmp-> next = i-> next;
    i -> next  = tmp;
  }
}

int remove(int id)
{
  elem *i = head;
    if(head->id == id)
    {
      delete head;
      head->id = i->next->id;
      head->clock = i->next->clock;
      head->next = i->next->next;
    }
  
    while(i-> next!=NULL && i->next->id != id)
    {
      i = i-> next;
    }
    
    if(i-> next == NULL) return -1; //if id not found
    else 
    {
      elem *t = i;
      delete i-> next;
      i -> next = t->next-> next;
      
    }
}

int sum_before(int id){
  elem *i = head;
  int sum=0;
  if(head->id == id)
  return 0;
  
    while(i != NULL && i-> id != id)
    {
      sum=sum + find_size(i->id)
      i = i-> next;
    }
    
    return sum; //if id not found function returns size of whole queue
}

int find_size(int legion_id, int legions[]) //returns size of legion with id=legion_id legions[]- tabela licznosci legionow
{
  int i=0;
  while(legions[i] != legion_id && legions[i]!= NULL) i++;
  if legions[i]= NULL return 0;
  else return legions[i];
}