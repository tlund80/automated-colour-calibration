using namespace cv;

class Feature {
public: 
    std::vector<cv::Point> region;
    std::vector<cv::Rect> vertexBounds;
    double angle; // -90 < angle <= 90
    double tolerance; // 0 <= tolerance <= 90

    Feature(std::vector<cv::Point> &region, std::vector<cv::Rect> &vertexBounds, double angle, double tolerance);
};

Feature::Feature(std::vector<cv::Point> &region, std::vector<cv::Rect> &vertexBounds, double angle, double tolerance) {
    this->region = region;
    this->vertexBounds = vertexBounds;
    this->angle = angle;
    this->tolerance = tolerance;
}
