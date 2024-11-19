//
// Created by lee on 11/14/24.
//

#ifndef FACE_H
#define FACE_H

#include <globals.h>
#include <specificworker.h>
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
    struct Point {
        float x, y;
    };

    struct Radius {
        float Value;
    };

    struct ConfigPart {
        Point P1, P2, P3, P4, P5, P6, Center;
        Radius Radius1, Radius2, Radius3;
    };

    Face();
    ~Face();

    Point createCoordinate(float x, float y);
    // Calculate a point on a Bézier curve between two points
    Point bezier(const Point& p1, const Point& p2, float t);
    // Get a list of points along a Bézier curve
    std::vector<Point> getPointsBezier(std::vector<Point>& points);
    // Interpolate between two configurations using Bézier curve
    std::map<std::string, ConfigPart> getBezierConfig(
        const std::map<std::string, ConfigPart>& old_config,
        const std::map<std::string, ConfigPart>& config_target,
        float t);

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

    std::map<std::string, std::map<std::string, std::map<std::string, int>>> DEFAULTCONFIGNEUTRAL;
    std::map<std::string, std::map<std::string, std::map<std::string, int>>> config;
    std::map<std::string, std::map<std::string, std::map<std::string, int>>> old_config;
    double t;
    std::map<std::string, std::map<std::string, std::map<std::string, int>>> config_target;
    bool stopped;
    bool isTalking;
    bool isListening;
    bool pupilFlag;
    int val_lim;
    int val_lim_x;
    int val_lim_y;

    cv::Mat rotateImage(const cv::Mat& image, double angle);
};

#endif //FACE_H
