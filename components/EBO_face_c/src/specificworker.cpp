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
      window(sf::VideoMode(Globals::res_x, Globals::res_y), "EBO FACE", sf::Style::None),
	  faceRenderer(window),
	  faceController(faceRenderer),
	  running(true) {

	this->startup_check_flag = startup_check;
}

/**
* \brief Default destructor
*/

SpecificWorker::~SpecificWorker() {
	running = false; // check to false to finish the thread
	if (animationThread.joinable()) {
		animationThread.join(); // waiting the thread to finish
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
		// loading all emotions from a directory
		faceController.loadAllEmotions("emotions_json");
	} catch (const std::exception &e) {
		std::cerr << "Error loading emotion configurations: " << e.what() << std::endl;
		return;
	}

	if (this->startup_check_flag) {
		this->startup_check();
	} else {
		this->setPeriod(STATES::Compute, 100);
		//this->setPeriod(STATES::Emergency, 500);
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
	std::cout << "Compute worker" << std::endl;

	try {
		if (!window.isOpen()) {
			window.create(sf::VideoMode(Globals::res_x, Globals::res_y), "Reopened Window");
		}

		static sf::Clock emotionTimer;
		static bool toggle = true;

		while (window.isOpen()) {
			sf::Event event{};
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) {
					window.close();
				}
			}

			// Switch emotions
			if (emotionTimer.getElapsedTime().asSeconds() > 3.0f) {
				if (toggle) {
					std::cout << "expressing joy..." << std::endl;
					EmotionalMotor_expressJoy();
				} else {
					std::cout << "expressing disgust..." << std::endl;
					EmotionalMotor_expressDisgust();
				}
				toggle = !toggle;
				emotionTimer.restart();
			}

			// Render the face
			faceRenderer.renderFace();
			window.display();
		}
	} catch (const std::exception &e) {
		std::cerr << "Exception caught in compute: " << e.what() << std::endl;
	}
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
		// checking if we need to update the animation
		if (faceController.shouldUpdate()) {
			faceController.update();
			faceController.disableUpdate();  // Reset the flag after the update is done
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));  // Check every 50ms or adjust as needed
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
