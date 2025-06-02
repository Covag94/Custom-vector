# Custom Vector Implementation

This project implements a custom dynamic array class template `Vector<T>`.

## Features

- Manual memory management
- Dynamic resizing with exponential growth
- Exception-safety
- Iterators

## Iterator Support

The `Vector<T>` class includes a VectorIterator class which provides:
- Postfix and prefix operators
- Element access
- Equality/inequality comparisons
- Support for const and non-const iteration(begin(), end(), cbegin(), cend())

## Vector vs std::list: Performance and Layout

To explore performance and memory layout differences, a benchmark was added comparing:

| Property                  | `Vector<T>`                        | `std::list<T>`                     |
|--------------------------|-------------------------------------|------------------------------------|
| Memory Layout            | Contiguous                         | Scattered (nodes allocated individually) |
| Iteration Performance    | Fast (cache-friendly)              | Slower (cache misses)             |
| Allocation Overhead      | Few large reallocations            | Many small allocations            |

## Iteration Benchmark

vector iteration time: 1063μs
list iteration time: 7953μs

# Build instructions

```bash
g++ -std=c++17 -O2 -o vector_benchmark main.cpp
./vector_benchmark