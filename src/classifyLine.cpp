#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <math.h>
#include "Feature.hpp"

using namespace cv;
using namespace std;

// Bounding quadrilateral containing the pixels to be classified as training data
// For all data structures, order is clockwise from topleft i.e.
// [0] = topLeft
// [1] = topRight
// [2] = botRight
// [3] = botLeft


void overlayTemplate(Mat &src, Mat &output, vector<Feature> features);
void highLightFeatures(Mat &img, vector<Feature> &features);
void fillFeatures(Mat &img, vector<Feature> &features);
void expandBoundingArea(vector<Vec4i> &lines, vector<Feature> &features);
void updateBoundingBox(vector<Point> &fillRegion, Point p, vector<Rect> &vertexBounds);
bool inRange(double x, double min, double max);
double radToDeg(double thetaInRad);
void help();

// Top left corner of image is (0,0)
int main(int argc, char** argv) {
    if(argc < 2) {
        help();
        exit(1);
    }
    const char* filename = argv[1];
    Mat colourSrc = imread(filename, 1);
    Mat greySrc = imread(filename, 0);
    if(greySrc.empty()) {
        help();
        cout << "can not open " << filename << endl;
        return -1;
    }

    Mat outputEdgeMap, outputEdgeMap_3ch;
    Canny(greySrc, outputEdgeMap, 50, 200, 3);
    cvtColor(outputEdgeMap, outputEdgeMap_3ch, CV_GRAY2BGR);
    vector<Vec4i> lines;
    HoughLinesP(outputEdgeMap, lines, 1, CV_PI/180, 50, 50, 80);

    // Seed initial bestSoFar to be away from the target area
    vector<Point> hLineRegion;
    hLineRegion.push_back(Point(320,240));
    hLineRegion.push_back(Point(320,240));
    hLineRegion.push_back(Point(320,240));
    hLineRegion.push_back(Point(320,240));

    // bounding box for each vertex of the lineRegion
    vector<Rect> hVertexBounds;
    hVertexBounds.push_back(Rect(Point(0,80),Point(320,115)));
    hVertexBounds.push_back(Rect(Point(320,120),Point(640,140)));
    hVertexBounds.push_back(Rect(Point(320,140),Point(640,160)));
    hVertexBounds.push_back(Rect(Point(0,115),Point(320,135)));

    vector<Point> vLineRegion;
    vLineRegion.push_back(Point(415,240));
    vLineRegion.push_back(Point(415,240));
    vLineRegion.push_back(Point(415,240));
    vLineRegion.push_back(Point(415,240));

    vector<Rect> vVertexBounds;
    vVertexBounds.push_back(Rect(Point(380,110),Point(415,240)));
    vVertexBounds.push_back(Rect(Point(415,110),Point(440,240)));
    vVertexBounds.push_back(Rect(Point(415,240),Point(475,480)));
    vVertexBounds.push_back(Rect(Point(360,240),Point(415,480)));

    vector<Feature> features;
    features.push_back(Feature(hLineRegion,hVertexBounds,0,20));
    features.push_back(Feature(vLineRegion,vVertexBounds,70,90));

    Mat blended;
    overlayTemplate(colourSrc,blended,features); 

    // Now update the region defined in the template to contain a feature
    // to converge to the feature
    expandBoundingArea(lines,features);

    fillFeatures(outputEdgeMap_3ch,features);
    
    highLightFeatures(outputEdgeMap_3ch,features);


    namedWindow("blended", CV_WINDOW_NORMAL);
    imshow("blended", blended);
    namedWindow("source", CV_WINDOW_NORMAL);
    imshow("source", greySrc);
    namedWindow( "detected lines", CV_WINDOW_NORMAL);
    imshow("detected lines", outputEdgeMap_3ch);

    waitKey();

    return 0;
}

