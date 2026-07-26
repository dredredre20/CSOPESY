// test_demand_paging.cpp -- temporary, standalone, NOT part of your real allocator logic
#include "DemandPagingAllocator.hpp"
#include "../Process/Process.hpp"
#include <iostream>

int main() {
    DemandPagingAllocator allocator(256, 16); // 256 bytes total, 16 bytes/frame -> 16 frames

    auto process1 = std::make_shared<Process>(1, "testProcess1");
    process1->setMemoryRequirement(64); // needs 4 pages (64/16)

    void* handle = allocator.allocate(process1);
    std::cout << allocator.visualizeMemory() << "\n";

    // Manually simulate a memory reference to page 0 -- this is what
    // a READ/WRITE instruction would eventually trigger automatically.
    allocator.accessPage(handle, 0);
    std::cout << "After accessing page 0:\n" << allocator.visualizeMemory() << "\n";

    allocator.accessPage(handle, 1);
    std::cout << "After accessing page 1:\n" << allocator.visualizeMemory() << "\n";

    std::cout << "Paged in: " << allocator.getNumPagedIn()
               << ", Paged out: " << allocator.getNumPagedOut() << "\n";

    return 0;
}