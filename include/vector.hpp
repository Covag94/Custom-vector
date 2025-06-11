#pragma once

#include <iostream>
#include <stdexcept>
#include <utility>
#include <cassert>
#include <type_traits>

template <typename T>
class Vector
{
public:
    // Follows LegacyRandomAccessIterator style
    template <bool IsConst>
    class VectorIteratorImpl
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = std::remove_cv<T>;
        using difference_type = std::ptrdiff_t;
        using reference = std::conditional<IsConst, const T &, T &>;
        using pointer = std::conditional<IsConst, const T *, T *>;

    private:
        pointer *m_ptr;

    public:
        /*
            Constructors
        */
        VectorIteratorImpl() : m_ptr(nullptr) {}

        explicit VectorIteratorImpl(pointer ptr) noexcept : m_ptr(ptr) {}

        VectorIteratorImpl &operator++() noexcept
        {
            m_ptr++;
            return *this;
        }
        VectorIteratorImpl operator++(int) noexcept
        {
            VectorIteratorImpl iterator = *this;
            ++(*this);
            return iterator;
        }

        VectorIteratorImpl &operator--() noexcept
        {
            m_ptr--;
            return *this;
        }

        VectorIteratorImpl operator--(int) noexcept
        {
            VectorIterator iterator = *this;
            --(*this);
            return iterator;
        }

        VectorIteratorImpl &operator+=(const size_t offset) noexcept
        {
            m_ptr += offset;
            return *this;
        }

        VectorIteratorImpl &operator-=(const size_t offset) noexcept
        {
            m_ptr -= offset;
            return *this;
        }

        friend VectorIteratorImpl operator+(VectorIteratorImpl it, difference_type n) noexcept
        {
            return it += n;
        }

        friend difference_type operator+(difference_type n, VectorIteratorImpl it) noexcept
        {
            return it += n;
        }

        friend VectorIteratorImpl operator-(VectorIteratorImpl it, difference_type n) noexcept
        {
            return it -= n;
        }

        // Distance between two operators
        friend difference_type operator-(difference_type n, VectorIteratorImpl it) noexcept
        {
            return it -= n;
        }

        reference operator[](difference_type index) const noexcept
        {
            return *(m_ptr + index);
        }

        reference operator*() const noexcept
        {
            return *m_ptr;
        }

        pointer operator->() const noexcept
        {
            return m_ptr;
        }

        /*
            Comparison opearators
            Need to be templated cause == should be valid for
            any combination of const it and it
        */
        template <bool R>
        bool operator==(const VectorIteratorImpl<R> &other) const noexcept
        {
            return m_ptr == other.m_ptr;
        }

        template <bool R>
        bool operator!=(const VectorIteratorImpl<R> &other) const noexcept
        {
            return m_ptr != other.m_ptr;
        }

        template <bool R>
        bool operator<(const VectorIteratorImpl<R> &other) const noexcept
        {
            return m_ptr < other.m_ptr;
        }

        template <bool R>
        bool operator<=(const VectorIteratorImpl<R> &other) const noexcept
        {
            return m_ptr <= other.m_ptr;
        }

        template <bool R>
        bool operator>(const VectorIteratorImpl<R> &other) const noexcept
        {
            return m_ptr > other.m_ptr;
        }

