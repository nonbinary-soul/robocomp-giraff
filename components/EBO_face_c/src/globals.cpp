//
// Created by lee on 11/14/24.
//

#include "globals.h"
using namespace std;

const int res_x = 1280;
const int res_y = 720;
const float fact_x = static_cast<float>(res_x) / 480;
const float fact_y = static_cast<float>(res_y) / 320;
const float OFFSET = 0.06666666666666667f;
const cv::Scalar WHITE(255, 255, 255);

// functions
int randInt(int min, int max) {
    random_device rd;
    mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}