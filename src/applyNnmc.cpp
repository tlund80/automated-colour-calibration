/*
 * applyNnmc.cpp
 * Given an .nnmc file, apply it to the given image
 * By Calvin Tam
 *
 */
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include "NNMC.hpp"
#include "NNMCinline.hpp"
#include "VisionDefs.hpp"
#include "rgb2yuv.hpp"
using namespace cv;
using namespace std;

cv::Vec3b convertColourToVec3b(Colour c);

int main(int argc, char *argv[]) {
    if(argc != 4) {
        cerr << "Usage: ./applyNnmc IMAGE_PATH OUTPUT_IMAGE_PATH NNMC_PATH" << endl;
        return 1;
    }
    
    cv::Mat origImage = imread(argv[1],1);
    cv::Mat saliencyImage = origImage.clone();
    // Pass nnmc to runswift code
    NNMC nnmc;
    nnmc.load(argv[3]);
    if(!(nnmc.isLoaded())) {
        cerr << "NNMC file not loaded correctly" << endl;
        return 1; 
    }
    else {
        for(int y = 0; y < origImage.rows; ++y) {
            for(int x = 0; x < origImage.cols; ++x) {
                PixelValues p = rgb2yuv(origImage.at<Vec3b>(y,x));
                Colour classified = nnmc.classify(p.y,p.u,p.v);
                Vec3b rgb = convertColourToVec3b(classified);
                saliencyImage.at<Vec3b>(y,x) = rgb;
            }
        }
    }
    // Save the colour classified saliency image
    imwrite(argv[2],saliencyImage); 
    cv::namedWindow(argv[1],CV_WINDOW_NORMAL);
    cv::imshow(argv[1],origImage);
    cv::namedWindow(argv[2], CV_WINDOW_NORMAL);
    cv::imshow(argv[2],saliencyImage);
    return 0;
}

cv::Vec3b convertColourToVec3b(Colour c) {
    switch(c) {
        case cBALL:         return Vec3b(0,165,255);    break;
        case cGOAL_YELLOW:  return Vec3b(0,255,255);    break;
        case cROBOT_BLUE:   return Vec3b(204,153,0);    break;
        case cROBOT_RED:    return Vec3b(0,0,139);      break;
        case cFIELD_GREEN:  return Vec3b(0,128,0);      break;
        case cWHITE:        return Vec3b(255,255,255);  break;
        case cBLACK:        return Vec3b(0,0,0);        break;
        case cBACKGROUND:   return Vec3b(153,0,204);    break;
        case cUNCLASSIFIED: return Vec3b(255,255,153);  break;
        default:            return Vec3b(0,0,0);        
    }
}
