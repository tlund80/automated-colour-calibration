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

    // Specify the bounding box of the feature to extract

    // define bounding rectangle 
    // Need at foreground mask for this one
    //Ball ball_dot_top.png
    //cv::Rect rectangle(240,420,50,50);

    
    // Goalpost 016.png
    //cv::Rect rectangle(125,80,50,240);
    
    std::vector<cv::Rect> features;
#if 1
    // 005.png T fieldline + Ball

    // Ball 005.png
    cv::Rect ball(420,150,100,100);
    
    // Fieldline 005.png
    cv::Rect fieldLine(250,100,150,380);

    features.push_back(ball);
    features.push_back(fieldLine);
#else 
    // 030.png Goalpost + Ball

    features.push_back(goalpost_1);
    features.push_back(goalpost_2);
    features.push_back(goalpost_3);

#endif

    // go through each bounding box
    // extract the feature
    // and build up the segmented foreground
    cv::Mat result; // segmentation result (4 possible values)
    cv::Mat bgModel,fgModel; // the models (internally used)
    cv::Mat foreground(image.size(),CV_8UC3,cv::Scalar(0,0,0));
    for(size_t i = 0; i < features.size(); ++i) {
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

        // draw rectangle on original image
        cv::rectangle(image, rectangle, cv::Scalar(255,0,0),1);
    }
    cv::namedWindow("Image", CV_WINDOW_NORMAL);
    cv::imshow("Image",image);

    // display result
    cv::namedWindow("Segmented Image", CV_WINDOW_NORMAL);
    cv::imshow("Segmented Image",foreground);

    waitKey();

    return 0;
}
