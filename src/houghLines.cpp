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

 #if 0 
  vector<Vec2f> lines;
  HoughLines(dst, lines, 1, CV_PI/180, 100, 0, 0 );

  for( size_t i = 0; i < lines.size(); i++ )
  {
     float rho = lines[i][0], theta = lines[i][1];
     Point pt1, pt2;
     double a = cos(theta), b = sin(theta);
     double x0 = a*rho, y0 = b*rho;
     pt1.x = cvRound(x0 + 1000*(-b));
     pt1.y = cvRound(y0 + 1000*(a));
     pt2.x = cvRound(x0 - 1000*(-b));
     pt2.y = cvRound(y0 - 1000*(a));
     line( cdst, pt1, pt2, Scalar(0,0,255), 1, CV_AA);
  }
 #else
  vector<Vec4i> lines;
  vector<Vec4i> leftPoints;
  HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 80 );
  for( size_t i = 0; i < lines.size(); i++ )
  {
    Vec4i l = lines[i];
    line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 1, CV_AA);
    if(l[0] > 450 &&
       l[2] < 900) {
        leftPoints.push_back(l);
    }
  }

   // fit a line for points on the left edge
  vector<Vec4i> leftLine; 
  fitLine(leftPoints,leftLine,CV_DIST_L1,0,0.01,0.01);
  cout << leftLine[0] << endl;
  // fit a line for point on the right edge

  // find top left corner of quadrilateral

  // top right

  // bottom right

  // bottom left


  Point pts[] = {Point(500,500),Point(600, 500),Point(550,1000),Point(450,1000)};
  fillConvexPoly(cdst,&pts[0],4,Scalar(0,255,0), CV_AA, 0);
  
 #endif
 imwrite( "gr.jpg", cdst);
 namedWindow("source", CV_WINDOW_NORMAL);
 imshow("source", src);
 namedWindow( "detected lines", CV_WINDOW_NORMAL);// Create a window for display.
 imshow("detected lines", cdst);

 waitKey();

 return 0;
}
