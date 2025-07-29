/**
 * @file ArmadilloBackend.cpp
 * @brief Implementation of the Armadillo-based linear algebra backend
 * @author Multibody Dynamics Solver Team
 */

#include "linalg/ArmadilloBackend.hpp"

#include <sstream>

namespace multibody {
namespace linalg {

//==============================================================================
// ArmadilloBackend Implementation
//==============================================================================

template <typename T>
Vector<T> ArmadilloBackend<T>::createVector(size_t size) const {
    Vector<T> vec(
        std::const_pointer_cast<ArmadilloBackend<T>>(
            std::static_pointer_cast<const ArmadilloBackend<T>>(this->shared_from_this())),
        size);

    // Create Armadillo vector
    auto* arma_vec = new arma::Col<T>(size);
    arma_vec->zeros();
    vec.data_ = arma_vec;

    return vec;
}

template <typename T>
Vector<T> ArmadilloBackend<T>::createVector(const std::vector<T>& data) const {
    Vector<T> vec(
        std::const_pointer_cast<ArmadilloBackend<T>>(
            std::static_pointer_cast<const ArmadilloBackend<T>>(this->shared_from_this())),
        data.size());

    // Create Armadillo vector from std::vector
    auto* arma_vec = new arma::Col<T>(data);
    vec.data_ = arma_vec;

    return vec;
}

template <typename T>
Vector<T> ArmadilloBackend<T>::createVector(std::initializer_list<T> data) const {
    std::vector<T> vec_data(data);
    return createVector(vec_data);
}

template <typename T>
Matrix<T> ArmadilloBackend<T>::createMatrix(size_t rows, size_t cols) const {
    Matrix<T> mat(
        std::const_pointer_cast<ArmadilloBackend<T>>(
            std::static_pointer_cast<const ArmadilloBackend<T>>(this->shared_from_this())),
        rows, cols);

    // Create Armadillo matrix
    auto* arma_mat = new arma::Mat<T>(rows, cols);
    arma_mat->zeros();
    mat.data_ = arma_mat;

    return mat;
}

template <typename T>
Matrix<T> ArmadilloBackend<T>::createMatrix(const std::vector<std::vector<T>>& data) const {
    if (data.empty()) {
        return createMatrix(0, 0);
    }

    size_t rows = data.size();
    size_t cols = data[0].size();

    Matrix<T> mat(
        std::const_pointer_cast<ArmadilloBackend<T>>(
            std::static_pointer_cast<const ArmadilloBackend<T>>(this->shared_from_this())),
        rows, cols);

    // Create Armadillo matrix from 2D vector
    auto* arma_mat = new arma::Mat<T>(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            (*arma_mat)(i, j) = data[i][j];
        }
    }
    mat.data_ = arma_mat;

