/**
 * Author: Yusuf Celikkiran
 * Project: Feature Tracking and Dynamic Object Detection
 * Date: April 2026
 * * Description: 
 * This system implements a motion-based object detection pipeline using 
 * Sparse Feature Tracking (Shi-Tomasi), Optical Flow (Lucas-Kanade), 
 * and Seeded Spatial Clustering. Optimized for JPG export and single-folder output.
 */

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <iomanip>

using namespace std;
using namespace cv;
namespace fs = std::filesystem;

/**
 * Calculates the Intersection over Union (IoU) between two rectangles.
 */
double calculateIoU(Rect gt, Rect pred) {
    if (gt.area() <= 0 || pred.area() <= 0) return 0;
    Rect inter = gt & pred;
    double interArea = (double)inter.area();
    double unionArea = (double)gt.area() + pred.area() - interArea;
    return (unionArea > 0) ? (interArea / unionArea) : 0;
}

/**
 * Reads the ground truth bounding box from the labels directory.
 */
Rect readLabel(const string& cat) {
    vector<string> paths = { "labels/" + cat + ".txt", "labels/single_" + cat + ".txt" };
    for (const auto& p : paths) {
        ifstream f(p);
        int x1, y1, x2, y2;
        if (f >> x1 >> y1 >> x2 >> y2) return Rect(Point(x1, y1), Point(x2, y2));
    }
    return Rect(0,0,0,0);
}

