cmake_minimum_required(VERSION 3.10)
project(DynamicObjectDetection)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. STANDART ARAMA (Windows/Normal Linux)
find_package(OpenCV QUIET)

if(OpenCV_FOUND)
    message(STATUS "Standard OpenCV found at: ${OpenCV_DIR}")
    include_directories(${OpenCV_INCLUDE_DIRS})
    set(LIBS ${OpenCV_LIBRARIES})
else()
    # 2. VLAB/MATLAB ÖZEL YOLLARI (Zorunlu Fallback)
    message(STATUS "Standard OpenCV not found. Using DEI VLAB specific paths...")
    
    set(VLAB_ROOT "/usr/local/MATLAB/R2025b/toolbox/images/opencv/opencvcg")
    set(VLAB_BIN "${VLAB_ROOT}/bin/glnxa64")
    
    # Tüm olası include klasörlerini ekliyoruz
    include_directories("${VLAB_ROOT}/include")
    include_directories("${VLAB_ROOT}/include/opencv4")
    
    # Kütüphaneleri TAM YOL ile veriyoruz (Linker hatalarını aşmak için en garanti yol)
    set(LIBS 
        "${VLAB_BIN}/libopencv_imgcodecs.so"
        "${VLAB_BIN}/libopencv_core.so"
        "${VLAB_BIN}/libopencv_imgproc.so"
        "${VLAB_BIN}/libopencv_highgui.so"
        "${VLAB_BIN}/libopencv_video.so"
        "${VLAB_BIN}/libopencv_videoio.so"
        "${VLAB_BIN}/libopencv_features2d.so"
        "${VLAB_BIN}/libopencv_calib3d.so"
    )
    message(STATUS "VLAB libraries manually linked with absolute paths.")
endif()

add_executable(ObjectDetector src/Yusuf_Celikkiran.cpp)
target_link_libraries(ObjectDetector ${LIBS})