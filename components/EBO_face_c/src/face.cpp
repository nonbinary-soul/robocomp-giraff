//
// Created by lee on 11/14/24.
//

#include "face.h"
using namespace std;

Face::Face()
    : pup_x(0), pup_y(0), t(0.9), stopped(false), isTalking(false),
      isListening(false), pupilaFlag(false), val_lim(10 * fact_x),
      val_lim_x(20 * fact_x), val_lim_y(20 * fact_y)
{
    img = cv::Mat::zeros(res_y, res_x, CV_8UC3);  // Initialize image with a blank background
    config = std::map<std::string, ConfigPart>();  // Initialize configuration maps
    old_config = config;
    config_target = config;
}

Face::~Face() {}

void Face::initialize()
{
    // Initialize face parameters and start the face animation thread
    std::thread(&Face::runFaceThread, this).detach();
}
