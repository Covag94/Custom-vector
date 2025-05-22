#pragma once

#include <iostream>
#include <stdexcept>
#include <utility>
#include <algorithm>

template <typename T>
class Vector
{
private:
    size_t capacity;
    size_t size;
    T *data;

public:
    // Prefer initializer list because if members are initialized
    // inside constructor's body they are first default constructed
    // and then assigned the initial value. Thus initializer list avoids
    // default construction + assignment and instead construct with
    // initial value straightaway
    Vector() : size(0), capacity(1), data(static_cast<T *>(operator new(sizeof(T) * capacity)))
    {
    }

    ~Vector()
    {
        // Explicit destructor call for each element in vector
        for (size_t i = 0; i < size; ++i)
        {
            data[i].~T();
        }

        // Finally free memory
        operator delete(data);
    }

    // Copy constructor
    Vector(const Vector &other)
    {
        if (other.size >= 0)
        {
            data = static_cast<T *>(operator new(sizeof(T) * other.size));
            capacity = other.capacity;
            size = other.size;

            for (size_t i = 0; i < other.size; ++i)
            {
                new (data + i) T(other.data[i]);
            }
        }
    }

    // Copy assignment operator
    Vector &operator=(const Vector &other)
    {
        if (this != &other)
        {
            for (size_t i = 0; i < size; ++i)
            {
                data[i].~T();
            }
            operator delete(data);

            size = other.size;
            capacity = other.capacity;
            data = static_cast<T *>(operator new(sizeof(T) * capacity));

            for (size_t i = 0; i < other.size; ++i)
            {
                new (data + i) T(other.data[i]);
            }
        }

        return *this;
    }

    Vector(Vector &&other) noexcept : size(other.size), capacity(other.capacity), data(other.data)
    {
        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    }

    Vector &operator=(Vector &&other) noexcept
    {
        if (this != &other)
        {
            for (size_t i = 0; i < size; ++i)
            {
                data[i].~T();
            }
            operator delete(data);

            size = other.size;
            capacity = other.capacity;
            data = other.data;

            other.data = nullptr;
            other.size = 0;
            other.capacity = 0;
        }

        return *this;
    }

    template <typename U>
    void push_back(U &&element)
    {
        if (size >= capacity)
        {
            // Need to delete memory and allocate bigger space
            capacity = (capacity == 0) ? 1 : 2 * capacity;

            // Just allocate memory without default construction
            T *newData = static_cast<T *>(operator new(sizeof(T) * capacity));

            for (size_t i = 0; i < size; ++i)
            {
                new (newData + i) T(std::move(data[i]));
                data[i].~T();
            }

            operator delete(data);
            data = newData;
        }

        new (data + size) T(std::forward<U>(element));
        size++;
    }

    size_t getSize() const
    {
        return size;
    }

    T &operator[](size_t index)
    {
        if (index >= size)
        {
            throw std::out_of_range("Index out of range");
        }

        return data[index];
    }

    void print() const
    {
        for (size_t i = 0; i < size; ++i)
        {
            std::cout << "Index : " << i << " has a value of : " << data[i] << "\n";
        }

        std::cout << "\n";
    }
};