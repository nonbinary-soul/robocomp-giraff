/*
 *    Copyright (C) 2021 by YOUR NAME HERE
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
#include <cppitertools/enumerate.hpp>

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(TuplePrx tprx, bool startup_check) : GenericWorker(tprx)
{
	this->startup_check_flag = startup_check;
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
	return true;
}

void SpecificWorker::initialize(int period)
{
	std::cout << "Initialize worker" << std::endl;

    // HOG
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
    try{ jointmotorsimple_proxy->setPosition("tablet_joint", RoboCompJointMotorSimple::MotorGoalPosition{0.0001, 1 });}  // radians. 0 vertical
    catch(const Ice::Exception &e){ std::cout << e.what() << std::endl;}

    // Load YOLO network
    std::string modelConfiguration = "yolov3/yolov3.cfg";
    std::string modelWeights = "yolov3/yolov3.weights";
    std::string classesFile = "yolov3/coco.names";
    std::ifstream ifs(classesFile.c_str());
    std::string line;
    while (getline(ifs, line)) classes.push_back(line);
    net = cv::dnn::readNetFromDarknet(modelConfiguration, modelWeights);
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);

	this->Period = period;
	if(this->startup_check_flag)
		this->startup_check();
	else
		timer.start(Period);
}

void SpecificWorker::compute()
{
    // get image, body and face
    const auto &[body_o, face_o] = read_image();
    //move_eyes();
    move_tablet(body_o, face_o);
    move_base(body_o, face_o);

    // belief function to create and maintain a represented person

//  Control program to decide what best to do next based on the represented person
//    switch(state)
//    {
//        case State::WAITING:
//            // if person detected move to BODY_DETECTED
//            break;
//        case State::BODY_DETECTED:
//            // while approaching and until face is visible. Then FACE_DETECTED
//            break;
//        case State::FACE_DETECTED:
//            // wait until close enough and stable
//            break;
//        case State::READY_TO_INTERACT:
//            // Wait for a signal or order and move to FOLLOW
//            break;
//        case State::START_FOLLOWING:
//            // check person is moving away
//            break;
//        case State::FOLLOWING:
//            // distance control
//            break;
//        case State::STOP:
//            // person has stopped
//            break;
//    };
}

SpecificWorker::DetectRes SpecificWorker::read_image()
{
    try
    {
        //RoboCompCameraRGBDSimple::TImage top_img = camerargbdsimple_proxy->getImage("camera_tablet");
        RoboCompCameraRGBDSimple::TRGBD rgbd = camerargbdsimple_proxy->getAll("camera_tablet");
        const auto &top_img = rgbd.image;
        const auto &top_depth = rgbd.depth;
        const int width = 300;
        const int height = 300;
        if (top_img.width != 0 and top_img.height != 0)
        {
            cv::Mat img(top_img.height, top_img.width, CV_8UC3, rgbd.image.image.data());
            cv::Mat depth(top_depth.height, top_depth.width, CV_32FC1, rgbd.depth.depth.data());
            cv::Mat n_img;
            cv::resize(img, n_img, cv::Size(width, height));
            DetectRes ret{{},{}};

            // face
            auto rectangles = face_detector.detect_face_rectangles(n_img);
            if (not rectangles.empty())
            {
                auto r = rectangles.front();
                cv::rectangle(n_img, r, cv::Scalar(0, 105, 205), 3);
                QRect rect = QRect(r.x, r.y, r.width, r.height);
                float k = depth.at<float>(rect.center().x(), rect.center().y()) * 1000;  //mmm
                std::get<0>(ret) = {(width/2)-rect.center().x(), (height/2)-rect.center().y(), k};
            }
            else //body
            {
//                auto boxes = body_detector.detector(n_img);
//                qInfo() << __FUNCTION__ << boxes.size();
                // YOLO
                auto people = yolo_detector(n_img);
                if (not people.empty())
                {
                    auto r = people.front();
                    cv::rectangle(n_img, r, cv::Scalar(0, 0, 255), 3);
                    QRect rect = QRect(r.x, r.y, r.width, r.height);
                    float k = depth.at<float>(rect.center().x(), rect.center().y()) * 1000;  //mmm
                    std::get<0>(ret) = {(inpWidth / 2) - rect.center().x(), (inpHeight / 2) - rect.center().y(), k};
                }
            }
            // ehow image
            cv::cvtColor(n_img, n_img, cv::COLOR_BGR2RGB);
            cv::imshow("Camera tablet", n_img);
            cv::waitKey(1);
            return ret;
        }
    }
    catch(const Ice::Exception &e){ std::cout << e.what() << std::endl;}
    qWarning() << "Empty image or comms problem";
    return {};
}
void SpecificWorker::move_tablet(std::optional<std::tuple<int,int,int>> body_o, std::optional<std::tuple<int,int,int>> face_o)
{
    if(body_o.has_value())
    {
        auto [_, body_y_error, __] = body_o.value();
        const float delta = 0.1;
        float tilt = (delta / 100) * body_y_error;  // map from -100,100 to -0.1,0.1 rads
        auto pos = jointmotorsimple_proxy->getMotorState("tablet_joint").pos;
        //qInfo() << __FUNCTION__ << "BODY: pos" << pos << "error" << body_y_error << "tilt" << tilt << pos - tilt;
        try { jointmotorsimple_proxy->setPosition("tablet_joint", RoboCompJointMotorSimple::MotorGoalPosition{pos - tilt, 1}); }  // radians. 0 vertical
        catch (const Ice::Exception &e) { std::cout << e.what() << std::endl; }
    }
    else if( face_o.has_value())
    {
        auto [_, face_y_error, __] = face_o.value();
        const float delta = 0.1;
        const float tilt = (delta / 100) * face_y_error;  // map from -100,100 to -0.1,0.1 rads
        const float pos = jointmotorsimple_proxy->getMotorState("tablet_joint").pos;
        //qInfo() << __FUNCTION__ << "FACE: pos" << pos << "error" << face_y_error << "tilt" << tilt << pos - tilt;
        try { jointmotorsimple_proxy->setPosition("tablet_joint", RoboCompJointMotorSimple::MotorGoalPosition{pos - tilt, 1}); }  // radians. 0 vertical
        catch (const Ice::Exception &e) { std::cout << e.what() << std::endl; }
    }
}
void SpecificWorker::move_base(std::optional<std::tuple<int,int,int>> body_o, std::optional<std::tuple<int,int,int>> face_o)
{
    // rotate base
    const float gain = 0.5;
    float advance = 0;
    float rot = 0.0;
    if(body_o.has_value())
    {
        auto &[body_x_error, _, body_dist] = body_o.value();
        rot = -(2.f / 100) * body_x_error;
        if(body_dist > 0 and body_dist < 800)
            advance = -(400.0 / 800.0) * body_dist;
    }
    else if(face_o.has_value())
    {
        auto &[face_x_error, _, face_dist] = face_o.value();
        rot = -(2.f / 100) * face_x_error;
        if(face_dist > 0 and face_dist < 800)
            advance = -(400.0 / 800.0) * face_dist;
    }
    try { differentialrobot_proxy->setSpeedBase(advance, gain * rot); }
    catch (const Ice::Exception &e) { std::cout << e.what() << std::endl; }
}
std::vector<cv::Rect> SpecificWorker::yolo_detector(cv::Mat &frame)
{
    auto blob = cv::dnn::blobFromImage(frame, 1 / 255.0, cv::Size(inpWidth, inpHeight), cv::Scalar(), true,false);
    net.setInput(blob);
    vector<cv::Mat> outs;
    net.forward(outs, get_outputs_names(net));

    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    for (size_t i = 0; i < outs.size(); ++i)
    {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            cv::Point classIdPoint;
            double confidence;
            // Get the value and location of the maximum score
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > confThreshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;
                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
    }
    // Perform non maximum suppression to eliminate redundant overlapping boxes with
    // lower confidences
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
    std::vector<cv::Rect> new_boxes;
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        cv::Rect box = boxes[idx];
        new_boxes.push_back(box);
        //drawPred(classIds[idx], confidences[idx], box.x, box.y, box.x + box.width, box.y + box.height, frame);
    }
    return new_boxes;
}
std::vector<cv::String> SpecificWorker::get_outputs_names(const cv::dnn::Net &net)
{
    static std::vector<cv::String> names;
    if (names.empty())
    {
        //Get the indices of the output layers, i.e. the layers with unconnected outputs
        vector<int> outLayers = net.getUnconnectedOutLayers();

        //get the names of all the layers in the network
        vector<cv::String> layersNames = net.getLayerNames();

        // Get the names of the output layers in names
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
            names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}

////////////////////////////////////////////////////////////////////////////////////
int SpecificWorker::startup_check()
{
	std::cout << "Startup check" << std::endl;
	QTimer::singleShot(200, qApp, SLOT(quit()));
	return 0;
}

/**************************************/
// From the RoboCompCameraRGBDSimple you can call this methods:
// this->camerargbdsimple_proxy->getAll(...)
// this->camerargbdsimple_proxy->getDepth(...)
// this->camerargbdsimple_proxy->getImage(...)

