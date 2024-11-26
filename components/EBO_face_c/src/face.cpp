//
// Created by lee on 11/14/24.
//

#include "face.h"

// TODO: emplear punteros si es posible
std::map<std::string, Face::FacialGeometry> Face::JSON2ConfigPart(const nlohmann::json& jsonData) const {
    std::map<std::string, FacialGeometry> configParts;

    for (auto& [partName, partData] : jsonData.items()) {
        FacialGeometry configPart = {
            {0.0f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 0.0f},
            0.0f, 0.0f, 0.0f
        };

        // radius
        if (partData.contains("r1") && partData["r1"].contains("value")) {
            configPart.r1.value = partData["r1"]["value"].get<float>() * fact_x;
        }

        if (partData.contains("r2") && partData["r2"].contains("value")) {
            configPart.r2.value = partData["r2"]["value"].get<float>() * fact_x;
        }

        if (partData.contains("r3") && partData["r3"].contains("value")) {
            configPart.r3.value = partData["r3"]["value"].get<float>() * fact_x;
        }

        // centers
        if (partData.contains("center") && partData["center"].contains("x") && partData["center"].contains("y")) {
            configPart.center.x = partData["center"]["x"].get<float>() * fact_x;
            configPart.center.y = partData["center"]["y"].get<float>() * fact_y;
        }

        // points
        if (partData.contains("p1") && partData["p1"].contains("x") && partData["p1"].contains("y")) {
            configPart.p1.x = partData["p1"]["x"].get<float>() * fact_x;
            configPart.p1.y = partData["p1"]["y"].get<float>() * fact_y;
        }

        if (partData.contains("p2") && partData["p2"].contains("x") && partData["p2"].contains("y")) {
            configPart.p2.x = partData["p2"]["x"].get<float>() * fact_x;
            configPart.p2.y = partData["p2"]["y"].get<float>() * fact_y;
        }

        if (partData.contains("p3") && partData["p3"].contains("x") && partData["p3"].contains("y")) {
            configPart.p3.x = partData["p3"]["x"].get<float>() * fact_x;
            configPart.p3.y = partData["p3"]["y"].get<float>() * fact_y;
        }

        if (partData.contains("p4") && partData["p4"].contains("x") && partData["p4"].contains("y")) {
            configPart.p4.x = partData["p4"]["x"].get<float>() * fact_x;
            configPart.p4.y = partData["p4"]["y"].get<float>() * fact_y;
        }

        if (partData.contains("p5") && partData["p5"].contains("x") && partData["p5"].contains("y")) {
            configPart.p5.x = partData["p5"]["x"].get<float>() * fact_x;
            configPart.p5.y = partData["p5"]["y"].get<float>() * fact_y;
        }

        if (partData.contains("p6") && partData["p6"].contains("x") && partData["p6"].contains("y")) {
            configPart.p6.x = partData["p6"]["x"].get<float>() * fact_x;
            configPart.p6.y = partData["p6"]["y"].get<float>() * fact_y;
        }

        // associating the configuration to the correct face part
        configParts[partName] = configPart;
    }

    // returning the configuration for an emotion
    return configParts;
}

