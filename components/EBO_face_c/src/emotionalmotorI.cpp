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
#include "emotionalmotorI.h"

EmotionalMotorI::EmotionalMotorI(GenericWorker *_worker)
{
	worker = _worker;
}


EmotionalMotorI::~EmotionalMotorI()
{
}


void EmotionalMotorI::expressAnger(const Ice::Current&)
{
	worker->EmotionalMotor_expressAnger();
}

void EmotionalMotorI::expressDisgust(const Ice::Current&)
{
	worker->EmotionalMotor_expressDisgust();
}

void EmotionalMotorI::expressFear(const Ice::Current&)
{
	worker->EmotionalMotor_expressFear();
}

void EmotionalMotorI::expressJoy(const Ice::Current&)
{
	worker->EmotionalMotor_expressJoy();
}

void EmotionalMotorI::expressSadness(const Ice::Current&)
{
	worker->EmotionalMotor_expressSadness();
}

void EmotionalMotorI::expressSurprise(const Ice::Current&)
{
	worker->EmotionalMotor_expressSurprise();
}

void EmotionalMotorI::isanybodythere(bool isAny, const Ice::Current&)
{
	worker->EmotionalMotor_isanybodythere(isAny);
}

void EmotionalMotorI::listening(bool setListening, const Ice::Current&)
{
	worker->EmotionalMotor_listening(setListening);
}

void EmotionalMotorI::pupposition(float x, float y, const Ice::Current&)
{
	worker->EmotionalMotor_pupposition(x, y);
}

void EmotionalMotorI::talking(bool setTalk, const Ice::Current&)
{
	worker->EmotionalMotor_talking(setTalk);
}