        template <bool R>
        bool operator>=(const VectorIteratorImpl<R> &other) const noexcept
        {
            return m_ptr >= other.m_ptr;
        }
    };

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
        if (m_data)
        {
            operator delete(m_data);
        }
        m_size = 0;
        m_capacity = 0;
    }

    T *allocate(size_t n)
    {
        return static_cast<T *>(operator new(sizeof(T) * n));
    }

    void shrink_to_fit()
    {
        if (m_capacity > m_size)
        {
            T *newData = allocate(m_size);
            size_t i = 0;

            try
            {
                for (; i < m_size; ++i)
                {
                    new (newData + i) T(std::move(m_data[i]));
                }
            }
            catch (...)
            {
                for (size_t j = 0; j < i; ++j)
                {
                    newData[j].~T();
                }

                operator delete(newData);
                throw;
            }

            // Clean up remainings of initial block
            destroyElements();
            operator delete(m_data);

            m_capacity = m_size;
            m_data = newData;
        }
    }

    /*
        Constructors
    */

    // Prefer initializer list because if members are initialized
    // inside constructor's body they are first default constructed
    // and then assigned the initial value. Thus initializer list avoids
    // default construction + assignment and instead construct with
    // initial value straightaway
    Vector() : m_capacity(0), m_size(0), m_data(nullptr)
    {
    }

    // Support for initializer list
    Vector(std::initializer_list<T> init) : m_capacity(init.size()), m_size(0), m_data(nullptr)
    {
        T *newData = allocate(init.size());
        size_t i = 0;

        try
        {
            // Initializer list does not provide [] operator. Need to use iterator
            for (auto it = init.begin(); it != init.end(); ++it)
            {
                new (newData + i) T(*it);
                ++i;
            }
        }
        catch (...)
        {
            for (size_t j = 0; j < i; ++j)
            {
                newData[j].~T();
            }
            operator delete(newData);
            throw;
        }

        m_size = init.size();
        m_data = newData;
    }

    /*
        Destructors
    */
    ~Vector()
    {
        destroyElements();
        deallocate();
    }

    // Copy constructor
    Vector(const Vector &other) : m_capacity(other.m_size), m_size(0), m_data(nullptr)
    {
        T *newData = allocate(other.m_size);
        size_t i = 0;

        try
        {
            for (; i < other.m_size; ++i)
            {
                new (newData + i) T(other.m_data[i]);
            }
        }
        catch (...)
        {
            for (size_t j = 0; j < i; ++j)
            {
                newData[j].~T();
            }
            operator delete(newData);
            throw;
        }

        m_size = other.m_size;
        m_data = newData;
    }

    // Swap should be noexcept
    void swap(Vector &other) noexcept
    {
        std::swap(m_capacity, other.m_capacity);
        std::swap(m_size, other.m_size);
        std::swap(m_data, other.m_data);
    }

    // Copy assignment operator
    // 1. Pass by value is on purpose
    // If exception is thrown during copying this will
    // leave current object intact and not in unknown state
    // 2. Note this does no longer require self-assignment check
    // since other is a NEW object
    // 3. Last but not least, allows for move or copy elision
    Vector &operator=(Vector other)
    {
        swap(other);
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
            size_t newCapacity = (m_capacity == 0) ? 1 : 2 * m_capacity;

            // Just allocate memory without default construction
            T *newData = allocate(newCapacity);
            size_t i = 0;

            try
            {
                for (; i < m_size; ++i)
                {
                    // move operation might throw
                    new (newData + i) T(std::move(m_data[i]));
                }

                new (newData + m_size) T(std::forward<U>(element));
            }
            catch (...)
            {
                for (size_t j = 0; j < i; ++j)
                {
                    newData[j].~T();
                }
                operator delete(newData);
                throw;
            }

            // Only after we haven't thrown destroy old elements
            destroyElements();
            operator delete(m_data);

            // After having ensured that allocation has succeeded
            // then can assign new capacity to member
            m_data = newData;
            m_capacity = newCapacity;
            m_size++;
        }
        else
        {
            new (m_data + m_size) T(std::forward<U>(element));
            m_size++;
        }
    }

    size_t getSize() const
    {
        return m_size;
    }

    T &at(size_t index)
    {
        if (index >= m_size)
        {
            throw std::out_of_range("Index out of range");
        }

        return m_data[index];
    }

    // Need to return const &
    const T &at(const size_t index) const
    {
        if (index >= m_size)
        {
            throw std::out_of_range("Index out of range");
        }

        return m_data[index];
    }

    T &operator[](const size_t index)
    {
        assert(index < m_size);
        return m_data[index];
    }

    const T &operator[](const size_t index) const
    {
        assert(index < m_size);
        return m_data[index];
    }

    void clear()
    {
        destroyElements();
        m_size = 0;
    }

    void reserve(const size_t newCapacity)
    {
        if (newCapacity <= m_capacity)
        {
            return;
        }

        T *newData = allocate(newCapacity);
        size_t i = 0;

        try
        {
            for (; i < m_size; ++i)
            {
                new (newData + i) T(std::move(m_data[i]));
            }
        }
        catch (...)
        {
            for (size_t j = 0; j < i; ++j)
            {
                newData[j].~T();
            }

            operator delete(newData);
            throw;
        }

        destroyElements();
        operator delete(m_data);
        m_data = newData;
        m_capacity = newCapacity;
    }

    VectorIterator begin() { return VectorIterator(m_data); }
    VectorIterator end() { return VectorIterator(m_data + m_size); }

    VectorIterator begin() const { return VectorIterator(m_data); }
    VectorIterator end() const { return VectorIterator(m_data + m_size); }

    VectorIterator cbegin() const { return begin(); }
    VectorIterator cend() const { return end(); }

    // For STL-compliance data() accessor
    T *data() { return m_data; }
    const T *data() const { return m_data; }

    bool empty() const { return m_size == 0; }

    size_t size() const
    {
        return m_size;
    }
    size_t capacity() const { return m_capacity; }

    friend std::ostream &operator<<(std::ostream &os, const Vector &v)
    {
        os << "[";
        for (size_t i = 0; i < v.size(); ++i)
        {
            os << v.m_data[i];
            if (i + 1 < v.size())
                os << ", ";
        }
        os << "]";
        return os;
    }
};