void debugEmotionsConfig(const std::unordered_map<std::string, std::map<std::string, Face::FacialGeometry>>& map) {
    if (map.empty()) {
        std::cout << "The Face::emotionsConfig map is empty." << std::endl;
        return;
    }

    for (const auto& [emotionName, configParts] : map) {
        std::cout << "Emotion: " << emotionName << std::endl;
        for (const auto& [partName, configPart] : configParts) {
            std::cout << "  Part: " << partName << std::endl;
            std::cout << "    p1: (" << configPart.p1.x << ", " << configPart.p1.y << ")" << std::endl;
            std::cout << "    p2: (" << configPart.p2.x << ", " << configPart.p2.y << ")" << std::endl;
            std::cout << "    p3: (" << configPart.p3.x << ", " << configPart.p3.y << ")" << std::endl;
            std::cout << "    p4: (" << configPart.p4.x << ", " << configPart.p4.y << ")" << std::endl;
            std::cout << "    p5: (" << configPart.p5.x << ", " << configPart.p5.y << ")" << std::endl;
            std::cout << "    p6: (" << configPart.p6.x << ", " << configPart.p6.y << ")" << std::endl;
            std::cout << "    center: (" << configPart.center.x << ", " << configPart.center.y << ")" << std::endl;
            std::cout << "    r1: " << configPart.r1.value << std::endl;
            std::cout << "    r2: " << configPart.r2.value << std::endl;
            std::cout << "    r3: " << configPart.r3.value << std::endl;
        }
    }
}

Face::Face()
    : t(0.9), stopped(false), isTalking(false), isListening(false),
      pupilFlag(false), pup_x(0),
      pup_y(0), val_lim(10 * fact_x), val_lim_x(20 * fact_x), val_lim_y(20 * fact_y)
{

    res_x = Globals::res_x;
    res_y = Globals::res_y;
    fact_x = Globals::fact_x;
    fact_y = Globals::fact_y;
    OFFSET = Globals::OFFSET;

    initEmotionsConfig();

    if (emotionsConfig.contains("neutral")) {
        DEFAULT_CONFIG_NEUTRAL = emotionsConfig["neutral"];
    } else std::cerr << "Neutral emotion not found!" << std::endl;

    img = cv::Mat::zeros(res_y, res_x, CV_8UC3);
    config = DEFAULT_CONFIG_NEUTRAL;
    old_config = DEFAULT_CONFIG_NEUTRAL;
    config_target = DEFAULT_CONFIG_NEUTRAL;
}

Face::~Face() {
  cout << "Destroying Face" << endl;
}

void Face::initEmotionsConfig() const {
    // Define path to json directory
    std::filesystem::path jsonPath = std::filesystem::path(__FILE__).parent_path() / "../JSON";

    // Checks if directory exists
    if (!exists(jsonPath) || !is_directory(jsonPath)) {
        std::cerr << "JSON directory not found: " << jsonPath << std::endl;
        return;
    }

    // Consults all files in the directory
    for (const auto& entry : std::filesystem::directory_iterator(jsonPath)) {
        // Processes only json files
        if (entry.path().extension() == ".json") {
            std::ifstream file(entry.path());
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << entry.path() << std::endl;
                continue;
            }

            // parses file to json
            nlohmann::json jsonData;
            try {
                file >> jsonData;
            } catch (const std::exception& e) {
                std::cerr << "Error parsing JSON file: " << entry.path() << std::endl;
                std::cerr << "Exception: " << e.what() << std::endl;
                continue;
            }

            // get key for the map
            std::string emotionName = entry.path().stem().string();

            // Converts JSON to map<string, FacialGeometry>
            emotionsConfig[emotionName] = JSON2ConfigPart(jsonData);

            // checking if the emotion has been added to the main map
            debugEmotionsConfig(emotionsConfig);
        }
    }

    std::cout << "Variable Face::emotionsConfig loaded successfully!" << std::endl;
}

Face::Point Face::createCoordinate(const float x, const float y) const {
	return {x * fact_x, y * fact_y};
}

Face::Point Face::bezier(const Point& p1, const Point& p2, const float interpolationFactor) {
	Point result{};
	result.x = p1.x + (p2.x - p1.x) * interpolationFactor;
	result.y = p1.y + (p2.y - p1.y) * interpolationFactor;
	return result;
}

