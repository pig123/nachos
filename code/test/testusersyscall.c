#include "syscall.h"

char buffer[10];

void  test() {
  int i ;
  for (i =0;i < 10;i ++) {
    buffer[i] = 'a'+i;
  }
}

char cmd[] = "testfile";

void main() {
  //test Exec
  Exec()
  //test Fork
  
  //test Yield
  
  //Join
  
  //Exit


}
