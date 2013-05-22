/*
 * compareError.cpp
 * Given two images of the same size
 * Compare pixel by pixel
 * Report the error rate between the test image and the ground truth image
 * By Calvin Tam
 *
 */
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "helpers/VisionDefs.hpp"
#include "helpers/colour2rgb.hpp"
#include <iostream>
#include <iomanip>
using namespace cv;
using namespace std;

bool isClassifiedColour(cv::Vec3b pixel);

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cerr << "Usage: ./compareError [GROUND_TRUTH_IMAGE_PATH] [TEST_IMAGE_PATH]" << endl;
        return 1;
    }

    cv::Mat truthImage = imread(argv[1],1);
    cv::Mat testImage = imread(argv[2],1);
    
    if(!(truthImage.rows == testImage.rows &&
       truthImage.cols == testImage.cols)) {
        cerr << "Error: images have different dimensions" << endl;
        return 1;
    }
    // Pixel-by-pixel comparison
    long numPixelMatches = 0;
    for(int y = 0; y < truthImage.rows; ++y) {
        for(int x = 0; x < truthImage.cols; ++x) {
            Vec3b truth = truthImage.at<Vec3b>(y,x);
            Vec3b test = testImage.at<Vec3b>(y,x);
            if(truth == test && isClassifiedColour(truth)) {
                numPixelMatches++;
            }
        }   
    }
    double errorRate = 1.0 - (double) numPixelMatches / (truthImage.rows * truthImage.cols);
    cout << std::setprecision(4) << errorRate * 100 << "%" << endl;
    cv::namedWindow(argv[1],CV_WINDOW_NORMAL);
    cv::imshow(argv[1],truthImage);
    cv::namedWindow(argv[2],CV_WINDOW_NORMAL);
    cv::imshow(argv[2],testImage);

    cv::waitKey();
    return 0;
}


// Given a pixel, return true if it is a classified SPL colour, else false
// This functions helps to exclude those pixels marked as 'unclassified' when performing pixel comparison
bool isClassifiedColour(cv::Vec3b pixel) {
    for(int i = 0; i < cNUM_COLOURS; ++i) {
        if(convertColourToRgb((Colour)i) == pixel) {
            return true;
        }
    }
    return false;
}
