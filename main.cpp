#include <iostream>
#include <chrono>

#include "N2Allocator.h"
#include "ListAllocator.h"

int main() {
    using namespace std::chrono;
    {
        steady_clock::time_point list_allocator_init_start = steady_clock::now();
        ListAllocator list_allocator(4096);
        steady_clock::time_point list_allocator_init_end = steady_clock::now();
        std::cerr << "List allocator initialization with one page of memory :"
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(
                          list_allocator_init_end - list_allocator_init_start).count()
                  << " ns" << std::endl;


        steady_clock::time_point n2_allocator_init_start = steady_clock::now();
        N2Allocator n2_allocator(
                {.block_16 = 64, .block_32 = 32, .block_64 = 16, .block_128 = 4, .block_256 = 2}); // 1 страница
        steady_clock::time_point n2_allocator_init_end = steady_clock::now();
        std::cerr << "N2 allocator initialization with one page of memory :"
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(
                          n2_allocator_init_end - n2_allocator_init_start).count()
                  << " ns" << std::endl;

        std::cerr << "\n";

    }
    std::cerr << "First test: Allocate 10 char[256] arrays, free 5 of them, allocate 10 char[128] arrays:\n";
    {
        std::vector<char *> pointers(15, 0);
        N2Allocator allocator(
                {.block_16 = 0, .block_32 = 0,.block_64 = 20, .block_128 = 20, .block_256 = 20, .block_512 = 10});
        steady_clock::time_point n2_test1_start = steady_clock::now();
        for (int i = 0; i < 10; ++i) {
            pointers[i] = (char *) allocator.allocate(256);
        }
        for (int i = 5; i < 10; ++i) {
            allocator.deallocate(pointers[i]);
        }
        for (int i = 5; i < 15; ++i) {
            pointers[i] = (char *) allocator.allocate(128);
        }
        steady_clock::time_point n2_test1_end = steady_clock::now();
        std::cerr << "N2 allocator first test:"
                  << std::chrono::duration_cast<std::chrono::microseconds>(n2_test1_end - n2_test1_start).count()
                  << " microseconds" << std::endl;
        allocator.PrintStatus(std::cerr);
        for (int i = 0; i < 15; ++i) {
            allocator.deallocate(pointers[i]);
        }
    }
    {
        ListAllocator allocator(4096);
        std::vector<char *> pointers(1000, 0);
        steady_clock::time_point test1_start = steady_clock::now();
        for (int i = 0; i < 10; ++i) {
            pointers[i] = (char *) allocator.allocate(256);
        }
        for (int i = 5; i < 10; ++i) {
            allocator.deallocate(pointers[i]);
        }
        for (int i = 5; i < 15; ++i) {
            pointers[i] = (char *) allocator.allocate(128);
        }
        steady_clock::time_point test1_end = steady_clock::now();
        std::cerr << "List allocator first test:"
                  << std::chrono::duration_cast<std::chrono::microseconds>(test1_end - test1_start).count()
                  << " microseconds" << std::endl;
        allocator.PrintStatus(std::cerr);
        for (int i = 0; i < 15; ++i) {
            allocator.deallocate(pointers[i]);
        }
    }
    std::cerr << "Second test: Allocate and free 750 20 bytes arrays:\n";
    {
        N2Allocator allocator({.block_16 = 0, .block_32 = 400, .block_64 = 400});
        std::vector<char *> pointers(750, 0);
        steady_clock::time_point alloc_start = steady_clock::now();
        for (int i = 0; i < 750; ++i) {
            pointers[i] = (char *) allocator.allocate(20);
        }
        steady_clock::time_point alloc_end = steady_clock::now();
        for (int i = 0; i < 750; ++i) {
            allocator.deallocate(pointers[i]);
        }
        steady_clock::time_point test_end = steady_clock::now();
        std::cerr << "N2 allocator second test:\n"
                  << "Allocation :" << duration_cast<std::chrono::microseconds>(alloc_end - alloc_start).count() << " microseconds" << "\n"
                  << "Deallocation :" << duration_cast<std::chrono::microseconds>(test_end - alloc_end).count() << " microseconds" << "\n";
    }
    {
        ListAllocator allocator(16000);
        std::vector<char *> pointers(750, 0);
        steady_clock::time_point alloc_start = steady_clock::now();
        for (int i = 0; i < 750; ++i) {
            pointers[i] = (char *) allocator.allocate(20);
        }
        steady_clock::time_point alloc_end = steady_clock::now();
        for (int i = 0; i < 750; ++i) {
            allocator.deallocate(pointers[i]);
        }
        steady_clock::time_point test_end = steady_clock::now();
        std::cerr << "List allocator second test:\n"
                << "Allocation :" << duration_cast<std::chrono::microseconds>(alloc_end - alloc_start).count() << " microseconds" << "\n"
                << "Deallocation :" << duration_cast<std::chrono::microseconds>(test_end - alloc_end).count() << " microseconds" << "\n";
    }
    std::cerr << "Third test: Allocate 500 20 bytes arrays, deallocate every second, allocate 250 12 bytes :\n";
    {
        N2Allocator allocator({.block_16 = 400, .block_32 = 700});
        std::vector<char *> pointers(750, 0);
        steady_clock::time_point test_start = steady_clock::now();
        for (int i = 0; i < 500; ++i) {
            pointers[i] = (char *) allocator.allocate(20);
        }
        for (int i = 0; i < 250; ++i) {
            allocator.deallocate(pointers[i * 2]);
        }
        for (int i = 500; i < 750; ++i) {
            pointers[i] = (char*) allocator.allocate(12);
        }
        steady_clock::time_point test_end = steady_clock::now();
        std::cerr << "N2 allocator third test:"
                  << std::chrono::duration_cast<std::chrono::microseconds>(test_end - test_start).count()
                  << " microseconds" << std::endl;
        allocator.PrintStatus(std::cerr);
        for (int i = 0; i < 250; ++i) {
            allocator.deallocate(pointers[i * 2 + 1]);
        }
        for (int i = 500; i < 750; ++i) {
            allocator.deallocate(pointers[i]);
        }
    }
    {
        ListAllocator allocator(16000);
        std::vector<char *> pointers(750, 0);
        steady_clock::time_point test_start = steady_clock::now();
        for (int i = 0; i < 500; ++i) {
            pointers[i] = (char *) allocator.allocate(20);
        }
        for (int i = 0; i < 250; ++i) {
            allocator.deallocate(pointers[i * 2]);
        }
        for (int i = 500; i < 750; ++i) {
            pointers[i] = (char*) allocator.allocate(12);
        }
        steady_clock::time_point test_end = steady_clock::now();
        std::cerr << "List allocator third test:"
                  << std::chrono::duration_cast<std::chrono::microseconds>(test_end - test_start).count()
                  << " microseconds" << std::endl;
        allocator.PrintStatus(std::cerr);
        for (int i = 0; i < 250; ++i) {
            allocator.deallocate(pointers[i * 2 + 1]);
        }
        for (int i = 500; i < 750; ++i) {
            allocator.deallocate(pointers[i]);
        }
    }
    std::cerr << "Fourth test: Allocate and free 1500 20 bytes arrays:\n";
    {
        N2Allocator allocator({.block_16 = 0, .block_32 = 800, .block_64 = 800});
        std::vector<char *> pointers(1500, 0);
        steady_clock::time_point alloc_start = steady_clock::now();
        for (int i = 0; i < 1500; ++i) {
            pointers[i] = (char *) allocator.allocate(20);
        }
        steady_clock::time_point alloc_end = steady_clock::now();
        for (int i = 0; i < 1500; ++i) {
            allocator.deallocate(pointers[i]);
        }
        steady_clock::time_point test_end = steady_clock::now();
        std::cerr << "N2 allocator fourth test:\n"
                  << "Allocation :" << duration_cast<std::chrono::microseconds>(alloc_end - alloc_start).count() << " microseconds" << "\n"
                  << "Deallocation :" << duration_cast<std::chrono::microseconds>(test_end - alloc_end).count() << " microseconds" << "\n";
    }
    {
        ListAllocator allocator(32000);
        std::vector<char *> pointers(1500, 0);
        steady_clock::time_point alloc_start = steady_clock::now();
        for (int i = 0; i < 1500; ++i) {
            pointers[i] = (char *) allocator.allocate(20);
        }
        steady_clock::time_point alloc_end = steady_clock::now();
        for (int i = 0; i < 1500; ++i) {
            allocator.deallocate(pointers[i]);
        }
        steady_clock::time_point test_end = steady_clock::now();
        std::cerr << "List allocator fourth test:\n"
                  << "Allocation :" << duration_cast<std::chrono::microseconds>(alloc_end - alloc_start).count() << " microseconds" << "\n"
                  << "Deallocation :" << duration_cast<std::chrono::microseconds>(test_end - alloc_end).count() << " microseconds" << "\n";
    }

    return 0;
}
