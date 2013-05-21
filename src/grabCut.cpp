#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "helpers/VisionDefs.hpp" // runswift code
#include "helpers/classifier.hpp" // runswift code
#include "helpers/rgb2yuv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

void inRange(int &n, int min, int max);
PixelValues rgb2yuv(cv::Vec3b i);

// Top left corner of image is (0,0)
int main(int argc, char** argv) {
    if(argc != 3) {
        cerr << "Usage: ./grabCut [IMAGE_PATH] [OUTPUT_NNMC_PATH]" << endl;
        return 1;
    }

    std::vector<cv::Mat> origImages; // list of images, same images are not copied, they merely reference to the same block of memory
    std::vector<cv::Mat> boxedImages; // list of images with the bounding box overlayed
    std::vector<int> featureColours; // list of colours for each feature
    std::vector<cv::Rect> features; // list of bounding boxes
    std::vector<cv::Mat> featuresExtracted; // list of extracted foreground images, bg is black 

    //****************** Add features ***************
    cv::Mat oImage = imread(argv[1],1);
    // Ball
    cv::Rect ball(235,370,50,50);
    features.push_back(ball);
    featureColours.push_back(cBALL);
    // Robot Blue
    cv::Rect robotBlue(160,260,60,30);
    features.push_back(robotBlue);
    featureColours.push_back(cROBOT_BLUE);
    // Robot Red
    cv::Rect robotRed(330,260,60,30);
    features.push_back(robotRed);
    featureColours.push_back(cROBOT_RED);
    // Goalpost 
    cv::Rect goalpostLeft(46,32,56,268);
    features.push_back(goalpostLeft);
    featureColours.push_back(cGOAL_YELLOW);

    cv::Rect goalpostTop(120,50,320,40);
    features.push_back(goalpostTop);
    featureColours.push_back(cGOAL_YELLOW);

    cv::Rect goalpostRight(460,55,40,250);
    features.push_back(goalpostRight);
    featureColours.push_back(cGOAL_YELLOW);
    
    // Field line
    cv::Rect fieldLine(440,320,100,20);
    features.push_back(fieldLine);
    featureColours.push_back(cWHITE);

    // Field Green
    cv::Rect fieldGreen(460,320,120,130);
    features.push_back(fieldGreen);
    featureColours.push_back(cFIELD_GREEN);
    
    // Associate each feature with its source image
    size_t numImagesToFill = features.size() - origImages.size();
    for(size_t i = 0; i < numImagesToFill; ++i) {
        origImages.push_back(oImage);
    }

    //************* Process all features *********************
    for(size_t i = 0; i < features.size(); ++i) {
        // go through each bounding box
        // extract the feature
        // and build up the segmented foreground
        cv::Mat result; // segmentation result (4 possible values)
        cv::Mat bgModel,fgModel; // the models (internally used)
        cv::Mat foreground(origImages[i].size(),CV_8UC3,cv::Scalar(0,0,0));
        cv::Rect rectangle = features[i];
        // GrabCut segmentation
        cv::grabCut(origImages[i],    // input image
                result,   // segmentation result
                rectangle,// rectangle containing foreground 
                bgModel,fgModel, // models
                1,        // number of iterations
                cv::GC_INIT_WITH_RECT); // use rectangle

        // Get the pixels marked as likely foreground
        cv::compare(result,cv::GC_PR_FGD,result,cv::CMP_EQ);
        // Generate output image
        origImages[i].copyTo(foreground,result); // bg pixels not copied

        featuresExtracted.push_back(foreground.clone());
        // draw rectangle on original image's copy
        cv::Mat overlayedImage = origImages[i].clone();
        cv::rectangle(overlayedImage, rectangle, cv::Scalar(255,0,0),1);
        boxedImages.push_back(overlayedImage);
    }

    //////////////////////////////////////////////////

    Classifier *cl = new Classifier();
    cl->newClassificationFile(); // initiliasing the weight table

    for(size_t i = 0; i < featuresExtracted.size(); ++i) {
        cv::Mat fg_image = featuresExtracted[i];
        // Iterate through the image matrix, featuresExtracted[i].fg_image
        for(int y = 0; y < fg_image.rows; ++y) {
            for(int x = 0; x < fg_image.cols; ++x) {
                Vec3b pixel_rgb = fg_image.at<Vec3b>(y,x);
                // if not black (i.e. foreground)
                // Add candidate points to the point cloud
                if(!(pixel_rgb[0] == 0 && pixel_rgb[1] == 0 && pixel_rgb[2] == 0)) {
                    cout << pixel_rgb << endl;
                    PixelValues p = rgb2yuv(pixel_rgb);
                    
                    // from calibrationTab.cpp
                    // update radii
                    float weight = 1;
                    int yRadius = 2, uRadius = 4, vRadius = 4;
                    // classify!
                    Colour colour = static_cast<Colour>(featureColours[i]);
                    if (!cl->isMostlyClassified(p.y, p.u, p.v, colour)) {
                        cl->beginAction();
                        cl->classify(p.y, p.u, p.v, weight, colour, yRadius,
                                uRadius, vRadius, false);
                        cl->endAction();
                    }
                    // end from calibration.cpp
                }
            }
        }
    }

    // Save the point cloud as an nnmc file
    cl->saveNnmc(argv[2]);

    // display result
    cv::namedWindow("Original", CV_WINDOW_NORMAL);
    cv::imshow("Original",oImage);

    for(size_t i = 0; i< featuresExtracted.size();++i) {
        std::ostringstream stream;
        stream << "Segmented Image (" << i << ")";
        cv::namedWindow(stream.str(), CV_WINDOW_NORMAL);
        cv::imshow(stream.str(),featuresExtracted[i]);

        std::ostringstream boxed;
        boxed << "Boxed (" << i << ")";
        cv::namedWindow(boxed.str(), CV_WINDOW_NORMAL);
        cv::imshow(boxed.str(),boxedImages[i]);
    }

    waitKey();

    return 0;
}
/* A handy helper function */
void inRange(int &n, int min, int max) {
   if (n < min) {
      n = min;
   }
   if (n > max) {
      n = max;
   }
}
