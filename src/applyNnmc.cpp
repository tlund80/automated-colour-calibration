/*
 * applyNnmc.cpp
 * Given an .nnmc file, apply it to the given image
 * By Calvin Tam
 *
 */
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include "helpers/NNMC.hpp"
#include "helpers/NNMCinline.hpp"
#include "helpers/VisionDefs.hpp"
#include "helpers/rgb2yuv.hpp"
#include "helpers/colour2rgb.hpp"
using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {
    if(argc != 4) {
        cerr << "Usage: ./applyNnmc [IMAGE_PATH] [OUTPUT_IMAGE_PATH] [NNMC_PATH]" << endl;
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
                Vec3b rgb = convertColourToRgb(classified);
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
    waitKey();
    return 0;
}
