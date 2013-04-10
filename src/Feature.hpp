using namespace cv;

class Feature {
public: 
    std::vector<cv::Point> region;
    std::vector<cv::Rect> vertexBounds;
    int minAbsAngle;
    int maxAbsAngle;

    Feature(std::vector<cv::Point> &region, std::vector<cv::Rect> &vertexBounds, int minAbsAngle, int maxAbsAngle);
};

Feature::Feature(std::vector<cv::Point> &region, std::vector<cv::Rect> &vertexBounds, int minAbsAngle, int maxAbsAngle) {
    this->region = region;
    this->vertexBounds = vertexBounds;
    this->minAbsAngle = minAbsAngle;
    this->maxAbsAngle = maxAbsAngle;
}
