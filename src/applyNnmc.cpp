/*
 * applyNnmc.cpp
 * Given an .nnmc file, apply it to the given image
 * By Calvin Tam
 *
 */

#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cerr << "Usage: ./applyNnmc IMAGE_PATH NNMC_PATH" << endl;
        return 1;
    }
    // pass nnmc to runswift code

}
