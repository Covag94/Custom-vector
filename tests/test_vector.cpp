#include <gtest/gtest.h>
#include "vector.hpp"
#include <string>
#include <stdexcept>

// ===== CONSTRUCTOR TESTS =====

TEST(ConstructorTest, EmptyVector) {
    Vector<int> v;
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.data(), nullptr);
}

TEST(ConstructorTest, InitializerListConstructor) {
    Vector<int> v{1, 2, 3, 4, 5};
    EXPECT_EQ(v.size(), 5);
    EXPECT_GE(v.capacity(), 5);
    EXPECT_FALSE(v.empty());
    
    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(v[i], static_cast<int>(i + 1));
    }
}

TEST(ConstructorTest, InitializerListConstructorEmpty) {
    Vector<int> v{};
    EXPECT_EQ(v.size(), 0);
    EXPECT_TRUE(v.empty());
}

TEST(ConstructorTest, CopyConstructor) {
    Vector<int> original{1, 2, 3, 4, 5};
    Vector<int> copy(original);
    
    EXPECT_EQ(copy.size(), original.size());
    EXPECT_GE(copy.capacity(), original.size());
    
    for (size_t i = 0; i < copy.size(); ++i) {
        EXPECT_EQ(copy[i], original[i]);
    }
    
    // Ensure deep copy
    copy[0] = 99;
    EXPECT_NE(copy[0], original[0]);
}

TEST(ConstructorTest, CopyConstructorEmpty) {
    Vector<int> original;
    Vector<int> copy(original);
    
    EXPECT_EQ(copy.size(), 0);
    EXPECT_TRUE(copy.empty());
}

TEST(ConstructorTest, MoveConstructor) {
    Vector<int> original{1, 2, 3, 4, 5};
    size_t original_size = original.size();
    int* original_data = original.data();
    
    Vector<int> moved(std::move(original));
    
    EXPECT_EQ(moved.size(), original_size);
    EXPECT_EQ(moved.data(), original_data);
    EXPECT_EQ(original.size(), 0);
    EXPECT_EQ(original.capacity(), 0);
    EXPECT_EQ(original.data(), nullptr);
}

// ===== ASSIGNMENT TESTS =====

TEST(VectorTest, CopyAssignment) {
    Vector<int> original{1, 2, 3, 4, 5};
    Vector<int> copy{10, 20};
    
    copy = original;
    
    EXPECT_EQ(copy.size(), original.size());
    for (size_t i = 0; i < copy.size(); ++i) {
        EXPECT_EQ(copy[i], original[i]);
    }
    
    // Test self-assignment
    copy = copy;
    EXPECT_EQ(copy.size(), original.size());
}

