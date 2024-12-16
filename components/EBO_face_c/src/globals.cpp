#include "globals.h"

const int Globals::res_x = 1920;
const int Globals::res_y = 1080;
const float Globals::fact_x = res_x / 480.0f;
const float Globals::fact_y = res_y / 320.0f;
const float Globals::interpolation_speed = 0.05f;
const float Globals::progress_increment = 0.07f;
const cv::Scalar Globals::WHITE(255, 255, 255);

int Globals::randInt(const int min, const int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}
