/**
 * @file LinearAlgebra.hpp
 * @brief Linear algebra abstraction layer for multibody dynamics solver
 * @author Multibody Dynamics Solver Team
 *
 * This header provides an abstraction layer for linear algebra operations,
 * allowing easy switching between different backends (Armadillo, Eigen, custom implementations).
 *
 * 此頭文件提供線性代數運算的抽象層，
 * 允許在不同後端（Armadillo、Eigen、自定義實現）之間輕鬆切換。
 */

#pragma once

#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace multibody {
namespace linalg {

// Forward declarations
template <typename T>
class Vector;
template <typename T>
class Matrix;

/**
 * @brief Exception thrown for linear algebra errors
 * 線性代數錯誤異常
 */
class LinearAlgebraException : public std::runtime_error {
  public:
    explicit LinearAlgebraException(const std::string& message)
        : std::runtime_error("LinearAlgebra Error: " + message) {}
};

/**
 * @brief Abstract base class for linear algebra backend implementations
 * 線性代數後端實現的抽象基類
 *
 * This class defines the interface that all backend implementations must provide.
 * 此類定義了所有後端實現必須提供的接口。
 */
template <typename T>
class LinearAlgebraBackend {
  public:
    virtual ~LinearAlgebraBackend() = default;

    // Vector operations / 向量運算
    virtual Vector<T> createVector(size_t size) const = 0;
    virtual Vector<T> createVector(const std::vector<T>& data) const = 0;
    virtual Vector<T> createVector(std::initializer_list<T> data) const = 0;

    // Matrix operations / 矩陣運算
    virtual Matrix<T> createMatrix(size_t rows, size_t cols) const = 0;
    virtual Matrix<T> createMatrix(const std::vector<std::vector<T>>& data) const = 0;
    virtual Matrix<T> createIdentity(size_t size) const = 0;
    virtual Matrix<T> createZeros(size_t rows, size_t cols) const = 0;
    virtual Matrix<T> createOnes(size_t rows, size_t cols) const = 0;

    // Linear algebra operations / 線性代數運算
    virtual Vector<T> solve(const Matrix<T>& A, const Vector<T>& b) const = 0;
    virtual Matrix<T> inverse(const Matrix<T>& A) const = 0;
    virtual T determinant(const Matrix<T>& A) const = 0;
    virtual std::vector<T> eigenvalues(const Matrix<T>& A) const = 0;

    // Backend information / 後端信息
    virtual std::string getBackendName() const = 0;
    virtual std::string getBackendVersion() const = 0;
};

/**
 * @brief Vector class with backend abstraction
 * 帶有後端抽象的向量類
 */
template <typename T>
class Vector {
  public:
    std::shared_ptr<LinearAlgebraBackend<T>> backend_;
    void* data_; // Backend-specific data pointer
    size_t size_;

  private:
    // Internal interface for backend implementations
    template <typename U>
    friend class LinearAlgebraBackend;

  public:
    // Constructors / 構造函數
    Vector(std::shared_ptr<LinearAlgebraBackend<T>> backend, size_t size);
    Vector(std::shared_ptr<LinearAlgebraBackend<T>> backend, const std::vector<T>& data);
    Vector(std::shared_ptr<LinearAlgebraBackend<T>> backend, std::initializer_list<T> data);

    // Copy and move semantics / 複製和移動語義
    Vector(const Vector& other);
    Vector(Vector&& other) noexcept;
    Vector& operator=(const Vector& other);
    Vector& operator=(Vector&& other) noexcept;

    // Destructor / 析構函數
    ~Vector();

    // Basic properties / 基本屬性
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    // Element access / 元素訪問
    T& operator[](size_t index);
    const T& operator[](size_t index) const;
    T& at(size_t index);
    const T& at(size_t index) const;

    // Vector operations / 向量運算
    Vector operator+(const Vector& other) const;
    Vector operator-(const Vector& other) const;
    Vector operator*(T scalar) const;
    Vector operator/(T scalar) const;
    Vector& operator+=(const Vector& other);
    Vector& operator-=(const Vector& other);
    Vector& operator*=(T scalar);
    Vector& operator/=(T scalar);

    // Mathematical operations / 數學運算
    T dot(const Vector& other) const;
    Vector cross(const Vector& other) const; // 3D only
    T norm() const;
    T norm2() const; // Squared norm
    Vector normalized() const;
    void normalize();

    // Utility functions / 實用函數
    std::vector<T> toStdVector() const;
    void fill(T value);
    void zeros();
    void ones();

