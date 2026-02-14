#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

#define LARGE_BLOCK_THRESHOLD 128 * 1000 // 128 KB


// struct for metadata
// inUse flag: 0 for false 1 for true
typedef struct heapMeta
{
    size_t size;
    int inUse;
    struct heapMeta *next;
} heapInfo;

// used for tracking the head of the block linked list
static heapInfo *blockHead = NULL;

// used for tracking the head of the free linked list
static heapInfo *freeHead = NULL;


// ask OS for more memory
// return NULL for mmap failure
// other wise return memPtr
void *getMem(size_t size)
{
    // create the pointer of heapInfo type and call sbrk with requested size + size for metadata
    heapInfo *memPtr = mmap(NULL, size+sizeof(heapInfo), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (memPtr == MAP_FAILED)
    {
        perror("mmap failed???\n");
        return NULL;
    }


    // initialize metadata
    memPtr->size = size;
    memPtr->inUse = 1;
    memPtr->next = NULL;

    return memPtr;
}

// look for a free block of memory then return it
// return NULL if no free memory can be reused
// otherwise return ptr 
void *findFreeMem(size_t size)
{
    if (freeHead == NULL){
        return NULL;
    }
    else if (freeHead->size >= size){
        heapInfo *ptr = freeHead;
        freeHead = freeHead->next;
        ptr->next = NULL;
        return ptr;
    }

    heapInfo *ptr;
    int found = 0;
    heapInfo *current = freeHead;


    // traverse the free linked list to the node that is before the node that is at least as big as the needed size 
    while (current->next != NULL)
    {
        if (current->next->size >= size)
        {
            ptr = current->next; 
            found = 1;
            break; 
        }
        current = current->next;
    }
    

    // return NULL means no free memory found
    if (found)
    {   
        // remove the target node from the linked list and reconnected it to the node that comes after the target if it exists
        if (ptr->next == NULL){
            current->next = NULL;
            
        }
        else {
            current->next = ptr->next;
        }
        ptr->next = NULL;

        return ptr;
    }
    else
    {
        return NULL;
    }
}

// traverse to the end of the block list and link the new block of memory
void addToBlockList(heapInfo *newBlock)
{
    // if blockHead is NULL then that means an empty block list
    if (blockHead == NULL){
        blockHead = newBlock;
        return;
    }

    heapInfo *current = blockHead;


    while (current->next != NULL)
    {
        current = current->next;
    }

    current->next = newBlock;
}

// traverse to the end of the free list and link the freed block of memory 
// remove the block from the block list
void addToFreeList(heapInfo *freeBlock){
    heapInfo *current = freeHead;
    heapInfo *blockCurrent = blockHead;

    if (freeHead == NULL){
        freeHead = freeBlock;
    }
    else {
        heapInfo *current = freeHead;
        heapInfo *blockCurrent = blockHead;

        while (current->next != NULL){
            current = current->next;
        }

        current->next = freeBlock;
    }


    if (blockHead == freeBlock){
        blockHead = blockHead->next;
    }
    else{  
        // traverse the block list to find the target node
        while (blockCurrent->next != freeBlock){
            blockCurrent = blockCurrent->next;
        }
        blockCurrent->next = freeBlock->next; 
    } 
    
    freeBlock->next = NULL;
}


// if the block is less than 128 kb then mark it as free and add it to the free list
//other wise call munmap() on it
void freeMem(void *memBlock)
{
    heapInfo *header = (heapInfo *)memBlock - 1;

    printf("Freeing memory with address: %p\n", header);


    if (header->size < LARGE_BLOCK_THRESHOLD){
        header->inUse = 0;
        addToFreeList(header);
    }
    else {
        size_t len = header->size + sizeof(heapInfo);
        int fail = munmap(header, len);
        if (fail == -1)
        {
            perror("failure with munmap()\n");
        }
    }
}

// The malloc function that gets called by the user.
void *myMalloc(size_t size)
{
    printf("myMalloc with size: %zu\n", size);
    heapInfo *memoryBlock = NULL; // the pointer to be returned

    if (size >= LARGE_BLOCK_THRESHOLD)
    {
        memoryBlock = getMem(size);
        return (memoryBlock + 1);
    }
    
    // if there is nodes in the free list then look through the free list for the right node
    // other wise allocate memory.
    // If there is no node in the free list of appropriate size then allocate
    if (freeHead == NULL)
    {
        memoryBlock = getMem(size);
        addToBlockList(memoryBlock);
        return (memoryBlock + 1);
    }
    else
    {   
        memoryBlock = findFreeMem(size);
        if (memoryBlock){
            memoryBlock->inUse = 1;
            memoryBlock->size = size;
            addToBlockList(memoryBlock);
        }
        else {
            memoryBlock = getMem(size);
            addToBlockList(memoryBlock);
        }

        return memoryBlock+1;
    }
}

int main()
{
    printf("=== Test 1: Basic allocation ===\n");
    int *ptr1 = myMalloc(sizeof(int));
    *ptr1 = 42;
    printf("Value: %d\n", *ptr1);
    freeMem(ptr1);
    
    printf("\n=== Test 2: Reuse ===\n");
    int *ptr2 = myMalloc(sizeof(int));
    *ptr2 = 100;
    printf("Value: %d\n", *ptr2);
    
    printf("\n=== Test 3: Multiple blocks ===\n");
    int *ptr3 = myMalloc(100);
    int *ptr4 = myMalloc(200);
    int *ptr5 = myMalloc(50);
    
    freeMem(ptr2);
    freeMem(ptr3);
    freeMem(ptr4);
    freeMem(ptr5);
    
    printf("\n=== Test 4: Large block ===\n");
    char *large = myMalloc(200000);  // > 128KB
    freeMem(large);
    
    printf("\nAll tests completed!\n");
    return 0;
}
