/**
 * @file test_linear_algebra.hpp
 * @brief Test suite for the linear algebra abstraction layer
 * @author Multibody Dynamics Solver Team
 * 
 * This file contains comprehensive tests for the linear algebra abstraction layer,
 * including tests for vectors, matrices, and different backends.
 * 
 * 此文件包含線性代數抽象層的綜合測試，
 * 包括向量、矩陣和不同後端的測試。
 */

#pragma once

#include "linalg/LinearAlgebra.hpp"
#include "linalg/ArmadilloBackend.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <cmath>

namespace multibody {
namespace linalg {
namespace testing {

/**
 * @brief Test fixture for linear algebra tests
 * 線性代數測試的測試固定裝置
 */
template<typename T>
class LinearAlgebraTest : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;
    
    // Helper functions / 輔助函數
    bool isNear(T a, T b, T tolerance = static_cast<T>(1e-10)) const;
    bool isVectorNear(const Vector<T>& a, const Vector<T>& b, T tolerance = static_cast<T>(1e-10)) const;
    bool isMatrixNear(const Matrix<T>& a, const Matrix<T>& b, T tolerance = static_cast<T>(1e-10)) const;
    
    std::shared_ptr<LinearAlgebraBackend<T>> backend_;
};

/**
 * @brief Test fixture specifically for Armadillo backend
 * 專門用於 Armadillo 後端的測試固定裝置
 */
template<typename T>
class ArmadilloBackendTest : public LinearAlgebraTest<T> {
protected:
    void SetUp() override;
};

/**
 * @brief Parameterized test fixture for testing multiple backends
 * 用於測試多個後端的參數化測試固定裝置
 */
template<typename T>
class MultiBackendTest : public ::testing::TestWithParam<std::shared_ptr<LinearAlgebraBackend<T>>> {
protected:
    void SetUp() override;
    
    std::shared_ptr<LinearAlgebraBackend<T>> getBackend() const {
        return GetParam();
    }
};

//==============================================================================
// Test Categories / 測試類別
//==============================================================================

/**
 * @brief Tests for vector operations
 * 向量運算測試
 */
template<typename T>
class VectorOperationsTest : public LinearAlgebraTest<T> {};

/**
 * @brief Tests for matrix operations
 * 矩陣運算測試
 */
template<typename T>
class MatrixOperationsTest : public LinearAlgebraTest<T> {};

/**
 * @brief Tests for linear algebra algorithms
 * 線性代數算法測試
 */
template<typename T>
class LinearAlgebraAlgorithmsTest : public LinearAlgebraTest<T> {};

/**
 * @brief Performance tests
 * 性能測試
 */
template<typename T>
class PerformanceTest : public LinearAlgebraTest<T> {};

/**
 * @brief Backend-specific tests
 * 後端特定測試
 */
template<typename T>
class BackendTest : public LinearAlgebraTest<T> {};

//==============================================================================
// Test Utilities / 測試工具
//==============================================================================

/**
 * @brief Generate test data for vectors and matrices
 * 為向量和矩陣生成測試數據
 */
template<typename T>
class TestDataGenerator {
public:
    static std::vector<T> generateRandomVector(size_t size, T min_val = -10, T max_val = 10);
    static std::vector<std::vector<T>> generateRandomMatrix(size_t rows, size_t cols, 
                                                           T min_val = -10, T max_val = 10);
    static std::vector<std::vector<T>> generateSymmetricMatrix(size_t size, 
                                                              T min_val = -10, T max_val = 10);
    static std::vector<std::vector<T>> generatePositiveDefiniteMatrix(size_t size);
    static std::vector<std::vector<T>> generateOrthogonalMatrix(size_t size);
};

/**
 * @brief Benchmark utilities for performance testing
 * 用於性能測試的基準工具
 */
template<typename T>
class BenchmarkUtils {
public:
    struct BenchmarkResult {
        double elapsed_time_ms;
        std::string operation_name;
        size_t data_size;
    };
    
    static BenchmarkResult benchmarkVectorOperation(
        const std::string& name,
        std::function<void()> operation,
        size_t vector_size
    );
    
    static BenchmarkResult benchmarkMatrixOperation(
        const std::string& name,
        std::function<void()> operation,
        size_t matrix_size
    );
    
    static void printBenchmarkResults(const std::vector<BenchmarkResult>& results);
};

//==============================================================================
// Test Macros / 測試宏
//==============================================================================

#define EXPECT_VECTOR_NEAR(vec1, vec2, tolerance) \
    EXPECT_TRUE(isVectorNear(vec1, vec2, tolerance)) \
        << "Vectors are not equal within tolerance " << tolerance

#define EXPECT_MATRIX_NEAR(mat1, mat2, tolerance) \
    EXPECT_TRUE(isMatrixNear(mat1, mat2, tolerance)) \
        << "Matrices are not equal within tolerance " << tolerance

#define EXPECT_SCALAR_NEAR(val1, val2, tolerance) \
    EXPECT_TRUE(isNear(val1, val2, tolerance)) \
        << "Scalars " << val1 << " and " << val2 \
        << " are not equal within tolerance " << tolerance

//==============================================================================
// Typed test definitions / 類型化測試定義
//==============================================================================

using TestTypes = ::testing::Types<float, double>;

TYPED_TEST_SUITE(LinearAlgebraTest, TestTypes);
TYPED_TEST_SUITE(ArmadilloBackendTest, TestTypes);
TYPED_TEST_SUITE(VectorOperationsTest, TestTypes);
TYPED_TEST_SUITE(MatrixOperationsTest, TestTypes);
TYPED_TEST_SUITE(LinearAlgebraAlgorithmsTest, TestTypes);
TYPED_TEST_SUITE(PerformanceTest, TestTypes);
TYPED_TEST_SUITE(BackendTest, TestTypes);

} // namespace testing
} // namespace linalg
} // namespace multibody