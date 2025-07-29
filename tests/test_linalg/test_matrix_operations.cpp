/**
 * @file test_matrix_operations.cpp
 * @brief Tests for matrix operations in the linear algebra abstraction layer
 * @author Multibody Dynamics Solver Team
 */

#include "test_linear_algebra.hpp"
#include <gtest/gtest.h>

namespace multibody {
namespace linalg {
namespace testing {

//==============================================================================
// MatrixOperationsTest Implementation
//==============================================================================

TYPED_TEST(MatrixOperationsTest, MatrixCreation) {
    using T = TypeParam;
    
    // Test empty matrix
    auto mat1 = LinearAlgebraFactory<T>::createMatrix(0, 0);
    EXPECT_EQ(mat1.rows(), 0);
    EXPECT_EQ(mat1.cols(), 0);
    EXPECT_TRUE(mat1.empty());
    
    // Test matrix with specific dimensions
    auto mat2 = LinearAlgebraFactory<T>::createMatrix(3, 4);
    EXPECT_EQ(mat2.rows(), 3);
    EXPECT_EQ(mat2.cols(), 4);
    EXPECT_EQ(mat2.size(), 12);
    EXPECT_FALSE(mat2.empty());
    EXPECT_FALSE(mat2.isSquare());
    
    // Test square matrix
    auto mat3 = LinearAlgebraFactory<T>::createMatrix(3, 3);
    EXPECT_TRUE(mat3.isSquare());
    
    // Test matrix from 2D vector
    std::vector<std::vector<T>> data = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    auto mat4 = LinearAlgebraFactory<T>::createMatrix(data);
    EXPECT_EQ(mat4.rows(), 3);
    EXPECT_EQ(mat4.cols(), 3);
    EXPECT_TRUE(mat4.isSquare());
    
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            EXPECT_EQ(mat4(i, j), data[i][j]);
        }
    }
}

TYPED_TEST(MatrixOperationsTest, SpecialMatrixCreation) {
    using T = TypeParam;
    
    // Test identity matrix
    auto identity = LinearAlgebraFactory<T>::createIdentity(3);
    EXPECT_EQ(identity.rows(), 3);
    EXPECT_EQ(identity.cols(), 3);
    EXPECT_TRUE(identity.isSquare());
    
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            if (i == j) {
                EXPECT_EQ(identity(i, j), static_cast<T>(1));
            } else {
                EXPECT_EQ(identity(i, j), static_cast<T>(0));
            }
        }
    }
    
    // Test zeros matrix
    auto zeros = LinearAlgebraFactory<T>::createZeros(2, 3);
    EXPECT_EQ(zeros.rows(), 2);
    EXPECT_EQ(zeros.cols(), 3);
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            EXPECT_EQ(zeros(i, j), static_cast<T>(0));
        }
    }
    
    // Test ones matrix
    auto ones = LinearAlgebraFactory<T>::createOnes(2, 3);
    EXPECT_EQ(ones.rows(), 2);
    EXPECT_EQ(ones.cols(), 3);
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            EXPECT_EQ(ones(i, j), static_cast<T>(1));
        }
    }
}

TYPED_TEST(MatrixOperationsTest, MatrixElementAccess) {
    using T = TypeParam;
    
    std::vector<std::vector<T>> data = {
        {1, 2, 3},
        {4, 5, 6}
    };
    auto mat = LinearAlgebraFactory<T>::createMatrix(data);
    
    // Test operator()
    EXPECT_EQ(mat(0, 0), static_cast<T>(1));
    EXPECT_EQ(mat(0, 2), static_cast<T>(3));
    EXPECT_EQ(mat(1, 1), static_cast<T>(5));
    
    // Test at() method
    EXPECT_EQ(mat.at(1, 0), static_cast<T>(4));
    EXPECT_EQ(mat.at(1, 2), static_cast<T>(6));
    
    // Test bounds checking
    EXPECT_THROW(mat.at(2, 0), LinearAlgebraException);
    EXPECT_THROW(mat.at(0, 3), LinearAlgebraException);
    
    // Test modification
    mat(0, 0) = static_cast<T>(10);
    EXPECT_EQ(mat(0, 0), static_cast<T>(10));
    
    mat.at(1, 1) = static_cast<T>(20);
    EXPECT_EQ(mat.at(1, 1), static_cast<T>(20));
}

