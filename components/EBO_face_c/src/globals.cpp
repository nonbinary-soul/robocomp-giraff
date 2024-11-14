//
// Created by lee on 11/14/24.
//

#include "globals.h"
using namespace std;

const cv::Scalar WHITE(255, 255, 255);

int randInt(int min, int max) {
    random_device rd;
    mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}