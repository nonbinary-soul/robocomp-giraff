/*
 *    Copyright (C) 2024 by YOUR NAME HERE
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "specificworker.h"

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(TuplePrx tprx, bool startup_check)
	: GenericWorker(tprx),
	  faceRenderer(Globals::res_x, Globals::res_y),
	  faceController(faceRenderer),
	  running(true) {

	this->startup_check_flag = startup_check;
	window.create(sf::VideoMode(Globals::res_x, Globals::res_y), "EBO FACE");
}

/**
* \brief Default destructor
*/

SpecificWorker::~SpecificWorker() {
	running = false;
	if (animationThread.joinable()) {
		animationThread.join();
	}

	std::cout << "SpecificWorker destroyed." << std::endl;

}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params)
{
	// manage the numeric locale setting, which affects how numbers are formatted
	// in this case, it is used "." instead of ","
	const std::string oldLocale = setlocale(LC_NUMERIC, nullptr);
	setlocale(LC_NUMERIC, "C");

	try
	{
		// res_x = std::stoi(params["res_x"].value);
		// res_y = std::stoi(params["res_y"].value);
		// fact_x = std::stof(params["fact_x"].value);
		// fact_y = std::stof(params["fact_y"].value);
		// OFFSET = std::stof(params["OFFSET"].value);
	}
	catch (const std::exception &e) { qFatal("Error reading config params"); }

	setlocale(LC_NUMERIC, oldLocale.c_str());

	return true;
}

void SpecificWorker::initialize() {
	std::cout << "Initialize worker" << std::endl;

	try {
		std::filesystem::path jsonPath = "JSON"; // Path to the JSON folder

		if (!exists(jsonPath) || !is_directory(jsonPath)) {
			throw std::runtime_error("JSON folder not found: " + jsonPath.string());
		}

		// Iterate through all files in the JSON folder
		for (const auto &entry : std::filesystem::directory_iterator(jsonPath)) {
			if (entry.path().extension() == ".json") { // Only process .json files
				std::ifstream inputFile(entry.path());
				if (!inputFile.is_open()) {
					std::cerr << "Error opening file: " << entry.path() << std::endl;
					continue;
				}

				// Parse the JSON file
				nlohmann::json jsonData;
				inputFile >> jsonData;

				// Get the emotion name (e.g., "anger") from the filename
				std::string emotionName = entry.path().stem().string(); // Get the filename without extension

				// Load the emotion into the FaceController
				faceController.loadEmotionConfig(emotionName, jsonData);

				std::cout << "Loaded emotion: " << emotionName << std::endl;
			}
		}
	} catch (const std::exception &e) {
		std::cerr << "Error loading emotion configurations: " << e.what() << std::endl;
		return;
	}

	if (this->startup_check_flag) {
		this->startup_check();
	} else {
		animationThread = std::thread(&SpecificWorker::startAnimationLoop, this);
	}
}

void SpecificWorker::emergency()
{
    std::cout << "Emergency worker" << std::endl;
	//computeCODE
	//
	//if (SUCCESSFUL)
    //  emmit goToRestore()
}

void SpecificWorker::compute() {
	static bool toggle = true; // Toggle between emotions
	static sf::Clock emotionTimer; // Timer to switch emotions every few seconds

	sf::Event event{};
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed ||
			(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
			window.close();
			exit(0);
			}
	}

	// Switch emotions every 3 seconds
	if (emotionTimer.getElapsedTime().asSeconds() > 3.0f) {
		if (toggle) {
			EmotionalMotor_expressAnger(); // Switch to "anger"
		} else {
			EmotionalMotor_expressDisgust(); // Switch to "disgust"
		}
		toggle = !toggle;
		emotionTimer.restart(); // Reset the timer
	}

	// Render the current face configuration
	window.clear();
	faceRenderer.render(window);
	window.display();
}

//Execute one when exiting to emergencyState
void SpecificWorker::restore()
{
    std::cout << "Restore worker" << std::endl;
	//computeCODE
	//Restore emergency component

}

int SpecificWorker::startup_check()
{
	std::cout << "Startup check" << std::endl;
	QTimer::singleShot(200, qApp, SLOT(quit()));
	return 0;
}

void SpecificWorker::startAnimationLoop() {
	while (running) {
		faceController.update();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

void SpecificWorker::EmotionalMotor_expressAnger() {
	faceController.setEmotion("anger");
}

void SpecificWorker::EmotionalMotor_expressDisgust() {
	faceController.setEmotion("disgust");
}

void SpecificWorker::EmotionalMotor_expressFear() {
	faceController.setEmotion("fear");
}

void SpecificWorker::EmotionalMotor_expressJoy() {
	faceController.setEmotion("joy");
}

void SpecificWorker::EmotionalMotor_expressSadness() {
	faceController.setEmotion("sadness");
}

void SpecificWorker::EmotionalMotor_expressSurprise() {
	faceController.setEmotion("surprise");
}

void SpecificWorker::EmotionalMotor_isanybodythere(bool isAny) {
	faceController.setPupilState(isAny);
}

void SpecificWorker::EmotionalMotor_listening(bool setListening) {
	faceController.setListening(setListening);
}

void SpecificWorker::EmotionalMotor_talking(bool setTalk) {
	faceController.setTalking(setTalk);
}

void SpecificWorker::EmotionalMotor_pupposition(float x, float y) {
	faceController.setPupilPosition(x, y);
}
