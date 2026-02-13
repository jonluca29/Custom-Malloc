# Custom-Malloc
A custom malloc implementation that utilizes mmap() for allocating, a free list for small deallocations and munmap() for large deallocations (>128 KB). This is an educational project to demonstrate and learn dynamic memory allocation.

# Features
- Hybrid allocation strategy
    - Small allocations are cached while large allocations (>128 KB) are directly allocated/deallocated with mmap()/munmap()
- Free list for memory reuse
