# Dynamic Object Detection Pipeline

This repository contains a C++ implementation of a motion-based object detection pipeline using classical Computer Vision algorithms. The project relies purely on kinematics and spatial features, without the use of pre-trained deep learning models.

## Key Features

- **Sparse Feature Tracking**: Uses Shi-Tomasi corner detection to identify strong features to track across frames.
- **Optical Flow**: Employs the Lucas-Kanade method to track feature movements.
- **Robust Motion Modeling**: Integrates RANSAC for affine transformation estimation, effectively rejecting background motion and isolating independently moving objects.
- **Spatial Clustering**: Implements a custom Seeded Region Growing architecture that groups moving points using spatial statistics (MAD) to estimate accurate bounding boxes.
- **Evaluation Metric**: Validated using Mean Intersection over Union (mIoU) across diverse object categories (e.g., bird, car, frog, sheep, squirrel).

## Tech Stack
- **Language**: C++
- **Libraries**: OpenCV (Core, ImgProc, HighGUI, Video, Calib3d)
- **Build System**: CMake

## Setup and Installation

1. **Clone the repository:**
   ```bash
   git clone <your-repo-url>
   cd ComputerVisionProject
   ```

2. **Build the project:**
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```

3. **Run the executable:**
   ```bash
   ./ObjectDetector
   ```
*(Note: Ensure you have the `data/` and `labels/` folders in your working directory to run the tests)*

## Results

The pipeline evaluates bounding box predictions against ground truth annotations and outputs the results, including visualized tracking boxes, directly to the output directory.

## Author
**Yusuf Celikkiran**
