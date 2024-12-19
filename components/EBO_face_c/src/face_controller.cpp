#include "face_controller.h"

FaceController::FaceController(FaceRenderer &renderer)
    : renderer(renderer), interpolation_progress(0.0f), isTalking(false),
      isListening(false), pupilActive(false), pupilPosition({0.0f, 0.0f}) {}

void FaceController::requestUpdate() {
    needUpdate = true;
}

void FaceController::disableUpdate() {
    needUpdate = false;
}

bool FaceController::shouldUpdate() const {
    return needUpdate;
}

void FaceController::setEmotion(const std::string &emotion) {
    std::lock_guard<std::mutex> lock(configMutex);

    auto it = emotionsConfig.find(emotion);
    if (it != emotionsConfig.end()) {
        targetConfig = it->second;
        interpolation_progress = 0.0f; // allows to init the interpolation process and change the face
        requestUpdate();
    } else {
        throw std::runtime_error("Emotion not found: " + emotion);
    }
}

void FaceController::setPupilPosition(float x, float y) {
    std::lock_guard<std::mutex> lock(configMutex);
    pupilPosition = {x, y};
    requestUpdate();
}

void FaceController::setPupilState(bool active) {
    std::lock_guard<std::mutex> lock(configMutex);
    pupilActive = active;
    requestUpdate();
}

void FaceController::setTalking(bool talking) {
    std::lock_guard<std::mutex> lock(configMutex);
    isTalking = talking;
    requestUpdate();
}

void FaceController::setListening(bool listening) {
    std::lock_guard<std::mutex> lock(configMutex);
    isListening = listening;
    requestUpdate();
}

void FaceController::update() {
    std::cout<<"FaceController::update"<<std::endl;

    std::lock_guard<std::mutex> lock(configMutex);

    // interpolate configurations if needed
    while (interpolation_progress < 1.0f) {
        applyInterpolation();

        // incrementing the progress and ensuring it doesn't exceed 1.0f
        interpolation_progress += Globals::progress_increment;
        interpolation_progress = std::min(interpolation_progress, 1.0f);
    }

    // updating face rendering
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

void FaceController::applyInterpolation() {
    // iterating over each key-value pair in targetConfig
    for (const auto &[featureName, targetPosition] : targetConfig) {
        // getting the current position (x,y) for the same feature
        auto &currentPosition = currentConfig[featureName];

        // interpolating the x and y coordinates
        // currentConfig is updated directly through currentPosition
        currentPosition.x += (targetPosition.x - currentPosition.x) * Globals::interpolation_speed;
        currentPosition.y += (targetPosition.y - currentPosition.y) * Globals::interpolation_speed;
    }
}

void FaceController::loadAllEmotions(const std::string &directoryPath) {
    namespace fs = std::filesystem;

    fs::path jsonPath = directoryPath;

    // checking if the directory exists and is valid
    if (!fs::exists(jsonPath) || !fs::is_directory(jsonPath)) {
        throw std::runtime_error("Emotions directory not found: " + jsonPath.string());
    }

    // iterating over all JSON files in the directory
    for (const auto &entry : fs::directory_iterator(jsonPath)) {
        if (entry.path().extension() == ".json") {
            std::ifstream inputFile(entry.path());
            if (!inputFile.is_open()) {
                std::cerr << "Error opening file: " << entry.path() << std::endl;
                continue;
            }

            nlohmann::json jsonData;
            try {
                inputFile >> jsonData;
            } catch (const nlohmann::json::parse_error &e) {
                std::cerr << "Error parsing JSON file: " << entry.path() << " - " << e.what() << std::endl;
                continue;
            }

            // getting the emotion name from the filename
            std::string emotionName = entry.path().stem().string();

            // using the parseJSON method to process and store the data
            parseEmotionJSON(emotionName, jsonData);

            std::cout << "Loaded emotion: " << emotionName << std::endl;
        }
    }
}

void FaceController::parseEmotionJSON(const std::string &emotionName, const nlohmann::json &jsonData) {

    // map to store the configuration of an emotion:
    // parts' names and its coordinates (x, y) per attribute (center, point, radius)
    std::map<std::string, sf::Vector2f> config;
    float fact_x = Globals::fact_x;
    float fact_y = Globals::fact_y;

    // iterating over each part and its attributes
    for (const auto &[part, attribute] : jsonData.items()) {
        sf::Vector2f coordinates;

        std::string part_name = Globals::to_snake_case(part);

        // processing the center
        if (attribute.contains("center")) {
            coordinates.x = attribute["center"]["x"].get<float>() * fact_x;
            coordinates.y = attribute["center"]["y"].get<float>() * fact_y;
            config[part_name + "_center"] = coordinates;
        }

        // processing the points
        std::vector<std::string> points = {"p1", "p2", "p3", "p4", "p5", "p6"};
        for (const std::string &point : points) {
            if (attribute.contains(point)) {
                coordinates.x = attribute[point]["x"].get<float>() * fact_x;
                coordinates.y = attribute[point]["y"].get<float>() * fact_y;
                config[part_name + "_" + point] = coordinates;
            }
        }

        // processing the radii
        std::vector<std::string> radii = {"r1", "r2", "r3"};
        for (const auto &radius : radii) {
            if (attribute.contains(radius)) {
                coordinates.x = attribute[radius]["value"].get<float>() * fact_x;
                coordinates.y = 0;
                config[part_name + "_" + radius] = coordinates;
            }
        }
    }

    // storing the processed configuration for the emotion
    emotionsConfig[emotionName] = config;
}
