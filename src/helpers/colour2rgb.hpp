#include "VisionDefs.hpp"

cv::Vec3b convertColourToRgb(Colour c) {
    switch(c) {
        case cBALL:         return cv::Vec3b(0,165,255);    break;
        case cGOAL_YELLOW:  return cv::Vec3b(0,255,255);    break;
        case cROBOT_BLUE:   return cv::Vec3b(204,153,0);    break;
        case cROBOT_RED:    return cv::Vec3b(0,0,139);      break;
        case cFIELD_GREEN:  return cv::Vec3b(0,128,0);      break;
        case cWHITE:        return cv::Vec3b(255,255,255);  break;
        case cBLACK:        return cv::Vec3b(0,0,0);        break;
        case cBACKGROUND:   return cv::Vec3b(153,0,204);    break;
        case cUNCLASSIFIED: return cv::Vec3b(255,255,153);  break;
        default:            return cv::Vec3b(0,0,0);        
    }
}