vector<Face::Point> Face::getPointsBezier(const vector<Point>& points) {
    vector<Point> bezierPoints;

    // Preallocate the vector to avoid reallocations during the loop
    bezierPoints.reserve(51);  // We will have 51 points from t=0 to t=1 (inclusive)

    // Use an integer loop counter for the number of iterations
    for (int iteration = 0; iteration <= 50; ++iteration) {
        float interpolationFactor = static_cast<float>(iteration) * 0.02f; // Calculate the interpolation factor for each iteration
        vector<Point> tempPoints = points;  // Create a copy of the original points

        // Perform the Bézier curve reduction in place
        for (size_t k = 0; k < tempPoints.size() - 1; ++k) {
            for (size_t i = 0; i < tempPoints.size() - 1 - k; ++i) {
                // Update points in place with Bezier calculations
                tempPoints[i] = bezier(tempPoints[i], tempPoints[i + 1], interpolationFactor);
            }
        }

        // Push the final point of the Bézier curve for this t value
        bezierPoints.emplace_back(tempPoints[0]);
    }

    return bezierPoints;
}

float interpolate(float start, float end, float t) {
    return start + (end - start) * t;
}

map<string, Face::FacialGeometry> Face::getBezierConfig(
    const map<string, FacialGeometry>& _old_config,
    const map<string, FacialGeometry>& _config_target,
    const float interpolationFactor) {

    map<string, FacialGeometry> interpolatedConfig;

    for (const auto& [part, old_part] : _old_config) {
        auto it = _config_target.find(part);
        if (it == _config_target.end()) {
            throw std::runtime_error("Part not found in configTarget: " + part);
        }

        const auto& [target_part_key, target_part] = *it;
        FacialGeometry new_part = old_part;

        // points interpolation
        new_part.p1 = bezier(old_part.p1, target_part.p1, interpolationFactor);
        new_part.p2 = bezier(old_part.p2, target_part.p2, interpolationFactor);
        new_part.p3 = bezier(old_part.p3, target_part.p3, interpolationFactor);
        new_part.p4 = bezier(old_part.p4, target_part.p4, interpolationFactor);

        if (old_part.p5.x != 0.0f || old_part.p5.y != 0.0f) {
            new_part.p5 = bezier(old_part.p5, target_part.p5, interpolationFactor);
        }
        if (old_part.p6.x != 0.0f || old_part.p6.y != 0.0f) {
            new_part.p6 = bezier(old_part.p6, target_part.p6, interpolationFactor);
        }

        new_part.center = bezier(old_part.center, target_part.center, interpolationFactor);

        // radius interpolation
        new_part.r1.value = interpolate(old_part.r1.value, target_part.r1.value, interpolationFactor);
        new_part.r2.value = interpolate(old_part.r2.value, target_part.r2.value, interpolationFactor);
        new_part.r3.value = interpolate(old_part.r3.value, target_part.r3.value, interpolationFactor);

        interpolatedConfig[part] = new_part;
    }

    return interpolatedConfig;
}

void Face::init()
{
    // Initialize face parameters and start the face animation thread
    std::thread(&Face::run, this).detach();
}

// Start the face thread (override from threading.Thread or similar)
void Face::run()
{
    // Get the current time at the start of the loop
    auto start = std::chrono::steady_clock::now();

    int sec = Globals::randInt(2, 6);

    while (!stopped)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        bool blinkFlag = false;

        // Get the current time and calculate the elapsed time
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = now - start;

        // If more than 'sec' seconds have passed, activate 'blinkFlag' and generate a new value for 'sec'
        if (elapsed_seconds.count() > sec)
        {
            blinkFlag = true;
            sec = Globals::randInt(2, 4);
            start = std::chrono::steady_clock::now();  // Reset the timer
        }

        moveFace(blinkFlag, isTalking, isListening);

        // Render the face and get the image
        cv::Mat path = render();

        // You can add more logic if needed, such as moving the pupil or recording points
        // if (isListening) {
        //     recordPoint();
        // }
        // if (pupilFlag) {
        //     movePupil();
        // }
    }
}

// Method to move the pupil
void Face::movePupil() {

}

