#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "helpers/VisionDefs.hpp" // runswift code
#include "helpers/classifier.hpp" // runswift code
#include "helpers/rgb2yuv.hpp"
#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;

void inRange(int &n, int min, int max);
PixelValues rgb2yuv(cv::Vec3b i);

struct feature {
    Colour colour;
    int x;
    int y;
    int width;
    int height;
};

// Reads features from a text file
vector<feature> read_ftrs(const char* file_name) {
    ifstream is(file_name);
    vector<feature> ftrs;
    string line;
    int val = -99999;
    if(is.is_open()) {
        while(is.good()) {
            getline(is,line);
            stringstream ss(line.c_str());
            // only accepts lines with the first char being a digit 
            // i.e. it also skips lines such as those starting with '#'
            if(isdigit(ss.peek())) {
                feature f;
                for(int i = 0; ss >> val; ++i) {
                    switch(i) {
                        case 0: f.colour = (Colour)val;     break;
                        case 1: f.x = val;                  break;
                        case 2: f.y = val;                  break;
                        case 3: f.width = val;              break;
                        case 4: f.height = val;             break;
                    }
                }
                ftrs.push_back(f);
            }
        }
        is.close();
    }

    return ftrs;
}

// Top left corner of image is (0,0)
int main(int argc, char** argv) {
    if(argc != 5 || !(std::string(argv[1]) == "grabcut" || std::string(argv[1]) == "fovea")) {
        cerr << "Usage: ./grabCut [ALGORITHM=(grabcut|fovea)] [IMAGE_PATH] [GRABCUT_TEMPLATE] [OUTPUT_NNMC_PATH]" << endl;
        return 1;
    }


    std::vector<cv::Mat> origImages; // list of images, same images are not copied, they merely reference to the same block of memory
    std::vector<cv::Mat> boxedImages; // list of images with the bounding box overlayed
    std::vector<int> featureColours; // list of colours for each feature
    std::vector<cv::Rect> features; // list of bounding boxes
    std::vector<cv::Mat> featuresExtracted; // list of extracted foreground images, bg is black 

    //****************** Add features ***************
    cv::Mat oImage = imread(argv[2],1);

    std::vector<feature> featuresTemplate = read_ftrs(argv[3]);
    for(size_t i = 0; i < featuresTemplate.size(); ++i) {
        feature f = featuresTemplate[i];
        features.push_back(cv::Rect(f.x,f.y,f.width,f.height));
        featureColours.push_back(f.colour);
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

        //Fill with the background value
        result = cv::Mat::ones(origImages[i].size(), CV_8U) * cv::GC_BGD;

        //Fill the bounding box 'rectangle' with the probably-foreground value.
        cv::rectangle(result, rectangle, cv::Scalar(cv::GC_PR_FGD),-1,8,0);

        //Fill a smaller rectangle within the 'rectangle' with the foreground value.
        // This is a scaled fovea that is proportionate to the bounding box
        // Centred and scaled down by SCALE  on both axes
        const double SCALE = 0.2;
        int x = rectangle.x + ((1-SCALE)/2 * rectangle.width);
        int y = rectangle.y + ((1-SCALE)/2 * rectangle.height);
        int width = SCALE * rectangle.width;
        int height = SCALE * rectangle.height;
        cv::Rect sureFG(x,y,width,height);
        cv::rectangle(result, sureFG , cv::Scalar(cv::GC_FGD),-1,8,0);
        
        // By default pick only certain foreground pixels
        int whichPixelsToSelect = cv::GC_FGD;
        if(std::string(argv[1]) == "grabcut") {
            cv::grabCut(origImages[i], result, sureFG, bgModel, fgModel, 1, cv::GC_INIT_WITH_MASK);
            // When using Grabcut, switch to picking probable foreground pixels (GC_PR_FGD)
            whichPixelsToSelect = cv::GC_PR_FGD;
            // Reset the 'certain' foreground pixels into probable foreground pixels
            // for ease of extraction in the next step
            cv::rectangle(result, sureFG , cv::Scalar(cv::GC_PR_FGD),-1,8,0);
        }
        
        cv::compare(result,whichPixelsToSelect,result,cv::CMP_EQ);
        // Get the pixels marked as foreground
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
    cl->saveNnmc(argv[4]);

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
