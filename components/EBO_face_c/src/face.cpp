//
// Created by lee on 11/14/24.
//

#include "face.h"

using namespace std;

Face::Face(int res_x, int res_y, float fact_x, float fact_y)
    : t(0.9), stopped(false), isTalking(false), isListening(false),
      pupilFlag(false), pup_x(0),
      pup_y(0), val_lim(10 * fact_x), val_lim_x(20 * fact_x), val_lim_y(20 * fact_y), res_x(res_x),
      res_y(res_y), fact_x(fact_x), fact_y(fact_y)
{
	DEFAULTCONFIGNEUTRAL = {
    	{"rightEyebrow", ConfigPart{
        	.p1 = createCoordinate(278, 99),
        	.p2 = createCoordinate(314, 73),
        	.p3 = createCoordinate(355, 99),
        	.p4 = createCoordinate(313, 94)
    	}},
    	{"leftEyebrow", ConfigPart{
        	.p1 = createCoordinate(122, 99),
        	.p2 = createCoordinate(160, 73),
        	.p3 = createCoordinate(201, 99),
        	.p4 = createCoordinate(160, 94)
    	}},
    	{"rightEye", ConfigPart{
	        .center = createCoordinate(316, 151),
	        .r1 = {.value = 34 * fact_x},
	        .r2 = {.value = 34 * fact_x}
	    }},
	    {"leftEye", ConfigPart{
	        .center = createCoordinate(161, 151),
	        .r1 = {.value = 34 * fact_x},
    	    .r2 = {.value = 34 * fact_x}
    	}},
    	{"mouth", ConfigPart{
	        .p1 = createCoordinate(170, 234),
    	    .p2 = createCoordinate(239, 231),
	        .p3 = createCoordinate(309, 234),
	        .p4 = createCoordinate(309, 242),
		    .p5 = createCoordinate(239, 241),
    	    .p6 = createCoordinate(170, 242)
    	}},
		{"rightPupil", ConfigPart{
        	.center = createCoordinate(316, 151),
        	.r3 = {.value = 5 * fact_x}
    	}},
    	{"leftPupil", ConfigPart{
	        .center = createCoordinate(161, 151),
	        .r3 = {.value = 5 * fact_x}
	    }},
	    {"tongue", ConfigPart{
	        .p1 = createCoordinate(199, 238),
	        .p2 = createCoordinate(239, 238),
	        .p3 = createCoordinate(309, 238),
	        .p4 = createCoordinate(273, 238)
	    }},
	    {"rightCheek", ConfigPart{
	        .p1 = createCoordinate(278, 187),
	        .p2 = createCoordinate(314, 188),
	        .p3 = createCoordinate(355, 187),
	        .p4 = createCoordinate(313, 187)
	    }},
    	{"leftCheek", ConfigPart{
	        .p1 = createCoordinate(122, 187),
        	.p2 = createCoordinate(160, 188),
        	.p3 = createCoordinate(201, 187),
	        .p4 = createCoordinate(160, 187)
	    }},
		{"rightEyelid", ConfigPart{
    	    .p1 = createCoordinate(266, 151),
        	.p2 = createCoordinate(314, 80),
        	.p3 = createCoordinate(369, 151),
        	.p4 = createCoordinate(313, 80)
    	}},
    	{"leftEyelid", ConfigPart{
       		.p1 = createCoordinate(112, 151),
        	.p2 = createCoordinate(160, 80),
        	.p3 = createCoordinate(214, 151),
        	.p4 = createCoordinate(160, 80)
    	}}
	};

    img = cv::Mat::zeros(res_y, res_x, CV_8UC3);
    config = DEFAULTCONFIGNEUTRAL;
    old_config = DEFAULTCONFIGNEUTRAL;
    config_target = DEFAULTCONFIGNEUTRAL;
}

Face::~Face() {
  cout << "Destroying Face" << endl;
}

Face::Point Face::createCoordinate(float x, float y) {
	return {x * fact_x, y * fact_y};
}

Face::Point Face::bezier(const Point& p1, const Point& p2, float interpolationFactor) {
	Point result;
	result.x = p1.x + (p2.x - p1.x) * interpolationFactor;
	result.y = p1.y + (p2.y - p1.y) * interpolationFactor;
	return result;
}

vector<Face::Point> Face::getPointsBezier(vector<Point>& points) {
    vector<Point> bezierPoints;

    // Preallocate the vector to avoid reallocations during the loop
    bezierPoints.reserve(51);  // We will have 51 points from t=0 to t=1 (inclusive)

    for (float interpolationFactor = 0; interpolationFactor <= 1.0; interpolationFactor += 0.02) {
        vector<Point> tempPoints = points;  // Create a copy of the original points

        // Perform the Bézier curve reduction in place
        for (size_t k = 0; k < tempPoints.size() - 1; ++k) {
            for (size_t i = 0; i < tempPoints.size() - 1 - k; ++i) {
                // Update points in place with Bezier calculations
                tempPoints[i] = bezier(tempPoints[i], tempPoints[i + 1], t);
            }
        }

        // Push the final point of the Bézier curve for this t value
        bezierPoints.push_back(tempPoints[0]);
    }

    return bezierPoints;
}

float interpolate(float start, float end, float t) {
    return start + (end - start) * t;
}

