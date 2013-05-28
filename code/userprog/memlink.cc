

/*
 header
prev|next|size|flag| size
____|____|____|____|________|
4     4    4    4  | size   |
--------------------
 */
///////////////////////////////////
MemLink::MemLink(char *pmemory){
  memory = pmemory;
  //memory = malloc(MEM_SIZE);
}
MemLink::~MemLink(){
  
  delete memList;
  memory = null;
}
void *MemLink::mem_malloc(int size)
{
  //malloc the memory block
}
void MemLink::mem_free(void *point) {
  //free the memory block
}
void MemLink::display() {
  //display the memory list.
}
/////////////////////////////////
