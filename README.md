# Custom Vector Implementation in C++

A robust implementation of a dynamic array (`Vector`) class template in C++, mimicking much of the functionality and interface of `std::vector` and unit tests.

## Features

- **Dynamic resizing**: Automatic capacity doubling on `push_back` when needed.
- **Exception safety**: Strong exception guarantee during allocation, copying, and element construction.
- **Manual memory management**: Uses `operator new` and placement `new` for precise control.
- **Copy and Move Semantics**: Supports deep copy construction, copy assignment (copy-and-swap idiom), move construction, and move assignment.
- **Custom iterator (`VectorIterator`)**:
  - Random access iterator complying with LegacyRandomAccessIterator requirements.
  - Supports increment/decrement (both prefix and postfix), addition/subtraction with offsets, and difference calculation.
- **Element access**: Provides `at()` (with bounds checking), and `operator[]` (unchecked) for accessing elements.
- **Capacity management**: Supports `reserve()`, `shrink_to_fit()`, `clear()`, `size()`, `capacity()`, and `empty()` methods.
- **Triviality Optimisations**: Enable trivial copy/move operatios when possible reducing overhead via custom C++20 concepts.

## Build Instructions

```bash
mkdir build && cd build
cmake ..
make

## Run tests
ctest --verbose
