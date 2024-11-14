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

/**
	\brief
	@author authorname
*/



#ifndef SPECIFICWORKER_H
#define SPECIFICWORKER_H

#define HIBERNATION_ENABLED

#include <genericworker.h>
#include <globals.h>
#include <face.h>
#include <opencv2/opencv.hpp>
#include <SDL2/SDL.h>
#include <map>
#include <string>
#include <mutex>
#include <random>
#include <thread>

// Mutex to protect shared data
extern std::mutex image_mutex;

// structures
typedef struct Point {
	float x, y;
} Point;

typedef struct Radius {
	float Value;
} Radius;

// Structure representing each part of the face configuration
typedef struct ConfigPart {
	Point P1, P2, P3, P4, P5, P6, Center; // Points for various facial parts
	Radius Radius1, Radius2, Radius3;      // Radius for circles like eyes and pupils
} ConfigPart;

// Structure for shared image data
typedef struct SharedData {
	cv::Mat image;
	std::mutex lock;
} SharedData;

class SpecificWorker : public GenericWorker
{
Q_OBJECT
public:
	SpecificWorker(MapPrx& mprx, bool startup_check);
	~SpecificWorker();
	bool setParams(RoboCompCommonBehavior::ParameterList params);

	void EmotionalMotor_expressAnger();
	void EmotionalMotor_expressDisgust();
	void EmotionalMotor_expressFear();
	void EmotionalMotor_expressJoy();
	void EmotionalMotor_expressSadness();
	void EmotionalMotor_expressSurprise();
	void EmotionalMotor_isanybodythere(const bool &isAny);
	void EmotionalMotor_listening(const bool &setListening);
	void EmotionalMotor_pupposition(const float x, const float y);
	void EmotionalMotor_talking(const bool &setTalk);


public slots:
	void initialize();
	void compute();
	void emergency();
	void restore();
	int startup_check();
private:
	bool startup_check_flag;

	//
	int randInt(int min, int max);
	Point createCoordinate(float x, float y);

	// screen management
	SharedData shared_data;
	void initWindow();

	// Calculate a point on a Bézier curve between two points
	Point bezier(const Point& p1, const Point& p2, float t);

	// Get a list of points along a Bézier curve
	std::vector<Point> getPointsBezier(std::vector<Point>& points);

	// Interpolate between two configurations using Bézier curve
	std::map<std::string, ConfigPart> getBezierConfig(
    const std::map<std::string, ConfigPart>& old_config,
    const std::map<std::string, ConfigPart>& config_target,
    float t);

    // CONFIGURATION MAP
	std::map<std::string, ConfigPart> DEFAULTCONFIGNEUTRAL;
};

#endif
