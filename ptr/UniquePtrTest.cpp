#include "UniquePtr.hpp"
#include <gtest/gtest.h>

using namespace ::testing;

class UniquePtrTest : public Test {
protected:
    // Helper function to test reset
    template <typename T>
    void testReset(UniquePtr<T>& ptr) {
        ptr.reset();
        EXPECT_EQ(ptr.get(), nullptr);
    }
};

// Tests for primary template UniquePtr<T>
TEST_F(UniquePtrTest, DefaultConstructor) {
    UniquePtr<int> p;
    EXPECT_EQ(p.get(), nullptr);
}

TEST_F(UniquePtrTest, ConstructorWithRawPointer) {
    UniquePtr<int> p(new int(10));
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(*p, 10);
}

TEST_F(UniquePtrTest, MoveConstructor) {
    UniquePtr<int> p1(new int(20));
    UniquePtr<int> p2(std::move(p1));
    EXPECT_EQ(p1.get(), nullptr);
    EXPECT_NE(p2.get(), nullptr);
    EXPECT_EQ(*p2, 20);
}

TEST_F(UniquePtrTest, MoveAssignmentOperator) {
    UniquePtr<int> p1(new int(30));
    UniquePtr<int> p2;
    p2 = std::move(p1);
    EXPECT_EQ(p1.get(), nullptr);
    EXPECT_NE(p2.get(), nullptr);
    EXPECT_EQ(*p2, 30);
}

TEST_F(UniquePtrTest, DereferenceOperators) {
    UniquePtr<int> p(new int(40));
    EXPECT_EQ(*p, 40);
}

TEST_F(UniquePtrTest, ArrowOperator) {
    struct TestStruct {
        int value;
        void setValue(int v) { value = v; }
    };

    UniquePtr<TestStruct> p(new TestStruct());
    p->setValue(50);
    EXPECT_EQ(p->value, 50);
}

TEST_F(UniquePtrTest, CustomDeleter) {
    bool deleterCalled = false;
    auto customDeleter = [&deleterCalled](int* p) {
        delete p;
        deleterCalled = true;
    };

    {
        UniquePtr<int, decltype(customDeleter)> p(new int(60), customDeleter);
        EXPECT_EQ(*p, 60);
        EXPECT_FALSE(deleterCalled);
    }
    EXPECT_TRUE(deleterCalled);
}

TEST_F(UniquePtrTest, Swap) {
    UniquePtr<int> p1(new int(70));
    UniquePtr<int> p2(new int(80));

    p1.swap(p2);
    EXPECT_EQ(*p1, 80);
    EXPECT_EQ(*p2, 70);
}

TEST_F(UniquePtrTest, Release) {
    UniquePtr<int> p(new int(90));
    int* rawPtr = p.release();
    EXPECT_EQ(*rawPtr, 90);
    EXPECT_EQ(p.get(), nullptr);
    delete rawPtr; // Manually clean up
}

// Tests for array-specialized UniquePtr<T[]>
TEST_F(UniquePtrTest, ArrayConstructor) {
    UniquePtr<int[]> p(5);
    for (size_t i = 0; i < 5; ++i) {
        p[i] = static_cast<int>(i * 10);
    }
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(p[i], static_cast<int>(i * 10));
    }
}

TEST_F(UniquePtrTest, ArrayMoveConstructor) {
    UniquePtr<int[]> p1(5);
    for (size_t i = 0; i < 5; ++i) {
        p1[i] = static_cast<int>(i * 10);
    }
    UniquePtr<int[]> p2(std::move(p1));
    EXPECT_EQ(p1.get(), nullptr);
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(p2[i], static_cast<int>(i * 10));
    }
}

TEST_F(UniquePtrTest, ArrayMoveAssignmentOperator) {
    UniquePtr<int[]> p1(5);
    for (size_t i = 0; i < 5; ++i) {
        p1[i] = static_cast<int>(i * 10);
    }
    UniquePtr<int[]> p2;
    p2 = std::move(p1);
    EXPECT_EQ(p1.get(), nullptr);
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(p2[i], static_cast<int>(i * 10));
    }
}

TEST_F(UniquePtrTest, ArrayReset) {
    UniquePtr<int[]> p(5);
    p.reset();
    EXPECT_EQ(p.get(), nullptr);
}

TEST_F(UniquePtrTest, MakeUniqueNonArray) {
    auto p = makeUnique<int>(100);
    EXPECT_EQ(*p, 100);
}

TEST_F(UniquePtrTest, MakeUniqueArray) {
    auto p = makeUnique<int[]>(3);
    for (size_t i = 0; i < 3; ++i) {
        p[i] = static_cast<int>(i * 100);
    }
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(p[i], static_cast<int>(i * 100));
    }
}