#pragma once

#include <iostream>
#include <stdexcept>
#include <utility>
#include <algorithm>

template <typename T>
class Vector
{
private:
    T *data;
    size_t size;
    size_t capacity;

public:
    Vector() : size(0), capacity(1)
    {
        data = new T[capacity]; // dynamically allocate memory on heap
    }

    ~Vector()
    {
        delete[] data;
        size = 0;
        capacity = 0;
    }

    // Copy constructor
    Vector(const Vector &other)
    {
        if (other.size >= 0)
        {
            data = new T[other.capacity];
            capacity = other.capacity;
            size = other.size;

            std::copy(other.data, other.data + size, data);
        }
    }

    // Copy assignment operator
    Vector &operator=(const Vector &other)
    {
        if (this != &other)
        {
            delete[] data;

            size = other.size;
            capacity = other.capacity;
            data = new T[capacity];

            for (size_t i = 0; i < size; ++i)
            {
                data[i] = other.data[i];
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
            delete[] data;
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

            T *data2 = new T[capacity];

            for (size_t i = 0; i < size; ++i)
            {
                data2[i] = std::move(data[i]);
            }

            delete[] data;

            data = data2;
        }

        data[size] = std::forward<U>(element);
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
            std::cout << "Index : " << i << " has a value of : " << data[i] << std::endl;
        }

        std::cout << std::endl;
    }
};