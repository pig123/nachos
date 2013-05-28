#include "syscall.h"

char filename[] = "helloworld.txt";
char buffer[30];

int main() {
  OpenFileId fp;
  int i;
  //we create a file with the filename
  Create(filename);
  //now we test the write function
  fp = Open(filename);
  //we put the string into the buffer.
  for (i =0;i < 10; i ++) {
    buffer[i] = 'a'+i;
  }
  Write(buffer,10,fp);
  Close(fp);
  //now we test the read function
  fp = Open(filename);
  //we clear the buffer
  for  (i =0;i < 10;i ++) {
    buffer[i] = 0;
  }
  Read(&buffer,10,fp);
  Close(fp);
}
