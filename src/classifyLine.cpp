#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

bool inRange(double x, double min, double max);
double radToDeg(double thetaInRad);

void help()
{
    cout << "\nThis program demonstrates line finding with the Hough transform.\n"
        "Usage:\n"
        "./houghlines <image_name>, Default is pic1.jpg\n" << endl;
}

int main(int argc, char** argv)
{
    const char* filename = argc >= 2 ? argv[1] : "pic1.jpg";
    Mat colourSrc = imread(filename, 1);
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
    Point hBestTopLeft(320,240);
    Point hBestBotLeft(320,240);
    Point hBestTopRight(320,240);
    Point hBestBotRight(320,240);
    Point vBestTopLeft(415,240);
    Point vBestBotLeft(415,240);
    Point vBestTopRight(415,240);
    Point vBestBotRight(415,240);
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        // Top left corner of image is (0,0)
        // End points of detected lines
        Point pts[] = {Point(l[0], l[1]),
                       Point(l[2], l[3])};

        cout << "x0 = " << l[0] << endl;
        cout << "y0 = " << l[1] << endl;
        cout << "x1 = " << l[2] << endl;
        cout << "y1 = " << l[3] << endl;
        // then theta is the principal arctan of the line (atan not atan2)
        // Hence -90 < radToDeg(theta) < 90
        double thetaInRad = (l[2] == l[0]) ? M_PI/2 : atan((double)(l[3]-l[1])/(l[2]-l[0]));
        double thetaInDeg = radToDeg(thetaInRad);
        cout << "thetaInDeg = " << thetaInDeg << endl;
        // For the almost horizontal field line
        // Only consider detected line segments with -20 < theta < 20 
        if(inRange(thetaInDeg,-20,20)) {
            for(int i = 0; i < 2; ++i) {
                if(pts[i].x < hBestTopLeft.x && pts[i].y > 80 && pts[i].y < 115) {
                    hBestTopLeft = pts[i];
                }
                if(pts[i].x < hBestBotLeft.x && pts[i].y > 115) {
                    hBestBotLeft = pts[i];
                }
                if(pts[i].x > hBestTopRight.x && pts[i].y > 120 && pts[i].y < 140) {
                    hBestTopRight = pts[i];
                }
                if(pts[i].x > hBestBotRight.x && pts[i].y > 140) {
                    hBestBotRight = pts[i];
                }
            }
        }

        // For the almost vertical field line
        // Only consider detected line segments with  70 < theta < 90 && -90 < theta < -70
        if(inRange(thetaInDeg,70,90) || inRange(thetaInDeg,-90,-70)) {
            for(int i = 0; i < 2; ++i) {
                if(pts[i].x < vBestTopLeft.x && pts[i].y > 100 && pts[i].y < 240) {
                    vBestTopLeft = pts[i];
                }
                if(pts[i].x < vBestBotLeft.x && pts[i].y > 240) {
                    vBestBotLeft = pts[i];
                }
                if(pts[i].x > vBestTopRight.x && pts[i].y > 100 && pts[i].y < 240) {
                    vBestTopRight = pts[i];
                }
                if(pts[i].x > vBestBotRight.x && pts[i].y > 240) {
                    vBestBotRight = pts[i];
                }
            }
        }
        //line( cdst, pt_a, pt_b, Scalar(0,0,255), 1, CV_AA);
    }

    cout << "====== Vertical line corners =====" << endl;
    cout << "vBestTopLeft=" <<  vBestTopLeft<< endl;
    cout << "vBestTopRight=" <<  vBestTopRight<< endl;
    cout << "vBestBotRight=" <<  vBestBotRight<< endl;
    cout << "vBestBotLeft=" <<  vBestBotLeft<< endl;

    Point h_pts[] = {hBestTopLeft,hBestTopRight,hBestBotRight,hBestBotLeft};
    Point v_pts[] = {vBestTopLeft,vBestTopRight,vBestBotRight,vBestBotLeft};

    vector<Point> h_pts_v;
    vector<Point> v_pts_v;
    for(int i = 0; i < 4; ++i) {
        h_pts_v.push_back(h_pts[i]);
        v_pts_v.push_back(v_pts[i]);
    }

    // fill the field line area
    //fillConvexPoly(cdst,&h_pts[0],4,Scalar(0,0,255), CV_AA, 0);
    //fillConvexPoly(cdst,&v_pts[0],4,Scalar(0,0,255), CV_AA, 0);
    
    // if the point is inside or on the contour
    for(int y = 0; y < colourSrc.rows; ++y) {
        for(int x = 0; x < colourSrc.cols; ++x) {
            Point p = Point(x,y);
            //Vec3b colour = colourSrc.at<Vec3b>(p);
            if(pointPolygonTest(h_pts_v,p,false) >= 0 ||
               pointPolygonTest(v_pts_v,p,false) >= 0) {
                //cout << "(x=" << x << ", y=" << y << ")" << endl;
                //cout << static_cast<int>(colour.val[0]) << endl;
                //cout << static_cast<int>(colour.val[1]) << endl;
                //cout << static_cast<int>(colour.val[2]) << endl;
                line(cdst,p,p,Scalar(255,255,0),2,CV_AA);
                line(cdst,p,p,Scalar(255,255,0),2,CV_AA);
            }
        }
    }
    
    // highlight the horizontal field line's edges
    line(cdst,hBestTopLeft,hBestTopRight,Scalar(0,0,255),1,CV_AA);
    line(cdst,hBestBotLeft,hBestBotRight,Scalar(0,0,255),1,CV_AA);

    // highlight the vertical field line's edges
    line(cdst,vBestTopLeft,vBestBotLeft,Scalar(0,0,255),1,CV_AA);
    line(cdst,vBestTopRight,vBestBotRight,Scalar(0,0,255),1,CV_AA);
    
    // Overlay the template used
    Mat temp = colourSrc.clone();
    Point template_pts[] = {Point(0,90), Point(640,120), Point(640,160),Point(0,140)};
    fillConvexPoly(temp,&template_pts[0],4,Scalar(255,0,0), CV_AA, 0);
    Point template_pts_2[] = {Point(380,100), Point(450,100), Point(480,480),Point(350,480)};
    fillConvexPoly(temp,&template_pts_2[0],4,Scalar(255,0,0), CV_AA, 0);
    Mat blended;
    double alpha = 0.6;
    double beta = 1.0 - alpha;
    double gamma = 0.0;
    addWeighted(colourSrc,alpha,temp,beta,gamma,blended);

    namedWindow("blended", CV_WINDOW_NORMAL);
    imshow("blended", blended);

    namedWindow("source", CV_WINDOW_NORMAL);
    imshow("source", src);
    namedWindow( "detected lines", CV_WINDOW_NORMAL);// Create a window for display.
    imshow("detected lines", cdst);

    waitKey();

    return 0;
}


double radToDeg(double thetaInRad) {
    return thetaInRad * 180 / M_PI;
}

bool inRange(double x, double min, double max) {
    return (x > min && x < max ? true : false);
}
