#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "VisionDefs.hpp" // runswift code
#include <iostream>

using namespace cv;
using namespace std;

// no transformation for the meantime
PixelValues rgb2yuv(cv::Vec3b rgbPixel) {
    return {rgbPixel[0],rgbPixel[1],rgbPixel[2]};
}

// Top left corner of image is (0,0)
int main(int argc, char** argv) {
    if(argc < 2) {
        exit(1);
    }
    const char* filename = argv[1];
    Mat image = imread(filename, 1);

    std::vector<int> featureColours; // list of colours for each feature
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
    featureColours.push_back(cBALL);
    featureColours.push_back(cWHITE);
#else 
    // 030.png Goalpost + Ball

    cv::Rect goalpost_left(110,40,50,240);
    cv::Rect goalpost_top(110,65,400,30);
    cv::Rect goalpost_right(460,50,50,240);
    features.push_back(goalpost_left);
    features.push_back(goalpost_top);
    features.push_back(goalpost_right);
    featureColours.push_back(cGOAL_YELLOW);
    featureColours.push_back(cGOAL_YELLOW);
    featureColours.push_back(cGOAL_YELLOW);

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

    Classifier cl();
    cl.newClassificationFile(); // initiliasing the weight table

    for(size_t i = 0; i < featuresExtracted.size(); ++i) {
        cv::Mat fg_image = featuresExtracted[i];
        // Iterate through the image matrix, featuresExtracted[i].fg_image
        for(size_t y = 0; y < fg_image.rows; ++y) {
            for(size_t x = 0; x < fg_image.cols; ++x) {
                Vec3b pixel_rgb = fg_image.at<Vec3b>(y,x);
                // if not black (i.e. foreground)
                // Add candidate points to the point cloud
                if(!(pixel_rgb[0] == 0 && pixel_rgb[1] == 0 && pixel_rgb[2] == 0)) {
                    //point_cloud.push_back(pixel);
                    PixelValues p = rgb2yuv(pixel_rgb);
                    cl.classify(p.y, p.u, p.v, 1, 
                            static_cast<Colour>(featureColours[i], 0, 0, 0, false);
                }
            }
        }
    }

    // Save the point cloud as an nnmc file
    cl.saveNnmc("output.nnmc");

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