void overlayTemplate(Mat &src, Mat &output, vector<Feature> features) {
    // Overlay the template used
    Mat temp = src.clone();
    
    for(size_t i = 0; i < features.size(); ++i) {
        //Point template_pts[] = {Point(0,90), Point(640,120), Point(640,160),Point(0,140)};
        Feature f = features[i];
        vector<Point> template_pts;
        template_pts.push_back(Point(f.vertexBounds[0].x, f.vertexBounds[0].y));
        template_pts.push_back(Point(f.vertexBounds[1].x + f.vertexBounds[1].width, f.vertexBounds[1].y));
        template_pts.push_back(Point(f.vertexBounds[2].x + f.vertexBounds[2].width, f.vertexBounds[2].y + f.vertexBounds[2].height));
        template_pts.push_back(Point(f.vertexBounds[3].x, f.vertexBounds[3].y + f.vertexBounds[3].height));

        // &template_pts[0] is the first element of the vector
        fillConvexPoly(temp,&template_pts[0],4,Scalar(255,0,0), CV_AA, 0);
    }

    //Point template_pts_2[] = {Point(380,100), Point(450,100), Point(480,480),Point(350,480)};
    //fillConvexPoly(temp,&template_pts_2[0],4,Scalar(255,0,0), CV_AA, 0);
    
    double alpha = 0.6;
    double beta = 1.0 - alpha;
    double gamma = 0.0;
    addWeighted(src,alpha,temp,beta,gamma,output);
}

// Given the features, highlight the bounds
void highLightFeatures(Mat &img, vector<Feature> &features) {
    // For each feature, highlight the bounding area's edges
    for(size_t i = 0; i < features.size(); ++i) {
        line(img,features[i].region[0],features[i].region[1],Scalar(0,0,255),1,CV_AA);
        line(img,features[i].region[1],features[i].region[2],Scalar(0,0,255),1,CV_AA);
        line(img,features[i].region[2],features[i].region[3],Scalar(0,0,255),1,CV_AA);
        line(img,features[i].region[3],features[i].region[0],Scalar(0,0,255),1,CV_AA);
    }
}

// Given the features, fill in pixels that are within it
void fillFeatures(Mat &img, vector<Feature> &features) {
    // if the point is inside or on the contour
    // colour it in
    for(int y = 0; y < img.rows; ++y) {
        for(int x = 0; x < img.cols; ++x) {
            Point p = Point(x,y);
            //Vec3b colour = colourSrc.at<Vec3b>(p);
            for(size_t i = 0; i < features.size(); ++i) {
                // if the point is in the feature, colour it in
                if(pointPolygonTest(features[i].region,p,false) >= 0) {
                    line(img,p,p,Scalar(255,255,0),2,CV_AA);
                }
            }
        }
    }
}

// Given the detected HoughLines and the features to be detected
// expand the boudning area of the features if they meet certain conditions
void expandBoundingArea(vector<Vec4i> &lines, vector<Feature> &features) {
    for(size_t i = 0; i < lines.size(); i++) {
        Vec4i l = lines[i];
        // End points of detected lines
        Point pts[] = {Point(l[0], l[1]),
            Point(l[2], l[3])};

        // then theta is the principal arctan of the line (atan not atan2)
        // Hence -90 < radToDeg(theta) < 90
        // Hence 0 <= absThetaInDeg <= 90
        double thetaInRad = (l[2] == l[0]) ? M_PI/2 : atan((double)(l[3]-l[1])/(l[2]-l[0]));
        double absThetaInDeg = abs(radToDeg(thetaInRad));
        cout << "absThetaInDeg = " << absThetaInDeg << endl;

        for(size_t k = 0; k < features.size(); ++k) {
            if(inRange(absThetaInDeg,features[k].minAbsAngle,features[k].maxAbsAngle)) {
                for(int i = 0; i < 2; ++i) {
                    cout << "Before: " << features[k].region << endl;
                    updateBoundingBox(features[k].region,pts[i],features[k].vertexBounds);
                    cout << "After: " << features[k].region << endl;
                }
            }
        }
    }
}

// Given a point, if it increases the scope of the bounding box,
// update the bounding box
// @param fillRegion the region so far determined to be inside the feature
// @param vertexBounds each vertex of the field line has its own bounding box
void updateBoundingBox(vector<Point> &fillRegion, Point p, vector<Rect> &vertexBounds) {
    if(p.x < fillRegion[0].x && vertexBounds[0].contains(p)) {
        fillRegion[0] = p;
    }
    if(p.x > fillRegion[1].x && vertexBounds[1].contains(p)) {
        fillRegion[1] = p;
    }
    if(p.x > fillRegion[2].x && vertexBounds[2].contains(p)) {
        fillRegion[2] = p;
    }
    if(p.x < fillRegion[3].x && vertexBounds[3].contains(p)) {
        fillRegion[3] = p;
    }
}

double radToDeg(double thetaInRad) {
    return thetaInRad * 180 / M_PI;
}

bool inRange(double x, double min, double max) {
    return (x > min && x < max ? true : false);
}

void help() {
    cout << "\nThis program demonstrates line finding with the Hough transform.\n"
        "Usage:\n"
        "./houghlines <image_name>" << endl;
}
