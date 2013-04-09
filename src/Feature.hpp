using namespace std;
using namespace cv;

class Feature {
public: 
    vector<Point> region;
    vector<Rect> vertexBounds;
    int minAbsAngle;
    int maxAbsAngle;

    Feature(vector<Point> &region, vector<Rect> &vertexBounds, int minAbsAngle, int maxAbsAngle);
};

Feature::Feature(vector<Point> &region, vector<Rect> &vertexBounds, int minAbsAngle, int maxAbsAngle) {
    this->region = region;
    this->vertexBounds = vertexBounds;
    this->minAbsAngle = minAbsAngle;
    this->maxAbsAngle = maxAbsAngle;
}
