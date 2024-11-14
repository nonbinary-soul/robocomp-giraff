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
    ~Face();
    void init();

    // Method executed when the thread starts; handles the continuous behavior of the face,
    // including blinking, talking, and other animations.
    void run();

    // controlling the movement of facial features
    void movePupil();
    void moveFace(bool blinkFlag, bool isTalking, bool isListening);

    // Method to draw the current configuration of the face
    void drawConfig(const std::map<std::string, std::map<std::string, std::map<std::string, int>>>& configAux);

    // Render the whole face
    cv::Mat render();

    // Rendering functions for different parts of the face
    void renderPupil(const std::vector<cv::Point>& points);
    void renderTongue(const std::vector<cv::Point>& points);
    void renderEyelid(const std::vector<cv::Point>& points);
    void renderCheek(const std::vector<cv::Point>& points);
    void renderEyebrow(const std::vector<cv::Point>& points);
    void renderEye(const std::vector<cv::Point>& points);
    void renderMouth(const std::vector<cv::Point>& points);

    // Method to record a point (used for tracking)
    void recordPoint();

    // Set the configuration of the face
    void setConfig(const std::map<std::string, std::map<std::string, std::map<std::string, int>>>& config);

    // Set whether the face is talking
    void setTalking(bool talking);
    // Set whether the face is listening
    void setListening(bool listening);

    // Stop the face thread
    void stop();

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

    cv::Mat rotateImage(const cv::Mat& image, double angle);
};

#endif //FACE_H
