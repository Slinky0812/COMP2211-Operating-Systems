#include "memory_management.h"

//define global header
static struct header* head = 0;

void* expand() {
	//create new block
	struct header* memory = 0;
	
	//expanding memory using sbrk
	memory = (struct header*) sbrk(4 * PAGE_SIZE);
	//sbrk error check
	if (memory == (void *) -1) {
		printf("sbrk failed\n");
		return 0;

	}
	//set variables - same as how we do it for the header
	memory->free = 1;
	memory->blockSize = (4 * PAGE_SIZE) - sizeof(struct header);
	memory->next = 0;

	//find tail of linked list, and make it point to this
	struct header* temp = head;
	while (temp->next != 0) {
		temp = temp->next;
	}
	temp->next = memory;

	//return pointer to newly allocated block
	return memory;
}

void* partition(struct header* freeBlock, int size) {
	//create block 
	struct header* split;

	//void* to add only 1 byte rather than size of the whole struct
	//changing it's place in memory
	split = (struct header*) ( (void*) freeBlock + sizeof(struct header) + size);

	//setting the size of split to be the remainder of the size we want
	split->blockSize = freeBlock->blockSize - (sizeof(struct header) + size);

	//split block is now free, and points to where freeBlock used to point to
	split->next = freeBlock->next;
	split->free = 1;

	//setting the size of the freeBlock and making it point to the split bloc
	freeBlock->blockSize = size;
	freeBlock->next = split;

	//return pointer to freeBlock
	return freeBlock;
}

void* _malloc (int size) {
	//check if the size given by the user is appropriate
	if (size <= 0) {
		printf("error, too little memory to be allocated\n");
		return 0;
	}

	//initialising head block
	if (head == 0) { //same as head == NULL
		//expanding memory using sbrk()
		head = (struct header*) sbrk(PAGE_SIZE);
		//check if sbrk call was invalid
		if (head == (void *) -1) {
			printf("sbrk failed\n");
			return 0;
		}

		//set this block to free
		head->free = 1;
		//size of the block the header belongs to
		head->blockSize = PAGE_SIZE - sizeof(struct header);
		//set the next block to NULL
		head->next = 0;
	}

	//traverse through linked list for an empty block
	struct header* freeBlock = head;
	//loops while freeBlock is not null
	while (freeBlock != 0) {
		//check if the block we're looking at is free and if it can fit the size we want
		if (freeBlock->free == 1 && freeBlock->blockSize >= size + sizeof(struct header)) {
			break;
		}
		//go next
		freeBlock = freeBlock->next;
	}

	//not enough memory, call expand function to increase memory size.
	if (freeBlock == 0) {
		freeBlock = expand();
	}

	//if the block is too big for the size, then partition the space
	if (freeBlock->blockSize > size + sizeof(struct header)) {
		freeBlock = partition(freeBlock, size);
	}

	//set freeBlock to no longer be free
	freeBlock->free = 0;

	//return pointer to freeBlock
	//+1 to point to the block of space, not the header
	return freeBlock + 1;

}

void _free (void* ptr) {
	//checking for a null pointer
	if (!ptr) {
		printf("error, invalid pointer\n");
		return;
	}

	//checking if free() has already been called on this block
	if (((struct header*) ptr - 1)->free == 1) {
		printf("free has already been called\n");
		return;
    }

	//set the block to be free
	((struct header*) ptr - 1)->free = 1;

	//traverse through linked list to find free blocks and merge them
	struct header* temp = head;
	//while the current block is not null
	while (temp != 0) {
		//check if the current block is free, the next block is not null, and the next one is free
		if (temp->free == 1 && temp->next != 0 && temp->next->free == 1) {
			//set the size of the current block by adding the size of the next block and it's header
			temp->blockSize = temp->blockSize + temp->next->blockSize + sizeof(struct header);
			//make it point to the correct block
			temp->next = temp->next->next;
		//increment temp
		} else {
			temp = temp->next;
		}
	}

}