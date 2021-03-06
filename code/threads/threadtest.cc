// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include <unistd.h>
#include "synch.h"
//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------
void TS();
void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 2; num++) {
      printf("***%s thread %d priority %d looped %d times\n",currentThread->getName(), which,currentThread->getPriority(), num);
      //TS();
	currentThread->Yield();
    }
}
//enum ThreadStatus { JUST_CREATED, RUNNING, READY, BLOCKED };
char *status(ThreadStatus s){
  if (s == JUST_CREATED) return "JUST_CREATED";
  if (s == RUNNING) return "RUNNING";
  if (s == READY) return "READY";
  if (s == BLOCKED) return "BLOCKED";
}

void TS() {
  ListElement *lnode;
  Thread *t;
  lnode = threadpooler->getThreadList()->getfirst();
  fprintf(stdout,"-----------threadinfo-----------------\n");
    fprintf(stdout,"tid  tname  uid   prid status\n");
  while(lnode!=NULL) {
    t = (Thread *)lnode->item;
    fprintf(stdout,"%d   %s     %d    %d   %s\n",t->getThreadid(),t->getName(),
	    t->getUserid(), t->getPriority(),status(t->getStatus()));
    lnode = lnode->next;
  }
  fprintf(stdout,"----------------------------------------\n");
}
//
//
//
#define N 10

int count =0;
//we define the List to store the item.
List *l = new List;

//we define a lock,then we define a condition.
//Condition *cond_full = new Condition("full");
//Condition *cond_empty = new Condition("empty");
Lock *lock_full = new Lock("full");
Lock *lock_empty = new Lock("empty");

Condition *cond = new Condition("condiotioln");

//we define the producer function
void producer(int a) {
  printf("enter producer\n");
  char *name;
  Lock *lock_tmp = new Lock("producer_lock");
  while(TRUE) {
    lock_tmp->Acquire();
    name = new char[30];
    sprintf(name,"helloworld count %d",count);
    if (count == N){
      cond->Wait(lock_full);
    }
    l->Append((void *)name);
    count = count + 1;
    if (count == 1) {
      cond->Signal(lock_empty);
    }
    lock_tmp->Release();
  }
}

//we define the consumer function
void consumer(int a) {
  printf("enter consumer\n");
  void *item;
  Lock *lock_tmp = new Lock("consumer_lock");
  while (TRUE) {
    lock_tmp->Acquire();
    if (count == 0){
      cond->Wait(lock_empty);
    }
    item = l->Remove();
    count = count -1;
    if (count == N-1) {
      cond->Signal(lock_full);
    }
    delete item;
    lock_tmp->Release();
  }
}

// we need to define three semaphore .
Semaphore *sem_mutex = new Semaphore("mutex",1);
Semaphore *sem_full = new Semaphore("full",0);
Semaphore *sem_empty = new Semaphore("empty",N);

//we define the producer function
//the second method to implement the producer.
void producer2(int a) {
  char *name;
  while(TRUE) {
    name = new char[30];
    sprintf(name,"helloworld count %d",count);
    sem_empty->P();
    sem_mutex->P();
    l->Append((void *)name);
    count = count + 1;
    sem_mutex->V();
    sem_full->V();
  }
}
//the second method to implement the consumer
void consumer2(int a) {
  void *item;
  while (TRUE) {
    sem_full->P();
    sem_mutex->P();
    item = l->Remove();
    count = count -1;
    sem_mutex->V();
    sem_empty->V();
    delete item;
  }
}


//the first method to slove the consumer_producer problem.
void ThreadTestCP1() {
  //first we create two thread,with the same priority.
  Thread *p = new Thread("producer");
  if (p->getThreadid() >0) {
    p->Fork(producer,1);
  } else {
    delete p;
    printf("Create thread producer error\n");
  }
  Thread *c = new Thread("consumer");
  if (c->getThreadid()>0) {
    c->Fork(consumer,1);
  } else {
    delete c;
    printf("Create thread consumer error\n");
  }
  //init the cond.
  
}

//the second method to slove the consumer_producer problem.
/*
void ThreadTestCP2() {

}
*/
/*
Condition *barrier_cond;
Lock *half_lock;

void barrier_test(int a) {
  int i ;
  for (i =0; i < 100 ; i ++) {
    printf("barrier_test--currentThread id: %d\n",currentThread->getThreadid());
    if (i ==49) {
      barrier_cond->Wait(half_lock);
    }
    //we check if the number in the cond queue is 4, we broadcast all.
    if (barrier_cond->getQueue->getCount() == 4) {
      barrier_cond->Broadcast(half_lock);
    }
  }
}
*/
/*
void Barrier() {
  Thread *t1 = new Thread("t1");
  if (t1->getThreadid()>0) {
    t1->Fork(barrier_test,1);
  }
  Thread *t2 = new Thread("t2");
  if (t2->getThreadid()>0) {
    t2->Fork(barrier_test,1);
  }
  Thread *t3 = new Thread("t3");
  if (t3->getThreadid()>0) {
    t3->Fork(barrier_test,1);
  }
  Thread *t4 = new Thread("t4");
  if (t4->getThreadid()>0) {
    t4->Fork(barrier_test,1);
  }
  barrier_cond = new Condition("Barrier_cond");
  half_lock = new Lock("half_lock");

}
*/


void ThreadTest() {
  //ThreadTestPriority();
  ThreadTestCP1();
  //ThreadTestCP2();
  //Barrier();
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------
void ThreadTestPriority() {
  DEBUG('t',"We create 128 thread\n");
  int i =0;
  Thread *t1,*t2,*t3;
  t1 = new Thread("fork1");
  if (t1->getThreadid()>0) {
    t1->setPriority(5);
    t1->Fork(SimpleThread,t1->getThreadid());
  }
  t2 = new Thread("fork2");
  if (t2->getThreadid()>0) {
    t2->setPriority(8);
    t2->Fork(SimpleThread,t2->getThreadid());
  }
  t3 = new Thread("fork3");
  if (t3->getThreadid()>0) {
    t3->setPriority(2);
    t3->Fork(SimpleThread,t3->getThreadid());
  }
  
  /*
  for (i =1; i < 5; i ++) {
    t = new Thread("forked thread");
    if (t->getThreadid()>0) {
      //we set 
      t->Fork(SimpleThread,t->getThreadid());
    } else break;
  }
  */
}
/*
void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}
*/
void ThreadTest1() 
{
    //
}