TYPED_TEST(MatrixOperationsTest, MatrixArithmetic) {
    using T = TypeParam;
    
    auto mat1 = LinearAlgebraFactory<T>::createMatrix({{1, 2}, {3, 4}});
    auto mat2 = LinearAlgebraFactory<T>::createMatrix({{5, 6}, {7, 8}});
    
    // Test addition
    auto result_add = mat1 + mat2;
    EXPECT_EQ(result_add.rows(), 2);
    EXPECT_EQ(result_add.cols(), 2);
    EXPECT_EQ(result_add(0, 0), static_cast<T>(6));
    EXPECT_EQ(result_add(0, 1), static_cast<T>(8));
    EXPECT_EQ(result_add(1, 0), static_cast<T>(10));
    EXPECT_EQ(result_add(1, 1), static_cast<T>(12));
    
    // Test subtraction
    auto result_sub = mat2 - mat1;
    EXPECT_EQ(result_sub(0, 0), static_cast<T>(4));
    EXPECT_EQ(result_sub(0, 1), static_cast<T>(4));
    EXPECT_EQ(result_sub(1, 0), static_cast<T>(4));
    EXPECT_EQ(result_sub(1, 1), static_cast<T>(4));
    
    // Test scalar multiplication
    auto result_mul = mat1 * static_cast<T>(2);
    EXPECT_EQ(result_mul(0, 0), static_cast<T>(2));
    EXPECT_EQ(result_mul(0, 1), static_cast<T>(4));
    EXPECT_EQ(result_mul(1, 0), static_cast<T>(6));
    EXPECT_EQ(result_mul(1, 1), static_cast<T>(8));
    
    // Test scalar division
    auto result_div = mat1 / static_cast<T>(2);
    EXPECT_SCALAR_NEAR(result_div(0, 0), static_cast<T>(0.5), static_cast<T>(1e-10));
    EXPECT_SCALAR_NEAR(result_div(0, 1), static_cast<T>(1.0), static_cast<T>(1e-10));
    EXPECT_SCALAR_NEAR(result_div(1, 0), static_cast<T>(1.5), static_cast<T>(1e-10));
    EXPECT_SCALAR_NEAR(result_div(1, 1), static_cast<T>(2.0), static_cast<T>(1e-10));
}

TYPED_TEST(MatrixOperationsTest, MatrixMultiplication) {
    using T = TypeParam;
    
    // Test matrix-matrix multiplication
    auto mat1 = LinearAlgebraFactory<T>::createMatrix({{1, 2}, {3, 4}});
    auto mat2 = LinearAlgebraFactory<T>::createMatrix({{5, 6}, {7, 8}});
    
    auto result = mat1 * mat2;
    EXPECT_EQ(result.rows(), 2);
    EXPECT_EQ(result.cols(), 2);
    
    // Expected result: [[1*5+2*7, 1*6+2*8], [3*5+4*7, 3*6+4*8]] = [[19, 22], [43, 50]]
    EXPECT_EQ(result(0, 0), static_cast<T>(19));
    EXPECT_EQ(result(0, 1), static_cast<T>(22));
    EXPECT_EQ(result(1, 0), static_cast<T>(43));
    EXPECT_EQ(result(1, 1), static_cast<T>(50));
    
    // Test identity matrix multiplication
    auto identity = LinearAlgebraFactory<T>::createIdentity(2);
    auto result_identity = mat1 * identity;
    EXPECT_MATRIX_NEAR(result_identity, mat1, static_cast<T>(1e-10));
    
    auto result_identity2 = identity * mat1;
    EXPECT_MATRIX_NEAR(result_identity2, mat1, static_cast<T>(1e-10));
}