// Method to move the face (e.g., blinking, mouth movement)
void Face::moveFace(bool blinkFlag, bool isTalking, bool isListening) {
    // Logic to move the face based on input flags
}

// Method to draw the face configuration
void Face::drawConfig(const map<string, Face::FacialGeometry>& configAux) const {
    sf::RenderWindow window(sf::VideoMode(res_x, res_y), "Face");
    window.clear(sf::Color::White);

    sf::RectangleShape rect(sf::Vector2f(res_x - 1, res_y - 1));
    rect.setFillColor(sf::Color::White);
    rect.setOutlineColor(sf::Color::Black);
    rect.setOutlineThickness(1);
    rect.setPosition(0, 0);

    renderEyebrow(config.at("rightEyebrow"), window);
    renderEyebrow(config.at("leftEyebrow"), window);

    renderEye(config.at("rightEye"), window);
    renderEye(config.at("leftEye"), window);

    renderMouth(config.at("mouth"), window);

    renderPupil(config.at("rightPupil"), window);
    renderPupil(config.at("leftPupil"), window);

    renderTongue(config.at("tongue"), window);

    renderCheek(config.at("rightCheek"), window);
    renderCheek(config.at("leftCheek"), window);

    renderEyelid(config.at("rightEyelid"), window);
    renderEyelid(config.at("leftEyelid"), window);

    window.draw(rect);
    window.display();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
}

// Method to render the face image
// Method to render the face image
cv::Mat Face::render() {
    // Check if animation is in progress (t <= 1.0) and if the target configuration is not empty
    if (t <= 1.0f && !config_target.empty()) {
        // Generate a new configuration using Bezier interpolation between the old and target configurations
        config = getBezierConfig(old_config, config_target, t);

        cout << "Animating step " << t << " - Interpolating configuration." << endl;

        // Draw the current configuration on the image (img)
        drawConfig(config);

        // Increment t by OFFSET, effectively progressing the animation step-by-step
        t += OFFSET;

        // Show the rendered image in a window called "Rendered Image"
        cv::imshow("Rendered Image", img);

        // Lock the mutex to safely update the shared image data across threads
        lock_guard<mutex> lock(shared_data.lock);
        shared_data.image = img; // Update the global shared image with the current frame
    } else {
        // If animation is complete, update the old and target configurations
        old_config = config_target;
        config_target = {}; // Reset target configuration

        cout << "Animation complete. Updating configuration." << endl;
    }

    // Return the rendered image (img)
    return img;
}

// Method to render the eyebrow
void Face::renderEyebrow(const FacialGeometry& eyebrow, sf::RenderWindow& window) {
    // Create a vector of points for the eyebrow
    vector<Face::Point> eyebrowPoints = {eyebrow.p1, eyebrow.p2, eyebrow.p3, eyebrow.p4};

    // Get the Bézier points for the eyebrow
    vector<sf::Vector2f> bezierPoints;
    vector<Face::Point> points = getPointsBezier(eyebrowPoints);

    // Convert the Face::Point points to sf::Vector2f
    for (const auto& point : points) {
        bezierPoints.emplace_back(point.x, point.y);
    }

    // Create a VertexArray to represent the eyebrow
    sf::VertexArray eyebrowShape(sf::LinesStrip, bezierPoints.size());

    for (size_t i = 0; i < bezierPoints.size(); ++i) {
        eyebrowShape[i].position = bezierPoints[i];
        eyebrowShape[i].color = sf::Color::Black;
    }

    // Draw the eyebrow on the window
    window.draw(eyebrowShape);
}

