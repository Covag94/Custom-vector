# Custom Vector Implementation (C++)

This project implements a custom dynamic array class template `Vector<T>` in C++, mimicking some of the functionality of `std::vector`.

## Features

- Manual memory management (allocation, destruction, deallocation)
- Dynamic resizing with exponential growth
- Exception-safe `push_back` and `reserve`
- Iterators: `begin()` / `end()` (const + non-const)
- Utility methods: `clear`, `empty`, `shrink_to_fit`, `print`

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