//
// Created by lee on 11/14/24.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include <opencv2/opencv.hpp>
#include <random>
#include <specificworker.h>

// colors
extern const cv::Scalar WHITE;

// functions
int randInt(int min, int max);

// Structure for shared image data
typedef struct SharedData {
    cv::Mat image;
    std::mutex lock;
} SharedData;

extern  SharedData shared_data;
#endif //GLOBALS_H
