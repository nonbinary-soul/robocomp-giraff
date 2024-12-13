#include "face_controller.h"

FaceController::FaceController(FaceRenderer &renderer)
    : renderer(renderer), interpolationFactor(0.0f), isTalking(false),
      isListening(false), pupilActive(false), pupilPosition({0.0f, 0.0f}) {}

void FaceController::setEmotion(const std::string &emotion) {
    std::lock_guard<std::mutex> lock(configMutex);

    auto it = emotionsConfig.find(emotion);
    if (it != emotionsConfig.end()) {
        targetConfig = it->second; // Set the target configuration
        interpolationFactor = 0.0f; // Reset the interpolation
    } else {
        throw std::runtime_error("Emotion not found: " + emotion);
    }
}

void FaceController::setPupilPosition(float x, float y) {
    std::lock_guard<std::mutex> lock(configMutex);
    pupilPosition = {x, y}; // Update the pupil position
}

void FaceController::setPupilState(bool active) {
    std::lock_guard<std::mutex> lock(configMutex);
    pupilActive = active; // Update the pupil activity state
}

void FaceController::setTalking(bool talking) {
    std::lock_guard<std::mutex> lock(configMutex);
    isTalking = talking;
}

void FaceController::setListening(bool listening) {
    std::lock_guard<std::mutex> lock(configMutex);
    isListening = listening;
}

void FaceController::update() {
    std::lock_guard<std::mutex> lock(configMutex);

    // Interpolate configurations if needed
    if (interpolationFactor < 1.0f) {
        interpolateConfigs();
        interpolationFactor += Globals::OFFSET;
        if (interpolationFactor > 1.0f)
            interpolationFactor = 1.0f;
    }

    // Update face rendering
    renderer.setFaceConfig(currentConfig);

    // Add logic for pupil movement
    if (pupilActive) {
        renderer.renderPupil(pupilPosition, 10.0f); // Render the pupil at the new position
    }

    // Add talking animation
    if (isTalking) {
        // Example: animate the mouth based on some predefined logic
        // Modify `currentConfig` for the mouth dynamically
    }

    if (isListening) {
        // Logic for animating listening states
    }
}

void FaceController::interpolateConfigs() {
    for (const auto &[key, target] : targetConfig) {
        auto &current = currentConfig[key];
        current.x += (target.x - current.x) * Globals::interpolation_speed;
        current.y += (target.y - current.y) * Globals::interpolation_speed;
    }
}

void FaceController::loadEmotionConfig(const std::string &emotion, const nlohmann::json &jsonData) {
    std::cout << "Loading emotion: " << emotion << "..." << std::endl;

    std::map<std::string, sf::Vector2f> config;
    float fact_x = Globals::fact_x;
    float fact_y = Globals::fact_y;

    std::cout << "Scaling factors: fact_x = " << fact_x << ", fact_y = " << fact_y << std::endl;

    for (auto &[partName, partData] : jsonData.items()) {
        sf::Vector2f partPosition;

        std::cout << "Processing part: " << partName << std::endl;

        // Process center
        if (partData.contains("center")) {
            partPosition.x = partData["center"]["x"].get<float>() * fact_x;
            partPosition.y = partData["center"]["y"].get<float>() * fact_y;
            config[partName + "_center"] = partPosition;

            std::cout << "  Center: "
                      << "Original (x=" << partData["center"]["x"] << ", y=" << partData["center"]["y"] << ")"
                      << " -> Scaled (x=" << partPosition.x << ", y=" << partPosition.y << ")" << std::endl;
        }

        // Process points
        std::vector<std::string> points = {"p1", "p2", "p3", "p4", "p5", "p6"};
        for (const std::string &point : points) {
            if (partData.contains(point)) {
                partPosition.x = partData[point]["x"].get<float>() * fact_x;
                partPosition.y = partData[point]["y"].get<float>() * fact_y;
                config[partName + "_" + point] = partPosition;

                std::cout << "  Point " << point << ": "
                          << "Original (x=" << partData[point]["x"] << ", y=" << partData[point]["y"] << ")"
                          << " -> Scaled (x=" << partPosition.x << ", y=" << partPosition.y << ")" << std::endl;
            }
        }

        // Process radii
        std::vector<std::string> radii = {"r1", "r2", "r3"};
        for (const auto &radius : radii) {
            if (partData.contains(radius)) {
                float scaledRadius = partData[radius]["value"].get<float>() * fact_x;
                config[partName + "_" + radius] = {scaledRadius, 0};

                std::cout << "  Radius " << radius << ": "
                          << "Original (value=" << partData[radius]["value"] << ")"
                          << " -> Scaled (value=" << scaledRadius << ")" << std::endl;
            }
        }
    }

    emotionsConfig[emotion] = config;
}
