# Testing and Code Quality Setup / 測試與代碼品質設置

This document describes the comprehensive testing and code quality framework for the Multibody Dynamics Solver project.

本文檔描述了多體動力學求解器專案的綜合測試和代碼品質框架。

## Overview / 概述

The project now includes:
- **Modern pytest-based testing framework** / **現代 pytest 測試框架**
- **C++ code formatting with clang-format (LLVM style)** / **使用 clang-format 的 C++ 代碼格式化（LLVM 風格）**
- **Python code quality with flake8, black, and isort** / **使用 flake8、black 和 isort 的 Python 代碼品質**
- **Automated testing integration** / **自動化測試整合**

## Quick Start / 快速開始

### 1. Install Dependencies / 安裝依賴

```bash
# Install Python development tools
pip install -r requirements.txt

# Install C++ formatting tools (macOS)
brew install clang-format

# Install C++ formatting tools (Linux)
sudo apt install clang-format
```

### 2. Run Tests / 運行測試

```bash
# Build and test
./build_python.sh

# Build with linting
./build_python.sh --lint

# Run tests only
pytest tests/

# Run legacy tests
python3 legacy_tests/test_python_bindings.py
```

### 3. Code Quality / 代碼品質

```bash
# Check code quality
./lint.sh

# Auto-format code
./format.sh
```

## File Structure / 檔案結構

```
Multibody-Dynamics-Solver/
├── tests/                          # Modern pytest test suite
│   ├── __init__.py
│   ├── conftest.py                 # Pytest configuration and fixtures
│   ├── test_multibody_solver.py    # Core solver tests
│   ├── test_gui.py                 # GUI functionality tests
│   ├── utils.py                    # Shared test utilities
│   └── README.md                   # Testing documentation
├── legacy_tests/                   # Legacy test scripts
│   ├── test_python_bindings.py     # Original Python binding tests
│   └── test_pyside6_gui.py         # Original GUI tests
├── .clang-format                   # C++ formatting configuration
├── .flake8                         # Python linting configuration
├── pyproject.toml                  # Modern Python project config
├── pytest.ini                     # Pytest configuration
├── lint.sh                        # Code quality check script
├── format.sh                      # Auto-formatting script
└── requirements.txt                # Updated with dev dependencies
```

## Testing Framework / 測試框架

### Modern Pytest Setup / 現代 Pytest 設置

- **Comprehensive test coverage** with unit and integration tests / **全面的測試覆蓋率**，包含單元和整合測試
- **Automatic dependency handling** with smart skipping / **自動依賴處理**，具備智能跳過功能
- **Parameterized tests** for different configurations / **參數化測試**適用於不同配置
- **Session-scoped fixtures** for expensive operations / **會話範圍的固定裝置**用於昂貴操作

### Test Categories / 測試類別

| Marker | Description | 標記 | 描述 |
|--------|-------------|------|------|
| `unit` | Unit tests for individual components | 單元測試 | 個別組件的單元測試 |
| `integration` | Integration tests for system functionality | 整合測試 | 系統功能的整合測試 |
| `gui` | GUI-related tests (requires PySide6) | GUI測試 | GUI相關測試（需要PySide6） |
| `slow` | Slow running tests | 慢速測試 | 運行緩慢的測試 |
| `requires_build` | Tests requiring compiled C++ module | 需要構建 | 需要編譯C++模組的測試 |

### Running Specific Tests / 運行特定測試

```bash
# Run only unit tests
pytest tests/ -m "unit"

# Run integration tests
pytest tests/ -m "integration"

# Run GUI tests (if PySide6 available)
pytest tests/ -m "gui"

# Run tests that don't require building
pytest tests/ -m "not requires_build"

# Run with coverage
pytest tests/ --cov=python_gui --cov-report=html
```

## Code Quality Framework / 代碼品質框架

### C++ Code Quality / C++ 代碼品質

- **clang-format** with LLVM style / 使用 LLVM 風格的 **clang-format**
- **Automatic formatting** via `./format.sh` / 透過 `./format.sh` **自動格式化**
- **CI-ready checking** via `./lint.sh` / 透過 `./lint.sh` **CI就緒檢查**

#### C++ Configuration (.clang-format)
- Base style: LLVM
- Column limit: 100
- Indent width: 4 spaces
- Pointer alignment: Left
- Include sorting: Enabled

### Python Code Quality / Python 代碼品質

- **flake8** for linting / **flake8** 用於代碼檢查
- **black** for formatting / **black** 用於格式化
- **isort** for import sorting / **isort** 用於導入排序

