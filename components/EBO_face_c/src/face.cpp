//
// Created by lee on 11/14/24.
//

#include "face.h"

using namespace std;

Face::Face()
    : pup_x(0), pup_y(0), t(0.9), stopped(false), isTalking(false),
      isListening(false), pupilFlag(false), val_lim(10 * fact_x),
      val_lim_x(20 * fact_x), val_lim_y(20 * fact_y)
{
  	DEFAULTCONFIGNEUTRAL = {
		{"rightEyebrow", {
			.P1 = createCoordinate(278, 99),
			.P2 = createCoordinate(314, 73),
			.P3 = createCoordinate(355, 99),
			.P4 = createCoordinate(313, 94)
		}},

		{"leftEyebrow", {
			.P1 = createCoordinate(122, 99),
			.P2 = createCoordinate(160, 73),
			.P3 = createCoordinate(201, 99),
			.P4 = createCoordinate(160, 94)
		}},

		{"rightEye", {
			.Center = createCoordinate(316, 151),
			.Radius1 = {34 * fact_x},
			.Radius2 = {34 * fact_x}
		}},

		{"leftEye", {
			.Center = createCoordinate(161, 151),
			.Radius1 = {34 * fact_x},
			.Radius2 = {34 * fact_x}
		}},

		{"mouth", {
			.P1 = createCoordinate(170, 234),
			.P2 = createCoordinate(239, 231),
			.P3 = createCoordinate(309, 234),
			.P4 = createCoordinate(309, 242),
			.P5 = createCoordinate(239, 241),
			.P6 = createCoordinate(170, 242)
		}},

		{"rightPupil", {
			.Center = createCoordinate(316, 151),
			.Radius3 = {5 * fact_x}
		}},

		{"leftPupil", {
			.Center = createCoordinate(161, 151),
			.Radius3 = {5 * fact_x}
		}},

		{"tongue", {
			.P1 = createCoordinate(199, 238),
			.P2 = createCoordinate(239, 238),
			.P3 = createCoordinate(309, 238),
			.P4 = createCoordinate(273, 238)
		}},

		{"rightCheek", {
			.P1 = createCoordinate(278, 187),
			.P2 = createCoordinate(314, 188),
			.P3 = createCoordinate(355, 187),
			.P4 = createCoordinate(313, 187)
		}},

		{"leftCheek", {
			.P1 = createCoordinate(122, 187),
			.P2 = createCoordinate(160, 188),
			.P3 = createCoordinate(201, 187),
			.P4 = createCoordinate(160, 187)
		}},

		{"rightEyelid", {
			.P1 = createCoordinate(266, 151),
			.P2 = createCoordinate(314, 80),
			.P3 = createCoordinate(369, 151),
			.P4 = createCoordinate(313, 80)
		}},

		{"leftEyelid", {
			.P1 = createCoordinate(112, 151),
			.P2 = createCoordinate(160, 80),
			.P3 = createCoordinate(214, 151),
			.P4 = createCoordinate(160, 80)
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

map<string, Face::ConfigPart> Face::getBezierConfig(
	const map<string, ConfigPart>& oldConfig,
	const map<string, ConfigPart>& configTarget,
	float interpolationFactor) {

	map<string, ConfigPart> interpolatedConfig; // Map to store the new interpolated configuration

	for (const auto& [part, old_part] : oldConfig) {
		ConfigPart new_part = old_part; // Copy each configuration part

		// Interpolate points (P1, P2, ..., P6, and Center) using Bezier
		new_part.P1 = bezier(old_part.P1, configTarget.at(part).P1, t);
		new_part.P2 = bezier(old_part.P2, configTarget.at(part).P2, t);
		new_part.P3 = bezier(old_part.P3, configTarget.at(part).P3, t);
		new_part.P4 = bezier(old_part.P4, configTarget.at(part).P4, t);
		new_part.P5 = bezier(old_part.P5, configTarget.at(part).P5, t);
		new_part.P6 = bezier(old_part.P6, configTarget.at(part).P6, t);
		new_part.Center = bezier(old_part.Center, configTarget.at(part).Center, t);

		// Interpolate radius values (Radius1, Radius2, Radius3) using Bezier
		new_part.Radius1.Value = bezier({old_part.Radius1.Value, 0},
										{configTarget.at(part).Radius1.Value, 0}, t).x;
		new_part.Radius2.Value = bezier({old_part.Radius2.Value, 0},
										{configTarget.at(part).Radius2.Value, 0}, t).x;
		new_part.Radius3.Value = bezier({old_part.Radius3.Value, 0},
										{configTarget.at(part).Radius3.Value, 0}, t).x;

		// Store the processed part in the final configuration map
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
void Face::setConfig(const map<string, map<string, map<string, int>>>& newConfig) {
    config_target = newConfig;
    old_config = config;
    t = 0.0;
}

// Method to set whether the face is talking
void Face::setTalking(bool talking) {
    isTalking = talking;
}

// Method to set whether the face is listening
void Face::setListening(bool listening) {
    isListening = listening;
}

// Method to stop the face thread
void Face::stop() {
    stopped = true;
    // Any necessary cleanup or joining of the thread goes here
}
