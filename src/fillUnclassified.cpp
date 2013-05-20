/*
 * fillUnclassified.cpp
 * Given a partially classified image
 * Fills in all unclassified pixels with the cUNCLASSIFIED colour
 * By Calvin tam
 */
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "helpers/VisionDefs.hpp"
#include "helpes/colour2rgb.hpp"
#include <iostream>
using namespace cv;
using namespace std;

bool isClassifiedColour(cv::Vec3b pixel);

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cerr << "Usage: ./fillUnclassified.cpp [IMAGE_PATH] [FILLED_IMAGE_PATH]" << endl;
        return 1;
    }

    cv::Mat partialImage = imread(argv[1],1);
    
    for(int y = 0; y < partialImage.rows; ++y) {
        for(int x = 0; x < partialImage.cols; ++x) {
            Vec3b pixel = partialImage.at<Vec3b>(y,x);
            if(!isClassifiedColour(pixel)) {
                partialImage.at<Vec3b>(y,x) = convertColourToRgb(cUNCLASSIFIED);
            }
        }   
    }
    imwrite(argv[2],partialImage);
    cv::namedWindow(argv[1],CV_WINDOW_NORMAL);
    cv::imshow(argv[1],partialImage);

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