// Method to render the eyelid
void Face::renderEyelid(const FacialGeometry& eyelid, sf::RenderWindow& window) {
    // Define eyelid points from the FacialGeometry object using Face::Point structure.
    vector<Face::Point> eyelidPoints = {eyelid.p1, eyelid.p2, eyelid.p3, eyelid.p4};

    // Get interpolated points using Bézier curve for both the upper and lower parts of the eyelid.
    vector<sf::Vector2f> bezierUpper;
    vector<Face::Point> upperPoints = getPointsBezier(eyelidPoints);  // Upper part of eyelid

    // Convert the Face::Point to sf::Vector2f for rendering.
    for (const auto& point : upperPoints) {
        bezierUpper.emplace_back(point.x, point.y);
    }

    // Create a vector for the lower part of the eyelid.
    vector<Face::Point> lowerPoints = {eyelid.p3, eyelid.p4, eyelid.p1};  // Lower part of eyelid
    vector<Face::Point> lowerBezierPoints = getPointsBezier(lowerPoints);  // Get interpolated points for lower part

    // Convert the Face::Point to sf::Vector2f for rendering.
    vector<sf::Vector2f> bezierLower;
    for (const auto& point : lowerBezierPoints) {
        bezierLower.emplace_back(point.x, point.y);
    }

    // Combine the upper and lower parts of the eyelid.
    vector<sf::Vector2f> eyelidBezierPoints = bezierUpper;
    eyelidBezierPoints.insert(eyelidBezierPoints.end(), bezierLower.begin(), bezierLower.end());

    // Create a VertexArray to represent the eyelid as a triangle fan.
    sf::VertexArray eyelidShape(sf::TrianglesFan, eyelidBezierPoints.size());

    // Assign points and color to the vertex array.
    for (size_t i = 0; i < eyelidBezierPoints.size(); ++i) {
        eyelidShape[i].position = eyelidBezierPoints[i];
        eyelidShape[i].color = sf::Color(255, 255, 255);
    }

    // Draw the eyelid shape on the window.
    window.draw(eyelidShape);
}

// Method to render the pupil (e.g., based on certain points)
void Face::renderPupil(const FacialGeometry& pupil, sf::RenderWindow& window) {
    // Create a CircleShape to render the pupil, using the radius (r3) from the pupil config
    sf::CircleShape pupilShape(pupil.r3.value);

    pupilShape.setFillColor(sf::Color::Black);

    // Set the position of the pupil based on the center coordinates, adjusting for the radius
    pupilShape.setPosition(pupil.center.x - pupil.r3.value, pupil.center.y - pupil.r3.value);

    // Draw the pupil on the window
    window.draw(pupilShape);
}

// Method to render the eye
void Face::renderEye(const FacialGeometry& eye, sf::RenderWindow& window) {
    sf::CircleShape eyeShape(eye.r1.value);  // Create the base circle with the radius on the X axis
    eyeShape.setFillColor(sf::Color::White);
    eyeShape.setOutlineColor(sf::Color::Black);
    eyeShape.setOutlineThickness(2);

    // Scale the Y axis to convert the circle into an ellipse
    eyeShape.setScale(1.f, eye.r2.value / eye.r1.value);

    // Position the center of the ellipse
    eyeShape.setPosition(eye.center.x - eye.r1.value, eye.center.y - eye.r2.value);

    window.draw(eyeShape);
}

// Method to render the cheek
void Face::renderCheek(const FacialGeometry& cheek, sf::RenderWindow& window) {
    // Define the points for the cheek from the FacialGeometry object.
    vector<Face::Point> cheekPoints1 = {cheek.p1, cheek.p2, cheek.p3};
    vector<Face::Point> cheekPoints2 = {cheek.p3, cheek.p4, cheek.p1};

    // Get interpolated points for both Bézier curves.
    vector<sf::Vector2f> bezierPoints1, bezierPoints2;

    // Interpolate the first curve (P1 -> P2 -> P3).
    vector<Point> points1 = getPointsBezier(cheekPoints1);
    for (const auto& point : points1) {
        bezierPoints1.emplace_back(point.x, point.y);
    }

    // Interpolate the second curve (P3 -> P4 -> P1).
    vector<Face::Point> points2 = getPointsBezier(cheekPoints2);
    for (const auto& point : points2) {
        bezierPoints2.emplace_back(point.x, point.y);
    }

    // Combine both sets of Bézier points.
    bezierPoints1.insert(bezierPoints1.end(), bezierPoints2.begin(), bezierPoints2.end());

    // Create a vertex array to represent the cheek shape.
    sf::VertexArray cheekShape(sf::TrianglesFan, bezierPoints1.size());

    // Assign points to the vertex array and set the color (white for the cheek).
    for (size_t i = 0; i < bezierPoints1.size(); ++i) {
        cheekShape[i].position = bezierPoints1[i];
        cheekShape[i].color = sf::Color(255, 255, 255);
    }

    // Draw the cheek shape onto the window.
    window.draw(cheekShape);
}

