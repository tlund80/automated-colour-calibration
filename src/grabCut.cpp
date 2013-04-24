#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "VisionDefs.hpp" // runswift code
#include "classifier.hpp" // runswift code
#include <iostream>

using namespace cv;
using namespace std;

void inRange(int &n, int min, int max);
PixelValues rgb2yuv(cv::Vec3b i);

// Top left corner of image is (0,0)
int main(int argc, char** argv) {
    std::vector<cv::Mat> origImages; // list of images, same images are not copied, they merely reference to the same block of memory
    std::vector<cv::Mat> boxedImages; // list of images with the bounding box overlayed
    std::vector<int> featureColours; // list of colours for each feature
    std::vector<cv::Rect> features; // list of bounding boxes
    std::vector<cv::Mat> featuresExtracted; // list of extracted foreground images, bg is black 

    //****************** Add features ***************
    cv::Mat ballAndFieldLine = imread("../images/005.png",1);
    for(int i = 0; i < 3; ++i) {
        origImages.push_back(ballAndFieldLine);
    }
    // Ball 005.png
    cv::Rect ball(420,150,100,100);
    features.push_back(ball);
    featureColours.push_back(cBALL);
    // Fieldline 005.png
    cv::Rect fieldLine(250,100,150,380);
    features.push_back(fieldLine);
    featureColours.push_back(cWHITE);
    cv::Rect fieldGreen(30,160,220,320);
    features.push_back(fieldGreen);
    featureColours.push_back(cFIELD_GREEN);

    // 030.png Goalpost + Ball
    cv::Mat goalpost = imread("../images/030.png",1);
    for(int i = 0; i < 3; ++i) {
        origImages.push_back(goalpost);
    }
    cv::Rect goalpost_left(110,40,50,240);
    features.push_back(goalpost_left);
    featureColours.push_back(cGOAL_YELLOW);
    cv::Rect goalpost_top(110,65,400,30);
    features.push_back(goalpost_top);
    featureColours.push_back(cGOAL_YELLOW);
    cv::Rect goalpost_right(460,50,50,240);
    features.push_back(goalpost_right);
    featureColours.push_back(cGOAL_YELLOW);

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
                    int yRadius = 1, uRadius = 2, vRadius = 2;
#if 0
                    bool autoWeight = false;
                    if (autoWeight) {
                        float weights[CMAX];
                        cl->colourInfo(p.y, p.u, p.v, weights);
                        float totalWeight = 0.0;
                        for (int i = 0; i < CMAX; i++) {
                            totalWeight += weights[i];
                        }
                        yRadius = static_cast<int>(10.0 / (1.0 + totalWeight));
                        uRadius = static_cast<int>(20.0 / (1.0 + totalWeight));
                        vRadius = static_cast<int>(20.0 / (1.0 + totalWeight));
                        inRange(yRadius, 1, 10);
                        inRange(uRadius, 1, 20);
                        inRange(vRadius, 1, 20);
                        weight = 1.0 + (totalWeight / 10.0);
                    }
#endif
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
    cl->saveNnmc("../build/output.nnmc");

    // display result
    cv::namedWindow("BallAndFieldLine", CV_WINDOW_NORMAL);
    cv::imshow("BallAndFieldLine",ballAndFieldLine);

    cv::namedWindow("Goalpost",CV_WINDOW_NORMAL);
    cv::imshow("Goalpost",goalpost);

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
// Note the input pixel vector is in BGR order
PixelValues rgb2yuv(cv::Vec3b i) {
    int b = 0;
    int g = 1;
    int r = 2;
    double y = 0.299*i[r] + 0.587*i[g] + 0.114*i[b];
    double u = (i[b] - y) * 0.564 + 128;
    double v = (i[r] - y) * 0.713 + 128;
    PixelValues p;
    p.y = static_cast<uint8_t>(y); 
    p.u = static_cast<uint8_t>(u);
    p.v = static_cast<uint8_t>(v);
    return p;
}