map<string, Face::ConfigPart> Face::getBezierConfig(
    const map<string, ConfigPart>& oldConfig,
    const map<string, ConfigPart>& configTarget,
    float interpolationFactor) {

    map<string, ConfigPart> interpolatedConfig;

    for (const auto& [part, old_part] : oldConfig) {
        auto it = configTarget.find(part);
        if (it == configTarget.end()) {
            throw std::runtime_error("Part not found in configTarget: " + part);
        }
        const ConfigPart& target_part = it->second;

        ConfigPart new_part = old_part;

        // points interpolation
        new_part.p1 = bezier(old_part.p1, target_part.p1, interpolationFactor);
        new_part.p2 = bezier(old_part.p2, target_part.p2, interpolationFactor);
        new_part.p3 = bezier(old_part.p3, target_part.p3, interpolationFactor);
        new_part.p4 = bezier(old_part.p4, target_part.p4, interpolationFactor);
        if (old_part.p5.x || old_part.p5.y) {
            new_part.p5 = bezier(old_part.p5, target_part.p5, interpolationFactor);
        }
        if (old_part.p6.x || old_part.p6.y) {
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
    thread(&Face::run, this).detach();
}

// Start the face thread (override from threading.Thread or similar)
void Face::run()
{
    // Get the current time at the start of the loop
    auto start = std::chrono::steady_clock::now();

    int sec = randInt(2, 6);

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
            sec = randInt(2, 4);
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
    // Create a copy of the current configuration
    auto configAux = config;

    // Initial pupil values (base coordinates)
    float leftPupilX = 161 * fact_x;
    float leftPupilY = 151 * fact_y;
    float rightPupilX = 316 * fact_x;
    float rightPupilY = 151 * fact_y;

    // Update the center position of the left and right pupils
    configAux["leftPupil"]["Center"]["x"] = leftPupilX + val_lim * pup_x;
    configAux["leftPupil"]["Center"]["y"] = leftPupilY + val_lim * pup_y;
    configAux["rightPupil"]["Center"]["x"] = rightPupilX + val_lim * pup_x;
    configAux["rightPupil"]["Center"]["y"] = rightPupilY + val_lim * pup_y;

    // Draw the updated configuration of the face with the moved pupils
    drawConfig(configAux);

    // Rotate the face image by 270 degrees
    cv::Mat rotatedImage;
    cv::rotate(img, rotatedImage, cv::ROTATE_90_CLOCKWISE);

    // Crop a specific region of the image
    cv::Rect area(280, 0, 1000 - 280, res_y - 1);
    cv::Mat croppedImage = rotatedImage(area);

    // If a background exists, combine the cropped image with it
    croppedImage.copyTo(background(cv::Rect(300, 0, croppedImage.cols, croppedImage.rows)));

    // Save the final image as "EBO_face.png"
    cv::imwrite("EBO_face.png", background);

    // Send the image to a framebuffer, if necessary (system-specific code)
    std::ofstream fb("/dev/fb0", std::ios::binary);
    if (fb.is_open()) {
        fb.write(reinterpret_cast<char*>(background.data), background.total() * background.elemSize());
        fb.close();
    }
}

// Method to move the face (e.g., blinking, mouth movement)
void Face::moveFace(bool blinkFlag, bool isTalking, bool isListening) {
    // Logic to move the face based on input flags
}

// Method to draw the face configuration
void Face::drawConfig(const map<string, map<string, map<string, int>>>& configAux) {
    // Logic to draw the current configuration
}

// Method to render the face image
cv::Mat Face::render() {
    // Logic to render the face and return the image
    return img;
}

// Method to render the pupil (e.g., based on certain points)
void Face::renderPupil(const vector<cv::Point>& points) {
    // Logic to render the pupil
}

// Method to render the tongue
void Face::renderTongue(const vector<cv::Point>& points) {
    // Logic to render the tongue
}

// Method to render the eyelid
void Face::renderEyelid(const vector<cv::Point>& points) {
    // Logic to render the eyelid
}

// Method to render the cheek
void Face::renderCheek(const vector<cv::Point>& points) {
    // Logic to render the cheek
}

// Method to render the eyebrow
void Face::renderEyebrow(const vector<cv::Point>& points) {
    // Logic to render the eyebrow
}

// Method to render the eye
void Face::renderEye(const vector<cv::Point>& points) {
    // Logic to render the eye
}

// Method to render the mouth
void Face::renderMouth(const vector<cv::Point>& points) {
    // Logic to render the mouth
}

// Method to record a point (e.g., for tracking purposes)
void Face::recordPoint() {
    // Logic to record a point (perhaps for tracking or saving data)
}

// Method to set the configuration for the face
void Face::setConfig(const map<string, ConfigPart> _config) {
    config_target = _config;
    old_config = config;
    t = 0.06666666666666667;
}

// Method to set whether the face is talking
void Face::setTalking(bool _talking) {
    isTalking = _talking;
}

// Method to set whether the face is listening
void Face::setListening(bool _listening) {
    isListening = _listening;
}

void Face::setPupilFlag(bool _pupilFlag) {
    pupilFlag = _pupilFlag;
}

void Face::setPupX(bool _pup_x) {
    pupilFlag = _pup_x;
}

void Face::setPupY(bool _pup_y) {
    pup_x = _pup_y;
}

// Method to stop the face thread
void Face::stop() {
    stopped = true;
    // Any necessary cleanup or joining of the thread goes here
}
