/*
 * compareError.cpp
 * Given two images of the same size
 * Compare pixel by pixel
 * Report the error rate between the test image and the ground truth image
 * By Calvin Tam
 *
 */
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cerr << "Usage: ./compareError.cpp GROUND_TRUTH_IMAGE_PATH TEST_IMAGE_PATH" << endl;
        return 1;
    }
    return 0;
}
