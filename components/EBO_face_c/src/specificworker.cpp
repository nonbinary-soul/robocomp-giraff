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

// Function to initialize the SFML window (equivalent to pygame in Python)
void SpecificWorker::initWindow() {
	// Create the SFML window
	sf::RenderWindow window(sf::VideoMode(Globals::res_x, Globals::res_y), "EBO FACE");
	if (!window.isOpen()) {
		std::cerr << "Error creating SFML window!" << std::endl;
		exit(1);
	}

	sf::Event event = sf::Event();
	while (window.isOpen()) { // heck if the window is still open
		// Poll events
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
		}

		// clean window
		window.clear(sf::Color::White);

		// Display the image, if available
		std::lock_guard guard(Face::shared_data.lock); // Protect shared data with a lock

		if (!Face::shared_data.image.empty()) {
			// Convert OpenCV image (BGR) to RGB
			cv::Mat img_rgb;
			cvtColor(Face::shared_data.image, img_rgb, cv::COLOR_BGR2RGB);

			// Create a SFML image
			sf::Image image;
			image.create(img_rgb.cols, img_rgb.rows, img_rgb.data);

			// Create a texture from the image
			sf::Texture texture;
			if (!texture.loadFromImage(image)) {
				std::cerr << "Error loading texture from image!" << std::endl;
				continue;
			}

			// Create a sprite to display the texture
			sf::Sprite sprite(texture);

			// draw the sprite
			window.draw(sprite);
		}

		// show the window
		window.display();
	}
}

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(TuplePrx tprx, bool startup_check)
	: GenericWorker(tprx)
{
	this->startup_check_flag = startup_check;

	// Uncomment if there's too many debug messages
	// but it removes the possibility to see the messages
	// shown in the console with qDebug()
	//    QLoggingCategory::setFilterRules("*.debug=false\n");
}

/**
* \brief Default destructor
*/
SpecificWorker::~SpecificWorker()
{
	std::cout << "Destroying SpecificWorker" << std::endl;
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

void SpecificWorker::initialize()
{
	std::cout << "Initialize worker" << std::endl;
	if(this->startup_check_flag)
	{
		this->startup_check();
	}
	else
	{
		#ifdef HIBERNATION_ENABLED
			hibernationChecker.start(500);
		#endif

		this->setPeriod(STATES::Compute, 100);
		//this->setPeriod(STATES::Emergency, 500);

		this->initWindow();
	}

}

void SpecificWorker::compute()
{
    std::cout << "Compute worker" << std::endl;
	//computeCODE
	//QMutexLocker locker(mutex);
	//try
	//{
	//  camera_proxy->getYImage(0,img, cState, bState);
    //    if (img.empty())
    //        emit goToEmergency()
	//  memcpy(image_gray.data, &img[0], m_width*m_height*sizeof(uchar));
	//  searchTags(image_gray);
	//}
	//catch(const Ice::Exception &e)
	//{
	//  std::cout << "Error reading from Camera" << e << std::endl;
	//}
	
	
}

void SpecificWorker::emergency()
{
    std::cout << "Emergency worker" << std::endl;
	//computeCODE
	//
	//if (SUCCESSFUL)
    //  emmit goToRestore()
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


void SpecificWorker::EmotionalMotor_expressAnger()
{
	if (Face::emotionsConfig.contains("anger")) {
		face.setConfig(Face::emotionsConfig["anger"]);
	} else {
		std::cerr << "Anger emotion not found!" << std::endl;
	}
}

void SpecificWorker::EmotionalMotor_expressDisgust()
{
	if (Face::emotionsConfig.contains("disgust")) {
		face.setConfig(Face::emotionsConfig["disgust"]);
	} else {
		std::cerr << "Disgust emotion not found!" << std::endl;
	}
}

void SpecificWorker::EmotionalMotor_expressFear()
{
	if (Face::emotionsConfig.contains("fear")) {
		face.setConfig(Face::emotionsConfig["fear"]);
	} else {
		std::cerr << "Fear emotion not found!" << std::endl;
	}
}

void SpecificWorker::EmotionalMotor_expressJoy()
{
	if (Face::emotionsConfig.contains("joy")) {
		face.setConfig(Face::emotionsConfig["joy"]);
	} else {
		std::cerr << "Joy emotion not found!" << std::endl;
	}
}

void SpecificWorker::EmotionalMotor_expressSadness()
{
	if (Face::emotionsConfig.contains("sadness")) {
		face.setConfig(Face::emotionsConfig["sadness"]);
	} else {
		std::cerr << "Sadness emotion not found!" << std::endl;
	}
}

void SpecificWorker::EmotionalMotor_expressSurprise()
{
	if (Face::emotionsConfig.contains("surprise")) {
		face.setConfig(Face::emotionsConfig["surprise"]);
	} else {
		std::cerr << "Surprise emotion not found!" << std::endl;
	}
}

void SpecificWorker::EmotionalMotor_isanybodythere(bool isAny)
{
	cout << (isAny ? (face.setPupilFlag(true), "active") : (face.setPupilFlag(false), "inactive")) << endl;
}

void SpecificWorker::EmotionalMotor_listening(bool setListening)
{
	face.setListening(setListening);
}

void SpecificWorker::EmotionalMotor_talking(bool setTalk)
{
	face.setTalking(setTalk);
}

void SpecificWorker::EmotionalMotor_pupposition(float x, float y)
{
	face.setPupX(x);
	face.setPupY(y);
}





