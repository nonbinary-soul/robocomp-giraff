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
#include <specificworker.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <SFML/Graphics.hpp>

class Face {
public:
    struct Point {
        float x, y;
    };

    struct Radius {
        float value;
    };

    struct ConfigPart {
        Point p1, p2, p3, p4, p5, p6, center;
        Radius r1, r2, r3;
    };

    Face(int res_x, int res_y, float fact_x, float fact_y, float OFFSET);
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
    void drawConfig(const std::map<std::string, Face::ConfigPart>& configAux);

    // Render the whole face
    cv::Mat render();

    // Rendering functions for different parts of the face
    void renderEyebrow(const ConfigPart& eyebrow, sf::RenderWindow& window);
    void renderEyelid(const ConfigPart& eyelid, sf::RenderWindow& window);
    void renderPupil(const ConfigPart& pupil, sf::RenderWindow& window);
    void renderEye(const ConfigPart& eye, sf::RenderWindow& window);
    void renderCheek(const ConfigPart& cheek, sf::RenderWindow& window);
    void renderMouth(const ConfigPart& mouth, sf::RenderWindow& window);
    void renderTongue(const ConfigPart& tongue, sf::RenderWindow& window);

    // Method to record a point (used for tracking)
    void recordPoint();

    // Set the configuration of the face
    void setConfig(std::map<std::string, ConfigPart> _config);

    // Set whether the face is talking
    void setTalking(bool _talking);
    // Set whether the face is listening
    void setListening(bool _listening);
    void setPupilFlag(bool _pupilFlag);
    void setPupX(bool _pup_x);
    void setPupY(bool _pup_y);

    // Stop the face thread
    void stop();

private:
    std::map<std::string, ConfigPart> DEFAULTCONFIGNEUTRAL;
    std::map<std::string, ConfigPart> config;
    std::map<std::string, ConfigPart> old_config;
    std::map<std::string, ConfigPart> config_target;

    cv::Mat img;

    double t;
    bool stopped;

    bool isTalking;
    bool isListening;
    bool pupilFlag;
    int pup_x;
    int pup_y;

    int val_lim;
    int val_lim_x;
    int val_lim_y;

    int res_x, res_y;
    float fact_x, fact_y;
    float OFFSET;

    cv::Mat rotateImage(const cv::Mat& image, double angle);
};

#endif //FACE_H
