#!/bin/bash

# Auto-formatting script for Multibody Dynamics Solver
# 多體動力學求解器自動格式化腳本

set -e  # Exit on error

echo "========================================================"
echo "Auto-formatting code / 自動格式化代碼"
echo "========================================================"

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if we're in the correct directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found. Please run this script from the project root."
    exit 1
fi

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

echo -e "\n1. Formatting C++ code (clang-format)..."
echo "=========================================="

if command_exists clang-format; then
    # Find all C++ files
    cpp_files=$(find src include python_bindings -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.cc" -o -name "*.cxx" 2>/dev/null || true)
    
    if [ -n "$cpp_files" ]; then
        echo "Formatting C++ files..."
        for file in $cpp_files; do
            if [ -f "$file" ]; then
                echo "  Formatting: $file"
                clang-format -i --style=file "$file"
            fi
        done
        echo -e "${GREEN}✓ C++ files formatted${NC}"
    else
        echo "No C++ files found to format"
    fi
else
    echo -e "${YELLOW}⚠ clang-format not found. Install with: brew install clang-format (macOS) or apt install clang-format (Linux)${NC}"
fi

echo -e "\n2. Formatting Python code (black)..."
echo "====================================="

if command_exists black; then
    echo "Running black formatter..."
    black .
    echo -e "${GREEN}✓ Python files formatted${NC}"
else
    echo -e "${YELLOW}⚠ black not found. Install with: pip install black${NC}"
fi

echo -e "\n3. Sorting Python imports (isort)..."
echo "====================================="

if command_exists isort; then
    echo "Running isort..."
    isort .
    echo -e "${GREEN}✓ Python imports sorted${NC}"
else
    echo -e "${YELLOW}⚠ isort not found. Install with: pip install isort${NC}"
fi

echo -e "\n========================================================"
echo -e "${GREEN}Auto-formatting completed!${NC}"
echo "========================================================"
echo ""
echo "All code has been automatically formatted according to:"
echo "  C++: LLVM style (via .clang-format)"
echo "  Python: Black style (via black and isort)"
echo ""
echo "You can now run ./lint.sh to verify code quality."
echo "========================================================"