#!/usr/bin/env python3
"""
Simple Multibody Dynamics Solver GUI using PySide6
簡化版多體動力學求解器圖形界面 (使用 PySide6)

Features:
- Modern Qt6-based interface with split-panel layout
- Real-time 3D-like animation visualization using QPainter
- Threaded simulation for responsive UI
- Interactive animation controls (zoom, pan, trail effects)
- Parameter controls with sliders
- Real-time simulation progress updates
- Multi-language support (Chinese/English)

Animation Features:
- Real-time multibody system visualization with 3D rotation
- Motion trail effects with transparency
- Interactive zoom (mouse wheel) and pan (left drag)
- 3D camera rotation (right drag) with pitch/yaw control
- Keyboard arrow keys for precise rotation control
- 3D perspective projection with rotating coordinate system
- Body orientation indicators
- Customizable trail display
- View reset functionality
- Animation export (GIF, PNG/JPEG sequences)

Requirements:
- PySide6: pip install PySide6
- Pillow: pip install Pillow (optional, for GIF export)
"""

import os
import sys
import threading
import time


def find_and_import_multibody_solver():
    """
    Find and import the multibody_solver module from possible locations
    尋找並導入 multibody_solver 模組
    """
    import glob

    # Get the directory of this script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)
    
    # Possible locations for the Python module
    search_paths = [
        # Standard build directory
        os.path.join(project_root, 'build_cmake'),
        # Same directory as the build files
        os.path.join(project_root, 'build'),
        # Project root
        project_root,
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

try:
    import datetime
    import math

    from PySide6.QtCore import QPointF, QRectF, QStandardPaths, Qt, QThread, QTimer, Signal
    from PySide6.QtGui import QBrush, QColor, QFont, QPainter, QPainterPath, QPen, QPixmap
    from PySide6.QtOpenGLWidgets import QOpenGLWidget
    from PySide6.QtWidgets import (
        QApplication,
        QCheckBox,
        QComboBox,
        QFileDialog,
        QFrame,
        QGridLayout,
        QGroupBox,
        QHBoxLayout,
        QLabel,
        QMainWindow,
        QMessageBox,
        QProgressDialog,
        QPushButton,
        QSlider,
        QSplitter,
        QStatusBar,
        QTextEdit,
        QVBoxLayout,
        QWidget,
    )
except ImportError as e:
    print(f"Failed to import PySide6: {e}")
    print("Please install PySide6: pip install PySide6")
    sys.exit(1)

try:
    mbs = find_and_import_multibody_solver()
except ImportError as e:
    print(f"Failed to import multibody_solver: {e}")
    sys.exit(1)

class AnimationWidget(QWidget):
    """Real-time animation widget for multibody system visualization"""
    
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setMinimumSize(400, 300)
        self.setStyleSheet("background-color: black;")
        self.setFocusPolicy(Qt.StrongFocus)  # Allow keyboard focus
        
        # Animation data
        self.body_positions = []
        self.body_angles = []
        self.chain_connections = []
        self.current_time = 0.0
        self.step_count = 0
        
        # View parameters
        self.scale = 50.0  # Pixels per unit
        self.offset_x = 200
        self.offset_y = 150
        
        # 3D rotation parameters
        self.rotation_x = 0.0  # Rotation around X axis (pitch)
        self.rotation_y = 0.0  # Rotation around Y axis (yaw)
        self.rotation_z = 0.0  # Rotation around Z axis (roll)
        
        # Mouse interaction state
        self.last_mouse_pos = None
        self.mouse_mode = None  # 'pan', 'rotate', or None
        
        # Animation settings
        self.show_trail = True
        self.trail_positions = []  # Store history for trail effect
        self.max_trail_length = 50
        
        # Export settings
        self.is_exporting = False
        self.export_frames = []
        self.export_frame_rate = 20  # FPS for export
        
        # Timer for smooth animation
        self.animation_timer = QTimer()
        self.animation_timer.timeout.connect(self.update)
        
    def start_animation(self):
        """Start the animation timer"""
        self.animation_timer.start(50)  # 20 FPS
        
    def stop_animation(self):
        """Stop the animation timer"""
        self.animation_timer.stop()
        
    def update_data(self, positions, angles, time_val, step):
        """Update animation data from simulation"""
        self.body_positions = positions
        self.body_angles = angles
        self.current_time = time_val
        self.step_count = step
        
        # Add to trail
        if positions and self.show_trail:
            self.trail_positions.append(list(positions))
            if len(self.trail_positions) > self.max_trail_length:
                self.trail_positions.pop(0)
        
        # Trigger repaint
        self.update()
        
        # Capture frame for export if needed
        if self.is_exporting and step % 5 == 0:  # Capture every 5th step to reduce file size
            # Wait for paint to complete then capture
            QApplication.processEvents()
            frame = self.capture_frame()
            self.export_frames.append(frame)
        
    def clear_trail(self):
        """Clear the trail history"""
        self.trail_positions.clear()
        
    def reset_view(self):
        """Reset camera view to default"""
        self.rotation_x = 0.0
        self.rotation_y = 0.0 
        self.rotation_z = 0.0
        self.scale = 50.0
        self.offset_x = 200
        self.offset_y = 150
        self.update()
        
    def start_export(self):
        """Start exporting animation frames"""
        self.is_exporting = True
        self.export_frames = []
        
    def stop_export(self):
        """Stop exporting animation frames"""
        self.is_exporting = False
        
    def capture_frame(self):
        """Capture current frame as QPixmap"""
        return self.grab()
        
    def get_export_frames(self):
        """Get captured frames for export"""
        return self.export_frames.copy()
        
    def clear_export_frames(self):
        """Clear captured frames"""
        self.export_frames.clear()
        
    def paintEvent(self, event):
        """Paint the animation"""
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        
        # Fill background
        painter.fillRect(self.rect(), QColor(20, 20, 30))
        
        # Draw coordinate system
        self.draw_coordinate_system(painter)
        
        # Draw trail
        if self.show_trail:
            self.draw_trail(painter)
        
        # Draw current multibody system
        self.draw_multibody_system(painter)
        
        # Draw info text
        self.draw_info_text(painter)
        
    def draw_coordinate_system(self, painter):
        """Draw coordinate system axes with rotation"""
        # Draw rotated coordinate axes
        origin = [0, 0, 0]
        
        # Define axis vectors (length 1.0)
        x_axis = [1, 0, 0]
        y_axis = [0, 1, 0] 
        z_axis = [0, 0, 1]
        
        # Apply rotation to axes
        x_rotated = self.rotate_point(x_axis)
        y_rotated = self.rotate_point(y_axis)
        z_rotated = self.rotate_point(z_axis)
        
        # Convert to screen coordinates
        origin_screen = self.world_to_screen(origin)
        x_screen = self.world_to_screen(x_rotated)
        y_screen = self.world_to_screen(y_rotated)
        z_screen = self.world_to_screen(z_rotated)
        
        # X axis (red)
        painter.setPen(QPen(QColor(255, 100, 100), 3))
        painter.drawLine(origin_screen, x_screen)
        painter.drawText(x_screen.x() + 5, x_screen.y() + 5, "X")
        
        # Y axis (green)
        painter.setPen(QPen(QColor(100, 255, 100), 3))
        painter.drawLine(origin_screen, y_screen)
        painter.drawText(y_screen.x() + 5, y_screen.y() + 5, "Y")
        
        # Z axis (blue)
        painter.setPen(QPen(QColor(100, 100, 255), 3))
        painter.drawLine(origin_screen, z_screen)
        painter.drawText(z_screen.x() + 5, z_screen.y() + 5, "Z")
        
    def draw_trail(self, painter):
        """Draw motion trail"""
        if not self.trail_positions:
            return
            
        painter.setPen(QPen(QColor(100, 100, 200, 100), 1))
        
        for i, positions in enumerate(self.trail_positions):
            alpha = int(255 * (i + 1) / len(self.trail_positions) * 0.3)
            painter.setPen(QPen(QColor(100, 150, 255, alpha), 1))
            
            # Draw connections between bodies in this frame
            for j in range(len(positions) - 1):
                if j + 1 < len(positions):
                    pos1 = self.world_to_screen(positions[j])
                    pos2 = self.world_to_screen(positions[j + 1])
                    painter.drawLine(pos1, pos2)
        
    def draw_multibody_system(self, painter):
        """Draw the current multibody system"""
        if not self.body_positions:
            return
            
        # Draw connections (joints) first
        painter.setPen(QPen(QColor(200, 200, 200), 3))
        for i in range(len(self.body_positions) - 1):
            if i + 1 < len(self.body_positions):
                pos1 = self.world_to_screen(self.body_positions[i])
                pos2 = self.world_to_screen(self.body_positions[i + 1])
                painter.drawLine(pos1, pos2)
        
        # Draw bodies
        for i, position in enumerate(self.body_positions):
            screen_pos = self.world_to_screen(position)
            
            # Ground body (larger, different color)
            if i == 0:
                painter.setBrush(QBrush(QColor(100, 255, 100)))
                painter.setPen(QPen(QColor(50, 200, 50), 2))
                painter.drawRect(screen_pos.x() - 15, screen_pos.y() - 15, 30, 30)
                painter.drawText(screen_pos.x() - 20, screen_pos.y() - 20, "Ground")
            else:
                # Regular bodies
                color_intensity = 200 + 55 * math.sin(i * 0.5)
                painter.setBrush(QBrush(QColor(int(color_intensity), 100, 255 - i * 20)))
                painter.setPen(QPen(QColor(255, 255, 255), 2))
                painter.drawEllipse(screen_pos.x() - 8, screen_pos.y() - 8, 16, 16)
                
                # Body number
                painter.setPen(QPen(QColor(255, 255, 255)))
                painter.drawText(screen_pos.x() + 12, screen_pos.y() + 5, f"B{i}")
                
                # Show angle if available
                if i < len(self.body_angles) and self.body_angles[i]:
                    angle = self.body_angles[i][2]  # Z rotation
                    line_length = 15
                    end_x = screen_pos.x() + line_length * math.cos(angle)
                    end_y = screen_pos.y() + line_length * math.sin(angle)
                    painter.setPen(QPen(QColor(255, 255, 100), 2))
                    painter.drawLine(screen_pos.x(), screen_pos.y(), end_x, end_y)
    
    def draw_info_text(self, painter):
        """Draw simulation information"""
        painter.setPen(QPen(QColor(255, 255, 255)))
        painter.setFont(QFont("Arial", 10))
        
        info_lines = [
            f"Time: {self.current_time:.3f}s",
            f"Step: {self.step_count}",
            f"Bodies: {len(self.body_positions)}",
            f"Scale: {self.scale:.1f}px/unit",
            f"Rotation X: {math.degrees(self.rotation_x):.1f}°",
            f"Rotation Y: {math.degrees(self.rotation_y):.1f}°",
            "",
            "Controls:",
            "• Mouse wheel: Zoom",
            "• Left drag: Pan view",
            "• Right drag: Rotate view",
            "• Arrow keys: Step rotate",
            "• R key: Reset view"
        ]
        
        y_pos = 20
        for line in info_lines:
            painter.drawText(10, y_pos, line)
            y_pos += 15
            
    def rotate_point(self, point):
        """Apply 3D rotation to a point"""
        if len(point) < 3:
            point = list(point) + [0] * (3 - len(point))
        
        x, y, z = point[0], point[1], point[2]
        
        # Rotation around X axis (pitch)
        if self.rotation_x != 0:
            cos_x = math.cos(self.rotation_x)
            sin_x = math.sin(self.rotation_x)
            y, z = y * cos_x - z * sin_x, y * sin_x + z * cos_x
        
        # Rotation around Y axis (yaw)
        if self.rotation_y != 0:
            cos_y = math.cos(self.rotation_y)
            sin_y = math.sin(self.rotation_y)
            x, z = x * cos_y + z * sin_y, -x * sin_y + z * cos_y
        
        # Rotation around Z axis (roll)
        if self.rotation_z != 0:
            cos_z = math.cos(self.rotation_z)
            sin_z = math.sin(self.rotation_z)
            x, y = x * cos_z - y * sin_z, x * sin_z + y * cos_z
        
        return [x, y, z]
    
    def world_to_screen(self, world_pos):
        """Convert world coordinates to screen coordinates with rotation"""
        if len(world_pos) >= 2:
            # Apply rotation first
            rotated_pos = self.rotate_point(world_pos)
            
            # Simple orthographic projection
            x = rotated_pos[0] * self.scale + self.offset_x
            y = -rotated_pos[1] * self.scale + self.offset_y  # Flip Y axis
            
            # Add perspective effect based on Z depth
            if len(rotated_pos) >= 3:
                z_depth = rotated_pos[2]
                # Simple perspective: objects further away appear smaller
                perspective_factor = 1.0 + z_depth * 0.1
                x += z_depth * self.scale * 0.2  # Slight horizontal offset for depth
                y += z_depth * self.scale * 0.1  # Slight vertical offset for depth
                
            return QPointF(x, y)
        return QPointF(self.offset_x, self.offset_y)
        
    def wheelEvent(self, event):
        """Handle mouse wheel for zoom"""
        zoom_factor = 1.1 if event.angleDelta().y() > 0 else 0.9
        self.scale *= zoom_factor
        self.scale = max(10, min(200, self.scale))  # Clamp zoom
        self.update()
        
    def mousePressEvent(self, event):
        """Handle mouse press for panning and rotation"""
        if event.button() == Qt.LeftButton:
            self.last_mouse_pos = event.position()
            self.mouse_mode = 'pan'
        elif event.button() == Qt.RightButton:
            self.last_mouse_pos = event.position()
            self.mouse_mode = 'rotate'
            
    def mouseMoveEvent(self, event):
        """Handle mouse move for panning and rotation"""
        if hasattr(self, 'last_mouse_pos') and self.last_mouse_pos is not None:
            delta = event.position() - self.last_mouse_pos
            
            if self.mouse_mode == 'pan':
                # Pan the view
                self.offset_x += delta.x()
                self.offset_y += delta.y()
            elif self.mouse_mode == 'rotate':
                # Rotate the view
                rotation_speed = 0.01
                self.rotation_y += delta.x() * rotation_speed  # Horizontal mouse movement -> Y rotation (yaw)
                self.rotation_x += delta.y() * rotation_speed  # Vertical mouse movement -> X rotation (pitch)
                
                # Clamp rotations to reasonable values
                self.rotation_x = max(-math.pi/2, min(math.pi/2, self.rotation_x))
            
            self.last_mouse_pos = event.position()
            self.update()
            
    def mouseReleaseEvent(self, event):
        """Handle mouse release"""
        self.last_mouse_pos = None
        self.mouse_mode = None
        
    def keyPressEvent(self, event):
        """Handle keyboard input for camera controls"""
        rotation_step = math.radians(15)  # 15 degree steps
        
        if event.key() == Qt.Key_R and event.modifiers() == Qt.NoModifier:
            # Reset view
            self.reset_view()
        elif event.key() == Qt.Key_Left:
            self.rotation_y -= rotation_step
            self.update()
        elif event.key() == Qt.Key_Right:
            self.rotation_y += rotation_step
            self.update()
        elif event.key() == Qt.Key_Up:
            self.rotation_x = max(-math.pi/2, self.rotation_x - rotation_step)
            self.update()
        elif event.key() == Qt.Key_Down:
            self.rotation_x = min(math.pi/2, self.rotation_x + rotation_step)
            self.update()
        else:
            super().keyPressEvent(event)


class ExportDialog(QMessageBox):
    """Dialog for selecting animation export options"""
    
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Export Animation / 導出動畫")
        self.setText("Select export format / 選擇導出格式:")
        
        # Add format selection combo box
        self.format_combo = QComboBox()
        self.format_combo.addItems([
            "GIF Animation (*.gif)",
            "PNG Image Sequence (*.png)", 
            "JPEG Image Sequence (*.jpg)"
        ])
        
        # Create custom layout (QMessageBox doesn't support custom widgets easily)
        # So we'll create a simple dialog instead
        
    @staticmethod
    def get_export_options(parent=None):
        """Show export dialog and return selected options"""
        dialog = QFileDialog(parent)
        dialog.setWindowTitle("Export Animation / 導出動畫")
        dialog.setAcceptMode(QFileDialog.AcceptSave)
        dialog.setFileMode(QFileDialog.AnyFile)
        
        # Set filters for different formats
        dialog.setNameFilters([
            "GIF Animation (*.gif)",
            "PNG Sequence (*.png)",
            "JPEG Sequence (*.jpg)"
        ])
        
        # Set default location
        default_name = f"animation_{datetime.datetime.now().strftime('%Y%m%d_%H%M%S')}"
        desktop = QStandardPaths.writableLocation(QStandardPaths.DesktopLocation)
        dialog.selectFile(os.path.join(desktop, default_name))
        
        if dialog.exec() == QFileDialog.Accepted:
            files = dialog.selectedFiles()
            if files:
                selected_filter = dialog.selectedNameFilter()
                return files[0], selected_filter
        
        return None, None


class ExportWorker(QThread):
    """Worker thread for exporting animation"""
    progress_updated = Signal(int, int)  # current, total
    export_finished = Signal(str)  # result message
    error_occurred = Signal(str)  # error message
    
    def __init__(self, frames, output_path, format_filter):
        super().__init__()
        self.frames = frames
        self.output_path = output_path
        self.format_filter = format_filter
        
    def run(self):
        try:
            if "GIF" in self.format_filter:
                self.export_gif()
            elif "PNG" in self.format_filter:
                self.export_png_sequence()
            elif "JPEG" in self.format_filter or "JPG" in self.format_filter:
                self.export_jpg_sequence()
        except Exception as e:
            self.error_occurred.emit(str(e))
            
    def export_gif(self):
        """Export frames as GIF animation"""
        try:
            # Try to use PIL for GIF creation
            import numpy as np
            from PIL import Image

            # Convert QPixmap frames to PIL Images
            pil_images = []
            for i, frame in enumerate(self.frames):
                self.progress_updated.emit(i, len(self.frames))
                
                # Convert QPixmap to PIL Image using a more reliable method
                qimage = frame.toImage()
                qimage = qimage.convertToFormat(qimage.Format_RGBA8888)
                
                width = qimage.width()
                height = qimage.height()
                
                # Get pointer to image data
                ptr = qimage.constBits()
                
                # Convert to numpy array, then to PIL Image
                try:
                    # Method 1: Direct conversion using sip
                    import sip
                    arr = np.frombuffer(sip.voidptr(ptr), dtype=np.uint8).reshape((height, width, 4))
                    pil_image = Image.fromarray(arr, 'RGBA')
                except (ImportError, AttributeError):
                    # Method 2: Using bytes conversion
                    bytes_per_line = qimage.bytesPerLine()
                    total_bytes = bytes_per_line * height
                    
                    # Create buffer from memoryview
                    buffer = bytearray(total_bytes)
                    for row in range(height):
                        row_start = row * bytes_per_line
                        for col in range(width * 4):  # 4 bytes per pixel (RGBA)
                            if row_start + col < total_bytes:
                                buffer[row_start + col] = ptr[row_start + col]
                    
                    # Convert to PIL Image
                    arr = np.frombuffer(buffer, dtype=np.uint8).reshape((height, width, 4))
                    pil_image = Image.fromarray(arr, 'RGBA')
                
                # Convert to RGB for GIF compatibility
                pil_images.append(pil_image.convert('RGB'))
            
            # Save as GIF
            if pil_images:
                pil_images[0].save(
                    self.output_path,
                    save_all=True,
                    append_images=pil_images[1:],
                    duration=100,  # 100ms per frame = 10 FPS
                    loop=0
                )
            
            self.export_finished.emit(f"GIF exported successfully to {self.output_path}")
            
        except ImportError:
            # Fallback: export as PNG sequence if PIL not available
            self.export_png_sequence()
            self.export_finished.emit(f"PIL not available, exported as PNG sequence instead")
        except Exception as e:
            # If GIF export fails, try a simpler method or fall back to PNG sequence
            try:
                self.export_gif_simple()
            except:
                self.export_png_sequence()
                self.export_finished.emit(f"GIF export failed ({str(e)}), exported as PNG sequence instead")
                
    def export_gif_simple(self):
        """Simple GIF export using temporary PNG files"""
        try:
            import shutil
            import tempfile

            from PIL import Image

            # Create temporary directory for PNG files
            with tempfile.TemporaryDirectory() as temp_dir:
                # Save frames as temporary PNG files
                png_files = []
                for i, frame in enumerate(self.frames):
                    self.progress_updated.emit(i, len(self.frames))
                    
                    temp_path = os.path.join(temp_dir, f"frame_{i:04d}.png")
                    frame.save(temp_path, "PNG")
                    png_files.append(temp_path)
                
                # Load PNG files and convert to GIF
                if png_files:
                    images = []
                    for png_file in png_files:
                        img = Image.open(png_file)
                        images.append(img.convert('RGB'))
                    
                    # Save as GIF
                    if images:
                        images[0].save(
                            self.output_path,
                            save_all=True,
                            append_images=images[1:],
                            duration=100,
                            loop=0
                        )
                        
                        self.export_finished.emit(f"GIF exported successfully to {self.output_path}")
                        return
                        
            # If we get here, something went wrong
            raise Exception("Failed to create GIF from PNG files")
            
        except Exception as e:
            raise Exception(f"Simple GIF export failed: {str(e)}")
        
    def export_png_sequence(self):
        """Export frames as PNG sequence"""
        base_path = os.path.splitext(self.output_path)[0]
        os.makedirs(base_path, exist_ok=True)
        
        for i, frame in enumerate(self.frames):
            self.progress_updated.emit(i, len(self.frames))
            frame_path = os.path.join(base_path, f"frame_{i:04d}.png")
            frame.save(frame_path, "PNG")
        
        self.export_finished.emit(f"PNG sequence exported to {base_path}/")
        
    def export_jpg_sequence(self):
        """Export frames as JPEG sequence"""
        base_path = os.path.splitext(self.output_path)[0]
        os.makedirs(base_path, exist_ok=True)
        
        for i, frame in enumerate(self.frames):
            self.progress_updated.emit(i, len(self.frames))
            frame_path = os.path.join(base_path, f"frame_{i:04d}.jpg")
            frame.save(frame_path, "JPEG", quality=90)
        
        self.export_finished.emit(f"JPEG sequence exported to {base_path}/")
    
    def export_jpeg_sequence(self):
        """Alias for export_jpg_sequence for consistency"""
        return self.export_jpg_sequence()


class SimulationWorker(QThread):
    """Worker thread for running simulation"""
    progress_updated = Signal(int, float, list, list)  # step, time, positions, angles
    simulation_finished = Signal(int, float)  # final step, final time
    error_occurred = Signal(str)  # error message
    
    def __init__(self, system, dt):
        super().__init__()
        self.system = system
        self.dt = dt
        self.is_running = True
        
    def run(self):
        step_count = 0
        current_time = 0.0
        
        try:
            while self.is_running and step_count < 5000:
                self.system.solve()
                current_time += self.dt
                step_count += 1
                
                if step_count % 10 == 0:  # Update more frequently for animation
                    positions = self.system.get_body_positions()
                    angles = self.system.get_body_angles()
                    self.progress_updated.emit(step_count, current_time, positions, angles)
                
                self.msleep(1)  # Small delay
                
        except Exception as e:
            self.error_occurred.emit(str(e))
            return
            
        self.simulation_finished.emit(step_count, current_time)
    
    def stop(self):
        self.is_running = False


class SimpleGUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("簡化版多體動力學求解器 / Simple Multibody Dynamics Solver")
        self.setGeometry(100, 100, 800, 600)
        
        # Simulation state
        self.system = None
        self.simulation_worker = None
        self.current_time = 0.0
        self.step_count = 0
        
        # UI variables
        self.bodies_count = 3
        self.dt_value = 0.001
        
        self.setup_ui()
        self.create_system()
        
    def setup_ui(self):
        """設置用戶界面"""
        # Central widget and main layout using splitter
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QHBoxLayout(central_widget)
        
        # Create splitter for animation and controls
        splitter = QSplitter(Qt.Horizontal)
        main_layout.addWidget(splitter)
        
        # Left side: Controls
        controls_widget = QWidget()
        controls_layout = QVBoxLayout(controls_widget)
        controls_widget.setMaximumWidth(400)
        controls_widget.setMinimumWidth(350)
        
        # Title
        title_label = QLabel("多體動力學求解器 / Multibody Dynamics Solver")
        title_font = QFont()
        title_font.setPointSize(14)
        title_font.setBold(True)
        title_label.setFont(title_font)
        title_label.setAlignment(Qt.AlignCenter)
        controls_layout.addWidget(title_label)
        
        # Parameters group
        params_group = QGroupBox("參數 / Parameters")
        params_layout = QGridLayout(params_group)
        
        # Number of bodies
        params_layout.addWidget(QLabel("物體數量 / Bodies:"), 0, 0)
        self.bodies_slider = QSlider(Qt.Horizontal)
        self.bodies_slider.setRange(1, 10)
        self.bodies_slider.setValue(3)
        self.bodies_slider.valueChanged.connect(self.on_bodies_changed)
        params_layout.addWidget(self.bodies_slider, 0, 1)
        self.bodies_label = QLabel("3")
        params_layout.addWidget(self.bodies_label, 0, 2)
        
        # Time step
        params_layout.addWidget(QLabel("時間步長 / Time Step:"), 1, 0)
        self.dt_slider = QSlider(Qt.Horizontal)
        self.dt_slider.setRange(1, 100)  # Will map to 0.0001 - 0.01
        self.dt_slider.setValue(10)  # Maps to 0.001
        self.dt_slider.valueChanged.connect(self.on_dt_changed)
        params_layout.addWidget(self.dt_slider, 1, 1)
        self.dt_label = QLabel("0.001")
        params_layout.addWidget(self.dt_label, 1, 2)
        
        controls_layout.addWidget(params_group)
        
        # Control buttons
        button_layout = QHBoxLayout()
        
        self.create_button = QPushButton("創建系統 / Create System")
        self.create_button.clicked.connect(self.create_system)
        button_layout.addWidget(self.create_button)
        
        self.start_button = QPushButton("開始 / Start")
        self.start_button.clicked.connect(self.start_simulation)
        button_layout.addWidget(self.start_button)
        
        self.stop_button = QPushButton("停止 / Stop")
        self.stop_button.clicked.connect(self.stop_simulation)
        self.stop_button.setEnabled(False)
        button_layout.addWidget(self.stop_button)
        
        self.reset_button = QPushButton("重置 / Reset")
        self.reset_button.clicked.connect(self.reset_simulation)
        button_layout.addWidget(self.reset_button)
        
        controls_layout.addLayout(button_layout)
        
        # Animation controls
        anim_group = QGroupBox("動畫控制 / Animation Controls")
        anim_layout = QVBoxLayout(anim_group)
        
        anim_button_layout = QHBoxLayout()
        self.show_trail_checkbox = QCheckBox("顯示軌跡 / Show Trail")
        self.show_trail_checkbox.setChecked(True)
        self.show_trail_checkbox.toggled.connect(self.toggle_trail)
        anim_button_layout.addWidget(self.show_trail_checkbox)
        
        self.clear_trail_button = QPushButton("清除軌跡 / Clear Trail")
        self.clear_trail_button.clicked.connect(self.clear_trail)
        anim_button_layout.addWidget(self.clear_trail_button)
        
        anim_layout.addLayout(anim_button_layout)
        
        # View control buttons
        view_button_layout = QHBoxLayout()
        self.reset_view_button = QPushButton("重置視角 / Reset View")
        self.reset_view_button.clicked.connect(self.reset_view)
        view_button_layout.addWidget(self.reset_view_button)
        
        anim_layout.addLayout(view_button_layout)
        
        # Export controls
        export_layout = QHBoxLayout()
        self.start_export_button = QPushButton("開始錄製 / Start Recording")
        self.start_export_button.clicked.connect(self.start_recording)
        export_layout.addWidget(self.start_export_button)
        
        self.stop_export_button = QPushButton("停止錄製 / Stop Recording")
        self.stop_export_button.clicked.connect(self.stop_recording)
        self.stop_export_button.setEnabled(False)
        export_layout.addWidget(self.stop_export_button)
        
        self.export_button = QPushButton("導出動畫 / Export Animation")
        self.export_button.clicked.connect(self.export_animation)
        self.export_button.setEnabled(False)
        export_layout.addWidget(self.export_button)
        
        anim_layout.addLayout(export_layout)
        controls_layout.addWidget(anim_group)
        
        # Output text area
        output_group = QGroupBox("輸出 / Output")
        output_layout = QVBoxLayout(output_group)
        
        self.output_text = QTextEdit()
        self.output_text.setMinimumHeight(200)
        self.output_text.setMaximumHeight(250)
        output_layout.addWidget(self.output_text)
        
        controls_layout.addWidget(output_group)
        
        # Add controls to splitter
        splitter.addWidget(controls_widget)
        
        # Right side: Animation widget
        self.animation_widget = AnimationWidget()
        splitter.addWidget(self.animation_widget)
        
        # Set splitter proportions (30% controls, 70% animation)
        splitter.setSizes([300, 700])
        
        # Status bar
        self.status_bar = QStatusBar()
        self.setStatusBar(self.status_bar)
        self.status_bar.showMessage("就緒 / Ready")
        
    def on_bodies_changed(self, value):
        """Handle bodies slider change"""
        self.bodies_count = value
        self.bodies_label.setText(str(value))
        
    def on_dt_changed(self, value):
        """Handle time step slider change"""
        # Map slider value (1-100) to dt range (0.0001-0.01)
        self.dt_value = 0.0001 + (value - 1) * (0.01 - 0.0001) / 99
        self.dt_label.setText(f"{self.dt_value:.4f}")
        
    def log_message(self, message):
        """記錄消息到輸出區域"""
        self.output_text.append(message)
        
    def set_status(self, message):
        """Update status bar"""
        self.status_bar.showMessage(message)
        
    def toggle_trail(self, checked):
        """Toggle trail display"""
        self.animation_widget.show_trail = checked
        if not checked:
            self.animation_widget.clear_trail()
            
    def clear_trail(self):
        """Clear animation trail"""
        self.animation_widget.clear_trail()
        
    def reset_view(self):
        """Reset animation view"""
        self.animation_widget.reset_view()
        
    def start_recording(self):
        """Start recording animation frames"""
        self.animation_widget.start_export()
        self.start_export_button.setEnabled(False)
        self.stop_export_button.setEnabled(True)
        self.export_button.setEnabled(False)
        self.log_message("開始錄製動畫幀... / Started recording animation frames...")
        self.set_status("錄製中... / Recording...")
        
    def stop_recording(self):
        """Stop recording animation frames"""
        self.animation_widget.stop_export()
        frames = self.animation_widget.get_export_frames()
        
        self.start_export_button.setEnabled(True)
        self.stop_export_button.setEnabled(False)
        self.export_button.setEnabled(len(frames) > 0)
        
        self.log_message(f"錄製完成，已捕獲 {len(frames)} 幀 / Recording finished, captured {len(frames)} frames")
        self.set_status(f"錄製完成: {len(frames)} 幀 / Recording completed: {len(frames)} frames")
        
    def export_animation(self):
        """Export recorded animation"""
        frames = self.animation_widget.get_export_frames()
        if not frames:
            QMessageBox.warning(self, "警告 / Warning", 
                              "沒有錄製的幀可供導出 / No recorded frames to export")
            return
        
        # Show export dialog
        output_path, format_filter = ExportDialog.get_export_options(self)
        if not output_path:
            return
            
        # Start export in worker thread
        self.export_worker = ExportWorker(frames, output_path, format_filter)
        self.export_worker.progress_updated.connect(self.on_export_progress)
        self.export_worker.export_finished.connect(self.on_export_finished)
        self.export_worker.error_occurred.connect(self.on_export_error)
        
        # Show progress dialog
        self.export_progress = QProgressDialog("導出動畫中... / Exporting animation...", 
                                             "取消 / Cancel", 0, len(frames), self)
        self.export_progress.setWindowTitle("導出進度 / Export Progress")
        self.export_progress.setWindowModality(Qt.WindowModal)
        self.export_progress.show()
        
        # Disable export controls during export
        self.export_button.setEnabled(False)
        self.export_worker.start()
        
    def on_export_progress(self, current, total):
        """Handle export progress update"""
        self.export_progress.setValue(current)
        
    def on_export_finished(self, message):
        """Handle export completion"""
        self.export_progress.close()
        self.export_button.setEnabled(True)
        self.log_message(f"✓ {message}")
        QMessageBox.information(self, "導出完成 / Export Complete", message)
        
    def on_export_error(self, error_message):
        """Handle export error"""
        self.export_progress.close()
        self.export_button.setEnabled(True)
        self.log_message(f"✗ 導出錯誤 / Export error: {error_message}")
        QMessageBox.critical(self, "導出錯誤 / Export Error", 
                           f"導出失敗 / Export failed: {error_message}")
        
    def create_system(self):
        """創建多體動力學系統"""
        try:
            self.set_status("創建系統中... / Creating system...")
            self.output_text.clear()
            
            dt = self.dt_value
            nbodies = self.bodies_count
            
            self.log_message(f"創建系統: {nbodies} 個物體, dt={dt}")
            self.log_message(f"Creating system: {nbodies} bodies, dt={dt}")
            
            # Create dynamics system
            self.system = mbs.DynamicsSystem(dt)
            self.log_message("✓ 動力學系統已創建 / Dynamics system created")
            
            # Create ground body
            ground = mbs.create_ground(0)
            self.system.Add(ground)
            self.log_message("✓ 地面物體已添加 / Ground body added")
            
            # Create chain of bodies
            prev_body = ground
            for i in range(nbodies):
                # Initial conditions
                pos = [0.0, 0.0, 0.0]
                vel = [0.0, 0.0, 0.0]
                acc = [0.0, 0.0, 0.0]
                ang = [0.0, 0.3, 0.3]
                ang_vel = [0.0, 0.0, 0.0]
                ang_acc = [0.0, 0.0, 0.0]
                mass = 1.0
                inertia = [1.0, 1.0, 1.0]
                force = [0.0, -9.8, 0.0]  # Gravity
                torque = [0.0, 0.0, 0.0]
                
                # Create body
                body = mbs.create_mobilized_body(i + 1, pos, vel, acc, ang, 
                                               ang_vel, ang_acc, mass, inertia, 
                                               force, torque)
                self.system.Add(body)
                
                # Create joint
                pi = [0.0, 0.0, 0.0]
                pj = [0.0, 1.0, 0.0]  # Connection point
                qi = [0.0, 0.0, 0.0]
                qj = [0.0, 0.0, 0.0]
                
                joint = mbs.create_joint(0, pi, pj, qi, qj, prev_body, body)
                self.system.Add(joint)
                
                prev_body = body
                self.log_message(f"✓ 物體 {i+1} 已創建 / Body {i+1} created")
            
            # Initialize system
            self.system.Assembly()
            self.system.init()
            
            self.log_message("✓ 系統初始化完成 / System initialized")
            self.log_message(f"系統狀態: {self.system.get_nbody()} 物體, {self.system.get_njoint()} 關節")
            self.log_message(f"System status: {self.system.get_nbody()} bodies, {self.system.get_njoint()} joints")
            
            self.current_time = 0.0
            self.step_count = 0
            self.set_status("系統已創建 / System created")
            
        except Exception as e:
            error_msg = f"創建系統失敗 / System creation failed: {str(e)}"
            self.log_message(f"✗ {error_msg}")
            QMessageBox.critical(self, "錯誤 / Error", error_msg)
            self.set_status("創建失敗 / Creation failed")
            
    def start_simulation(self):
        """開始模擬"""
        if self.system is None:
            QMessageBox.warning(self, "警告 / Warning", "請先創建系統 / Please create system first")
            return
            
        if self.simulation_worker and self.simulation_worker.isRunning():
            QMessageBox.information(self, "信息 / Info", "模擬已在運行 / Simulation already running")
            return
            
        self.set_status("模擬運行中... / Simulation running...")
        self.log_message("開始模擬... / Starting simulation...")
        
        # Create and start simulation worker
        self.simulation_worker = SimulationWorker(self.system, self.dt_value)
        self.simulation_worker.progress_updated.connect(self.on_simulation_progress)
        self.simulation_worker.simulation_finished.connect(self.on_simulation_finished)
        self.simulation_worker.error_occurred.connect(self.on_simulation_error)
        self.simulation_worker.start()
        
        # Update button states
        self.start_button.setEnabled(False)
        self.stop_button.setEnabled(True)
        
        # Start animation
        self.animation_widget.start_animation()
        
    def stop_simulation(self):
        """停止模擬"""
        if self.simulation_worker and self.simulation_worker.isRunning():
            self.simulation_worker.stop()
            self.simulation_worker.wait(1000)  # Wait up to 1 second
            
        self.set_status("模擬已停止 / Simulation stopped")
        self.log_message("模擬已停止 / Simulation stopped")
        
        # Update button states
        self.start_button.setEnabled(True)
        self.stop_button.setEnabled(False)
        
        # Stop animation
        self.animation_widget.stop_animation()
        
        # Stop animation
        self.animation_widget.stop_animation()
        
    def reset_simulation(self):
        """重置模擬"""
        self.stop_simulation()
        self.current_time = 0.0
        self.step_count = 0
        self.animation_widget.clear_trail()
        
        # Reset export state
        self.animation_widget.stop_export()
        self.animation_widget.clear_export_frames()
        self.start_export_button.setEnabled(True)
        self.stop_export_button.setEnabled(False)
        self.export_button.setEnabled(False)
        
        self.create_system()
        
    def on_simulation_progress(self, step, current_time, positions, angles):
        """Handle simulation progress update"""
        self.step_count = step
        self.current_time = current_time
        
        # Update animation
        self.animation_widget.update_data(positions, angles, current_time, step)
        
        # Log progress less frequently for better performance
        if step % 100 == 0 and positions:
            first_body_pos = positions[0]
            message = f"Step {step}, Time: {current_time:.3f}s, Body1 pos: [{first_body_pos[0]:.3f}, {first_body_pos[1]:.3f}, {first_body_pos[2]:.3f}]"
            self.log_message(message)
            
    def on_simulation_finished(self, final_step, final_time):
        """Handle simulation completion"""
        self.step_count = final_step
        self.current_time = final_time
        
        final_msg = f"模擬完成: {final_step} 步, {final_time:.3f}s / Simulation completed: {final_step} steps, {final_time:.3f}s"
        self.log_message(final_msg)
        self.set_status("模擬完成 / Simulation completed")
        
        # Update button states
        self.start_button.setEnabled(True)
        self.stop_button.setEnabled(False)
        
        # Stop animation
        self.animation_widget.stop_animation()
        
    def on_simulation_error(self, error_msg):
        """Handle simulation error"""
        self.log_message(f"✗ 模擬錯誤 / Simulation error: {error_msg}")
        self.set_status("模擬錯誤 / Simulation error")
        
        # Update button states
        self.start_button.setEnabled(True)
        self.stop_button.setEnabled(False)
        
        # Stop animation
        self.animation_widget.stop_animation()
        
    def closeEvent(self, event):
        """Handle window close event"""
        self.stop_simulation()
        self.animation_widget.stop_animation()
        event.accept()

def main():
    """主函數"""
    app = QApplication(sys.argv)
    app.setApplicationName("Multibody Dynamics Solver")
    app.setApplicationVersion("1.0")
    
    window = SimpleGUI()
    window.show()
    
    print("Starting PySide6 GUI...")
    print("啟動 PySide6 圖形界面...")
    
    sys.exit(app.exec())

if __name__ == "__main__":
    main()