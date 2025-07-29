#!/bin/bash
# Build script for Multibody Dynamics Solver Python bindings
# 多體動力學求解器 Python 綁定構建腳本

set -e  # Exit on any error

echo "Building Multibody Dynamics Solver Python bindings..."
echo "構建多體動力學求解器 Python 綁定..."

# Create build directory
BUILD_DIR="build_cmake"
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning existing build directory..."
    echo "清理現有構建目錄..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo ""
echo "=== Configuration ==="
echo "Build directory: $(pwd)"
echo "Source directory: $(dirname $(pwd))"

# Configure with CMake
echo ""
echo "Configuring with CMake..."
echo "使用 CMake 配置..."
cmake -DCMAKE_BUILD_TYPE=Release \
      -DUSE_ARMADILLO=ON \
      -DBUILD_LINALG_TESTS=OFF \
      ..

# Build the project
echo ""
echo "Building project..."
echo "構建專案..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Copy the Python module to the parent directory for easy access
echo ""
echo "Copying Python module to project root..."
echo "將 Python 模組複製到專案根目錄..."
cp multibody_solver*.so ../

echo ""
echo "=== Build Complete ==="
echo "Python module: $(ls -la ../multibody_solver*.so)"
echo "Executable: $(ls -la MultibodyDynamicsSolver)"

echo ""
echo "You can now test the Python bindings:"
echo "現在可以測試 Python 綁定："
echo "  python3 -c \"import multibody_solver; print('Success!')\""
echo "  python3 legacy_tests/test_python_bindings.py"
echo "  python3 python_gui/simple_gui.py"
echo ""
echo "Or run the tests:"
echo "或運行測試："
echo "  python3 -m pytest tests/"
echo ""

cd ..
echo "Build completed successfully! 構建成功完成！"