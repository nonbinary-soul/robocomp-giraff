#ifndef GLOBALS_H
#define GLOBALS_H

#include <opencv2/opencv.hpp>
#include <random>
#include <specificworker.h>

class Globals {
public:
    // variables
    static const int res_x;
    static const int res_y;
    static const float fact_x;
    static const float fact_y;
    static const float OFFSET;

    // colors
    static const cv::Scalar WHITE;

    // functions
    static int randInt(int min, int max);

    typedef struct SharedData {
        cv::Mat image;
        std::mutex lock;
    } SharedData;

    static SharedData shared_data;
};

#endif //GLOBALS_H
