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

/**
	\brief
	@author authorname
*/



#ifndef SPECIFICWORKER_H
#define SPECIFICWORKER_H
#define M_PI 3.14159265358979323846
#include <genericworker.h>
#include "Hungarian.h"
#include "dsr/api/dsr_api.h"
#include "dsr/gui/dsr_gui.h"
#include <doublebuffer/DoubleBuffer.h>
#include <opencv2/opencv.hpp>
#include <fps/fps.h>
#include <chrono>
#include <custom_widget.h>
#include <qcustomplot/qcustomplot.h>

class SpecificWorker : public GenericWorker
{
Q_OBJECT
public:
	SpecificWorker(TuplePrx tprx, bool startup_check);
	~SpecificWorker();
	bool setParams(RoboCompCommonBehavior::ParameterList params);
    struct PersonData
    {
        int vector_pos;
        int id;
        std::tuple<vector<cv::Point3f>, vector<cv::Point2i>> joints;
        RoboCompHumanCameraBody::TImage image;
        cv::Point3f personCoords_robot;
        cv::Point3f personCoords_world;
        cv::Point2i pixels;
        vector<float> orientation_filter;
        float orientation;
    };

    struct leaderData
    {
        int pix_x;
        int pix_y;
        cv::Point3f position;
        DSR::Node node;
        cv::Mat ROI;
        vector<cv::Mat> ROI_memory;
    };

public slots:
	void compute();
	int startup_check();
	void initialize(int period);

private:
    std::unique_ptr<DSR::RT_API> rt;

    // DSR graph
    std::shared_ptr<DSR::DSRGraph> G;
    std::shared_ptr<DSR::InnerEigenAPI> inner_eigen;

    //DSR params
    std::string agent_name;
    int agent_id;

    bool tree_view;
    bool graph_view;
    bool qscene_2d_view;
    bool osg_3d_view;

    // DSR graph viewer
    std::unique_ptr<DSR::DSRViewer> graph_viewer;
    QHBoxLayout mainLayout;
    void modify_node_slot(std::uint64_t, const std::string &type){};
    void modify_attrs_slot(std::uint64_t id, const std::vector<std::string>& att_names){};
    void modify_edge_slot(std::uint64_t from, std::uint64_t to,  const std::string &type){};

    void del_edge_slot(std::uint64_t from, std::uint64_t to, const std::string &edge_tag){};
    void del_node_slot(std::uint64_t from){};
    bool startup_check_flag;

    float servo_position;
    int april_pix_x;
    double correlation_th = 0.5;
    bool existTag = false;

    int last_people_number = 0;
    bool danger = false;
    bool occlussion = false;
    int occlussion_counter = 0;

    // Test variable
    cv::Mat last_leader_image;
    int memory_size = 10;

    cv::Point3f zero_pos = {0.0, 0.0, 0.0};
    cv::Point2i zero_pix = {0, 0};
    std::vector<cv::Point3f> leader_joints;
    cv::Point3f leader_position;

    vector<std::string> eyeList = {"1", "2"};
    vector<std::string> earList = {"3", "4"};
    vector<std::string> hipList = {"11", "12"};
    vector<std::string> chestList = {"5", "6"};

    vector<RoboCompHumanCameraBody::TImage> leader_ROI_memory;
    std::chrono::high_resolution_clock::time_point t_start;
    vector<int> jointPreference = {1, 6 ,7, 16, 17, 2, 3, 8, 9, 12 ,13, 4, 5, 10, 11, 14, 15, 0};

    vector<std::string> avoidedJoints = {"0","1","2", "3", "4","7", "8", "9", "10", "15", "16", "14", "13"};

    float alpha = 0.5;
    float beta = 1 - alpha;

    // QCustomPlot
    QCustomPlot custom_plot;
    QCPGraph *err_img, *err_dist;

    // Functions

    double correlation(cv::Mat &image_1, cv::Mat &image_2);
    RoboCompHumanCameraBody::PeopleData test_person();
    std::int32_t increase_lambda_cont(std::int32_t lambda_cont);
    std::int32_t decrease_lambda_cont(std::int32_t lambda_cont);
    cv::Point3f dictionary_values_to_3d_point(RoboCompHumanCameraBody::KeyPoint item);
    cv::Point3f cross_product(cv::Point3f p1, cv::Point3f p2);
    float get_degrees_between_vectors(cv::Point2f vector_1, cv::Point2f vector_2, std::string format);
    float calculate_orientation(RoboCompHumanCameraBody::Person person);
    float distance_3d(cv::Point3f p1, cv::Point3f p2);
    std::optional<SpecificWorker::leaderData> getting_leader_data();
    void remove_person(DSR::Node person_node, bool direct_remove); // direct_remove = false in python
    void update_person(DSR::Node node, SpecificWorker::PersonData persondata);
    std::optional<cv::Point3i> get_body_color(list<list<RoboCompHumanCameraBody::KeyPoint>> list);
//    SpecificWorker::PersonData people_comparison(SpecificWorker::leaderData leader_data, cv::Point2i max_corr_point, vector<SpecificWorker::PersonData> people_list);
    int people_comparison(SpecificWorker::leaderData node_data, cv::Point2i max_corr_point, SpecificWorker::PersonData person);
    double people_comparison_corr(SpecificWorker::leaderData node_data , cv::Point2i max_corr_point, SpecificWorker::PersonData person);
    std::optional<std::tuple<vector<cv::Point3f>, vector<cv::Point2i>>> get_joint_list(RoboCompHumanCameraBody::TJoints joints);
    void insert_mind(std::uint64_t parent_id, std::int32_t person_id);
    bool danger_detection(float correlation, SpecificWorker::leaderData leader_data, vector<SpecificWorker::PersonData> people_list);
    void insert_person(SpecificWorker::PersonData persondata, bool direct_insert);
//    std::optional<cv::Mat> update_graph(vector<SpecificWorker::PersonData> people_list, cv::Mat image);
    void update_graph(vector<SpecificWorker::PersonData> people_list);
//    std::optional<JointCoords> get_person_coords(RoboCompHumanCameraBody::Person p);
    std::optional<cv::Point2i> get_person_pixels(RoboCompHumanCameraBody::Person p);
    float dot_product3D(cv::Point3f vector_a, cv::Point3f vector_b);
    float dot_product(cv::Point2f vector_a, cv::Point2f vector_b);

    //local widget
    Custom_widget custom_widget;
    FPSCounter fps;

    void draw_timeseries(float error_dist, float error_img);

    std::optional<std::tuple<cv::Point3f, cv::Point3f, cv::Point2i>> position_filter(std::tuple<vector<cv::Point3f>, vector<cv::Point2i>> person_joints);
};

#endif
