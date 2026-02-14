# Custom-Malloc
A custom malloc implementation that utilizes mmap() for allocating, a free list for small deallocations and munmap() for large deallocations (>128 KB). This is an educational project to demonstrate and learn dynamic memory allocation.

# Features
- Hybrid allocation strategy
    - Small allocations are cached while large allocations (>128 KB) are directly allocated/deallocated with mmap()/munmap()
- Free list for memory reuse

# What I Learned
- System calls: How mmap() and munmap() interact with the OS to request and return memory
- Memory layout: Understanding headers, metadata, and user data regions in allocated blocks
- Pointer Arithmetic: How adding 1 to a struct pointer moves by sizeof(struct), not 1 byte
- Address space: Each process has its own isolated memory space protected by the OS
- Linked lists in practice: Implementing and managing two separate linked lists (block list and free list)
- Edge cases: Handling special cases like removing the head node, empty lists, and single-node lists
- Memory reuse: How free lists enable efficient memory recycling without repeated OS calls
  
# What I Struggled With
- Linked List Edge Cases
    - The problem: My removal logic crashed when trying to remove the head node because I was looking for a "node before" that didn't exist
    - The solution: Added special case handling: if (head == target) before the general traversal logic
    - Lesson learned: Always consider edge cases - empty lists, single nodes, and head/tail operations
- NULL Pointer Dereferences
    - The problem: Checking current->next->size before verifying current->next != NULL caused segmentation faults
    - The solution: Always check if a pointer is NULL before dereferencing it: if (current->next != NULL && current->next->size >= size)
    - Why it matters: Dereferencing NULL is undefined behaviour and crashes programs
- Infinite loops
    - The problem: Forgot to add an else block in addToFreeList, causing freeBlock->next = freeBlock (self-referencing loop)
    - The solution: Proper control flow with if/else to ensure code paths don't overlap
    - Debugging approach: Added debug prints to trace execution flow and identify where the loop occurred
