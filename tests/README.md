# Testing Framework / 測試框架

This directory contains the test suite for the Multibody Dynamics Solver project using pytest.

這個目錄包含使用 pytest 的多體動力學求解器專案測試套件。

## Test Structure / 測試結構

```
tests/
├── __init__.py           # Test package initialization
├── conftest.py          # Pytest configuration and shared fixtures
├── test_multibody_solver.py  # Core multibody solver tests
├── test_gui.py          # GUI functionality tests
├── utils.py             # Shared test utilities
└── README.md            # This file
```

## Running Tests / 運行測試

### Prerequisites / 前置要求

1. **Build the project** / **構建專案**:
   ```bash
   ./build_python.sh
   ```

2. **Install test dependencies** / **安裝測試依賴**:
   ```bash
   pip install -r requirements.txt
   ```

### Test Commands / 測試命令

1. **Run all tests** / **運行所有測試**:
   ```bash
   pytest tests/
   ```

2. **Run specific test categories** / **運行特定測試類別**:
   ```bash
   # Unit tests only
   pytest tests/ -m "unit"
   
   # Integration tests only  
   pytest tests/ -m "integration"
   
   # GUI tests (requires PySide6)
   pytest tests/ -m "gui"
   
   # Tests that require built module
   pytest tests/ -m "requires_build"
   ```

3. **Run with verbose output** / **詳細輸出運行**:
   ```bash
   pytest tests/ -v
   ```

4. **Run with coverage** / **運行覆蓋率測試**:
   ```bash
   pytest tests/ --cov=python_gui --cov-report=html
   ```

5. **Legacy test mode** / **舊版測試模式**:
   ```bash
   python3 test_python_bindings.py
   python3 test_pyside6_gui.py
   ```

## Test Categories / 測試類別

### Markers / 標記

- `unit`: Unit tests for individual components / 個別組件的單元測試
- `integration`: Integration tests for system functionality / 系統功能的整合測試
- `gui`: GUI-related tests (requires PySide6) / GUI 相關測試（需要 PySide6）
- `slow`: Slow running tests / 運行緩慢的測試
- `requires_build`: Tests that require compiled C++ module / 需要編譯 C++ 模組的測試

### Test Files / 測試檔案

#### test_multibody_solver.py
Tests for the core multibody solver functionality:
- Module import and basic functionality
- Dynamics system creation and management
- Body and joint creation
- Simulation stepping
- Parametrized tests for different configurations

核心多體求解器功能測試：
- 模組導入和基本功能
- 動力學系統創建和管理
- 物體和關節創建
- 模擬步驟
- 不同配置的參數化測試

#### test_gui.py
Tests for the PySide6 GUI functionality:
- GUI code syntax validation
- PySide6 import testing
- Animation features testing
- Export functionality testing
- Requirements validation

PySide6 GUI 功能測試：
- GUI 代碼語法驗證
- PySide6 導入測試
- 動畫功能測試
- 導出功能測試
- 需求驗證

## Fixtures / 固定裝置

### Available Fixtures / 可用固定裝置

- `multibody_solver`: Session-scoped fixture providing the compiled module
- `pyside6`: Session-scoped fixture for PySide6 availability
- `dynamics_system`: Basic dynamics system with ground body
- `sample_multibody_system`: Complete system with bodies and joints

## Configuration / 配置

### pytest.ini
Main pytest configuration with test discovery, output options, and markers.

主要 pytest 配置，包含測試發現、輸出選項和標記。

### pyproject.toml
Modern Python project configuration including:
- Black code formatting
- isort import sorting  
- pytest options
- coverage settings

現代 Python 專案配置，包括：
- Black 代碼格式化
- isort 導入排序
- pytest 選項
- 覆蓋率設定

## Troubleshooting / 故障排除

### Common Issues / 常見問題

1. **Module not found: multibody_solver**
   - Solution: Run `./build_python.sh` to build the C++ module
   - 解決方案：運行 `./build_python.sh` 來構建 C++ 模組

2. **Module not found: PySide6**
   - Solution: Install with `pip install PySide6`
   - 解決方案：使用 `pip install PySide6` 安裝

3. **Module not found: pytest**
   - Solution: Install with `pip install pytest`
   - 解決方案：使用 `pip install pytest` 安裝

4. **Tests skipped**
   - Tests are automatically skipped if dependencies are missing
   - This is normal behavior - install missing dependencies to run all tests
   - 如果缺少依賴項，測試會自動跳過
   - 這是正常行為 - 安裝缺少的依賴項以運行所有測試

### Environment Check / 環境檢查

Use the test utilities to check your environment:

使用測試工具檢查您的環境：

```python
from tests.utils import TestEnvironment

env = TestEnvironment()
print("Build available:", env.has_build())
print("PySide6 available:", env.has_pyside6())
print("Multibody solver available:", env.has_multibody_solver())
print("Missing requirements:", env.get_requirements())
```

## Integration with Build System / 與構建系統整合

The tests are integrated with the build system:

測試與構建系統整合：

- `./build_python.sh` - Runs tests after building
- `./build_python.sh --lint` - Runs tests and linting
- `./lint.sh` - Code quality checks only
- `./format.sh` - Auto-format code

- `./build_python.sh` - 構建後運行測試
- `./build_python.sh --lint` - 運行測試和代碼檢查
- `./lint.sh` - 僅代碼質量檢查
- `./format.sh` - 自動格式化代碼