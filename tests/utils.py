"""
Shared test utilities for Multibody Dynamics Solver tests
多體動力學求解器測試的共享工具
"""

import os
import subprocess
import sys
from pathlib import Path
from typing import Any, Dict, List, Optional


def get_project_root() -> Path:
    """Get the project root directory"""
    return Path(__file__).parent.parent

def get_build_directory() -> Optional[Path]:
    """Find the build directory containing the compiled module"""
    project_root = get_project_root()
    
    possible_paths = [
        project_root / 'build_cmake',
        project_root / 'build',
    ]
    
    for path in possible_paths:
        if path.exists():
            # Check if it contains the multibody_solver module
            module_files = list(path.glob('multibody_solver*'))
            if module_files:
                return path
    
    return None

def run_command(cmd: List[str], cwd: Optional[Path] = None) -> Dict[str, Any]:
    """
    Run a command and return the result
    執行命令並返回結果
    """
    if cwd is None:
        cwd = get_project_root()
    
    try:
        result = subprocess.run(
            cmd,
            cwd=cwd,
            capture_output=True,
            text=True,
            check=False
        )
        return {
            'returncode': result.returncode,
            'stdout': result.stdout,
            'stderr': result.stderr,
            'success': result.returncode == 0
        }
    except Exception as e:
        return {
            'returncode': -1,
            'stdout': '',
            'stderr': str(e),
            'success': False
        }

def check_file_syntax(file_path: Path) -> bool:
    """
    Check if a Python file has valid syntax
    檢查 Python 檔案是否有有效語法
    """
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            compile(f.read(), str(file_path), 'exec')
        return True
    except SyntaxError:
        return False
    except Exception:
        return False

def get_python_files(directory: Path) -> List[Path]:
    """
    Get all Python files in a directory recursively
    遞歸獲取目錄中的所有 Python 檔案
    """
    python_files = []
    for file_path in directory.rglob('*.py'):
        # Skip __pycache__ and .pytest_cache directories
        if '__pycache__' not in str(file_path) and '.pytest_cache' not in str(file_path):
            python_files.append(file_path)
    return python_files

def get_cpp_files(directory: Path) -> List[Path]:
    """
    Get all C++ files in a directory recursively
    遞歸獲取目錄中的所有 C++ 檔案
    """
    cpp_files = []
    extensions = ['*.cpp', '*.hpp', '*.h', '*.cc', '*.cxx']
    
    for ext in extensions:
        cpp_files.extend(directory.rglob(ext))
    
    return cpp_files

def is_module_available(module_name: str) -> bool:
    """
    Check if a Python module is available for import
    檢查 Python 模組是否可以導入
    """
    try:
        __import__(module_name)
        return True
    except ImportError:
        return False

def get_git_root() -> Optional[Path]:
    """Get the git repository root directory"""
    try:
        result = subprocess.run(
            ['git', 'rev-parse', '--show-toplevel'],
            capture_output=True,
            text=True,
            check=True
        )
        return Path(result.stdout.strip())
    except (subprocess.CalledProcessError, FileNotFoundError):
        return None

class TestEnvironment:
    """Helper class to manage test environment"""
    
    def __init__(self):
        self.project_root = get_project_root()
        self.build_dir = get_build_directory()
        self.git_root = get_git_root()
    
    def has_build(self) -> bool:
        """Check if the project has been built"""
        return self.build_dir is not None
    
    def has_pyside6(self) -> bool:
        """Check if PySide6 is available"""
        return is_module_available('PySide6')
    
    def has_multibody_solver(self) -> bool:
        """Check if multibody_solver module is available"""
        if not self.has_build():
            return False
        
        # Add build directory to path temporarily
        if str(self.build_dir) not in sys.path:
            sys.path.insert(0, str(self.build_dir))
        
        return is_module_available('multibody_solver')
    
    def get_requirements(self) -> List[str]:
        """Get list of missing requirements"""
        missing = []
        
        if not self.has_build():
            missing.append("Project needs to be built (run ./build_python.sh)")
        
        if not self.has_pyside6():
            missing.append("PySide6 not installed (pip install PySide6)")
        
        return missing