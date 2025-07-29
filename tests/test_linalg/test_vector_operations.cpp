/**
 * @file test_vector_operations.cpp
 * @brief Tests for vector operations in the linear algebra abstraction layer
 * @author Multibody Dynamics Solver Team
 */

#include "test_linear_algebra.hpp"
#include <gtest/gtest.h>

namespace multibody {
namespace linalg {
namespace testing {

//==============================================================================
// VectorOperationsTest Implementation
//==============================================================================

TYPED_TEST(VectorOperationsTest, VectorCreation) {
    using T = TypeParam;
    
    // Test empty vector
    auto vec1 = LinearAlgebraFactory<T>::createVector(0);
    EXPECT_EQ(vec1.size(), 0);
    EXPECT_TRUE(vec1.empty());
    
    // Test vector with specific size
    auto vec2 = LinearAlgebraFactory<T>::createVector(5);
    EXPECT_EQ(vec2.size(), 5);
    EXPECT_FALSE(vec2.empty());
    
    // Test vector from std::vector
    std::vector<T> data = {1, 2, 3, 4, 5};
    auto vec3 = LinearAlgebraFactory<T>::createVector(data);
    EXPECT_EQ(vec3.size(), 5);
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(vec3[i], data[i]);
    }
    
    // Test vector from initializer list
    auto vec4 = LinearAlgebraFactory<T>::createVector({1, 2, 3});
    EXPECT_EQ(vec4.size(), 3);
    EXPECT_EQ(vec4[0], static_cast<T>(1));
    EXPECT_EQ(vec4[1], static_cast<T>(2));
    EXPECT_EQ(vec4[2], static_cast<T>(3));
}

TYPED_TEST(VectorOperationsTest, VectorElementAccess) {
    using T = TypeParam;
    
    auto vec = LinearAlgebraFactory<T>::createVector({1, 2, 3, 4, 5});
    
    // Test operator[]
    EXPECT_EQ(vec[0], static_cast<T>(1));
    EXPECT_EQ(vec[4], static_cast<T>(5));
    
    // Test at() method
    EXPECT_EQ(vec.at(1), static_cast<T>(2));
    EXPECT_EQ(vec.at(3), static_cast<T>(4));
    
    // Test bounds checking
    EXPECT_THROW(vec.at(10), LinearAlgebraException);
    
    // Test modification
    vec[0] = static_cast<T>(10);
    EXPECT_EQ(vec[0], static_cast<T>(10));
    
    vec.at(1) = static_cast<T>(20);
    EXPECT_EQ(vec.at(1), static_cast<T>(20));
}

TYPED_TEST(VectorOperationsTest, VectorArithmetic) {
    using T = TypeParam;
    
    auto vec1 = LinearAlgebraFactory<T>::createVector({1, 2, 3});
    auto vec2 = LinearAlgebraFactory<T>::createVector({4, 5, 6});
    
    // Test addition
    auto result_add = vec1 + vec2;
    EXPECT_EQ(result_add.size(), 3);
    EXPECT_EQ(result_add[0], static_cast<T>(5));
    EXPECT_EQ(result_add[1], static_cast<T>(7));
    EXPECT_EQ(result_add[2], static_cast<T>(9));
    
    // Test subtraction
    auto result_sub = vec2 - vec1;
    EXPECT_EQ(result_sub.size(), 3);
    EXPECT_EQ(result_sub[0], static_cast<T>(3));
    EXPECT_EQ(result_sub[1], static_cast<T>(3));
    EXPECT_EQ(result_sub[2], static_cast<T>(3));
    
    // Test scalar multiplication
    auto result_mul = vec1 * static_cast<T>(2);
    EXPECT_EQ(result_mul.size(), 3);
    EXPECT_EQ(result_mul[0], static_cast<T>(2));
    EXPECT_EQ(result_mul[1], static_cast<T>(4));
    EXPECT_EQ(result_mul[2], static_cast<T>(6));
    
    // Test scalar division
    auto result_div = vec1 / static_cast<T>(2);
    EXPECT_EQ(result_div.size(), 3);
    EXPECT_SCALAR_NEAR(result_div[0], static_cast<T>(0.5), static_cast<T>(1e-10));
    EXPECT_SCALAR_NEAR(result_div[1], static_cast<T>(1.0), static_cast<T>(1e-10));
    EXPECT_SCALAR_NEAR(result_div[2], static_cast<T>(1.5), static_cast<T>(1e-10));
}

TYPED_TEST(VectorOperationsTest, VectorCompoundAssignment) {
    using T = TypeParam;
    
    auto vec1 = LinearAlgebraFactory<T>::createVector({1, 2, 3});
    auto vec2 = LinearAlgebraFactory<T>::createVector({4, 5, 6});
    
    // Test +=
    vec1 += vec2;
    EXPECT_EQ(vec1[0], static_cast<T>(5));
    EXPECT_EQ(vec1[1], static_cast<T>(7));
    EXPECT_EQ(vec1[2], static_cast<T>(9));
    
    // Test -=
    vec1 -= vec2;
    EXPECT_EQ(vec1[0], static_cast<T>(1));
    EXPECT_EQ(vec1[1], static_cast<T>(2));
    EXPECT_EQ(vec1[2], static_cast<T>(3));
    
    // Test *=
    vec1 *= static_cast<T>(3);
    EXPECT_EQ(vec1[0], static_cast<T>(3));
    EXPECT_EQ(vec1[1], static_cast<T>(6));
    EXPECT_EQ(vec1[2], static_cast<T>(9));
    
    // Test /=
    vec1 /= static_cast<T>(3);
    EXPECT_EQ(vec1[0], static_cast<T>(1));
    EXPECT_EQ(vec1[1], static_cast<T>(2));
    EXPECT_EQ(vec1[2], static_cast<T>(3));
}

TYPED_TEST(VectorOperationsTest, VectorDotProduct) {
    using T = TypeParam;
    
    auto vec1 = LinearAlgebraFactory<T>::createVector({1, 2, 3});
    auto vec2 = LinearAlgebraFactory<T>::createVector({4, 5, 6});
    
    // Test dot product: (1*4 + 2*5 + 3*6) = 4 + 10 + 18 = 32
    T dot_result = vec1.dot(vec2);
    EXPECT_SCALAR_NEAR(dot_result, static_cast<T>(32), static_cast<T>(1e-10));
    
    // Test orthogonal vectors
    auto vec3 = LinearAlgebraFactory<T>::createVector({1, 0, 0});
    auto vec4 = LinearAlgebraFactory<T>::createVector({0, 1, 0});
    T dot_orthogonal = vec3.dot(vec4);
    EXPECT_SCALAR_NEAR(dot_orthogonal, static_cast<T>(0), static_cast<T>(1e-10));
}

TYPED_TEST(VectorOperationsTest, VectorCrossProduct) {
    using T = TypeParam;
    
    // Test standard basis vectors
    auto i = LinearAlgebraFactory<T>::createVector({1, 0, 0});
    auto j = LinearAlgebraFactory<T>::createVector({0, 1, 0});
    auto k = LinearAlgebraFactory<T>::createVector({0, 0, 1});
    
    // i × j = k
    auto cross_ij = i.cross(j);
    EXPECT_VECTOR_NEAR(cross_ij, k, static_cast<T>(1e-10));
    
    // j × k = i
    auto cross_jk = j.cross(k);
    EXPECT_VECTOR_NEAR(cross_jk, i, static_cast<T>(1e-10));
    
    // k × i = j
    auto cross_ki = k.cross(i);
    EXPECT_VECTOR_NEAR(cross_ki, j, static_cast<T>(1e-10));
    
    // Test anti-commutativity: a × b = -(b × a)
    auto vec1 = LinearAlgebraFactory<T>::createVector({1, 2, 3});
    auto vec2 = LinearAlgebraFactory<T>::createVector({4, 5, 6});
    auto cross12 = vec1.cross(vec2);
    auto cross21 = vec2.cross(vec1);
    auto neg_cross21 = cross21 * static_cast<T>(-1);
    EXPECT_VECTOR_NEAR(cross12, neg_cross21, static_cast<T>(1e-10));
}

TYPED_TEST(VectorOperationsTest, VectorNorms) {
    using T = TypeParam;
    
    // Test unit vector norms
    auto unit_x = LinearAlgebraFactory<T>::createVector({1, 0, 0});
    EXPECT_SCALAR_NEAR(unit_x.norm(), static_cast<T>(1), static_cast<T>(1e-10));
    EXPECT_SCALAR_NEAR(unit_x.norm2(), static_cast<T>(1), static_cast<T>(1e-10));
    
    // Test 3-4-5 triangle
    auto vec345 = LinearAlgebraFactory<T>::createVector({3, 4, 0});
    EXPECT_SCALAR_NEAR(vec345.norm(), static_cast<T>(5), static_cast<T>(1e-10));
    EXPECT_SCALAR_NEAR(vec345.norm2(), static_cast<T>(25), static_cast<T>(1e-10));
    
    // Test general case
    auto vec = LinearAlgebraFactory<T>::createVector({1, 2, 3});
    T expected_norm = std::sqrt(static_cast<T>(1 + 4 + 9));
    EXPECT_SCALAR_NEAR(vec.norm(), expected_norm, static_cast<T>(1e-10));
    EXPECT_SCALAR_NEAR(vec.norm2(), static_cast<T>(14), static_cast<T>(1e-10));
}

TYPED_TEST(VectorOperationsTest, VectorNormalization) {
    using T = TypeParam;
    
    auto vec = LinearAlgebraFactory<T>::createVector({3, 4, 0});
    
    // Test normalized() - creates new vector
    auto normalized_vec = vec.normalized();
    EXPECT_SCALAR_NEAR(normalized_vec.norm(), static_cast<T>(1), static_cast<T>(1e-10));
    EXPECT_SCALAR_NEAR(normalized_vec[0], static_cast<T>(0.6), static_cast<T>(1e-10));
    EXPECT_SCALAR_NEAR(normalized_vec[1], static_cast<T>(0.8), static_cast<T>(1e-10));
    EXPECT_SCALAR_NEAR(normalized_vec[2], static_cast<T>(0), static_cast<T>(1e-10));
    
    // Original vector should be unchanged
    EXPECT_SCALAR_NEAR(vec.norm(), static_cast<T>(5), static_cast<T>(1e-10));
    
    // Test normalize() - modifies in-place
    vec.normalize();
    EXPECT_SCALAR_NEAR(vec.norm(), static_cast<T>(1), static_cast<T>(1e-10));
    EXPECT_SCALAR_NEAR(vec[0], static_cast<T>(0.6), static_cast<T>(1e-10));
    EXPECT_SCALAR_NEAR(vec[1], static_cast<T>(0.8), static_cast<T>(1e-10));
    EXPECT_SCALAR_NEAR(vec[2], static_cast<T>(0), static_cast<T>(1e-10));
}

TYPED_TEST(VectorOperationsTest, VectorUtilityFunctions) {
    using T = TypeParam;
    
    auto vec = LinearAlgebraFactory<T>::createVector(5);
    
    // Test fill
    vec.fill(static_cast<T>(7));
    for (size_t i = 0; i < vec.size(); ++i) {
        EXPECT_EQ(vec[i], static_cast<T>(7));
    }
    
    // Test zeros
    vec.zeros();
    for (size_t i = 0; i < vec.size(); ++i) {
        EXPECT_EQ(vec[i], static_cast<T>(0));
    }
    
    // Test ones
    vec.ones();
    for (size_t i = 0; i < vec.size(); ++i) {
        EXPECT_EQ(vec[i], static_cast<T>(1));
    }
    
    // Test toStdVector
    auto std_vec = vec.toStdVector();
    EXPECT_EQ(std_vec.size(), 5);
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(std_vec[i], static_cast<T>(1));
    }
}

TYPED_TEST(VectorOperationsTest, VectorCopyAndMove) {
    using T = TypeParam;
    
    auto original = LinearAlgebraFactory<T>::createVector({1, 2, 3, 4, 5});
    
    // Test copy constructor
    auto copied(original);
    EXPECT_EQ(copied.size(), original.size());
    for (size_t i = 0; i < original.size(); ++i) {
        EXPECT_EQ(copied[i], original[i]);
    }
    
    // Test copy assignment
    auto assigned = LinearAlgebraFactory<T>::createVector(3);
    assigned = original;
    EXPECT_EQ(assigned.size(), original.size());
    for (size_t i = 0; i < original.size(); ++i) {
        EXPECT_EQ(assigned[i], original[i]);
    }
    
    // Test move constructor
    auto moved(std::move(copied));
    EXPECT_EQ(moved.size(), 5);
    EXPECT_EQ(moved[0], static_cast<T>(1));
    EXPECT_EQ(moved[4], static_cast<T>(5));
    
    // Test move assignment
    auto move_assigned = LinearAlgebraFactory<T>::createVector(2);
    move_assigned = std::move(assigned);
    EXPECT_EQ(move_assigned.size(), 5);
    EXPECT_EQ(move_assigned[0], static_cast<T>(1));
    EXPECT_EQ(move_assigned[4], static_cast<T>(5));
}

} // namespace testing
} // namespace linalg
} // namespace multibody