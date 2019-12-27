#include "N2Allocator.h"

N2Allocator::N2Allocator(const N2AllocatorInit &init_data)
        : lists(index_to_size.size()) {
    std::vector<unsigned int> mem_sizes = {init_data.block_16,
                                           init_data.block_32,
                                           init_data.block_64,
                                           init_data.block_128,
                                           init_data.block_256,
                                           init_data.block_512,
                                           init_data.block_1024};
    unsigned int sum = 0;
    for (int i = 0; i < mem_sizes.size(); ++i) {
        sum += mem_sizes[i] * index_to_size[i];
    }
    data = (char *) malloc(sum);
    char *data_copy = data;
    for (int i = 0; i < mem_sizes.size(); ++i) {
        for (int j = 0; j < mem_sizes[i]; ++j) {
            lists[i].push_back(data_copy);
            *((int*)data_copy) = (int)index_to_size[i];
            data_copy += index_to_size[i];
        }
    }
    mem_size = sum;

}

N2Allocator::~N2Allocator() {
    free(data);
}

void *N2Allocator::allocate(size_t mem_size) {
    if (mem_size == 0) {
        return nullptr;
    }
    mem_size += sizeof(int);
    int index = -1;
    for (int i = 0; i < lists.size(); ++i) {
        if (index_to_size[i] >= mem_size && !lists[i].empty()) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        throw std::bad_alloc();
    }
    char *to_return = lists[index].front();
    lists[index].pop_front();
    return (void*)(to_return + sizeof(int));
}

void N2Allocator::deallocate(void *ptr) {
    char *c_ptr = (char *) (ptr);
    c_ptr = c_ptr - sizeof(int);
    int block_size = *((int*)c_ptr);
    int index = std::lower_bound(index_to_size.begin(), index_to_size.end(), block_size) - index_to_size.begin();
    if (index == index_to_size.size()) {
        throw std::logic_error("this pointer wasnt allocated by this allocator");
    }
    lists[index].push_back(c_ptr);
}

void N2Allocator::PrintStatus(std::ostream& os) const {
    int free_sum = 0;
    for (int i = 0; i < lists.size(); ++i) {
        os << "List with " << index_to_size[i] << " byte blocks, size: " << lists[i].size() << "\n";
        free_sum += lists[i].size() * index_to_size[i];
    }
    int occ_sum = mem_size - free_sum;

    os << "Occupied memory " << occ_sum << "\n";
    os << "Free memory " << free_sum << "\n\n";
}