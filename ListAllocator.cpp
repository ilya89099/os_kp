#include "ListAllocator.h"


std::ostream& operator << (std::ostream& os, const MemoryNode& node) {
    return os << "Node: capacity " << node.capacity << ", type " << (node.type == MemoryNodeType::Hole ? "Hole" : "Occupied");
}

ListAllocator::ListAllocator(size_t data_size) {
    data = (char *) malloc(data_size);
    mem_list.push_front({data, data_size, MemoryNodeType::Hole});
}

ListAllocator::~ListAllocator() {
    free(data);
}

void *ListAllocator::allocate(size_t mem_size) {
    if (mem_size == 0) {
        return nullptr;
    }
    size_t size_of_node = 0;
    auto needed_node = mem_list.end();
    for (auto it = mem_list.begin(); it != mem_list.end(); ++it) {
        if (it->type == MemoryNodeType::Hole && it->capacity >= mem_size &&
            (size_of_node == 0 || it->capacity < size_of_node)) {
            size_of_node = it->capacity;
            needed_node = it;
        }
    }
    if (size_of_node == 0) {
        throw std::bad_alloc();
    }
    if (mem_size == size_of_node) {
        needed_node->type = MemoryNodeType::Occupied;
    } else {
        MemoryNode new_node{needed_node->beginning + mem_size,
                            needed_node->capacity - mem_size,
                            MemoryNodeType::Hole};
        needed_node->capacity = mem_size;
        needed_node->type = MemoryNodeType::Occupied;
        mem_list.insert(std::next(needed_node), new_node);
    }
    return (void *) (needed_node->beginning);
}

void ListAllocator::deallocate(void *ptr) {
    auto it = std::find_if(mem_list.begin(), mem_list.end(), [ptr](const MemoryNode &node) {
        return node.beginning == (char *) ptr && node.type == MemoryNodeType::Occupied;
    });
    if (it == mem_list.end()) {
        throw std::logic_error("This pointer wasnt allocated by this allocator");
    }
    it->type = MemoryNodeType::Hole;
    if (it != mem_list.begin() && std::prev(it)->type == MemoryNodeType::Hole) {
        auto prev_it = std::prev(it);
        prev_it->capacity += it->capacity;
        mem_list.erase(it);
        it = prev_it;
    }
    if (std::next(it) != mem_list.end() && std::next(it)->type == MemoryNodeType::Hole) {
        auto next_it = std::next(it);
        it->capacity += next_it->capacity;
        mem_list.erase(next_it);
    }
}

void ListAllocator::PrintStatus(std::ostream& os) const {
    int occ_sum = 0;
    int free_sum = 0;
    for (const auto& elem : mem_list) {
        os << elem << "\n";
        if (elem.type == MemoryNodeType::Hole) {
            free_sum += elem.capacity;
        } else {
            occ_sum += elem.capacity;
        }
    }
    os << "Occupied memory " << occ_sum << "\n";
    os << "Free memory " << free_sum << "\n\n";

}