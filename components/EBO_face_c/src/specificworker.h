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

#include <genericworker.h>
#include <face_renderer.h>
#include <face_controller.h>
#include <globals.h>
#include <SFML/Graphics.hpp>
#include <mutex>
#include <thread>
#include <map>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

class SpecificWorker : public GenericWorker {
	Q_OBJECT
public:
	SpecificWorker(TuplePrx tprx, bool startup_check);
	~SpecificWorker() override;
	bool setParams(RoboCompCommonBehavior::ParameterList params) override;

	// Métodos de la interfaz EmotionalMotor
	void EmotionalMotor_expressAnger() override;
	void EmotionalMotor_expressDisgust() override;
	void EmotionalMotor_expressFear() override;
	void EmotionalMotor_expressJoy() override;
	void EmotionalMotor_expressSadness() override;
	void EmotionalMotor_expressSurprise() override;
	void EmotionalMotor_isanybodythere(bool isAny) override;
	void EmotionalMotor_listening(bool setListening) override;
	void EmotionalMotor_talking(bool setTalk) override;
	void EmotionalMotor_pupposition(float x, float y) override;

public slots:
	void initialize() override;
	void compute() override;
	void emergency() override;
	void restore() override;
	int startup_check();

private:
	bool startup_check_flag;

	sf::RenderWindow window;
	FaceRenderer faceRenderer;
	FaceController faceController;

	std::thread animationThread;
	bool running;
	void startAnimationLoop();
};

#endif // SPECIFICWORKER_H