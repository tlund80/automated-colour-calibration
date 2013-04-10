#include <cv.h>


void overlayTemplate(Mat &src, Mat &output, std::vector<Feature> features);
void highLightFeatures(Mat &img, std::vector<Feature> &features);
void fillFeatures(Mat &img, std::vector<Feature> &features);
void expandBoundingArea(std::vector<Vec4i> &lines, std::vector<Feature> &features);
void updateBoundingBox(std::vector<Point> &fillRegion, Point p, std::vector<Rect> &vertexBounds);
bool inRange(double x, double min, double max);
double radToDeg(double thetaInRad);
void help();
