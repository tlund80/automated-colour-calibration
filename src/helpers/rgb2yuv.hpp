#include "VisionDefs.hpp"

// Note the input pixel vector is in BGR order
PixelValues rgb2yuv(cv::Vec3b i) {
    int b = 0;
    int g = 1;
    int r = 2;
    double y = 0.299*i[r] + 0.587*i[g] + 0.114*i[b];
    double u = (i[b] - y) * 0.564 + 128;
    double v = (i[r] - y) * 0.713 + 128;
    PixelValues p;
    p.y = static_cast<uint8_t>(y); 
    p.u = static_cast<uint8_t>(u);
    p.v = static_cast<uint8_t>(v);
    return p;
}