// Method to render the mouth
void Face::renderMouth(const FacialGeometry& mouth, sf::RenderWindow& window) {
    // Define mouth points from FacialGeometry.
    vector<Face::Point> mouthPoints = {mouth.p1, mouth.p2, mouth.p3, mouth.p4, mouth.p5, mouth.p6};

    // Get interpolated points using Bézier curve.
    vector<sf::Vector2f> bezierPoints;
    vector<Face::Point> points = getPointsBezier(mouthPoints);

    // Convert custom points to sf::Vector2f for rendering.
    for (const auto& point : points) {
        bezierPoints.emplace_back(point.x, point.y);
    }

    // Create a VertexArray to represent the mouth as a triangle fan.
    sf::VertexArray mouthShape(sf::TrianglesFan, bezierPoints.size());

    // Assign points to VertexArray and set color.
    for (size_t i = 0; i < bezierPoints.size(); ++i) {
        mouthShape[i].position = bezierPoints[i];
        mouthShape[i].color = sf::Color::Black;
    }

    // Draw the mouth on the window.
    window.draw(mouthShape);
}

// Method to render the tongue
void Face::renderTongue(const FacialGeometry& tongue, sf::RenderWindow& window) {
    // Define the tongue points from the FacialGeometry.
    vector tonguePoints = {tongue.p1, tongue.p2, tongue.p3, tongue.p4};

    // Get interpolated points using Bézier curve.
    vector<sf::Vector2f> bezierPoints;
    vector<Point> points = getPointsBezier(tonguePoints);

    // Convert custom Face::Point to sf::Vector2f for rendering.
    for (const auto& point : points) {
        bezierPoints.emplace_back(point.x, point.y);
    }

    // Create a vertex array for the tongue shape.
    sf::VertexArray tongueShape(sf::TrianglesFan, bezierPoints.size());

    // Assign points and colors to the vertex array.
    for (size_t i = 0; i < bezierPoints.size(); ++i) {
        tongueShape[i].position = bezierPoints[i];
        tongueShape[i].color = sf::Color(131, 131, 255);
    }

    // Draw the tongue shape onto the window.
    window.draw(tongueShape);
}

// Method to record a point (e.g., for tracking purposes)
void Face::recordPoint() {
    // Logic to record a point (perhaps for tracking or saving data)
}

// Method to set the configuration for the face
void Face::setConfig(const std::map<std::string, FacialGeometry>& _config) {
    config_target = _config;
    old_config = config;
    t = 0.06666666666666667;
}

// Method to set whether the face is talking
void Face::setTalking(const bool _talking) {
    isTalking = _talking;
}

// Method to set whether the face is listening
void Face::setListening(const bool _listening) {
    isListening = _listening;
}

void Face::setPupilFlag(const bool _pupilFlag) {
    pupilFlag = _pupilFlag;
}

void Face::setPupX(const float _pup_x) {
    pup_x = _pup_x;
}

void Face::setPupY(const float _pup_y) {
    pup_y = _pup_y;
}

// Method to stop the face thread
void Face::stop() {
    stopped = true;
    // Any necessary cleanup or joining of the thread goes here
}