TYPED_TEST(MatrixOperationsTest, MatrixVectorMultiplication) {
    using T = TypeParam;
    
    auto mat = LinearAlgebraFactory<T>::createMatrix({{1, 2, 3}, {4, 5, 6}});
    auto vec = LinearAlgebraFactory<T>::createVector({7, 8, 9});
    
    auto result = mat * vec;
    EXPECT_EQ(result.size(), 2);
    
    // Expected result: [1*7+2*8+3*9, 4*7+5*8+6*9] = [50, 122]
    EXPECT_EQ(result[0], static_cast<T>(50));
    EXPECT_EQ(result[1], static_cast<T>(122));
}

TYPED_TEST(MatrixOperationsTest, MatrixTranspose) {
    using T = TypeParam;
    
    auto mat = LinearAlgebraFactory<T>::createMatrix({{1, 2, 3}, {4, 5, 6}});
    auto transposed = mat.transpose();
    
    EXPECT_EQ(transposed.rows(), 3);
    EXPECT_EQ(transposed.cols(), 2);
    
    EXPECT_EQ(transposed(0, 0), static_cast<T>(1));
    EXPECT_EQ(transposed(0, 1), static_cast<T>(4));
    EXPECT_EQ(transposed(1, 0), static_cast<T>(2));
    EXPECT_EQ(transposed(1, 1), static_cast<T>(5));
    EXPECT_EQ(transposed(2, 0), static_cast<T>(3));
    EXPECT_EQ(transposed(2, 1), static_cast<T>(6));
    
    // Test transpose of transpose
    auto double_transposed = transposed.transpose();
    EXPECT_MATRIX_NEAR(double_transposed, mat, static_cast<T>(1e-10));
}

TYPED_TEST(MatrixOperationsTest, MatrixInverse) {
    using T = TypeParam;
    
    // Test 2x2 matrix inverse
    auto mat = LinearAlgebraFactory<T>::createMatrix({{1, 2}, {3, 4}});
    auto inv = mat.inverse();
    
    EXPECT_EQ(inv.rows(), 2);
    EXPECT_EQ(inv.cols(), 2);
    
    // Verify A * A^-1 = I
    auto product = mat * inv;
    auto identity = LinearAlgebraFactory<T>::createIdentity(2);
    EXPECT_MATRIX_NEAR(product, identity, static_cast<T>(1e-10));
    
    // Test 3x3 matrix inverse
    auto mat3x3 = LinearAlgebraFactory<T>::createMatrix({
        {2, -1, 0},
        {-1, 2, -1},
        {0, -1, 2}
    });
    auto inv3x3 = mat3x3.inverse();
    
    auto product3x3 = mat3x3 * inv3x3;
    auto identity3x3 = LinearAlgebraFactory<T>::createIdentity(3);
    EXPECT_MATRIX_NEAR(product3x3, identity3x3, static_cast<T>(1e-8));
}

TYPED_TEST(MatrixOperationsTest, MatrixDeterminant) {
    using T = TypeParam;
    
    // Test 2x2 determinant
    auto mat2x2 = LinearAlgebraFactory<T>::createMatrix({{1, 2}, {3, 4}});
    T det2x2 = mat2x2.determinant();
    // det([[1,2],[3,4]]) = 1*4 - 2*3 = -2
    EXPECT_SCALAR_NEAR(det2x2, static_cast<T>(-2), static_cast<T>(1e-10));
    
    // Test 3x3 determinant
    auto mat3x3 = LinearAlgebraFactory<T>::createMatrix({
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    });
    T det3x3 = mat3x3.determinant();
    // This is a singular matrix, so determinant should be 0
    EXPECT_SCALAR_NEAR(det3x3, static_cast<T>(0), static_cast<T>(1e-10));
    
    // Test identity matrix determinant
    auto identity = LinearAlgebraFactory<T>::createIdentity(3);
    T det_identity = identity.determinant();
    EXPECT_SCALAR_NEAR(det_identity, static_cast<T>(1), static_cast<T>(1e-10));
}

TYPED_TEST(MatrixOperationsTest, MatrixTrace) {
    using T = TypeParam;
    
    auto mat = LinearAlgebraFactory<T>::createMatrix({
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    });
    
    T trace = mat.trace();
    // trace = 1 + 5 + 9 = 15
    EXPECT_SCALAR_NEAR(trace, static_cast<T>(15), static_cast<T>(1e-10));
    
    // Test identity matrix trace
    auto identity = LinearAlgebraFactory<T>::createIdentity(4);
    T trace_identity = identity.trace();
    EXPECT_SCALAR_NEAR(trace_identity, static_cast<T>(4), static_cast<T>(1e-10));
}

