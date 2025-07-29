# Animation Features / 動畫功能

## Overview / 概述

The PySide6 GUI now includes a real-time animation window that visualizes the multibody dynamics simulation using Qt's native graphics capabilities.

PySide6 GUI 現在包含一個實時動畫視窗，使用 Qt 原生圖形功能來可視化多體動力學模擬。

## Features / 功能特點

### Real-time Visualization / 實時可視化
- Live display of multibody system motion during simulation
- Smooth 20 FPS animation updates
- 3D-like perspective projection with coordinate system

### Interactive Controls / 互動控制
- **Mouse Wheel**: Zoom in/out (10x - 200x scale)
- **Left Click + Drag**: Pan view around the simulation space
- **Right Click + Drag**: Rotate 3D camera view (pitch and yaw)
- **Arrow Keys**: Precise 15-degree rotation steps
- **R Key**: Reset camera view to default position
- **Trail Toggle**: Show/hide motion trails
- **Clear Trail**: Reset trail history
- **Reset View Button**: Return to default camera position
- **Start Recording**: Begin capturing animation frames
- **Stop Recording**: End frame capture
- **Export Animation**: Save recorded frames as GIF or image sequence

### Visual Elements / 視覺元素
- **Ground Body**: Green square representing the fixed base
- **Mobile Bodies**: Colored circles with body numbers (B1, B2, etc.)
- **Joints**: White lines connecting bodies
- **Body Orientation**: Yellow lines showing rotation angles
- **Motion Trails**: Semi-transparent trails showing movement history
- **Coordinate System**: RGB axes (X-red, Y-green, Z-blue) that rotate with camera view

### Information Display / 信息顯示
- Current simulation time
- Step count
- Number of bodies
- Current zoom scale
- Current rotation angles (X and Y)
- Interactive control hints

## Usage / 使用方法

1. **Start Simulation**: Click "開始 / Start" to begin simulation and animation
2. **View Controls**: 
   - Use mouse wheel to zoom in/out for better detail
   - Left click and drag to pan around the simulation space
   - Right click and drag to rotate the 3D camera view
   - Use arrow keys for precise rotation adjustments
   - Press 'R' key to reset camera to default view
3. **Trail Effects**: 
   - Toggle "顯示軌跡 / Show Trail" to see motion history
   - Click "清除軌跡 / Clear Trail" to reset trail data
4. **Animation Recording**:
   - Click "開始錄製 / Start Recording" to begin capturing frames
   - Run simulation while recording to capture animation
   - Click "停止錄製 / Stop Recording" to finish capture
   - Click "導出動畫 / Export Animation" to save as GIF or image sequence
5. **Camera Control**:
   - Click "重置視角 / Reset View" to return camera to default position
6. **3D camera rotation**: Use right click and drag to rotate the 3D camera view with pitch and yaw control
6. **Reset**: Click "重置 / Reset" to restart simulation and clear trails

## Technical Implementation / 技術實現

### Animation Widget (AnimationWidget)
- Custom QWidget with paintEvent override
- QPainter-based 2D graphics with 3D projection
- Real-time data updates from simulation worker thread
- Mouse event handling for interaction

### Coordinate Transformation
- 3D rotation matrices for pitch/yaw camera control
- World coordinates → Rotated coordinates → Screen coordinates  
- Y-axis flip for proper physics display
- Perspective effect for Z-axis depth
- Scalable zoom with center preservation
- Real-time coordinate system rotation display

### Performance Optimization
- Efficient painting with anti-aliasing
- Limited trail history (50 frames max)
- Update frequency control (every 10 simulation steps)
- Threaded simulation to keep GUI responsive

### Export Formats / 導出格式
- **GIF Animation**: Animated GIF with loop (requires Pillow, multiple fallback methods)
- **PNG Sequence**: High-quality lossless image sequence
- **JPEG Sequence**: Compressed image sequence with 90% quality

### Export Reliability / 導出可靠性
The GIF export uses multiple fallback methods to ensure compatibility:
1. **Primary**: Direct QImage to PIL conversion with numpy
2. **Fallback 1**: Manual byte-by-byte conversion 
3. **Fallback 2**: Temporary PNG files converted to GIF
4. **Final Fallback**: PNG sequence if all GIF methods fail

## Requirements / 系統要求

### Required / 必需
```bash
pip install PySide6
```

### Optional / 可選
```bash
pip install Pillow  # For GIF animation export
pip install numpy   # For improved GIF conversion (recommended)
```

The animation system uses PySide6's built-in graphics capabilities. Pillow is needed for GIF export, numpy improves GIF conversion reliability.

動畫系統使用 PySide6 的內建圖形功能。GIF 導出需要 Pillow，numpy 能提高 GIF 轉換的可靠性。