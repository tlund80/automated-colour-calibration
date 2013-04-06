#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

void help()
{
    cout << "\nThis program demonstrates line finding with the Hough transform.\n"
        "Usage:\n"
        "./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}

int main(int argc, char** argv)
{
    const char* filename = argc >= 2 ? argv[1] : "pic1.jpg";

    Mat src = imread(filename, 0);
    if(src.empty())
    {
        help();
        cout << "can not open " << filename << endl;
        return -1;
    }

    Mat dst, cdst;
    Canny(src, dst, 50, 200, 3);
    cvtColor(dst, cdst, CV_GRAY2BGR);

    vector<Vec4i> lines;
    HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 80 );
    Point bestTopLeft(320,240);
    Point bestBotLeft(320,240);
    Point bestTopRight(320,240);
    Point bestBotRight(320,240);
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        // End points of detected lines
        Point pts[] = {Point(l[0], l[1]),
                       Point(l[2], l[3])};
        // Top left corner of image is (0,0)
        for(int i = 0; i < 2; ++i) {
            if(pts[i].x < bestTopLeft.x && pts[i].y > 80 && pts[i].y < 115) {
                bestTopLeft = pts[i];
            }
            if(pts[i].x < bestBotLeft.x && pts[i].y > 115) {
                bestBotLeft = pts[i];
            }
            if(pts[i].x > bestTopRight.x && pts[i].y > 120 && pts[i].y < 140) {
                bestTopRight = pts[i];
            }
            if(pts[i].x > bestBotRight.x && pts[i].y > 140) {
                bestBotRight = pts[i];
            }
        }
        //line( cdst, pt_a, pt_b, Scalar(0,0,255), 1, CV_AA);
    }

    Point pts[] = {bestTopLeft,bestTopRight,bestBotRight,bestBotLeft};
    fillConvexPoly(cdst,&pts[0],4,Scalar(255,255,255), CV_AA, 0);

    line(cdst,bestTopLeft,bestTopRight,Scalar(0,255,0),1,CV_AA);
    line(cdst,bestBotLeft,bestBotRight,Scalar(255,0,0),1,CV_AA);
    
    namedWindow("source", CV_WINDOW_NORMAL);
    imshow("source", src);
    namedWindow( "detected lines", CV_WINDOW_NORMAL);// Create a window for display.
    imshow("detected lines", cdst);

    waitKey();

    return 0;
}
