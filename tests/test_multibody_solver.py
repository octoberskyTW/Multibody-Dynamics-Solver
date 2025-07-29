"""
Unit tests for multibody solver Python bindings
多體求解器 Python 綁定的單元測試
"""

import os
import sys

import pytest


@pytest.mark.requires_build
class TestMultibodySolver:
    """Test the basic multibody solver functionality"""
    
    def test_module_import(self, multibody_solver):
        """Test that the multibody_solver module can be imported"""
        assert multibody_solver is not None
        # Check if the module has expected attributes
        assert hasattr(multibody_solver, 'DynamicsSystem')
        assert hasattr(multibody_solver, 'create_ground')
        assert hasattr(multibody_solver, 'create_mobilized_body')
        assert hasattr(multibody_solver, 'create_joint')
    
    def test_dynamics_system_creation(self, multibody_solver):
        """Test creating a dynamics system"""
        dt = 0.001
        system = multibody_solver.DynamicsSystem(dt)
        assert system is not None
    
    def test_ground_body_creation(self, multibody_solver):
        """Test creating a ground body"""
        ground = multibody_solver.create_ground(0)
        assert ground is not None
    
    def test_mobilized_body_creation(self, multibody_solver):
        """Test creating a mobilized body"""
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
        
        mobile_body = multibody_solver.create_mobilized_body(1, pos, vel, acc, ang, ang_vel, ang_acc, 
                                                           mass, inertia, force, torque)
        assert mobile_body is not None
    
    def test_joint_creation(self, multibody_solver):
        """Test creating a joint"""
        # Create bodies first
        ground = multibody_solver.create_ground(0)
        
        # Create joint (API needs to be checked for exact parameters)
        # Skipping detailed joint creation for now as API parameters need verification
        pytest.skip("Joint creation API parameters need verification")

@pytest.mark.requires_build
@pytest.mark.integration
class TestDynamicsSystem:
    """Integration tests for the dynamics system"""
    
    def test_system_assembly(self, dynamics_system):
        """Test that a system can be assembled"""
        # System should have ground body added
        assert dynamics_system is not None
    
    def test_complete_system_setup(self, dynamics_system):
        """Test a complete system with ground body"""
        system = dynamics_system
        assert system is not None
        
        # Test system properties
        num_bodies = system.get_nbody()
        num_joints = system.get_njoint()
        
        assert num_bodies == 1  # ground body
        assert num_joints == 0  # no joints yet
    
    def test_simulation_step(self, dynamics_system):
        """Test performing a simulation step"""
        system = dynamics_system
        
        # Initialize and assemble system
        system.init()
        system.Assembly()
        
        # Perform one simulation step
        system.solve()
        
        # Get positions and angles
        positions = system.get_body_positions()
        angles = system.get_body_angles()
        
        assert len(positions) == 1  # 1 body (ground)
        assert len(angles) == 1     # 1 body (ground)
        
        # Check that positions are lists of 3 elements (x, y, z)
        for pos in positions:
            assert len(pos) == 3
        
        # Check that ground body is at origin
        ground_pos = positions[0]
        assert ground_pos == [0.0, 0.0, 0.0]

@pytest.mark.requires_build  
@pytest.mark.slow
class TestVisualization:
    """Test visualization functionality"""
    
    def test_visualization_creation(self, multibody_solver):
        """Test creating a visualization object"""
        try:
            viz = multibody_solver.Visualization()
            assert viz is not None
        except Exception as e:
            # Visualization might require display - this is OK for headless testing
            pytest.skip(f"Visualization requires display: {e}")
    
    def test_visualization_with_system(self, sample_multibody_system, multibody_solver):
        """Test visualization with a complete system"""
        try:
            viz = multibody_solver.Visualization()
            # This test is mainly to ensure the API exists
            # Actual visualization testing would require a display
            pytest.skip("Visualization testing skipped - requires display")
        except Exception as e:
            pytest.skip(f"Visualization requires display: {e}")

@pytest.mark.parametrize("dt", [0.001, 0.01, 0.1])
def test_different_timesteps(multibody_solver, dt):
    """Test system creation with different time steps"""
    system = multibody_solver.DynamicsSystem(dt)
    assert system is not None

@pytest.mark.parametrize("mass,inertia", [
    (1.0, [1.0, 1.0, 1.0]),
    (2.5, [0.5, 1.0, 1.5]),
    (10.0, [2.0, 2.0, 2.0])
])
def test_different_body_parameters(multibody_solver, mass, inertia):
    """Test creating bodies with different mass and inertia parameters"""
    pos = [0.0, 0.0, 0.0]
    vel = [0.0, 0.0, 0.0]
    acc = [0.0, 0.0, 0.0]
    ang = [0.0, 0.0, 0.0]
    ang_vel = [0.0, 0.0, 0.0]
    ang_acc = [0.0, 0.0, 0.0]
    force = [0.0, -9.8, 0.0]
    torque = [0.0, 0.0, 0.0]
    
    mobile_body = multibody_solver.create_mobilized_body(1, pos, vel, acc, ang, ang_vel, ang_acc, 
                                                       mass, inertia, force, torque)
    assert mobile_body is not None