    return mat;
}

template <typename T>
Matrix<T> ArmadilloBackend<T>::createIdentity(size_t size) const {
    Matrix<T> mat(
        std::const_pointer_cast<ArmadilloBackend<T>>(
            std::static_pointer_cast<const ArmadilloBackend<T>>(this->shared_from_this())),
        size, size);

    // Create identity matrix
    auto* arma_mat = new arma::Mat<T>();
    *arma_mat = arma::eye<arma::Mat<T>>(size, size);
    mat.data_ = arma_mat;

    return mat;
}

template <typename T>
Matrix<T> ArmadilloBackend<T>::createZeros(size_t rows, size_t cols) const {
    Matrix<T> mat(
        std::const_pointer_cast<ArmadilloBackend<T>>(
            std::static_pointer_cast<const ArmadilloBackend<T>>(this->shared_from_this())),
        rows, cols);

    // Create zero matrix
    auto* arma_mat = new arma::Mat<T>();
    *arma_mat = arma::zeros<arma::Mat<T>>(rows, cols);
    mat.data_ = arma_mat;

    return mat;
}

template <typename T>
Matrix<T> ArmadilloBackend<T>::createOnes(size_t rows, size_t cols) const {
    Matrix<T> mat(
        std::const_pointer_cast<ArmadilloBackend<T>>(
            std::static_pointer_cast<const ArmadilloBackend<T>>(this->shared_from_this())),
        rows, cols);

    // Create ones matrix
    auto* arma_mat = new arma::Mat<T>();
    *arma_mat = arma::ones<arma::Mat<T>>(rows, cols);
    mat.data_ = arma_mat;

    return mat;
}

template <typename T>
Vector<T> ArmadilloBackend<T>::solve(const Matrix<T>& A, const Vector<T>& b) const {
    const auto& arma_A = getArmaMatrix(A);
    const auto& arma_b = getArmaVector(b);

    // Solve linear system
    arma::Col<T> result = arma::solve(arma_A, arma_b);

    return createVectorFromArma(result);
}

template <typename T>
Matrix<T> ArmadilloBackend<T>::inverse(const Matrix<T>& A) const {
    const auto& arma_A = getArmaMatrix(A);

    // Compute matrix inverse
    arma::Mat<T> result = arma::inv(arma_A);

    return createMatrixFromArma(result);
}

template <typename T>
T ArmadilloBackend<T>::determinant(const Matrix<T>& A) const {
    const auto& arma_A = getArmaMatrix(A);

    // Compute determinant
    return arma::det(arma_A);
}

template <typename T>
std::vector<T> ArmadilloBackend<T>::eigenvalues(const Matrix<T>& A) const {
    const auto& arma_A = getArmaMatrix(A);

    // Compute eigenvalues
    arma::Col<T> eigvals;
    arma::eig_sym(eigvals, arma_A);

    // Convert to std::vector
    std::vector<T> result(eigvals.begin(), eigvals.end());
    return result;
}

template <typename T>
std::string ArmadilloBackend<T>::getBackendVersion() const {
    std::ostringstream oss;
    oss << arma::arma_version::major << "." << arma::arma_version::minor << "."
        << arma::arma_version::patch;
    return oss.str();
}

template <typename T>
arma::Col<T>& ArmadilloBackend<T>::getArmaVector(Vector<T>& vec) {
    return *static_cast<arma::Col<T>*>(vec.getBackendData());
}

template <typename T>
const arma::Col<T>& ArmadilloBackend<T>::getArmaVector(const Vector<T>& vec) {
    return *static_cast<const arma::Col<T>*>(vec.getBackendData());
}

template <typename T>
arma::Mat<T>& ArmadilloBackend<T>::getArmaMatrix(Matrix<T>& mat) {
    return *static_cast<arma::Mat<T>*>(mat.getBackendData());
}

template <typename T>
const arma::Mat<T>& ArmadilloBackend<T>::getArmaMatrix(const Matrix<T>& mat) {
    return *static_cast<const arma::Mat<T>*>(mat.getBackendData());
}

template <typename T>
std::shared_ptr<ArmadilloBackend<T>> ArmadilloBackend<T>::create() {
    return std::shared_ptr<ArmadilloBackend<T>>(new ArmadilloBackend<T>());
}

template <typename T>
Vector<T> ArmadilloBackend<T>::createVectorFromArma(const arma::Col<T>& arma_vec) const {
    Vector<T> vec(
        std::const_pointer_cast<ArmadilloBackend<T>>(
            std::static_pointer_cast<const ArmadilloBackend<T>>(this->shared_from_this())),
        arma_vec.n_elem);

    auto* new_arma_vec = new arma::Col<T>(arma_vec);
    vec.data_ = new_arma_vec;

    return vec;
}

template <typename T>
Matrix<T> ArmadilloBackend<T>::createMatrixFromArma(const arma::Mat<T>& arma_mat) const {
    Matrix<T> mat(
        std::const_pointer_cast<ArmadilloBackend<T>>(
            std::static_pointer_cast<const ArmadilloBackend<T>>(this->shared_from_this())),
        arma_mat.n_rows, arma_mat.n_cols);

    auto* new_arma_mat = new arma::Mat<T>(arma_mat);
    mat.data_ = new_arma_mat;

    return mat;
}

//==============================================================================
// Utility Functions
//==============================================================================

namespace armadillo_utils {

template <typename T>
void initializeAsDefault() {
    auto backend = ArmadilloBackend<T>::create();
    LinearAlgebraFactory<T>::setDefaultBackend(backend);
}

bool isArmadilloAvailable() {
    try {
        arma::vec test_vec(3);
        test_vec.ones();
        return true;
    } catch (...) {
        return false;
    }
}

std::string getArmadilloInfo() {
    std::ostringstream oss;
    oss << "Armadillo " << arma::arma_version::major << "." << arma::arma_version::minor << "."
        << arma::arma_version::patch;

    oss << " (LAPACK: " << (arma::arma_config::lapack ? "Yes" : "No") << ")";
    oss << " (BLAS: " << (arma::arma_config::blas ? "Yes" : "No") << ")";

    return oss.str();
}

void configureArmadillo(bool use_blas, bool use_lapack) {
    // Note: Armadillo configuration is typically done at compile time
    // This function exists for API completeness but has limited runtime effect
    if (!use_blas || !use_lapack) {
        std::cerr << "Warning: BLAS/LAPACK configuration is compile-time only" << std::endl;
    }
}

// Explicit instantiations for utility functions
template void initializeAsDefault<double>();
template void initializeAsDefault<float>();

} // namespace armadillo_utils

//==============================================================================
// Explicit template instantiations
//==============================================================================

template class ArmadilloBackend<double>;
template class ArmadilloBackend<float>;

} // namespace linalg
} // namespace multibody