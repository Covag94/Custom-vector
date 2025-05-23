# Custom `Vector<T>` Implementation in C++

This project implements a dynamic array container similar to `std::vector`, written from scratch in C++. It features manual memory management, strong exception safety, and full support for move semantics.

---

## Features

- Dynamic resizing with amortized `O(1)` insertions
- Strong exception safety in copy constructor, assignment, and `push_back`
- Move-aware construction and assignment
- Manual memory allocation and explicit destruction
- `shrink_to_fit` to reduce memory usage
- Bounds-checked element access

---

# Build instructions

```bash
mkdir build
cd build
cmake ..
make
./vector_app