/**************************************/
// From the RoboCompCameraRGBDSimple you can use this types:
// RoboCompCameraRGBDSimple::TImage
// RoboCompCameraRGBDSimple::TDepth
// RoboCompCameraRGBDSimple::TRGBD

/**************************************/
// From the RoboCompDifferentialRobot you can call this methods:
// this->differentialrobot_proxy->correctOdometer(...)
// this->differentialrobot_proxy->getBasePose(...)
// this->differentialrobot_proxy->getBaseState(...)
// this->differentialrobot_proxy->resetOdometer(...)
// this->differentialrobot_proxy->setOdometer(...)
// this->differentialrobot_proxy->setOdometerPose(...)
// this->differentialrobot_proxy->setSpeedBase(...)
// this->differentialrobot_proxy->stopBase(...)

/**************************************/
// From the RoboCompDifferentialRobot you can use this types:
// RoboCompDifferentialRobot::TMechParams

/**************************************/
// From the RoboCompEmotionalMotor you can call this methods:
// this->emotionalmotor_proxy->expressAnger(...)
// this->emotionalmotor_proxy->expressDisgust(...)
// this->emotionalmotor_proxy->expressFear(...)
// this->emotionalmotor_proxy->expressJoy(...)
// this->emotionalmotor_proxy->expressSadness(...)
// this->emotionalmotor_proxy->expressSurprise(...)




/////// HOG

// body
// vector<cv::Rect> found;
// vector<double> weights;
// cv::Mat n_img_bw;
// cv::cvtColor(n_img, n_img_bw, cv::COLOR_BGR2GRAY);
// hog.detectMultiScale(n_img_bw, found, weights);