TYPED_TEST(MatrixOperationsTest, MatrixUtilityFunctions) {
    using T = TypeParam;
    
    auto mat = LinearAlgebraFactory<T>::createMatrix(2, 3);
    
    // Test fill
    mat.fill(static_cast<T>(7));
    for (size_t i = 0; i < mat.rows(); ++i) {
        for (size_t j = 0; j < mat.cols(); ++j) {
            EXPECT_EQ(mat(i, j), static_cast<T>(7));
        }
    }
    
    // Test zeros
    mat.zeros();
    for (size_t i = 0; i < mat.rows(); ++i) {
        for (size_t j = 0; j < mat.cols(); ++j) {
            EXPECT_EQ(mat(i, j), static_cast<T>(0));
        }
    }
    
    // Test ones
    mat.ones();
    for (size_t i = 0; i < mat.rows(); ++i) {
        for (size_t j = 0; j < mat.cols(); ++j) {
            EXPECT_EQ(mat(i, j), static_cast<T>(1));
        }
    }
    
    // Test identity (square matrix only)
    auto square_mat = LinearAlgebraFactory<T>::createMatrix(3, 3);
    square_mat.identity();
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            if (i == j) {
                EXPECT_EQ(square_mat(i, j), static_cast<T>(1));
            } else {
                EXPECT_EQ(square_mat(i, j), static_cast<T>(0));
            }
        }
    }
    
    // Test toStdVector
    auto original = LinearAlgebraFactory<T>::createMatrix({{1, 2}, {3, 4}});
    auto std_vec = original.toStdVector();
    EXPECT_EQ(std_vec.size(), 2);
    EXPECT_EQ(std_vec[0].size(), 2);
    EXPECT_EQ(std_vec[0][0], static_cast<T>(1));
    EXPECT_EQ(std_vec[0][1], static_cast<T>(2));
    EXPECT_EQ(std_vec[1][0], static_cast<T>(3));
    EXPECT_EQ(std_vec[1][1], static_cast<T>(4));
}

TYPED_TEST(MatrixOperationsTest, MatrixCompoundAssignment) {
    using T = TypeParam;
    
    auto mat1 = LinearAlgebraFactory<T>::createMatrix({{1, 2}, {3, 4}});
    auto mat2 = LinearAlgebraFactory<T>::createMatrix({{5, 6}, {7, 8}});
    
    // Test +=
    mat1 += mat2;
    EXPECT_EQ(mat1(0, 0), static_cast<T>(6));
    EXPECT_EQ(mat1(0, 1), static_cast<T>(8));
    EXPECT_EQ(mat1(1, 0), static_cast<T>(10));
    EXPECT_EQ(mat1(1, 1), static_cast<T>(12));
    
    // Test -=
    mat1 -= mat2;
    EXPECT_EQ(mat1(0, 0), static_cast<T>(1));
    EXPECT_EQ(mat1(0, 1), static_cast<T>(2));
    EXPECT_EQ(mat1(1, 0), static_cast<T>(3));
    EXPECT_EQ(mat1(1, 1), static_cast<T>(4));
    
    // Test *=
    mat1 *= static_cast<T>(2);
    EXPECT_EQ(mat1(0, 0), static_cast<T>(2));
    EXPECT_EQ(mat1(0, 1), static_cast<T>(4));
    EXPECT_EQ(mat1(1, 0), static_cast<T>(6));
    EXPECT_EQ(mat1(1, 1), static_cast<T>(8));
    
    // Test /=
    mat1 /= static_cast<T>(2);
    EXPECT_EQ(mat1(0, 0), static_cast<T>(1));
    EXPECT_EQ(mat1(0, 1), static_cast<T>(2));
    EXPECT_EQ(mat1(1, 0), static_cast<T>(3));
    EXPECT_EQ(mat1(1, 1), static_cast<T>(4));
}

} // namespace testing
} // namespace linalg
} // namespace multibody