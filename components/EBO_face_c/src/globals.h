//
// Created by lee on 11/14/24.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include <opencv2/opencv.hpp>
#include <random>

// screen resolution
extern const int res_x;
extern const int res_y;
extern const float fact_x;
extern const float fact_y;


extern const float OFFSET;

// colors
extern const cv::Scalar WHITE;

// functions
int randInt(int min, int max);

#endif //GLOBALS_H