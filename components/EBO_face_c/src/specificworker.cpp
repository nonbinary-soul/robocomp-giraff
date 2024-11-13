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

// Generating random integers
int randInt(int min, int max) {
	// Create a random device and a generator
	std::random_device rd;  // Non-deterministic random generator
	std::mt19937 gen(rd());  // Mersenne Twister random number generator

	// Create a uniform distribution in the given range [min, max]
	std::uniform_int_distribution<> dis(min, max);

	// Generate and return a random number
	return dis(gen);
}

Point createCoordinate(float x, float y) {
	return {x * fact_x, y * fact_y};
}

map<std::string, ConfigPart> DEFAULTCONFIGNEUTRAL = {
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

// Function to initialize the SDL window (equivalent to pygame in Python)
void SpecificWorker::initWindow() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
		exit(1);
	}

	SDL_Window* window = SDL_CreateWindow("PIL Image Display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, res_x, res_y, SDL_WINDOW_FULLSCREEN);
	if (window == nullptr) {
		std::cerr << "Error creating SDL window: " << SDL_GetError() << std::endl;
		exit(1);
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		std::cerr << "Error creating SDL renderer: " << SDL_GetError() << std::endl;
		exit(1);
	}

	// Main loop
	bool running = true;
	SDL_Event event;
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
		}

		// Display the image, if available
		std::lock_guard<std::mutex> guard(shared_data.lock); // Protect shared data with a lock
		if (!shared_data.image.empty()) {
			// Convert OpenCV image (BGR) to an image SDL can display
			cv::Mat img_rgb;
			cv::cvtColor(shared_data.image, img_rgb, cv::COLOR_BGR2RGB);
			SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(img_rgb.data, img_rgb.cols, img_rgb.rows, 24, img_rgb.step, 0x0000FF, 0x00FF00, 0xFF0000, 0x000000);
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			SDL_DestroyTexture(texture);
			SDL_FreeSurface(surface);
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

Point SpecificWorker::bezier(const Point& p1, const Point& p2, float t) {
	Point result;
	result.x = p1.x + (p2.x - p1.x) * t;
	result.y = p1.y + (p2.y - p1.y) * t;
	return result;
}

std::vector<Point> SpecificWorker::getPointsBezier(std::vector<Point>& points) {
    std::vector<Point> bezierPoints;

    // Preallocate the vector to avoid reallocations during the loop
    bezierPoints.reserve(51);  // We will have 51 points from t=0 to t=1 (inclusive)

    for (float t = 0; t <= 1.0; t += 0.02) {
        std::vector<Point> tempPoints = points;  // Create a copy of the original points

        // Perform the Bezier curve reduction in place
        for (size_t k = 0; k < tempPoints.size() - 1; ++k) {
            for (size_t i = 0; i < tempPoints.size() - 1 - k; ++i) {
                // Update points in place with Bezier calculations
                tempPoints[i] = bezier(tempPoints[i], tempPoints[i + 1], t);
            }
        }

        // Push the final point of the Bezier curve for this t value
        bezierPoints.push_back(tempPoints[0]);
    }

    return bezierPoints;
}

std::map<std::string, ConfigPart> SpecificWorker::getBezierConfig(
    const std::map<std::string, ConfigPart>& old_config,
    const std::map<std::string, ConfigPart>& config_target,
    float t) {

    std::map<std::string, ConfigPart> config; // Mapa para almacenar la nueva configuración

    for (const auto& [parte, old_part] : old_config) {
        ConfigPart new_part = old_part; // Copiar la parte de configuración original

        // Procesar puntos (P1, P2, ..., P6 y Center)
        new_part.P1 = bezier(old_part.P1, config_target.at(parte).P1, t);
        new_part.P2 = bezier(old_part.P2, config_target.at(parte).P2, t);
        new_part.P3 = bezier(old_part.P3, config_target.at(parte).P3, t);
        new_part.P4 = bezier(old_part.P4, config_target.at(parte).P4, t);
        new_part.P5 = bezier(old_part.P5, config_target.at(parte).P5, t);
        new_part.P6 = bezier(old_part.P6, config_target.at(parte).P6, t);
        new_part.Center = bezier(old_part.Center, config_target.at(parte).Center, t);

        // Procesar radios (Radius1, Radius2, Radius)
        new_part.Radius1.Value = bezier({old_part.Radius1.Value, 0}, {config_target.at(parte).Radius1.Value, 0}, t).x;
        new_part.Radius2.Value = bezier({old_part.Radius2.Value, 0}, {config_target.at(parte).Radius2.Value, 0}, t).x;
        new_part.Radius3.Value = bezier({old_part.Radius3.Value, 0}, {config_target.at(parte).Radius3.Value, 0}, t).x;

        // Asignar la parte procesada a la nueva configuración
        config[parte] = new_part;
    }

    return config;
}

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(MapPrx& mprx, bool startup_check) : GenericWorker(mprx)
{
	this->startup_check_flag = startup_check;
	// Uncomment if there's too many debug messages
	// but it removes the possibility to see the messages
	// shown in the console with qDebug()
//	QLoggingCategory::setFilterRules("*.debug=false\n");
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
//	THE FOLLOWING IS JUST AN EXAMPLE
//	To use innerModelPath parameter you should uncomment specificmonitor.cpp readConfig method content
//	try
//	{
//		RoboCompCommonBehavior::Parameter par = params.at("InnerModelPath");
//		std::string innermodel_path = par.value;
//		innerModel = std::make_shared(innermodel_path);
//	}
//	catch(const std::exception &e) { qFatal("Error reading config params"); }
	

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
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
//implementCODE

}

void SpecificWorker::EmotionalMotor_expressDisgust()
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
//implementCODE

}

void SpecificWorker::EmotionalMotor_expressFear()
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
//implementCODE

}

void SpecificWorker::EmotionalMotor_expressJoy()
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
//implementCODE

}

void SpecificWorker::EmotionalMotor_expressSadness()
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
//implementCODE

}

void SpecificWorker::EmotionalMotor_expressSurprise()
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
//implementCODE

}

void SpecificWorker::EmotionalMotor_isanybodythere(const bool &isAny)
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
//implementCODE

}

void SpecificWorker::EmotionalMotor_listening(const bool &setListening)
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
//implementCODE

}

void SpecificWorker::EmotionalMotor_pupposition(const float x, const float y)
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
//implementCODE

}

void SpecificWorker::EmotionalMotor_talking(const bool &setTalk)
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
//implementCODE

}


