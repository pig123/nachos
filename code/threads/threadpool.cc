#include "threadpool.h"
#include "copyright.h"


ThreadPool::ThreadPool() {
  count =0;
  threadList = new List;
}
ThreadPool::~ThreadPool() {
  delete threadList;
}

Thread *ThreadPool::getThreadById(int id) {
  return (Thread *)threadList->getElementByKey(id);
}

Thread *ThreadPool::getThreadByName(char *name)
{
  //return (Thread *)threadList->getElementByName((void *)name);
  ListElement *tmp;
  Thread *t;
  int flag = 0;
  tmp = (ListElement *)threadList->getfirst();
  while (tmp!=NULL) {
    t = (Thread *)tmp->item;
    if (strcmp(name,t->getName()) == 0) {
      flag = 1;
      break;
    }
    else {
      tmp = tmp->next;
    }
  }
  if (flag == 1) return t;
  else return NULL;
}

int ThreadPool::getMax() {
  int max = 0;
  if (count > 0) {
    ListElement *tmp;
    tmp =(ListElement *)threadList->getfirst();
    if (tmp!= NULL) {
      max = tmp->key;
      while (tmp->next!=NULL) {
	tmp = tmp->next;
	if (max<= tmp->key) {
	  max = tmp->key;
	}
      }
    }
  }
  return max;
}
