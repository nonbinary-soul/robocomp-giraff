#ifndef GLOBALS_H
#define GLOBALS_H

#include <opencv2/opencv.hpp>
#include <random>

class Globals {
public:
    // variables
    static const int res_x;
    static const int res_y;
    static const float fact_x;
    static const float fact_y;
    static const float OFFSET;
    static const float interpolation_speed;

    // colors
    static const cv::Scalar WHITE;

    // functions
    static int randInt(int min, int max);
};

#endif //GLOBALS_H
