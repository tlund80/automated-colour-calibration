#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;


// Top left corner of image is (0,0)
int main(int argc, char** argv) {
    if(argc < 2) {
        exit(1);
    }
    const char* filename = argv[1];
    Mat image = imread(filename, 1);

    std::vector<cv::Rect> features; // list of bounding boxes
    std::vector<cv::Mat> featuresExtracted; // list of extracted foreground images, bg is black 
#if 0
    // 005.png T fieldline + Ball

    // Ball 005.png
    cv::Rect ball(420,150,100,100);
    
    // Fieldline 005.png
    cv::Rect fieldLine(250,100,150,380);

    features.push_back(ball);
    features.push_back(fieldLine);
#else 
    // 030.png Goalpost + Ball

    cv::Rect goalpost_left(110,40,50,240);
    cv::Rect goalpost_top(110,65,400,30);
    cv::Rect goalpost_right(460,50,50,240);
    features.push_back(goalpost_left);
    features.push_back(goalpost_top);
    features.push_back(goalpost_right);

#endif

    cv::Mat imageCopy = image.clone();
    for(size_t i = 0; i < features.size(); ++i) {
        // go through each bounding box
        // extract the feature
        // and build up the segmented foreground
        cv::Mat result; // segmentation result (4 possible values)
        cv::Mat bgModel,fgModel; // the models (internally used)
        cv::Mat foreground(image.size(),CV_8UC3,cv::Scalar(0,0,0));
        cv::Rect rectangle = features[i];
        // GrabCut segmentation
        cv::grabCut(image,    // input image
                result,   // segmentation result
                rectangle,// rectangle containing foreground 
                bgModel,fgModel, // models
                1,        // number of iterations
                cv::GC_INIT_WITH_RECT); // use rectangle

        // Get the pixels marked as likely foreground
        cv::compare(result,cv::GC_PR_FGD,result,cv::CMP_EQ);
        // Generate output image
        image.copyTo(foreground,result); // bg pixels not copied

        featuresExtracted.push_back(foreground.clone());
        // draw rectangle on original image's copy
        cv::rectangle(imageCopy, rectangle, cv::Scalar(255,0,0),1);
    }

    //////////////////////////////////////////////////

    // Iterate through each image matrix
    // Note what feature it is
    // For non-black pixels, update the point cloud
    // Save the point cloud as an nnmc file
    ////////////////////////////////////////////////
    // display result
    cv::namedWindow("Image", CV_WINDOW_NORMAL);
    cv::imshow("Image",imageCopy);

    for(size_t i = 0; i< featuresExtracted.size();++i) {
        std::ostringstream stream;
        stream << "Segmented Image (" << i << ")";
        cv::namedWindow(stream.str(), CV_WINDOW_NORMAL);
        cv::imshow(stream.str(),featuresExtracted[i]);
    }

    waitKey();

    return 0;
}
