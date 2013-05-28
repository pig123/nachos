
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "copyright.h"
#include "thread.h"
#include "list.h"

//we define the thread pool class,then to manager the all the public threads.
//it store all the threads we created.
class ThreadPool {
 public:
  ThreadPool();
  ~ThreadPool();
 private:
  int count; // the thread in the pool
  List *threadList;//the thread List

 public:
  int getCount(){ return count;}
  void setCound(int count_tmp){count = count_tmp;}
  int addCount(){count ++; if(count>128)return 0; else return 1;}
  int decCount(){count --; if (count <0) count =0;}
  int getMax();// we get the max id in the threadList.
  Thread *getThreadById(int id); //we use the key to store the id.
  Thread *getThreadByName(char *name);//we use the name to find the thread.
  List *getThreadList() {return threadList;}
};


#endif
