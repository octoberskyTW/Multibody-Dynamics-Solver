/**
 * @file LinearAlgebra.cpp
 * @brief Implementation of the linear algebra abstraction layer
 * @author Multibody Dynamics Solver Team
 */

#include "linalg/LinearAlgebra.hpp"

#include <algorithm>
#include <cmath>

namespace multibody {
namespace linalg {

// Static member initialization
template <typename T>
std::shared_ptr<LinearAlgebraBackend<T>> LinearAlgebraFactory<T>::default_backend_ = nullptr;

//==============================================================================
// Vector Implementation
//==============================================================================

template <typename T>
Vector<T>::Vector(std::shared_ptr<LinearAlgebraBackend<T>> backend, size_t size)
    : backend_(backend), data_(nullptr), size_(size) {
    if (!backend_) {
        throw LinearAlgebraException("Backend is null");
    }
}

template <typename T>
Vector<T>::Vector(std::shared_ptr<LinearAlgebraBackend<T>> backend, const std::vector<T>& data)
    : backend_(backend), data_(nullptr), size_(data.size()) {
    if (!backend_) {
        throw LinearAlgebraException("Backend is null");
    }
}

template <typename T>
Vector<T>::Vector(std::shared_ptr<LinearAlgebraBackend<T>> backend, std::initializer_list<T> data)
    : backend_(backend), data_(nullptr), size_(data.size()) {
    if (!backend_) {
        throw LinearAlgebraException("Backend is null");
    }
}

template <typename T>
Vector<T>::Vector(const Vector& other)
    : backend_(other.backend_), data_(nullptr), size_(other.size_) {
    // Backend-specific copy will be implemented in derived classes
}

template <typename T>
Vector<T>::Vector(Vector&& other) noexcept
    : backend_(std::move(other.backend_)), data_(other.data_), size_(other.size_) {
    other.data_ = nullptr;
    other.size_ = 0;
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& other) {
    if (this != &other) {
        backend_ = other.backend_;
        size_ = other.size_;
        // Backend-specific copy will be implemented in derived classes
    }
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector&& other) noexcept {
    if (this != &other) {
        backend_ = std::move(other.backend_);
        data_ = other.data_;
        size_ = other.size_;
        other.data_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

template <typename T>
Vector<T>::~Vector() {
    // Backend-specific cleanup will be implemented in derived classes
}

template <typename T>
T& Vector<T>::at(size_t index) {
    if (index >= size_) {
        throw LinearAlgebraException("Vector index out of bounds");
    }
    return (*this)[index];
}

template <typename T>
const T& Vector<T>::at(size_t index) const {
    if (index >= size_) {
        throw LinearAlgebraException("Vector index out of bounds");
    }
    return (*this)[index];
}

//==============================================================================
// Matrix Implementation
//==============================================================================

template <typename T>
Matrix<T>::Matrix(std::shared_ptr<LinearAlgebraBackend<T>> backend, size_t rows, size_t cols)
    : backend_(backend), data_(nullptr), rows_(rows), cols_(cols) {
    if (!backend_) {
        throw LinearAlgebraException("Backend is null");
    }
}

template <typename T>
Matrix<T>::Matrix(std::shared_ptr<LinearAlgebraBackend<T>> backend,
                  const std::vector<std::vector<T>>& data)
    : backend_(backend),
      data_(nullptr),
      rows_(data.size()),
      cols_(data.empty() ? 0 : data[0].size()) {
    if (!backend_) {
        throw LinearAlgebraException("Backend is null");
    }

    // Validate that all rows have the same number of columns
    for (const auto& row : data) {
        if (row.size() != cols_) {
            throw LinearAlgebraException("Inconsistent matrix dimensions");
        }
    }
}

template <typename T>
Matrix<T>::Matrix(const Matrix& other)
    : backend_(other.backend_), data_(nullptr), rows_(other.rows_), cols_(other.cols_) {
    // Backend-specific copy will be implemented in derived classes
}

template <typename T>
Matrix<T>::Matrix(Matrix&& other) noexcept
    : backend_(std::move(other.backend_)),
      data_(other.data_),
      rows_(other.rows_),
      cols_(other.cols_) {
    other.data_ = nullptr;
    other.rows_ = 0;
    other.cols_ = 0;
}

template <typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix& other) {
    if (this != &other) {
        backend_ = other.backend_;
        rows_ = other.rows_;
        cols_ = other.cols_;
        // Backend-specific copy will be implemented in derived classes
    }
    return *this;
}

template <typename T>
Matrix<T>& Matrix<T>::operator=(Matrix&& other) noexcept {
    if (this != &other) {
        backend_ = std::move(other.backend_);
        data_ = other.data_;
        rows_ = other.rows_;
        cols_ = other.cols_;
        other.data_ = nullptr;
        other.rows_ = 0;
        other.cols_ = 0;
    }
    return *this;
}

template <typename T>
Matrix<T>::~Matrix() {
    // Backend-specific cleanup will be implemented in derived classes
}

template <typename T>
T& Matrix<T>::at(size_t row, size_t col) {
    if (row >= rows_ || col >= cols_) {
        throw LinearAlgebraException("Matrix index out of bounds");
    }
    return (*this)(row, col);
}

template <typename T>
const T& Matrix<T>::at(size_t row, size_t col) const {
    if (row >= rows_ || col >= cols_) {
        throw LinearAlgebraException("Matrix index out of bounds");
    }
    return (*this)(row, col);
}

//==============================================================================
// LinearAlgebraFactory Implementation
//==============================================================================

template <typename T>
void LinearAlgebraFactory<T>::setDefaultBackend(std::shared_ptr<LinearAlgebraBackend<T>> backend) {
    default_backend_ = backend;
}

template <typename T>
std::shared_ptr<LinearAlgebraBackend<T>> LinearAlgebraFactory<T>::getDefaultBackend() {
    if (!default_backend_) {
        throw LinearAlgebraException("No default backend set. Please set a backend first.");
    }
    return default_backend_;
}

template <typename T>
Vector<T> LinearAlgebraFactory<T>::createVector(size_t size) {
    return getDefaultBackend()->createVector(size);
}

template <typename T>
Vector<T> LinearAlgebraFactory<T>::createVector(const std::vector<T>& data) {
    return getDefaultBackend()->createVector(data);
}

template <typename T>
Vector<T> LinearAlgebraFactory<T>::createVector(std::initializer_list<T> data) {
    return getDefaultBackend()->createVector(data);
}

template <typename T>
Matrix<T> LinearAlgebraFactory<T>::createMatrix(size_t rows, size_t cols) {
    return getDefaultBackend()->createMatrix(rows, cols);
}

template <typename T>
Matrix<T> LinearAlgebraFactory<T>::createMatrix(const std::vector<std::vector<T>>& data) {
    return getDefaultBackend()->createMatrix(data);
}

template <typename T>
Matrix<T> LinearAlgebraFactory<T>::createIdentity(size_t size) {
    return getDefaultBackend()->createIdentity(size);
}

template <typename T>
Matrix<T> LinearAlgebraFactory<T>::createZeros(size_t rows, size_t cols) {
    return getDefaultBackend()->createZeros(rows, cols);
}

template <typename T>
Matrix<T> LinearAlgebraFactory<T>::createOnes(size_t rows, size_t cols) {
    return getDefaultBackend()->createOnes(rows, cols);
}

template <typename T>
Vector<T> LinearAlgebraFactory<T>::solve(const Matrix<T>& A, const Vector<T>& b) {
    return getDefaultBackend()->solve(A, b);
}

template <typename T>
Matrix<T> LinearAlgebraFactory<T>::inverse(const Matrix<T>& A) {
    return getDefaultBackend()->inverse(A);
}

template <typename T>
T LinearAlgebraFactory<T>::determinant(const Matrix<T>& A) {
    return getDefaultBackend()->determinant(A);
}

template <typename T>
std::vector<T> LinearAlgebraFactory<T>::eigenvalues(const Matrix<T>& A) {
    return getDefaultBackend()->eigenvalues(A);
}

//==============================================================================
// Scalar multiplication operators
//==============================================================================

template <typename T>
Vector<T> operator*(T scalar, const Vector<T>& vec) {
    return vec * scalar;
}

template <typename T>
Matrix<T> operator*(T scalar, const Matrix<T>& mat) {
    return mat * scalar;
}

//==============================================================================
// Explicit template instantiations
//==============================================================================

// Vector instantiations
template class Vector<double>;
template class Vector<float>;

// Matrix instantiations
template class Matrix<double>;
template class Matrix<float>;

// LinearAlgebraFactory instantiations
template class LinearAlgebraFactory<double>;
template class LinearAlgebraFactory<float>;

// Operator instantiations
template Vector<double> operator*(double scalar, const Vector<double>& vec);
template Vector<float> operator*(float scalar, const Vector<float>& vec);
template Matrix<double> operator*(double scalar, const Matrix<double>& mat);
template Matrix<float> operator*(float scalar, const Matrix<float>& mat);

} // namespace linalg
} // namespace multibody