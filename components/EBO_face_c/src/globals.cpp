#include "globals.h"

using namespace std;

const int Globals::res_x = 1920;
const int Globals::res_y = 1080;
const float Globals::fact_x = 1.5f;
const float Globals::fact_y = 1.5f;
const float Globals::OFFSET = 10.0f;
const cv::Scalar Globals::WHITE(255, 255, 255);

int Globals::randInt(const int min, const int max) {
    random_device rd;
    mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}
