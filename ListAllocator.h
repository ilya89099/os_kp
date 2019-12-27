#pragma once

#include <iostream>
#include <algorithm>
#include <list>

enum class MemoryNodeType {
    Hole,
    Occupied
};

struct MemoryNode {
    char* beginning;
    size_t capacity;
    MemoryNodeType type;
};

std::ostream& operator << (std::ostream& os, const MemoryNode& node);
class ListAllocator {
public:
    explicit ListAllocator(size_t data_size);

    ~ListAllocator();

    void* allocate(size_t mem_size);


    void deallocate(void* ptr);

    void PrintStatus(std::ostream& os) const;

private:

    std::list<MemoryNode> mem_list;
    char* data;
};