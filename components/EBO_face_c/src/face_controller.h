#ifndef FACE_CONTROLLER_H
#define FACE_CONTROLLER_H

#include "face_renderer.h"
#include <nlohmann/json.hpp>
#include <map>
#include <string>
#include <mutex>
#include <iostream>
#include <globals.h>

class FaceController {
public:
    explicit FaceController(FaceRenderer &renderer);
    void setEmotion(const std::string &emotion); // Set the emotion
    void setPupilPosition(float x, float y); // Update pupil position
    void setPupilState(bool active); // Enable or disable pupil movement
    void setTalking(bool talking); // Enable or disable talking
    void setListening(bool listening); // Enable or disable listening
    void update(); // Update the animation state
    void loadEmotionConfig(const std::string &emotion, const nlohmann::json &jsonData); // Load configuration from JSON

private:
    FaceRenderer &renderer;
    std::map<std::string, std::map<std::string, sf::Vector2f>> emotionsConfig; // Emotion configurations
    std::map<std::string, sf::Vector2f> currentConfig; // Current face configuration
    std::map<std::string, sf::Vector2f> targetConfig; // Target face configuration
    std::mutex configMutex;

    float interpolationFactor;
    bool isTalking;
    bool isListening;
    bool pupilActive;
    sf::Vector2f pupilPosition;

    void interpolateConfigs(); // Smooth transition between configurations
};

#endif // FACE_CONTROLLER_H