#### Python Configuration
- Line length: 100 characters / 行長度：100 字符
- Import style: Google / 導入風格：Google
- Docstring convention: Google / 文檔字符串約定：Google

### Quality Check Commands / 品質檢查命令

```bash
# Check all code quality
./lint.sh

# Auto-fix formatting issues
./format.sh

# Individual tools
clang-format -i src/*.cpp include/*.hpp    # C++ formatting
black .                                    # Python formatting
isort .                                    # Python import sorting
flake8 .                                   # Python linting
```

## Integration with Build System / 與建構系統整合

### Build Script Integration / 建構腳本整合

```bash
# Normal build and test
./build_python.sh

# Build with code quality checks
./build_python.sh --lint
```

### Automated Workflow / 自動化工作流程

1. **Build C++ module** / **構建 C++ 模組**
2. **Run pytest tests** (or legacy if pytest unavailable) / **運行 pytest 測試**（如果 pytest 不可用則使用舊版）
3. **Optional: Code quality checks** (with --lint flag) / **可選：代碼品質檢查**（使用 --lint 標誌）

## Development Workflow / 開發工作流程

### Recommended Workflow / 推薦工作流程

1. **Write code** / **編寫代碼**
2. **Format code**: `./format.sh` / **格式化代碼**：`./format.sh`
3. **Check quality**: `./lint.sh` / **檢查品質**：`./lint.sh`
4. **Run tests**: `pytest tests/` / **運行測試**：`pytest tests/`
5. **Build and validate**: `./build_python.sh --lint` / **構建和驗證**：`./build_python.sh --lint`

### Pre-commit Workflow / 提交前工作流程

```bash
# Format and check before commit
./format.sh && ./lint.sh && pytest tests/
```

## Configuration Files / 配置檔案

### Key Configuration Files / 關鍵配置檔案

- **`.clang-format`**: C++ formatting rules / C++ 格式化規則
- **`.flake8`**: Python linting configuration / Python 代碼檢查配置
- **`pyproject.toml`**: Modern Python project settings / 現代 Python 專案設置
- **`pytest.ini`**: Pytest-specific configuration / Pytest 特定配置
- **`requirements.txt`**: All dependencies including dev tools / 所有依賴項包括開發工具

## Troubleshooting / 故障排除

### Common Issues / 常見問題

1. **pytest not found** / **找不到 pytest**
   ```bash
   pip install pytest
   ```

2. **clang-format not found** / **找不到 clang-format**
   ```bash
   # macOS
   brew install clang-format
   
   # Linux
   sudo apt install clang-format
   ```

3. **PySide6 tests skipped** / **PySide6 測試被跳過**
   ```bash
   pip install PySide6
   ```

4. **Module not found: multibody_solver** / **找不到模組：multibody_solver**
   ```bash
   ./build_python.sh
   ```

### Environment Check / 環境檢查

```python
from tests.utils import TestEnvironment
env = TestEnvironment()
print("Missing requirements:", env.get_requirements())
```

## Migration from Legacy Tests / 從舊版測試遷移

The legacy test files have been moved to `legacy_tests/` for backward compatibility:
- `legacy_tests/test_python_bindings.py`
- `legacy_tests/test_pyside6_gui.py`

舊版測試檔案已移至 `legacy_tests/` 以保持向後兼容性：
- `legacy_tests/test_python_bindings.py`
- `legacy_tests/test_pyside6_gui.py`

The build script automatically falls back to legacy tests if pytest is not available.

如果 pytest 不可用，建構腳本會自動回退到舊版測試。

## Benefits / 好處

### For Developers / 對開發者

- **Consistent code style** across the project / 專案中**一致的代碼風格**
- **Automated quality checks** / **自動化品質檢查**
- **Comprehensive test coverage** / **全面的測試覆蓋率**
- **Easy-to-use scripts** / **易於使用的腳本**

### For CI/CD / 對 CI/CD

- **Ready for continuous integration** / **準備好進行持續整合**
- **Standardized testing interface** / **標準化測試介面**
- **Automated formatting checks** / **自動化格式檢查**
- **Configurable quality gates** / **可配置的品質門檻**

## Future Enhancements / 未來增強

- **Pre-commit hooks** for automatic formatting / **預提交鉤子**用於自動格式化
- **Coverage reporting** integration / **覆蓋率報告**整合
- **Performance benchmarking** tests / **性能基準**測試
- **Documentation testing** / **文檔測試**