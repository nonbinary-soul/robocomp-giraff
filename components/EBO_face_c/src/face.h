//
// Created by lee on 11/14/24.
//

#ifndef FACE_H
#define FACE_H

#include <globals.h>
#include <thread>
#include <chrono>
#include <random>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <opencv2/opencv.hpp>

class Face {
public:
    Face();
    void run();
    void movePupila();

private:
    cv::Mat img;
    int pup_x;
    int pup_y;
    std::map<std::string, std::map<std::string, std::map<std::string, int>>> config;
    std::map<std::string, std::map<std::string, std::map<std::string, int>>> old_config;
    double t;
    std::map<std::string, std::map<std::string, std::map<std::string, int>>> config_target;
    bool stopped;
    bool isTalking;
    bool isListening;
    bool pupilaFlag;
    int val_lim;
    int val_lim_x;
    int val_lim_y;

    void moveFace(bool pestaneoFlag, bool isTalking, bool isListening);
    void drawConfig(const std::map<std::string, std::map<std::string, std::map<std::string, int>>>& configaux);
    cv::Mat rotateImage(const cv::Mat& image, double angle);
};

#endif //FACE_H
