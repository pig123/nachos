

//we use this class to record the memory,
//to manager the free or malloc function.

//first we new a big memory to the list.
//we store the pointer to the freelist.
//

#define MEM_SIZE 4096 //4k

typedef struct _MEM_Block{
  void *prev;//point to the prev mem block
  void *next;//point to the next mem block
  unsigned int size;//the size of the mem
  unsigned int flag;//we to define use or not.
  void *memory;
}mem_block,pmem_block;

class MemLink {
  MemLink(char *pmemory);
  ~MemLink();
  //there are the free memory block in the freeList
  List *freeList;
  void *memory;
 public:
  // to malloc a memory from the memory manager
  void *mem_malloc(int size);
  // to free the memory from the memory manager
  void mem_free(void *point);
  // to display all the memory block
  void display();
}