    // Backend access / 後端訪問
    void* getBackendData() { return data_; }
    const void* getBackendData() const { return data_; }
    std::shared_ptr<LinearAlgebraBackend<T>> getBackend() { return backend_; }
};

/**
 * @brief Matrix class with backend abstraction
 * 帶有後端抽象的矩陣類
 */
template <typename T>
class Matrix {
  public:
    std::shared_ptr<LinearAlgebraBackend<T>> backend_;
    void* data_; // Backend-specific data pointer
    size_t rows_;
    size_t cols_;

  private:
    // Internal interface for backend implementations
    template <typename U>
    friend class LinearAlgebraBackend;

  public:
    // Constructors / 構造函數
    Matrix(std::shared_ptr<LinearAlgebraBackend<T>> backend, size_t rows, size_t cols);
    Matrix(std::shared_ptr<LinearAlgebraBackend<T>> backend,
           const std::vector<std::vector<T>>& data);

    // Copy and move semantics / 複製和移動語義
    Matrix(const Matrix& other);
    Matrix(Matrix&& other) noexcept;
    Matrix& operator=(const Matrix& other);
    Matrix& operator=(Matrix&& other) noexcept;

    // Destructor / 析構函數
    ~Matrix();

    // Basic properties / 基本屬性
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
    size_t size() const { return rows_ * cols_; }
    bool empty() const { return rows_ == 0 || cols_ == 0; }
    bool isSquare() const { return rows_ == cols_; }

    // Element access / 元素訪問
    T& operator()(size_t row, size_t col);
    const T& operator()(size_t row, size_t col) const;
    T& at(size_t row, size_t col);
    const T& at(size_t row, size_t col) const;

    // Matrix operations / 矩陣運算
    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    Matrix operator*(T scalar) const;
    Matrix operator/(T scalar) const;
    Vector<T> operator*(const Vector<T>& vec) const;
    Matrix& operator+=(const Matrix& other);
    Matrix& operator-=(const Matrix& other);
    Matrix& operator*=(const Matrix& other);
    Matrix& operator*=(T scalar);
    Matrix& operator/=(T scalar);

    // Matrix properties and operations / 矩陣屬性和運算
    Matrix transpose() const;
    Matrix inverse() const;
    T determinant() const;
    T trace() const;
    std::vector<T> eigenvalues() const;

    // Utility functions / 實用函數
    std::vector<std::vector<T>> toStdVector() const;
    void fill(T value);
    void zeros();
    void ones();
    void identity(); // Make this an identity matrix (square matrices only)

    // Backend access / 後端訪問
    void* getBackendData() { return data_; }
    const void* getBackendData() const { return data_; }
    std::shared_ptr<LinearAlgebraBackend<T>> getBackend() { return backend_; }
};

/**
 * @brief Linear algebra factory for creating vectors and matrices
 * 創建向量和矩陣的線性代數工廠
 */
template <typename T>
class LinearAlgebraFactory {
  private:
    static std::shared_ptr<LinearAlgebraBackend<T>> default_backend_;

  public:
    // Backend management / 後端管理
    static void setDefaultBackend(std::shared_ptr<LinearAlgebraBackend<T>> backend);
    static std::shared_ptr<LinearAlgebraBackend<T>> getDefaultBackend();

    // Vector creation / 向量創建
    static Vector<T> createVector(size_t size);
    static Vector<T> createVector(const std::vector<T>& data);
    static Vector<T> createVector(std::initializer_list<T> data);

    // Matrix creation / 矩陣創建
    static Matrix<T> createMatrix(size_t rows, size_t cols);
    static Matrix<T> createMatrix(const std::vector<std::vector<T>>& data);
    static Matrix<T> createIdentity(size_t size);
    static Matrix<T> createZeros(size_t rows, size_t cols);
    static Matrix<T> createOnes(size_t rows, size_t cols);

    // Linear algebra operations / 線性代數運算
    static Vector<T> solve(const Matrix<T>& A, const Vector<T>& b);
    static Matrix<T> inverse(const Matrix<T>& A);
    static T determinant(const Matrix<T>& A);
    static std::vector<T> eigenvalues(const Matrix<T>& A);
};

// Common type aliases / 常見類型別名
using VectorD = Vector<double>;
using VectorF = Vector<float>;
using MatrixD = Matrix<double>;
using MatrixF = Matrix<float>;

using LinearAlgebraFactoryD = LinearAlgebraFactory<double>;
using LinearAlgebraFactoryF = LinearAlgebraFactory<float>;

// Scalar multiplication operators / 標量乘法運算符
template <typename T>
Vector<T> operator*(T scalar, const Vector<T>& vec);

template <typename T>
Matrix<T> operator*(T scalar, const Matrix<T>& mat);

} // namespace linalg
} // namespace multibody