"""
Tests for PySide6 GUI functionality
PySide6 GUI 功能測試
"""

import ast
import os
import sys
from pathlib import Path

import pytest


@pytest.mark.gui
class TestGUISyntax:
    """Test GUI code syntax and structure"""
    
    def test_gui_syntax_validation(self):
        """Test that the PySide6 GUI code has valid syntax"""
        project_root = Path(__file__).parent.parent
        gui_file = project_root / 'python_gui' / 'simple_gui.py'
        
        assert gui_file.exists(), f"GUI file not found: {gui_file}"
        
        # Read and parse the GUI file
        with open(gui_file, 'r', encoding='utf-8') as f:
            source = f.read()
        
        # This will raise SyntaxError if the syntax is invalid
        ast.parse(source)
    
    def test_gui_imports(self):
        """Test that GUI imports are properly structured"""
        project_root = Path(__file__).parent.parent
        gui_file = project_root / 'python_gui' / 'simple_gui.py'
        
        with open(gui_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check for key imports
        assert 'from PySide6.QtWidgets import' in content
        assert 'from PySide6.QtCore import' in content
        assert 'from PySide6.QtGui import' in content
        assert 'find_and_import_multibody_solver' in content
    
    def test_gui_classes_present(self):
        """Test that expected GUI classes are present"""
        project_root = Path(__file__).parent.parent
        gui_file = project_root / 'python_gui' / 'simple_gui.py'
        
        with open(gui_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check for key classes
        assert 'class AnimationWidget' in content
        assert 'class ExportWorker' in content
        assert 'class SimulationWorker' in content
        assert 'class SimpleGUI' in content

@pytest.mark.gui
class TestGUIFunctionality:
    """Test GUI functionality (requires PySide6)"""
    
    def test_pyside6_availability(self, pyside6):
        """Test that PySide6 is available"""
        assert pyside6 is not None
        assert hasattr(pyside6, '__version__')
    
    def test_gui_module_loading(self, pyside6):
        """Test that GUI module can be loaded without errors"""
        project_root = Path(__file__).parent.parent
        sys.path.insert(0, str(project_root / 'python_gui'))
        
        try:
            # Import the find function to test the import mechanism
            from simple_gui import find_and_import_multibody_solver
            assert callable(find_and_import_multibody_solver)
        except ImportError as e:
            # If PySide6 is not installed, this is expected
            if "PySide6" in str(e):
                pytest.skip("PySide6 not installed")
            else:
                raise
    
    @pytest.mark.requires_build
    def test_multibody_solver_discovery(self, pyside6):
        """Test that the GUI can discover the multibody solver module"""
        project_root = Path(__file__).parent.parent
        sys.path.insert(0, str(project_root / 'python_gui'))
        
        try:
            from simple_gui import find_and_import_multibody_solver
            mbs = find_and_import_multibody_solver()
            assert mbs is not None
            assert hasattr(mbs, 'DynamicsSystem')
        except ImportError as e:
            if "multibody_solver" in str(e):
                pytest.skip("multibody_solver module not built")
            else:
                raise

@pytest.mark.gui
class TestAnimationFeatures:
    """Test animation-related features"""
    
    def test_animation_features_documented(self):
        """Test that animation features are documented"""
        project_root = Path(__file__).parent.parent
        doc_file = project_root / 'ANIMATION_FEATURES.md'
        
        assert doc_file.exists(), "Animation features documentation not found"
        
        with open(doc_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check for key animation features
        assert 'Real-time Visualization' in content
        assert 'Interactive Controls' in content
        assert 'Export Animation' in content
        assert '3D camera rotation' in content
    
    def test_export_functionality_structure(self):
        """Test that export functionality is properly structured"""
        project_root = Path(__file__).parent.parent
        gui_file = project_root / 'python_gui' / 'simple_gui.py'
        
        with open(gui_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check for export-related code
        assert 'export_gif' in content
        assert 'export_png_sequence' in content
        assert 'export_jpeg_sequence' in content
        assert 'ExportWorker' in content

@pytest.mark.gui
class TestGUIRequirements:
    """Test GUI requirements and dependencies"""
    
    def test_requirements_file_exists(self):
        """Test that requirements.txt exists and contains necessary packages"""
        project_root = Path(__file__).parent.parent
        req_file = project_root / 'requirements.txt'
        
        assert req_file.exists(), "requirements.txt not found"
        
        with open(req_file, 'r') as f:
            content = f.read()
        
        # Check for key requirements
        assert 'PySide6' in content
        assert 'Pillow' in content
        assert 'numpy' in content
    
    def test_gui_file_structure(self):
        """Test that GUI file structure is correct"""
        project_root = Path(__file__).parent.parent
        gui_dir = project_root / 'python_gui'
        
        assert gui_dir.exists(), "python_gui directory not found"
        assert (gui_dir / 'simple_gui.py').exists(), "simple_gui.py not found"

@pytest.mark.parametrize("feature", [
    "Real-time visualization",
    "3D camera rotation", 
    "Animation export",
    "Motion trails",
    "Interactive controls"
])
def test_animation_features_present(feature):
    """Test that specific animation features are mentioned in documentation"""
    project_root = Path(__file__).parent.parent
    doc_file = project_root / 'ANIMATION_FEATURES.md'
    
    if doc_file.exists():
        with open(doc_file, 'r', encoding='utf-8') as f:
            content = f.read().lower()
        
        assert feature.lower() in content, f"Feature '{feature}' not documented"
    else:
        pytest.skip("Animation features documentation not found")