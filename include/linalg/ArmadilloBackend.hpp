/**
 * @file ArmadilloBackend.hpp
 * @brief Armadillo-based implementation of the linear algebra backend
 * @author Multibody Dynamics Solver Team
 *
 * This file implements the linear algebra abstraction layer using the Armadillo library.
 * Armadillo is a high-quality linear algebra library for C++.
 *
 * 此文件使用 Armadillo 庫實現線性代數抽象層。
 * Armadillo 是一個高質量的 C++ 線性代數庫。
 */

#pragma once

#include "LinearAlgebra.hpp"

#include <armadillo>
#include <memory>

namespace multibody {
namespace linalg {

/**
 * @brief Armadillo-based backend implementation
 * 基於 Armadillo 的後端實現
 */
template <typename T>
class ArmadilloBackend : public LinearAlgebraBackend<T>,
                         public std::enable_shared_from_this<ArmadilloBackend<T>> {
  public:
    ArmadilloBackend() = default;
    ~ArmadilloBackend() override = default;

    // Vector operations / 向量運算
    Vector<T> createVector(size_t size) const override;
    Vector<T> createVector(const std::vector<T>& data) const override;
    Vector<T> createVector(std::initializer_list<T> data) const override;

    // Matrix operations / 矩陣運算
    Matrix<T> createMatrix(size_t rows, size_t cols) const override;
    Matrix<T> createMatrix(const std::vector<std::vector<T>>& data) const override;
    Matrix<T> createIdentity(size_t size) const override;
    Matrix<T> createZeros(size_t rows, size_t cols) const override;
    Matrix<T> createOnes(size_t rows, size_t cols) const override;

    // Linear algebra operations / 線性代數運算
    Vector<T> solve(const Matrix<T>& A, const Vector<T>& b) const override;
    Matrix<T> inverse(const Matrix<T>& A) const override;
    T determinant(const Matrix<T>& A) const override;
    std::vector<T> eigenvalues(const Matrix<T>& A) const override;

    // Backend information / 後端信息
    std::string getBackendName() const override { return "Armadillo"; }
    std::string getBackendVersion() const override;

    // Armadillo-specific helper functions / Armadillo 特定的輔助函數
    static arma::Col<T>& getArmaVector(Vector<T>& vec);
    static const arma::Col<T>& getArmaVector(const Vector<T>& vec);
    static arma::Mat<T>& getArmaMatrix(Matrix<T>& mat);
    static const arma::Mat<T>& getArmaMatrix(const Matrix<T>& mat);

    // Factory method / 工廠方法
    static std::shared_ptr<ArmadilloBackend<T>> create();

  private:
    // Internal helper functions / 內部輔助函數
    Vector<T> createVectorFromArma(const arma::Col<T>& arma_vec) const;
    Matrix<T> createMatrixFromArma(const arma::Mat<T>& arma_mat) const;
};

/**
 * @brief Specializations for common types
 * 常見類型的特化
 */
using ArmadilloBackendD = ArmadilloBackend<double>;
using ArmadilloBackendF = ArmadilloBackend<float>;

/**
 * @brief Utility functions for Armadillo backend
 * Armadillo 後端的實用函數
 */
namespace armadillo_utils {

/**
 * @brief Initialize Armadillo backend as default
 * 將 Armadillo 後端初始化為默認後端
 */
template <typename T>
void initializeAsDefault();

/**
 * @brief Check if Armadillo is available and working
 * 檢查 Armadillo 是否可用且工作正常
 */
bool isArmadilloAvailable();

/**
 * @brief Get Armadillo library information
 * 獲取 Armadillo 庫信息
 */
std::string getArmadilloInfo();

/**
 * @brief Set Armadillo configuration
 * 設置 Armadillo 配置
 */
void configureArmadillo(bool use_blas = true, bool use_lapack = true);

} // namespace armadillo_utils

} // namespace linalg
} // namespace multibody