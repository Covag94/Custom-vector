#pragma once

#include <iostream>
#include <stdexcept>
#include <utility>
#include <algorithm>

template <typename T>
class Vector
{
private:
    size_t m_capacity;
    size_t m_size;
    T *m_data;

public:
    void destroyElements()
    {
        // Explicit destructor call for each element in vector
        for (size_t i = 0; i < m_size; ++i)
        {
            m_data[i].~T();
        }
    }

    void deallocate()
    {
        operator delete(m_data);
        m_size = 0;
        m_capacity = 0;
    }

    T *allocate(size_t n)
    {
        return static_cast<T *>(operator new(sizeof(T) * n));
    }

    // Prefer initializer list because if members are initialized
    // inside constructor's body they are first default constructed
    // and then assigned the initial value. Thus initializer list avoids
    // default construction + assignment and instead construct with
    // initial value straightaway
    Vector() : m_capacity(1), m_size(0), m_data(static_cast<T *>(operator new(sizeof(T) * m_capacity)))
    {
    }

    ~Vector()
    {
        destroyElements();
        deallocate();
    }

    // Copy constructor
    Vector(const Vector &other)
    {
        if (other.m_size >= 0)
        {
            m_data = allocate(other.m_size);
            m_capacity = other.m_capacity;
            m_size = other.m_size;

            for (size_t i = 0; i < other.m_size; ++i)
            {
                new (m_data + i) T(other.m_data[i]);
            }
        }
    }

    // Copy assignment operator
    Vector &operator=(const Vector &other)
    {
        if (this != &other)
        {
            destroyElements();
            deallocate();

            m_size = other.m_size;
            m_capacity = other.m_capacity;
            m_data = allocate(m_capacity);

            for (size_t i = 0; i < other.m_size; ++i)
            {
                new (m_data + i) T(other.m_data[i]);
            }
        }

        return *this;
    }

    Vector(Vector &&other) noexcept : m_size(other.m_size), m_capacity(other.m_capacity), m_data(other.m_data)
    {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    Vector &operator=(Vector &&other) noexcept
    {
        if (this != &other)
        {
            destroyElements();
            deallocate();

            m_size = other.m_size;
            m_capacity = other.m_capacity;
            m_data = other.m_data;

            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }

        return *this;
    }

    template <typename U>
    void push_back(U &&element)
    {
        if (m_size >= m_capacity)
        {
            // Need to delete memory and allocate bigger space
            m_capacity = (m_capacity == 0) ? 1 : 2 * m_capacity;

            // Just allocate memory without default construction
            T *newData = allocate(m_capacity);

            for (size_t i = 0; i < m_size; ++i)
            {
                new (newData + i) T(std::move(m_data[i]));
                m_data[i].~T();
            }

            operator delete(m_data);
            m_data = newData;
        }

        new (m_data + m_size) T(std::forward<U>(element));
        m_size++;
    }

    size_t getSize() const
    {
        return m_size;
    }

    T &operator[](size_t index)
    {
        if (index >= m_size)
        {
            throw std::out_of_range("Index out of range");
        }

        return m_data[index];
    }

    void print() const
    {
        for (size_t i = 0; i < m_size; ++i)
        {
            std::cout << "Index : " << i << " has a value of : " << m_data[i] << "\n";
        }

        std::cout << "\n";
    }
};