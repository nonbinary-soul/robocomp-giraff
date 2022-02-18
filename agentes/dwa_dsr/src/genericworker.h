/*
 *    Copyright (C) 2022 by YOUR NAME HERE
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
#ifndef GENERICWORKER_H
#define GENERICWORKER_H

#include "config.h"
#include <stdint.h>
#include <qlog/qlog.h>

#if Qt5_FOUND
	#include <QtWidgets>
#else
	#include <QtGui>
#endif
#include <ui_mainUI.h>
#include <CommonBehavior.h>

#include <BillCoppelia.h>
#include <DifferentialRobot.h>
#include <FullPoseEstimation.h>
#include <GenericBase.h>
#include <Laser.h>
#include <MoveTowards.h>
#include <OmniRobot.h>


#define CHECK_PERIOD 5000
#define BASIC_PERIOD 100


using TuplePrx = std::tuple<RoboCompBillCoppelia::BillCoppeliaPrxPtr,RoboCompDifferentialRobot::DifferentialRobotPrxPtr,RoboCompDifferentialRobot::DifferentialRobotPrxPtr,RoboCompFullPoseEstimation::FullPoseEstimationPrxPtr,RoboCompFullPoseEstimation::FullPoseEstimationPrxPtr,RoboCompLaser::LaserPrxPtr,RoboCompLaser::LaserPrxPtr,RoboCompOmniRobot::OmniRobotPrxPtr>;


class GenericWorker : public QMainWindow, public Ui_guiDlg
{
Q_OBJECT
public:
	GenericWorker(TuplePrx tprx);
	virtual ~GenericWorker();
	virtual void killYourSelf();
	virtual void setPeriod(int p);

	virtual bool setParams(RoboCompCommonBehavior::ParameterList params) = 0;
	QMutex *mutex;


	RoboCompBillCoppelia::BillCoppeliaPrxPtr billcoppelia_proxy;
	RoboCompDifferentialRobot::DifferentialRobotPrxPtr differentialrobot_proxy;
	RoboCompDifferentialRobot::DifferentialRobotPrxPtr differentialrobot1_proxy;
	RoboCompFullPoseEstimation::FullPoseEstimationPrxPtr fullposeestimation_proxy;
	RoboCompFullPoseEstimation::FullPoseEstimationPrxPtr fullposeestimation1_proxy;
	RoboCompLaser::LaserPrxPtr laser_proxy;
	RoboCompLaser::LaserPrxPtr laser1_proxy;
	RoboCompOmniRobot::OmniRobotPrxPtr omnirobot_proxy;

	virtual RoboCompMoveTowards::Command MoveTowards_move(float x, float y, float alpha) = 0;

protected:

	QTimer timer;
	int Period;

private:


public slots:
	virtual void compute() = 0;
	virtual void initialize(int period) = 0;
	
signals:
	void kill();
};

#endif
