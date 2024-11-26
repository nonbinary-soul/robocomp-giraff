//
// Created by lee on 11/14/24.
//

#ifndef FACE_H
#define FACE_H

#include <globals.h>
#include <thread>
#include <chrono>
#include <random>
#include <string>
#include <map>
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

    struct FacialGeometry {
        Point p1, p2, p3, p4, p5, p6, center;
        Radius r1, r2, r3;
    };

    static std::unordered_map<std::string, std::map<std::string, FacialGeometry>> emotionsConfig;

    Face();
    Face(int res_x, int res_y, float fact_x, float fact_y, float OFFSET);
    ~Face();

    [[nodiscard]] Point createCoordinate(float x, float y) const;
    // Calculate a point on a Bézier curve between two points
    static Point bezier(const Point& p1, const Point& p2, float interpolationFactor);
    // Get a list of points along a Bézier curve
    static std::vector<Point> getPointsBezier(const std::vector<Point>& points) ;
    // Interpolate between two configurations using Bézier curve
    static std::map<std::string, FacialGeometry> getBezierConfig(
        const std::map<std::string, FacialGeometry>& _old_config,
        const std::map<std::string, FacialGeometry>& _config_target,
        float interpolationFactor);

    void init();
    // Method executed when the thread starts; handles the continuous behavior of the face,
    // including blinking, talking, and other animations.
    void run();

    // controlling the movement of facial features
    void movePupil();
    void moveFace(bool blinkFlag, bool isTalking, bool isListening);

    // Method to draw the current configuration of the face
    void drawConfig(const std::map<std::string, FacialGeometry>& configAux) const;

    // Render the whole face
    cv::Mat render();

    // Rendering functions for different parts of the face
    static void renderEyebrow(const FacialGeometry& eyebrow, sf::RenderWindow& window);
    static void renderEyelid(const FacialGeometry& eyelid, sf::RenderWindow& window);
    static void renderPupil(const FacialGeometry& pupil, sf::RenderWindow& window);
    static void renderEye(const FacialGeometry& eye, sf::RenderWindow& window);
    static void renderCheek(const FacialGeometry& cheek, sf::RenderWindow& window);
    static void renderMouth(const FacialGeometry& mouth, sf::RenderWindow& window);
    static void renderTongue(const FacialGeometry& tongue, sf::RenderWindow& window);

    // Method to record a point (used for tracking)
    void recordPoint();

    // Set the configuration of the face
    void setConfig(const std::map<std::string, FacialGeometry>& _config);

    // Set whether the face is talking
    void setTalking(bool _talking);
    // Set whether the face is listening
    void setListening(bool _listening);
    void setPupilFlag(bool _pupilFlag);
    void setPupX(float _pup_x);
    void setPupY(float _pup_y);

    // Stop the face thread
    void stop();

private:
    std::map<std::string, FacialGeometry> DEFAULT_CONFIG_NEUTRAL;
    std::map<std::string, FacialGeometry> config;
    std::map<std::string, FacialGeometry> old_config;
    std::map<std::string, FacialGeometry> config_target;

    cv::Mat img;

    double t;
    bool stopped;

    bool isTalking;
    bool isListening;
    bool pupilFlag;
    float pup_x;
    float pup_y;

    float val_lim;
    float val_lim_x, val_lim_y;

    int res_x, res_y;
    float fact_x, fact_y;
    float OFFSET;

    cv::Mat rotateImage(const cv::Mat& image, double angle);

    [[nodiscard]] std::map<std::string, FacialGeometry> JSON2ConfigPart(const nlohmann::json& jsonData) const;
    void initEmotionsConfig() const;
};

#endif //FACE_H
