#include "kernel/types.h"
#include "user/user.h"

//defining a macro equal to one page size
#define PAGE_SIZE 4096

//struct to contain variables needed for a block of memory
//works as a linked list
struct header {
	uint8 free; //shows whether the block is free or already contains memory. 1 is free, 0 is not
	int blockSize; //how much memory is stored in the block
	struct header* next; //points to the next block in memory
};

//function prototypes
void* expand();
void* partition(struct header* freeBlock, int size);
void* _malloc (int size);
void _free (void* ptr);