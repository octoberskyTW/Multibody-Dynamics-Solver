#!/bin/bash

# Lint script for Multibody Dynamics Solver
# 多體動力學求解器代碼檢查腳本

set -e  # Exit on error

echo "========================================================"
echo "Running code quality checks / 運行代碼質量檢查"
echo "========================================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if we're in the correct directory
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}Error: CMakeLists.txt not found. Please run this script from the project root.${NC}"
    exit 1
fi

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

echo -e "\n1. C++ Code Formatting Check (clang-format)..."
echo "================================================"

if command_exists clang-format; then
    echo "✓ clang-format found"
    
    # Find all C++ files
    cpp_files=$(find src include python_bindings -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.cc" -o -name "*.cxx" 2>/dev/null || true)
    
    if [ -n "$cpp_files" ]; then
        echo "Checking C++ files for formatting..."
        format_issues=0
        
        for file in $cpp_files; do
            if [ -f "$file" ]; then
                echo "  Checking: $file"
                # Check if file needs formatting
                if ! clang-format --style=file --dry-run --Werror "$file" >/dev/null 2>&1; then
                    echo -e "    ${YELLOW}⚠ Formatting issues found${NC}"
                    format_issues=$((format_issues + 1))
                else
                    echo -e "    ${GREEN}✓ OK${NC}"
                fi
            fi
        done
        
        if [ $format_issues -gt 0 ]; then
            echo -e "\n${YELLOW}Found formatting issues in $format_issues files.${NC}"
            echo "To fix, run: clang-format -i \$(find src include python_bindings -name '*.cpp' -o -name '*.hpp' -o -name '*.h' -o -name '*.cc' -o -name '*.cxx')"
        else
            echo -e "\n${GREEN}✓ All C++ files are properly formatted${NC}"
        fi
    else
        echo "No C++ files found to check"
    fi
else
    echo -e "${YELLOW}⚠ clang-format not found. Install with: brew install clang-format (macOS) or apt install clang-format (Linux)${NC}"
fi

echo -e "\n2. Python Code Quality Check (flake8)..."
echo "========================================="

if command_exists flake8; then
    echo "✓ flake8 found"
    
    # Find Python files
    python_files=$(find . -name "*.py" -not -path "./build*" -not -path "./_deps*" -not -path "./.git*" -not -path "./__pycache__*" -not -path "./.pytest_cache*")
    
    if [ -n "$python_files" ]; then
        echo "Running flake8 on Python files..."
        if flake8 .; then
            echo -e "${GREEN}✓ All Python files pass flake8 checks${NC}"
        else
            echo -e "${RED}✗ flake8 found issues${NC}"
        fi
    else
        echo "No Python files found to check"
    fi
else
    echo -e "${YELLOW}⚠ flake8 not found. Install with: pip install flake8${NC}"
fi

echo -e "\n3. Python Import Sorting Check (isort)..."
echo "=========================================="

if command_exists isort; then
    echo "✓ isort found"
    
    echo "Checking import sorting..."
    if isort --check-only --diff .; then
        echo -e "${GREEN}✓ All imports are properly sorted${NC}"
    else
        echo -e "${YELLOW}⚠ Some imports need sorting. Run: isort .${NC}"
    fi
else
    echo -e "${YELLOW}⚠ isort not found. Install with: pip install isort${NC}"
fi

echo -e "\n4. Python Code Formatting Check (black)..."
echo "==========================================="

if command_exists black; then
    echo "✓ black found"
    
    echo "Checking Python code formatting..."
    if black --check --diff .; then
        echo -e "${GREEN}✓ All Python files are properly formatted${NC}"
    else
        echo -e "${YELLOW}⚠ Some files need formatting. Run: black .${NC}"
    fi
else
    echo -e "${YELLOW}⚠ black not found. Install with: pip install black${NC}"
fi

echo -e "\n========================================================"
echo -e "${GREEN}Code quality check completed!${NC}"
echo "========================================================"
echo ""
echo "To install all linting tools:"
echo "  C++: brew install clang-format (macOS) or apt install clang-format (Linux)"
echo "  Python: pip install flake8 black isort"
echo ""
echo "To auto-fix issues:"
echo "  C++: clang-format -i \$(find src include python_bindings -name '*.cpp' -o -name '*.hpp' -o -name '*.h')"
echo "  Python: black . && isort ."
echo "========================================================"