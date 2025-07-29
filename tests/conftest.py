"""
Pytest configuration and shared fixtures
Pytest 配置和共享 fixtures
"""

import glob
import os
import sys
from pathlib import Path

import pytest

# Add project root to Python path
project_root = Path(__file__).parent.parent
sys.path.insert(0, str(project_root))

def find_and_import_multibody_solver():
    """
    Find and import the multibody_solver module from possible locations
    尋找並導入 multibody_solver 模組
    """
    # Get the project root directory
    project_root = Path(__file__).parent.parent
    
    # Possible locations for the Python module
    search_paths = [
        # Standard build directory
        project_root / 'build_cmake',
        # Alternative build directory
        project_root / 'build',
        # Project root
        project_root,
        # Current directory
        Path.cwd(),
        # Build directory relative to current working directory
        Path.cwd() / 'build_cmake',
        Path.cwd() / 'build'
    ]
    
    print("Searching for multibody_solver module...")
    
    for search_path in search_paths:
        if not search_path.exists():
            continue
            
        print(f"  Checking: {search_path}")
        
        # Look for multibody_solver shared libraries
        patterns = [
            'multibody_solver*.so',      # Linux
            'multibody_solver*.dylib',   # macOS
            'multibody_solver*.pyd',     # Windows
        ]
        
        found_files = []
        for pattern in patterns:
            found_files.extend(list(search_path.glob(pattern)))
        
        if found_files:
            print(f"  Found module files: {found_files}")
            
            # Add this path to sys.path if not already there
            abs_search_path = str(search_path.absolute())
            if abs_search_path not in sys.path:
                sys.path.insert(0, abs_search_path)
                print(f"  Added to Python path: {abs_search_path}")
            
            # Try to import the module
            try:
                import multibody_solver as mbs
                print(f"✓ Successfully imported multibody_solver from: {abs_search_path}")
                return mbs
            except ImportError as e:
                print(f"  Import failed: {e}")
                continue
    
    # If we get here, we couldn't find or import the module
    return None

@pytest.fixture(scope="session")
def multibody_solver():
    """
    Session-scoped fixture to import the multibody_solver module
    會話範圍的 fixture 來導入 multibody_solver 模組
    """
    mbs = find_and_import_multibody_solver()
    if mbs is None:
        pytest.skip("multibody_solver module not available - please build the project first")
    return mbs

@pytest.fixture(scope="session")
def pyside6():
    """
    Session-scoped fixture to check PySide6 availability
    會話範圍的 fixture 來檢查 PySide6 可用性
    """
    try:
        import PySide6
        return PySide6
    except ImportError:
        pytest.skip("PySide6 not available - install with: pip install PySide6")

@pytest.fixture
def dynamics_system(multibody_solver):
    """
    Create a basic dynamics system for testing
    創建基本的動力學系統用於測試
    """
    dt = 0.001
    system = multibody_solver.DynamicsSystem(dt)
    
    # Add ground body
    ground = multibody_solver.create_ground(0)
    system.Add(ground)
    
    return system

@pytest.fixture
def sample_multibody_system(multibody_solver):
    """
    Create a sample multibody system with multiple bodies for testing
    創建包含多個物體的樣本多體系統用於測試
    """
    dt = 0.001
    system = multibody_solver.DynamicsSystem(dt)
    
    # Add ground body
    ground = multibody_solver.create_ground(0)
    system.Add(ground)
    
    # Add mobilized body
    pos = [0.0, 0.0, 1.0]
    vel = [0.0, 0.0, 0.0]
    acc = [0.0, 0.0, 0.0]
    ang = [0.0, 0.0, 0.0]
    ang_vel = [0.0, 0.0, 0.0]
    ang_acc = [0.0, 0.0, 0.0]
    mass = 1.0
    inertia = [1.0, 1.0, 1.0]
    force = [0.0, -9.8, 0.0]
    torque = [0.0, 0.0, 0.0]
    
    body = multibody_solver.create_mobilized_body(1, pos, vel, acc, ang, ang_vel, ang_acc, 
                                                mass, inertia, force, torque)
    system.Add(body)
    
    # Add joint
    pi = [0.0, 0.0, 0.0]
    pj = [0.0, 0.0, 1.0]
    qi = [0.0, 0.0, 0.0]
    qj = [0.0, 0.0, 0.0]
    
    joint = multibody_solver.create_joint(0, pi, pj, qi, qj, ground, body)
    system.Add(joint)
    
    return system

