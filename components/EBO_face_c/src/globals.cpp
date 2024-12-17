#include "globals.h"

// Constants
const int Globals::res_x = 1920;
const int Globals::res_y = 1080;
const float Globals::fact_x = res_x / 480.0f;
const float Globals::fact_y = res_y / 320.0f;
const float Globals::interpolation_speed = 0.03f;
const float Globals::progress_increment = 0.05f;
const cv::Scalar Globals::WHITE(255, 255, 255);

int Globals::randInt(const int min, const int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

std::string Globals::to_snake_case(const std::string &str) {
    std::string result;
    result.reserve(str.size());

    for (const char c : str) {
        if (std::isupper(static_cast<unsigned char>(c))) {
            if (!result.empty()) {
                result += '_';
            }
            result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        } else {
            result += c;
        }
    }

    return result;
}
