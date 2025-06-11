#include <iostream>
#include <list>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include "../include/vector.hpp"

struct Tracked
{
    int value;

    Tracked(int v = 0) : value(v)
    {
        std::cout << "Constructing " << this << "\n";
    }

    Tracked(const Tracked &other) : value(other.value)
    {
        std::cout << "Copy Constructing " << this << "\n";
    }

    Tracked(Tracked &&other) noexcept : value(other.value)
    {
        std::cout << "Move Constructing " << this << "\n";
    }

    ~Tracked()
    {
        std::cout << "Destructing " << this << "\n";
    }
};

void compareAddresses()
{
    std::cout << "\n--- Comparing Memory Layouts ---\n";
    Vector<Tracked> vec;
    std::list<Tracked> lst;

    for (int i = 0; i < 5; ++i)
    {
        vec.push_back(Tracked(i));
    }

    for (int i = 0; i < 5; ++i)
    {
        lst.push_back(Tracked(i));
    }

    std::cout << "\nVector Addresses:\n";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        std::cout << "vec[" << i << "] = " << reinterpret_cast<uintptr_t>(&vec[i]) << "\n";
    }

    std::cout << "\nList Addresses:\n";
    int index = 0;
    for (const auto &elem : lst)
    {
        std::cout << "list[" << index++ << "] = " << reinterpret_cast<uintptr_t>(&elem) << "\n";
    }
}

void benchmarkIteration()
{
    std::cout << "\n--- Iteration Benchmark ---\n";

    Vector<int> vec;
    std::list<int> lst;

    for (int i = 0; i < 1000000; ++i)
    {
        vec.push_back(i);
        lst.push_back(i);
    }

    volatile long long sum = 0;

    auto start = std::chrono::high_resolution_clock::now();
    for (auto v : vec)
    {
        sum += v;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Vector iteration time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " µs\n";

    start = std::chrono::high_resolution_clock::now();
    for (int v : lst)
    {
        sum += v;
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "List iteration time:   " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " µs\n";
}

int main()
{
    // Compare performance of list and vector
    {
        compareAddresses();
        benchmarkIteration();
    }

    // Construct from initializer list
    const Vector<int> vec{1, 2, 3, 4, 5};

    std::cout << "Not using iterators:\n";
    for(size_t i=0;i<vec.size(); ++i) {
        std::cout << vec[i] << "\n";
    }

    std::cout << "Range-based for loop:\n";
    for(const auto& v : vec) {
        std::cout << v << "\n";
    }

    std::cout << "Explicit Iterator:\n";
    for(Vector<int>::VectorIteratorImpl<true> it=vec.begin(); it!=vec.end(); it++) {
        std::cout << *it << std::endl;
    }
}