int main() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

    vector<string> categories = {"bird", "car", "frog", "sheep", "squirrel"};
    double totalIoU = 0; 
    int correctDetections = 0;

    // Create a flat output directory
    string outDir = "output";
    fs::create_directories(outDir);

    cout << "---------------------------------------------" << endl;
    cout << "  Dynamic Object Detection Pipeline           " << endl;
    cout << "---------------------------------------------" << endl;

    for (const string& cat : categories) {
        string path = "data/" + cat;
        if (!fs::exists(path)) continue;

        vector<string> imgPaths;
        for (const auto& e : fs::directory_iterator(path)) imgPaths.push_back(e.path().string());
        sort(imgPaths.begin(), imgPaths.end());

        // FIX: Added cv:: for imread
        cv::Mat firstFrame = cv::imread(imgPaths[0]);
        if (firstFrame.empty()) continue;
        int W = firstFrame.cols, H = firstFrame.rows;

        // FIX: Added cv:: for Ptr, CLAHE, and createCLAHE
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8,8));
        vector<Mat> graySequence;
        int frameInterval = max(1, (int)imgPaths.size() / 25);
        for(int i=0; i<(int)imgPaths.size(); i+=frameInterval) {
            // FIX: Added cv:: for imread and cvtColor
            cv::Mat img = cv::imread(imgPaths[i]); if(img.empty()) continue;
            cv::Mat g; cv::cvtColor(img, g, cv::COLOR_BGR2GRAY);
            clahe->apply(g, g); graySequence.push_back(g);
        }

        vector<Point2f> points0;
        goodFeaturesToTrack(graySequence[0], points0, 4500, 0.005, 3);
        if (points0.empty()) continue;

        vector<Point2f> prevPoints = points0;
        vector<float> motionStrength(points0.size(), 0.0f);
        vector<int> temporalStability(points0.size(), 0);

        for (size_t i = 1; i < graySequence.size(); i++) {
            vector<Point2f> nextPoints; vector<uchar> status; vector<float> err;
            calcOpticalFlowPyrLK(graySequence[i-1], graySequence[i], prevPoints, nextPoints, status, err, Size(21, 21), 3);
            
            vector<Point2f> inliersPrev, inliersNext; vector<int> activeIdx;
            for(size_t j=0; j<status.size(); j++) {
                if(status[j]) { inliersPrev.push_back(prevPoints[j]); inliersNext.push_back(nextPoints[j]); activeIdx.push_back((int)j); }
            }

            if (inliersPrev.size() >= 4) {
                vector<uchar> transformationMask;
                estimateAffinePartial2D(inliersPrev, inliersNext, transformationMask, RANSAC, 1.1);
                for(size_t k=0; k<transformationMask.size(); k++) {
                    int pIdx = activeIdx[k];
                    if(!transformationMask[k]) {
                        temporalStability[pIdx]++;
                        motionStrength[pIdx] += (float)temporalStability[pIdx];
                    } else {
                        temporalStability[pIdx] = 0;
                    }
                }
            }
            prevPoints = nextPoints;
        }

        float maxScore = 0; for(float s : motionStrength) if(s > maxScore) maxScore = s;
        Rect predictedBox(0,0,0,0);

        if (maxScore > 0) {
            float seedThr = maxScore * 0.40f;
            vector<Point2f> seeds; vector<int> sIdx;
            for(size_t j=0; j<points0.size(); j++) if(motionStrength[j] >= seedThr) { seeds.push_back(points0[j]); sIdx.push_back((int)j); }
            
            if (!seeds.empty()) {
                float diag = sqrt((float)W*W + H*H);
                float grpD = diag * 0.08f;
                vector<int> labels;
                int nC = partition(seeds, labels, [grpD](const Point2f& a, const Point2f& b) { return norm(a-b)<grpD; });
                vector<double> scrs(nC, 0);
                for (size_t i = 0; i < seeds.size(); i++) scrs[labels[i]] += motionStrength[sIdx[i]];
                int bestL = (int)(max_element(scrs.begin(), scrs.end()) - scrs.begin());

                Point2f ctr(0,0); int cnt=0;
                for(size_t i=0; i<seeds.size(); i++) if(labels[i]==bestL) { ctr+=seeds[i]; cnt++; }
                ctr *= (1.0/cnt);

                float expT = maxScore * 0.12f;
                vector<Point2f> exp;
                for(size_t j=0; j<points0.size(); j++) if(motionStrength[j]>=expT && norm(points0[j]-ctr) < grpD*1.8f) exp.push_back(points0[j]);
                if (exp.empty()) for(size_t i=0; i<seeds.size(); i++) if(labels[i]==bestL) exp.push_back(seeds[i]);

                vector<float> fx, fy; for(auto& p : exp) { fx.push_back(p.x); fy.push_back(p.y); }
                sort(fx.begin(), fx.end()); sort(fy.begin(), fy.end());
                float mx = fx[fx.size()/2], my = fy[fy.size()/2];
                vector<float> dx, dy; for(auto& p : exp) { dx.push_back(fabs(p.x-mx)); dy.push_back(fabs(p.y-my)); }
                sort(dx.begin(), dx.end()); sort(dy.begin(), dy.end());
                float madX = max(10.0f, dx[dx.size()/2]), madY = max(10.0f, dy[dy.size()/2]);

                vector<Point2f> final;
                for(auto& p : exp) if(fabs(p.x-mx) < madX*3.5f && fabs(p.y-my) < madY*3.5f) final.push_back(p);

                if (!final.empty()) {
                    predictedBox = boundingRect(final);
                    int pad = max(2, (int)(min(predictedBox.width, predictedBox.height) * 0.15));
                    predictedBox.x = max(0, predictedBox.x-pad); predictedBox.y = max(0, predictedBox.y-pad);
                    predictedBox.width = min(W-predictedBox.x, predictedBox.width+2*pad); predictedBox.height = min(H-predictedBox.y, predictedBox.height+2*pad);
                }
            }
        }

        Rect gt = readLabel(cat); double iouValue = calculateIoU(gt, predictedBox);
        totalIoU += iouValue; if (iouValue >= 0.5) correctDetections++;
        cout << "Category: " << left << setw(10) << cat << "| IoU: " << fixed << setprecision(4) << iouValue << endl;

        // Save Results with unique names in a single folder
        Mat resImg = firstFrame.clone();
        // FIX: Added cv:: for rectangle and Scalar
        cv::rectangle(resImg, predictedBox, cv::Scalar(0,255,0), 2);
        cv::rectangle(resImg, gt, cv::Scalar(0,0,255), 1);
        
        // Save as JPG for better compression
        // FIX: Added cv:: for imwrite
        cv::imwrite(outDir + "/" + cat + "_result.jpg", resImg);
        
        ofstream pF(outDir + "/" + cat + "_prediction.txt");
        pF << predictedBox.x << " " << predictedBox.y << " " << (predictedBox.x+predictedBox.width) << " " << (predictedBox.y+predictedBox.height) << endl;
    }

    cout << "---------------------------------------------" << endl;
    cout << "FINAL mIoU      : " << totalIoU / 5 << endl;
    cout << "Detection Acc (%) : " << (correctDetections / 5.0) * 100 << endl;
    cout << "---------------------------------------------" << endl;

    return 0;
}