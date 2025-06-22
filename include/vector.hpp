#pragma once

#include <iostream>
#include <stdexcept>
#include <utility>
#include <cassert>
#include <type_traits>
#include <concepts>
#include <cstring>

template <typename T>
concept TriviallyCopyConstructible = std::is_trivially_copy_constructible_v<T>;

template <typename T>
concept TriviallyCopyAssignable = std::is_trivially_copy_assignable_v<T>;

template <typename T>
concept TriviallyMoveConstructible = std::is_trivially_move_constructible_v<T>;

template <typename T>
concept TriviallyMoveAssignable = std::is_trivially_move_assignable_v<T>;

template <typename T>
concept TriviallyDestructible = std::is_trivially_destructible_v<T>;

template <typename T>
class Vector
{
public:
    // Follows LegacyRandomAccessIterator style
    template <bool IsConst>
    class VectorIteratorImpl
    {
    public:
        using iterator_category = typename std::random_access_iterator_tag;
        using value_type = typename std::remove_cv<T>;
        using difference_type = std::ptrdiff_t;
        using reference = typename std::conditional<IsConst, const T &, T &>::type;
        using pointer = typename std::conditional<IsConst, const T *, T *>::type;

    private:
        pointer m_ptr;

    public:
        /*
            Constructors
        */
        VectorIteratorImpl() : m_ptr(nullptr) {}

        explicit VectorIteratorImpl(pointer ptr) noexcept : m_ptr(ptr) {}

        template <bool R = IsConst, typename std::enable_if<R, int>::type = 0>
        VectorIteratorImpl(const VectorIteratorImpl<false> &other) noexcept : m_ptr(other.m_ptr) {}

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
            VectorIteratorImpl iterator = *this;
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

        friend VectorIteratorImpl operator+(difference_type n, VectorIteratorImpl it) noexcept
        {
            return it += n;
        }

        friend VectorIteratorImpl operator-(VectorIteratorImpl it, difference_type n) noexcept
        {
            return it -= n;
        }

        // Distance between two operators
        friend difference_type operator-(const VectorIteratorImpl &lhs, const VectorIteratorImpl &rhs) noexcept
        {
            return lhs.m_ptr - rhs.m_ptr;
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

    using iterator = VectorIteratorImpl<false>;
    using const_iterator = VectorIteratorImpl<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    size_t m_capacity;
    size_t m_size;
    T *m_data;

public:
    void destroyElements()
    {
        if constexpr (!TriviallyDestructible<T>)
        {
            for (size_t i = 0; i < m_size; ++i)
            {
                m_data[i].~T();
            }
        }
    }

    void copyElements(T *dest, const T *src, size_t count)
    {
        if constexpr (TriviallyCopyConstructible<T>)
        {
            // memcpy for trivially copy constructible types
            std::memcpy(dest, src, count * sizeof(T));
        }
        else
        {
            size_t i = 0;
            try
            {
                for (; i < count; ++i)
                {
                    new (dest + i) T(src[i]);
                }
            }
            catch (...)
            {
                for (size_t j = 0; j < i; ++j)
                {
                    dest[j].~T();
                }
                throw;
            }
        }
    }

    void moveElements(T *dest, T *src, size_t count)
    {
        if constexpr (TriviallyMoveConstructible<T>)
        {
            std::memmove(dest, src, count * sizeof(T));
        }
        else
        {
            size_t i = 0;

            try
            {
                for (; i < count; ++i)
                {
                    new (dest + i) T(std::move(src[i]));
                }
            }
            catch (...)
            {
                for (size_t j = 0; j < i; ++j)
                {
                    dest[j].~T();
                }
                throw;
            }
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

    T *allocate(size_t n) noexcept(std::is_nothrow_constructible_v<T>)
    {
        return static_cast<T *>(operator new(sizeof(T) * n));
    }

    void shrink_to_fit() noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        if (m_capacity > m_size)
        {
            T *newData = allocate(m_size);
            try
            {
                moveElements(newData, m_data, m_size);
            }
            catch (...)
            {
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
    Vector(std::initializer_list<T> init) noexcept(std::is_nothrow_copy_constructible_v<T>) : m_capacity(init.size()), m_size(0), m_data(nullptr)
    {
        if (init.size() == 0)
        {
            return;
        }

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
    Vector(const Vector &other) noexcept(std::is_nothrow_copy_constructible_v<T>) : m_capacity(other.m_size), m_size(0), m_data(nullptr)
    {
        if (other.m_size == 0)
        {
            return;
        }

        T *newData = allocate(other.m_size);

        try
        {
            copyElements(newData, other.m_data, other.m_size);
        }
        catch (...)
        {
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

    Vector(Vector &&other) noexcept
        : m_size(std::exchange(other.m_size, 0)),
          m_capacity(std::exchange(other.m_capacity, 0)),
          m_data(std::exchange(other.m_data, nullptr)) {}

    Vector &operator=(Vector &&other) noexcept
    {
        if (this != &other)
        {
            destroyElements();
            deallocate();

            m_size = std::exchange(other.m_size, 0);
            m_capacity = std::exchange(other.m_capacity, 0);
            m_data = std::exchange(other.m_data, nullptr);
        }
        return *this;
    }

    template <typename U>
    void push_back(U &&element) noexcept(std::is_nothrow_copy_constructible_v<T> &&
                                         std::is_nothrow_move_constructible_v<T> && std::is_nothrow_destructible_v<T>)
    {
        if (m_size >= m_capacity)
        {
            // Need to delete memory and allocate bigger space
            size_t newCapacity = (m_capacity == 0) ? 1 : 2 * m_capacity;

            // Just allocate memory without default construction
            T *newData = allocate(newCapacity);
            try
            {
                moveElements(newData, m_data, m_size);
                new (newData + m_size) T(std::forward<U>(element));
            }
            catch (...)
            {
                if constexpr (!TriviallyDestructible<T>)
                {
                    for (size_t j = 0; j <= m_size; ++j)
                    {
                        newData[j].~T();
                    }
                }
                operator delete(newData);
                throw;
            }

            // Only after we haven't thrown destroy old elements
            if constexpr (!TriviallyDestructible<T>)
            {
                destroyElements();
            }
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

    [[nodiscard]] size_t getSize() const
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

    void reserve(const size_t newCapacity) noexcept(std::is_nothrow_move_constructible_v<T>)
    {
        if (newCapacity <= m_capacity)
        {
            return;
        }

        T *newData = allocate(newCapacity);

        try
        {
            moveElements(newData, m_data, m_size);
        }
        catch (...)
        {
            operator delete(newData);
            throw;
        }

        destroyElements();
        operator delete(m_data);
        m_data = newData;
        m_capacity = newCapacity;
    }

    // For STL-compliance data() accessor
    [[nodiscard]] T *data() { return m_data; }
    [[nodiscard]] const T *data() const { return m_data; }

    bool empty() const { return m_size == 0; }

    [[nodiscard]] size_t size() const
    {
        return m_size;
    }
    [[nodiscard]] size_t capacity() const { return m_capacity; }

    /*
        Iterators access
    */
    iterator begin() noexcept { return iterator(m_data); }
    iterator end() noexcept { return iterator(m_data + m_size); }

    const_iterator begin() const noexcept { return const_iterator(m_data); }
    const_iterator end() const noexcept { return const_iterator(m_data + m_size); }

    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }

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