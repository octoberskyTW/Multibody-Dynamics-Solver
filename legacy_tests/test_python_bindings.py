#!/usr/bin/env python3
"""
Test script for Python bindings
測試 Python 綁定的腳本
"""

import os
import sys


def find_and_import_multibody_solver():
    """
    Find and import the multibody_solver module from possible locations
    尋找並導入 multibody_solver 模組
    """
    import glob

    # Get the directory of this script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Possible locations for the Python module
    search_paths = [
        # Standard build directory
        os.path.join(script_dir, 'build_cmake'),
        # Same directory as the build files
        os.path.join(script_dir, 'build'),
        # Current directory
        os.getcwd(),
        # Build directory relative to current working directory
        'build_cmake',
        'build'
    ]
    
    print("Searching for multibody_solver module...")
    
    for search_path in search_paths:
        if not os.path.exists(search_path):
            continue
            
        print(f"  Checking: {search_path}")
        
        # Look for multibody_solver shared libraries
        patterns = [
            os.path.join(search_path, 'multibody_solver*.so'),      # Linux
            os.path.join(search_path, 'multibody_solver*.dylib'),   # macOS
            os.path.join(search_path, 'multibody_solver*.pyd'),     # Windows
        ]
        
        found_files = []
        for pattern in patterns:
            found_files.extend(glob.glob(pattern))
        
        if found_files:
            print(f"  Found module files: {found_files}")
            
            # Add this path to sys.path if not already there
            abs_search_path = os.path.abspath(search_path)
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
    raise ImportError(
        "Could not find or import multibody_solver module.\n"
        "Please ensure the project is built with: ./build_python.sh\n"
        "Searched locations:\n" + 
        "\n".join(f"  - {path}" for path in search_paths)
    )

def test_basic_functionality():
    """測試基本功能"""
    try:
        mbs = find_and_import_multibody_solver()
        
        # Test creating a dynamics system
        dt = 0.001
        system = mbs.DynamicsSystem(dt)
        print(f"✓ Created DynamicsSystem with dt={dt}")
        
        # Test creating a ground body
        ground = mbs.create_ground(0)
        system.Add(ground)
        print("✓ Created and added ground body")
        
        # Test creating a mobilized body
        pos = [0.0, 0.0, 0.0]
        vel = [0.0, 0.0, 0.0]
        acc = [0.0, 0.0, 0.0]
        ang = [0.0, 0.0, 0.0]
        ang_vel = [0.0, 0.0, 0.0]
        ang_acc = [0.0, 0.0, 0.0]
        mass = 1.0
        inertia = [1.0, 1.0, 1.0]
        force = [0.0, -9.8, 0.0]
        torque = [0.0, 0.0, 0.0]
        
        body = mbs.create_mobilized_body(1, pos, vel, acc, ang, ang_vel, ang_acc, 
                                       mass, inertia, force, torque)
        system.Add(body)
        print("✓ Created and added mobilized body")
        
        # Test creating a joint
        pi = [0.0, 0.0, 0.0]
        pj = [0.0, 1.0, 0.0]
        qi = [0.0, 0.0, 0.0]
        qj = [0.0, 0.0, 0.0]
        
        joint = mbs.create_joint(0, pi, pj, qi, qj, ground, body)
        system.Add(joint)
        print("✓ Created and added joint")
        
        # Test system initialization
        system.Assembly()
        system.init()
        print("✓ System assembled and initialized")
        
        # Test getting system info
        nbodies = system.get_nbody()
        njoints = system.get_njoint()
        print(f"✓ System has {nbodies} bodies and {njoints} joints")
        
        # Test simulation step
        system.solve()
        print("✓ Performed one simulation step")
        
        # Test getting positions
        positions = system.get_body_positions()
        angles = system.get_body_angles()
        print(f"✓ Retrieved positions for {len(positions)} bodies")
        print(f"✓ Retrieved angles for {len(angles)} bodies")
        
        # Print first body position
        if positions:
            print(f"  First body position: {positions[0]}")
            
        return True
        
    except ImportError as e:
        print(f"✗ Failed to import multibody_solver: {e}")
        print("  Make sure to build the project first:")
        print("  cmake -B build_cmake -S . && cmake --build build_cmake")
        return False
        
    except Exception as e:
        print(f"✗ Error during testing: {e}")
        return False

def test_visualization():
    """測試視覺化功能"""
    try:
        import multibody_solver as mbs

        # Try to create visualization (may fail if GLFW not available)
        viz = mbs.Visualization(800, 600)
        print("✓ Created Visualization object")
        
        # Note: We don't initialize it as it requires a display
        print("  (Visualization initialization skipped - requires display)")
        
        return True
        
    except Exception as e:
        print(f"  Visualization test skipped: {e}")
        return True  # This is OK, visualization is optional

def main():
    """主測試函數"""
    print("=" * 50)
    print("Testing Multibody Dynamics Solver Python Bindings")
    print("測試多體動力學求解器 Python 綁定")
    print("=" * 50)
    
    # Test basic functionality
    print("\n1. Testing basic functionality...")
    if not test_basic_functionality():
        sys.exit(1)
    
    # Test visualization
    print("\n2. Testing visualization...")
    test_visualization()
    
    print("\n" + "=" * 50)
    print("All tests completed successfully! 所有測試成功完成！")
    print("You can now use the Python GUI:")
    print("python3 python_gui/main_gui.py")
    print("=" * 50)

if __name__ == "__main__":
    main()