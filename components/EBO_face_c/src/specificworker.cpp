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

Point SpecificWorker::createCoordinate(float x, float y) {
	return {x * fact_x, y * fact_y};
}

void SpecificWorker::initializeMapDefaultConfigNeutral() {
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
}

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

vector<Point> SpecificWorker::getPointsBezier(vector<Point>& points) {
    vector<Point> bezierPoints;

    // Preallocate the vector to avoid reallocations during the loop
    bezierPoints.reserve(51);  // We will have 51 points from t=0 to t=1 (inclusive)

    for (float t = 0; t <= 1.0; t += 0.02) {
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

map<string, ConfigPart> SpecificWorker::getBezierConfig(
	const map<string, ConfigPart>& old_config,
	const map<string, ConfigPart>& config_target,
	float t) {

	map<string, ConfigPart> config; // Map to store the new interpolated configuration

	for (const auto& [part, old_part] : old_config) {
		ConfigPart new_part = old_part; // Copy each configuration part

		// Interpolate points (P1, P2, ..., P6, and Center) using Bezier
		new_part.P1 = bezier(old_part.P1, config_target.at(part).P1, t);
		new_part.P2 = bezier(old_part.P2, config_target.at(part).P2, t);
		new_part.P3 = bezier(old_part.P3, config_target.at(part).P3, t);
		new_part.P4 = bezier(old_part.P4, config_target.at(part).P4, t);
		new_part.P5 = bezier(old_part.P5, config_target.at(part).P5, t);
		new_part.P6 = bezier(old_part.P6, config_target.at(part).P6, t);
		new_part.Center = bezier(old_part.Center, config_target.at(part).Center, t);

		// Interpolate radius values (Radius1, Radius2, Radius3) using Bezier
		new_part.Radius1.Value = bezier({old_part.Radius1.Value, 0},
										{config_target.at(part).Radius1.Value, 0}, t).x;
		new_part.Radius2.Value = bezier({old_part.Radius2.Value, 0},
										{config_target.at(part).Radius2.Value, 0}, t).x;
		new_part.Radius3.Value = bezier({old_part.Radius3.Value, 0},
										{config_target.at(part).Radius3.Value, 0}, t).x;

		// Store the processed part in the final configuration map
		config[part] = new_part;
	}

	return config;
}

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(TuplePrx tprx, bool startup_check) : GenericWorker(tprx)
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

		this->initializeMapDefaultConfigNeutral();
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

void SpecificWorker::EmotionalMotor_isanybodythere(bool isAny)
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
//implementCODE

}

void SpecificWorker::EmotionalMotor_listening(bool setListening)
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
//implementCODE

}

void SpecificWorker::EmotionalMotor_pupposition(float x, float y)
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
//implementCODE

}

void SpecificWorker::EmotionalMotor_talking(bool setTalk)
{
#ifdef HIBERNATION_ENABLED
	hibernation = true;
#endif
//implementCODE

}



