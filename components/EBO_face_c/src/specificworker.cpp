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
using namespace std;

// Function to initialize the SFML window (equivalent to pygame in Python)
void SpecificWorker::initWindow() {
	// Create the SFML window
	sf::RenderWindow window(sf::VideoMode(res_x, res_y), "EBO FACE");
	if (!window.isOpen()) {
		std::cerr << "Error creating SFML window!" << std::endl;
		exit(1);
	}

	sf::Event event;
	while (window.isOpen()) { // heck if the window is still open
		// Poll events
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();
		}

		// clean window
		window.clear(sf::Color::White);

		// Display the image, if available
		std::lock_guard<std::mutex> guard(shared_data.lock); // Protect shared data with a lock

		if (!shared_data.image.empty()) {
			// Convert OpenCV image (BGR) to RGB
			cv::Mat img_rgb;
			cv::cvtColor(shared_data.image, img_rgb, cv::COLOR_BGR2RGB);

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
	: GenericWorker(tprx), face(res_x, res_y, fact_x, fact_y)
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
	const string oldLocale=setlocale(LC_NUMERIC,nullptr);
	setlocale(LC_NUMERIC,"C");

	try
	{
		res_x = stoi(params["res_x"].value);
		res_y = stoi(params["res_y"].value);
		fact_x = stof(params["fact_x"].value);
		fact_y = stof(params["fact_y"].value);
		OFFSET = stof(params["OFFSET"].value);
	}
	catch(const std::exception &e) { qFatal("Error reading config params"); }

	setlocale(LC_NUMERIC,oldLocale.c_str());

	return true;
}

// TODO: emplear punteros si es posible
map<string, Face::ConfigPart> SpecificWorker::JSON2ConfigPart(const nlohmann::json& jsonData) const {
	map<string, Face::ConfigPart> configParts;

	for (auto& [partName, partData] : jsonData.items()) {
		Face::ConfigPart configPart;

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

void SpecificWorker::initEmotionsConfig() {
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
				continue;
			}

			// get key for the map
			std::string emotionName = entry.path().stem().string();

			// Converts JSON to map<string, ConfigPart>
			emotionsConfig[emotionName] = JSON2ConfigPart(jsonData);
		}
	}

	std::cout << "Variable emotionsConfig loaded successfully!" << std::endl;
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

		this->initEmotionsConfig();
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
	if (emotionsConfig.contains("anger")) {
		face.setConfig(emotionsConfig["anger"]);
	} else {
		std::cerr << "Anger emotion not found!" << std::endl;
	}
}

void SpecificWorker::EmotionalMotor_expressDisgust()
{
	if (emotionsConfig.contains("disgust")) {
		face.setConfig(emotionsConfig["disgust"]);
	} else {
		std::cerr << "Disgust emotion not found!" << std::endl;
	}
}

void SpecificWorker::EmotionalMotor_expressFear()
{
	if (emotionsConfig.contains("fear")) {
		face.setConfig(emotionsConfig["fear"]);
	} else {
		std::cerr << "Fear emotion not found!" << std::endl;
	}
}

void SpecificWorker::EmotionalMotor_expressJoy()
{
	if (emotionsConfig.contains("joy")) {
		face.setConfig(emotionsConfig["joy"]);
	} else {
		std::cerr << "Joy emotion not found!" << std::endl;
	}
}

void SpecificWorker::EmotionalMotor_expressSadness()
{
	if (emotionsConfig.contains("sadness")) {
		face.setConfig(emotionsConfig["sadness"]);
	} else {
		std::cerr << "Sadness emotion not found!" << std::endl;
	}
}

void SpecificWorker::EmotionalMotor_expressSurprise()
{
	if (emotionsConfig.contains("surprise")) {
		face.setConfig(emotionsConfig["surprise"]);
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





