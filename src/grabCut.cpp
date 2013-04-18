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
    // Ball 005.png
    //cv::Rect rectangle(420,150,100,100);
    // Fieldline 005.png
    //cv::Rect rectangle(250,100,150,380);
    
    // Goalpost 016.png
    cv::Rect rectangle(125,80,50,240);
    cv::Mat result; // segmentation result (4 possible values)
    cv::Mat bgModel,fgModel; // the models (internally used)

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
    cv::Mat foreground(image.size(),CV_8UC3,cv::Scalar(255,255,255));
    image.copyTo(foreground,result); // bg pixels not copied

    // draw rectangle on original image
    cv::rectangle(image, rectangle, cv::Scalar(255,255,255),1);
    cv::namedWindow("Image", CV_WINDOW_NORMAL);
    cv::imshow("Image",image);

    // display result
    cv::namedWindow("Segmented Image", CV_WINDOW_NORMAL);
    cv::imshow("Segmented Image",foreground);

    waitKey();

    return 0;
}
