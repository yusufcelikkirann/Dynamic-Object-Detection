<div align="center">
  <h1>Dynamic Object Detection via Sparse Feature Tracking<br/>and Seeded Spatial Clustering</h1>
  
  <p><strong>Yusuf Celikkiran</strong></p>
  
  <img src="https://img.shields.io/badge/Language-C++17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" />
  <img src="https://img.shields.io/badge/Library-OpenCV%204.x-5C3EE8?style=for-the-badge&logo=opencv&logoColor=white" />
  <img src="https://img.shields.io/badge/Build-CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white" />
  <img src="https://img.shields.io/badge/Metric-mIoU-orange?style=for-the-badge" />
  
  <br/><br/>
</div>

## Abstract

This project presents a **category-agnostic dynamic object detection system** that operates without pre-trained deep learning models. The proposed pipeline combines classical Computer Vision techniques — including Shi-Tomasi corner detection, Lucas-Kanade optical flow, RANSAC-based affine motion modeling, and a novel Seeded Region Growing architecture with Median Absolute Deviation (MAD) based spatial clustering — to detect and localize independently moving objects in image sequences. The system is evaluated across five morphologically distinct categories and validated using the **Mean Intersection over Union (mIoU)** metric against ground truth annotations.

---

## 1. Introduction

Modern object detection is predominantly driven by convolutional neural networks. However, understanding the mathematical foundations of motion estimation and spatial reasoning remains essential for building robust vision systems. This project revisits classical approaches and demonstrates that accurate, category-agnostic detection can be achieved through principled algorithmic design without reliance on learned representations.

---

## 2. Methodology

The detection pipeline consists of four sequential stages:

### 2.1 Feature Extraction — Shi-Tomasi Corner Detection
Robust trackable features are extracted from input frames using the Shi-Tomasi *Good Features to Track* algorithm with adaptive thresholding. CLAHE (Contrast Limited Adaptive Histogram Equalization) is applied as a preprocessing step to normalize illumination variations across sequences.

### 2.2 Temporal Motion Estimation — Lucas-Kanade Optical Flow
Extracted features are tracked across uniformly sampled frames using the pyramidal Lucas-Kanade method (window size: 21×21, pyramid levels: 3). This yields per-feature displacement vectors that encode the motion field of the scene.

### 2.3 Background Motion Rejection — RANSAC Affine Modeling
A partial affine transformation is estimated via `estimateAffinePartial2D` with RANSAC (threshold: 1.1 px). Inliers correspond to background/camera motion; **outliers** represent independently moving objects. A temporal stability score is accumulated across frames, weighting consistently moving features higher to suppress transient noise.

### 2.4 Object Localization — Seeded Spatial Clustering
High-confidence motion points (seeds) are spatially partitioned using a distance-based graph clustering approach scaled to 8% of the image diagonal. The cluster with the highest cumulative motion energy is selected as the target object. The final bounding box is refined through:
- **Region expansion** with a relaxed motion threshold to recover object boundaries
- **MAD-based outlier filtering** (3.5σ) to eliminate spatially inconsistent points
- **Adaptive padding** proportional to the detected object scale

---

## 3. Evaluation

| Metric | Description |
|--------|-------------|
| **IoU** | Intersection over Union between predicted and ground truth bounding boxes |
| **mIoU** | Mean IoU computed across all five test categories |
| **Detection Accuracy** | Percentage of categories achieving IoU ≥ 0.5 |

The system is evaluated on five categories with significantly different visual characteristics:

| Category | Type | Characteristics |
|----------|------|----------------|
| `bird` | Deformable | Non-rigid body, wing articulation |
| `car` | Rigid | Structured geometry, uniform motion |
| `frog` | Small-scale | Low pixel footprint, sudden displacement |
| `sheep` | Textured | Complex fur texture, subtle movement |
| `squirrel` | Irregular | Non-uniform shape, rapid direction changes |

---

## 4. Technical Stack

| Component | Technology |
|-----------|-----------|
| Language | C++17 |
| Vision Library | OpenCV 4.x (Core, ImgProc, Video, Calib3d, HighGUI) |
| Build System | CMake 3.10+ |
| Output Format | JPEG (optimized compression) |

---

## 5. Build & Run

```bash
# Clone
git clone https://github.com/yusufcelikkirann/Dynamic-Object-Detection.git
cd Dynamic-Object-Detection

# Build
mkdir build && cd build
cmake ..
cmake --build .

# Execute
./ObjectDetector
```

> **Note:** Ensure `data/` and `labels/` directories are present in the project root before execution. Results (annotated images and prediction coordinates) are exported to the `output/` directory.

---

## 6. Repository Structure

```
Dynamic-Object-Detection/
├── src/
│   └── Yusuf_Celikkiran.cpp    # Main pipeline implementation
├── data/                        # Input image sequences (per category)
├── labels/                      # Ground truth bounding box annotations
├── output/                      # Generated predictions and visualizations
├── include/                     # Header files
├── CMakeLists.txt               # Build configuration
└── README.md
```

---

## 7. Key References

- **Shi, J. & Tomasi, C.** (1994). *Good Features to Track.* IEEE CVPR.
- **Lucas, B.D. & Kanade, T.** (1981). *An Iterative Image Registration Technique with an Application to Stereo Vision.* IJCAI.
- **Fischler, M.A. & Bolles, R.C.** (1981). *Random Sample Consensus: A Paradigm for Model Fitting.* Communications of the ACM.
- **Bradski, G.** (2000). *The OpenCV Library.* Dr. Dobb's Journal of Software Tools.

---

<div align="center">
  <sub>Developed by <strong>Yusuf Celikkiran</strong> · 2026</sub>
</div>
