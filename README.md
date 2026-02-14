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

# 
