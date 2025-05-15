# CustomVector

A simple, custom implementation of a dynamic array class (`Vector<T>`) in C++. This mimics the behavior of `std::vector` and supports:

- Dynamic resizing
- Copy/move constructors and assignment
- `push_back` with perfect forwarding
- Basic index access with bounds checking
- Printing contents

# Build instructions

```bash
mkdir build
cd build
cmake ..
make
./vector_app