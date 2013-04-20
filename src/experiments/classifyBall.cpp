#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;
/** @function main */
int main(int argc, char** argv)
{
    Mat srcOrig;

    /// Read the image
    srcOrig = imread( argv[1], 1 );
    //src_grayOrig = imread(argv[1], 0);

    if( !srcOrig.data )
    { return -1; }

    namedWindow("trackbar",CV_WINDOW_NORMAL);
    namedWindow("bwImage",CV_WINDOW_NORMAL);
    namedWindow("src with detected Hough Circles", CV_WINDOW_NORMAL);
    namedWindow("edgeMap",CV_WINDOW_NORMAL);
    namedWindow("saturated", CV_WINDOW_NORMAL);

    double alpha=1.0; // 1.0 - 3.0
    double beta=0; // 0-100

    int alphaLevel=0;
    int betaLevel=255;
    createTrackbar("alpha","trackbar", &alphaLevel, 255);
    createTrackbar("beta","trackbar", &betaLevel, 255);

    int thresholdType = 0;
    int thresholdValue = 0;
    int max_binary_value = 255;
    string trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
    createTrackbar(trackbar_type,"trackbar",&thresholdType,5);
    createTrackbar("Threshold Value","trackbar",&thresholdValue,255);
    /// Initialize values
    //  std::cout<<"* Enter the alpha value [1.0-3.0]: ";std::cin>>alpha;
    //  std::cout<<"* Enter the beta value [0-100]: "; std::cin>>beta;

    do {
        alpha = (double)alphaLevel/255 * 2.0 + 1.0;
        beta = (double)betaLevel/255 * 100;
        Mat src = srcOrig.clone();

        Mat saturated = Mat::zeros( src.size(), src.type() );
        /// Do the operation saturated(i,j) = alpha*image(i,j) + beta
        for( int y = 0; y < src.rows; y++ ) { 
            for( int x = 0; x < src.cols; x++ ) { 
                for( int c = 0; c < 3; c++ ) {
                    saturated.at<Vec3b>(y,x)[c] =
                        saturate_cast<uchar>( alpha*( src.at<Vec3b>(y,x)[c] ) + beta );
                }
            }
        }

        Mat src_gray,bwImage;
        cvtColor(src,src_gray,CV_RGB2GRAY);
        
        threshold(src_gray, bwImage, thresholdValue, max_binary_value,thresholdType);
        Mat edgeMap;
        //Canny(saturated,edgeMap, 150,200,3);
        Canny(bwImage,edgeMap, 150,200,3);

        /// Reduce the noise so we avoid false circle detection
        GaussianBlur( edgeMap, edgeMap, Size(7, 7), 2, 2 );
#if 1
        vector<Vec3f> circles;

        /// Apply the Hough Transform to find the circles
        HoughCircles(edgeMap, circles, CV_HOUGH_GRADIENT, 1, 1, 200, 100, 0, 0 );
        cout << circles.size() << endl;
        /// Draw the circles detected
        for( size_t i = 0; i < circles.size(); i++ )
        {
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            // circle center
            circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
            // circle outline
            circle( src, center, radius, Scalar(255,0,0), 3, 8, 0 );
        }
#endif
        imshow("src with detected Hough Circles",src);
        imshow("bwImage",bwImage);
        imshow("edgeMap", edgeMap);
        imshow("saturated", saturated);
    } while(waitKey(0) != 27);  
    return 0;
}
