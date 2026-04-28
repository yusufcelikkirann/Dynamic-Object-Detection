<div align="center">
  <h1>🎯 Dynamic Object Detection Pipeline</h1>
  <p><strong>A Kinematics and Spatial Clustering based approach to Object Detection from scratch using C++ and OpenCV</strong></p>

  <img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" />
  <img src="https://img.shields.io/badge/OpenCV-5C3EE8?style=for-the-badge&logo=opencv&logoColor=white" />
  <img src="https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white" />
  
  <br />
  <br />
</div>

## 📖 Overview

This repository features a highly robust **Motion-Based Object Detection Pipeline** built entirely without relying on pre-trained deep learning models. Instead, it leverages classical Computer Vision algorithms and mathematical modeling to achieve category-agnostic dynamic object tracking.

By integrating Sparse Feature Tracking, Optical Flow, and advanced Spatial Clustering, the system can autonomously isolate and track moving entities (from birds to vehicles) against complex backgrounds.

---

## ⚡ Core Architecture

The pipeline processes input sequences through four main algorithmic stages:

1. **Feature Extraction (Shi-Tomasi)**: Extracts strong, reliable corners from the video frames.
2. **Motion Tracking (Lucas-Kanade)**: Calculates the optical flow of the extracted features across consecutive frames to obtain precise motion vectors.
3. **Outlier Rejection (RANSAC)**: Fits an affine transformation model to estimate camera/background movement, effectively isolating the independently moving subjects.
4. **Spatial Clustering (Seeded Region Growing)**: Utilizes Median Absolute Deviation (MAD) to dynamically cluster active motion points and generate highly accurate bounding boxes.

---

## 📊 Performance & Evaluation

The pipeline was rigorously evaluated using the **Mean Intersection over Union (mIoU)** metric across five completely distinct object categories (`bird`, `car`, `frog`, `sheep`, `squirrel`).

- ✅ **Category-Agnostic**: Detects objects based solely on kinematics, bypassing standard morphological biases.
- ✅ **Optimized**: Native C++ implementation ensures high-speed frame processing and strict memory management.
- ✅ **Precision**: Consistently achieves an IoU > 0.5 against ground truth data, proving the mathematical robustness of the clustering phase.

---

## 🛠️ Setup & Installation

### Prerequisites
- **C++17** compiler
- **CMake** (v3.10+)
- **OpenCV** (v4.x)

### Build Instructions

```bash
# 1. Clone the repository
git clone https://github.com/yusufcelikkirann/Dynamic-Object-Detection.git
cd Dynamic-Object-Detection

# 2. Configure with CMake
mkdir build && cd build
cmake ..

# 3. Compile the pipeline
cmake --build .
```

### Execution
Ensure the `data/` and `labels/` folders are present in the project root directory before running the executable.
```bash
./ObjectDetector
```

Predictions and visual tracking results (with drawn bounding boxes) will be automatically generated and exported to the `output/` directory.

---

<div align="center">
  <i>Developed by <b>Yusuf Celikkiran</b></i>
</div>