/*TEST(VectorTest, MoveAssignment) {
    Vector<int> original{1, 2, 3, 4, 5};
    Vector<int> target{10, 20};
    size_t original_size = original.size();
    
    target = std::move(original);
    
    EXPECT_EQ(target.size(), original_size);
    EXPECT_EQ(original.size(), 0);
    EXPECT_EQ(original.data(), nullptr);
}

// ===== PUSH_BACK TESTS =====

TEST(VectorTest, PushBackBasic) {
    Vector<int> v;
    
    v.push_back(1);
    EXPECT_EQ(v.size(), 1);
    EXPECT_EQ(v[0], 1);
    
    v.push_back(2);
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v[1], 2);
}

TEST(VectorTest, PushBackReallocation) {
    Vector<int> v;
    size_t initial_capacity = v.capacity();
    
    // Push enough elements to force reallocation
    for (int i = 0; i < 10; ++i) {
        v.push_back(i);
    }
    
    EXPECT_EQ(v.size(), 10);
    EXPECT_GE(v.capacity(), 10);
    EXPECT_GT(v.capacity(), initial_capacity);
    
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(v[i], i);
    }
}

TEST(VectorTest, PushBackMoveSemantics) {
    NonTrivialType::resetCounters();
    Vector<NonTrivialType> v;
    
    NonTrivialType obj(42);
    v.push_back(std::move(obj));
    
    EXPECT_EQ(v.size(), 1);
    EXPECT_EQ(v[0].value, 42);
    EXPECT_GT(NonTrivialType::move_count, 0);
}

// ===== POP_BACK TESTS =====

TEST(VectorTest, PopBackBasic) {
    Vector<int> v{1, 2, 3, 4, 5};
    
    v.pop_back();
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v[3], 4); // Last element should now be 4
    
    v.pop_back();
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v[2], 3);
}

TEST(VectorTest, PopBackEmptyThrows) {
    Vector<int> v;
    EXPECT_THROW(v.pop_back(), std::out_of_range);
}

TEST(VectorTest, PopBackDestructorCalled) {
    NonTrivialType::resetCounters();
    {
        Vector<NonTrivialType> v;
        v.push_back(NonTrivialType(1));
        v.push_back(NonTrivialType(2));
        
        int destructions_before = NonTrivialType::destruction_count;
        v.pop_back();
        int destructions_after = NonTrivialType::destruction_count;
        
        EXPECT_GT(destructions_after, destructions_before);
        EXPECT_EQ(v.size(), 1);
    }
}

// ===== ERASE TESTS =====

TEST(VectorTest, EraseSingleElement) {
    Vector<int> v{1, 2, 3, 4, 5};
    
    auto it = v.erase(v.begin() + 2); // Erase element at index 2 (value 3)
    
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 4); // 4 moved to position 2
    EXPECT_EQ(v[3], 5);
    
    // Return iterator should point to element that moved into erased position
    EXPECT_EQ(*it, 4);
}

TEST(VectorTest, EraseFirstElement) {
    Vector<int> v{1, 2, 3, 4, 5};
    
    auto it = v.erase(v.begin());
    
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v[0], 2);
    EXPECT_EQ(*it, 2);
}

TEST(VectorTest, EraseLastElement) {
    Vector<int> v{1, 2, 3, 4, 5};
    
    auto it = v.erase(v.end() - 1);
    
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v[3], 4);
    EXPECT_EQ(it, v.end()); // Should return end()
}

TEST(VectorTest, EraseRange) {
    Vector<int> v{1, 2, 3, 4, 5, 6, 7};
    
    // Erase elements 2, 3, 4 (indices 1, 2, 3)
    auto it = v.erase(v.begin() + 1, v.begin() + 4);
    
    EXPECT_EQ(v.size(), 4);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 5); // 5 moved to position 1
    EXPECT_EQ(v[2], 6);
    EXPECT_EQ(v[3], 7);
    
    EXPECT_EQ(*it, 5);
}

TEST(VectorTest, EraseEmptyRange) {
    Vector<int> v{1, 2, 3, 4, 5};
    size_t original_size = v.size();
    
    auto it = v.erase(v.begin() + 2, v.begin() + 2); // Empty range
    
    EXPECT_EQ(v.size(), original_size); // Size should be unchanged
    EXPECT_EQ(it, v.begin() + 2); // Should return iterator to first
}

TEST(VectorTest, EraseOutOfRange) {
    Vector<int> v{1, 2, 3};
    
    EXPECT_THROW(v.erase(v.end()), std::out_of_range);
    EXPECT_THROW(v.erase(v.begin() - 1), std::out_of_range);
}

TEST(VectorTest, EraseInvalidRange) {
    Vector<int> v{1, 2, 3, 4, 5};
    
    EXPECT_THROW(v.erase(v.begin() + 3, v.begin() + 1), std::invalid_argument);
}

// ===== ACCESS TESTS =====

TEST(VectorTest, AtMethod) {
    Vector<int> v{1, 2, 3, 4, 5};
    
    EXPECT_EQ(v.at(0), 1);
    EXPECT_EQ(v.at(4), 5);
    
    // Const version
    const Vector<int>& cv = v;
    EXPECT_EQ(cv.at(2), 3);
    
    // Out of range
    EXPECT_THROW(v.at(5), std::out_of_range);
    EXPECT_THROW(cv.at(10), std::out_of_range);
}

TEST(VectorTest, SubscriptOperator) {
    Vector<int> v{1, 2, 3, 4, 5};
    
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[4], 5);
    
    v[2] = 99;
    EXPECT_EQ(v[2], 99);
    
    // Const version
    const Vector<int>& cv = v;
    EXPECT_EQ(cv[2], 99);
}

// ===== CAPACITY TESTS =====

TEST(VectorTest, Reserve) {
    Vector<int> v{1, 2, 3};
    size_t original_capacity = v.capacity();
    
    v.reserve(100);
    
    EXPECT_GE(v.capacity(), 100);
    EXPECT_EQ(v.size(), 3); // Size unchanged
    EXPECT_EQ(v[0], 1); // Data preserved
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);
    
    // Reserve smaller than current capacity should do nothing
    v.reserve(5);
    EXPECT_GE(v.capacity(), 100);
}

TEST(VectorTest, ShrinkToFit) {
    Vector<int> v;
    v.reserve(100);
    v.push_back(1);
    v.push_back(2);
    
    EXPECT_GE(v.capacity(), 100);
    EXPECT_EQ(v.size(), 2);
    
    v.shrink_to_fit();
    
    EXPECT_EQ(v.capacity(), v.size());
    EXPECT_EQ(v[0], 1); // Data preserved
    EXPECT_EQ(v[1], 2);
}

TEST(VectorTest, Clear) {
    Vector<int> v{1, 2, 3, 4, 5};
    size_t original_capacity = v.capacity();
    
    v.clear();
    
    EXPECT_EQ(v.size(), 0);
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.capacity(), original_capacity); // Capacity unchanged
}

// ===== ITERATOR TESTS =====

TEST(VectorTest, IteratorBasic) {
    Vector<int> v{1, 2, 3, 4, 5};
    
    // Test begin/end
    EXPECT_EQ(*v.begin(), 1);
    EXPECT_EQ(*(v.end() - 1), 5);
    
    // Test iteration
    int expected = 1;
    for (auto it = v.begin(); it != v.end(); ++it) {
        EXPECT_EQ(*it, expected++);
    }
}

TEST(VectorTest, ConstIterator) {
    Vector<int> v{1, 2, 3, 4, 5};
    const Vector<int>& cv = v;
    
    // Test const iterators
    EXPECT_EQ(*cv.begin(), 1);
    EXPECT_EQ(*cv.cbegin(), 1);
    
    // Range-based for loop
    int expected = 1;
    for (const auto& val : cv) {
        EXPECT_EQ(val, expected++);
    }
}

TEST(VectorTest, IteratorArithmetic) {
    Vector<int> v{1, 2, 3, 4, 5};
    
    auto it = v.begin();
    EXPECT_EQ(*it, 1);
    
    it += 2;
    EXPECT_EQ(*it, 3);
    
    it -= 1;
    EXPECT_EQ(*it, 2);
    
    EXPECT_EQ(*(it + 2), 4);
    EXPECT_EQ(*(it - 1), 1);
    
    EXPECT_EQ(it[3], 5); // Subscript operator
}

TEST(VectorTest, IteratorComparison) {
    Vector<int> v{1, 2, 3, 4, 5};
    
    auto begin_it = v.begin();
    auto end_it = v.end();
    auto mid_it = v.begin() + 2;
    
    EXPECT_TRUE(begin_it < end_it);
    EXPECT_TRUE(begin_it <= mid_it);
    EXPECT_TRUE(end_it > begin_it);
    EXPECT_TRUE(mid_it >= begin_it);
    EXPECT_TRUE(begin_it == begin_it);
    EXPECT_TRUE(begin_it != end_it);
    
    // Distance
    EXPECT_EQ(end_it - begin_it, 5);
    EXPECT_EQ(mid_it - begin_it, 2);
}

TEST(VectorTest, IteratorConstConversion) {
    Vector<int> v{1, 2, 3, 4, 5};
    
    // Non-const to const iterator conversion should work
    Vector<int>::iterator it = v.begin();
    Vector<int>::const_iterator cit = it; // Should compile
    
    EXPECT_EQ(*it, *cit);
    EXPECT_TRUE(it == cit);
    EXPECT_TRUE(cit == it);
}

// ===== UTILITY TESTS =====

TEST(VectorTest, Empty) {
    Vector<int> v;
    EXPECT_TRUE(v.empty());
    
    v.push_back(1);
    EXPECT_FALSE(v.empty());
    
    v.clear();
    EXPECT_TRUE(v.empty());
}

TEST(VectorTest, Data) {
    Vector<int> v{1, 2, 3, 4, 5};
    
    int* ptr = v.data();
    EXPECT_EQ(ptr[0], 1);
    EXPECT_EQ(ptr[4], 5);
    
    // Const version
    const Vector<int>& cv = v;
    const int* cptr = cv.data();
    EXPECT_EQ(cptr[2], 3);
}

TEST(VectorTest, StreamOutput) {
    Vector<int> v{1, 2, 3, 4, 5};
    
    std::ostringstream oss;
    oss << v;
    
    EXPECT_EQ(oss.str(), "[1, 2, 3, 4, 5]");
}

// ===== EXCEPTION SAFETY TESTS =====

TEST(VectorTest, ExceptionSafetyInConstructor) {
    NonTrivialType::resetCounters();
    
    try {
        Vector<NonTrivialType> v{NonTrivialType(1), NonTrivialType(2), NonTrivialType(3)};
    } catch (...) {
        // If construction fails, destructors should have been called
        EXPECT_GE(NonTrivialType::destruction_count, 0);
    }
}

TEST(VectorTest, ExceptionSafetyInPushBack) {
    Vector<NonTrivialType> v;
    NonTrivialType::resetCounters();
    
    // Fill vector to force reallocation on next push_back
    for (int i = 0; i < 10; ++i) {
        v.push_back(NonTrivialType(i));
    }
    
    size_t size_before = v.size();
    
    try {
        v.push_back(NonTrivialType(999));
        // If we get here, push_back succeeded
        EXPECT_EQ(v.size(), size_before + 1);
    } catch (...) {
        // If push_back failed, vector should be in valid state
        EXPECT_EQ(v.size(), size_before);
    }
}

// ===== PERFORMANCE/EDGE CASE TESTS =====

TEST(VectorTest, LargeVector) {
    Vector<int> v;
    const size_t large_size = 10000;
    
    for (size_t i = 0; i < large_size; ++i) {
        v.push_back(static_cast<int>(i));
    }
    
    EXPECT_EQ(v.size(), large_size);
    
    for (size_t i = 0; i < large_size; ++i) {
        EXPECT_EQ(v[i], static_cast<int>(i));
    }
}

TEST(VectorTest, SwapFunction) {
    Vector<int> v1{1, 2, 3};
    Vector<int> v2{4, 5, 6, 7, 8};
    
    v1.swap(v2);
    
    EXPECT_EQ(v1.size(), 5);
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v1[0], 4);
    EXPECT_EQ(v2[0], 1);
}*/