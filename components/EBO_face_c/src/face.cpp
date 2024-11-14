//
// Created by lee on 11/14/24.
//

#include "face.h"
using namespace std;

Face::Face()
    : pup_x(0), pup_y(0), t(0.9), stopped(false), isTalking(false),
      isListening(false), pupilaFlag(false), val_lim(10 * fact_x),
      val_lim_x(20 * fact_x), val_lim_y(20 * fact_y)
{
    img = cv::Mat::zeros(res_y, res_x, CV_8UC3);
    config = map<string, map<string, map<string, int>>>();
    old_config = config;
    config_target = config;
}

Face::~Face() {
  cout << "Destroying Face" << endl;
}

void Face::init()
{
    // Initialize face parameters and start the face animation thread
    thread(&Face::run, this).detach();
}

// Start the face thread (override from threading.Thread or similar)
void Face::run()
{
    // Get the current time at the start of the loop
    auto start = std::chrono::steady_clock::now();

    int sec = randInt(2, 6);

    while (!stopped)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        bool blinkFlag = false;

        // Get the current time and calculate the elapsed time
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = now - start;

        // If more than 'sec' seconds have passed, activate 'blinkFlag' and generate a new value for 'sec'
        if (elapsed_seconds.count() > sec)
        {
            blinkFlag = true;
            sec = randInt(2, 4);
            start = std::chrono::steady_clock::now();  // Reset the timer
        }

        moveFace(blinkFlag, isTalking, isListening);

        // Render the face and get the image
        cv::Mat path = render();

        // You can add more logic if needed, such as moving the pupil or recording points
        // if (isListening) {
        //     recordPoint();
        // }
        // if (pupilFlag) {
        //     movePupil();
        // }
    }
}

// Method to move the pupil
void Face::movePupil() {
    // Logic to move the pupil based on some conditions or input
}

// Method to move the face (e.g., blinking, mouth movement)
void Face::moveFace(bool blinkFlag, bool isTalking, bool isListening) {
    // Logic to move the face based on input flags
}

// Method to draw the face configuration
void Face::drawConfig(const map<string, map<string, map<string, int>>>& configAux) {
    // Logic to draw the current configuration
}

// Method to render the face image
cv::Mat Face::render() {
    // Logic to render the face and return the image
    return img;
}

// Method to render the pupil (e.g., based on certain points)
void Face::renderPupil(const vector<cv::Point>& points) {
    // Logic to render the pupil
}

// Method to render the tongue
void Face::renderTongue(const vector<cv::Point>& points) {
    // Logic to render the tongue
}

// Method to render the eyelid
void Face::renderEyelid(const vector<cv::Point>& points) {
    // Logic to render the eyelid
}

// Method to render the cheek
void Face::renderCheek(const vector<cv::Point>& points) {
    // Logic to render the cheek
}

// Method to render the eyebrow
void Face::renderEyebrow(const vector<cv::Point>& points) {
    // Logic to render the eyebrow
}

// Method to render the eye
void Face::renderEye(const vector<cv::Point>& points) {
    // Logic to render the eye
}

// Method to render the mouth
void Face::renderMouth(const vector<cv::Point>& points) {
    // Logic to render the mouth
}

// Method to record a point (e.g., for tracking purposes)
void Face::recordPoint() {
    // Logic to record a point (perhaps for tracking or saving data)
}

// Method to set the configuration for the face
void Face::setConfig(const map<string, map<string, map<string, int>>>& config) {
    this->config = config;
}

// Method to set whether the face is talking
void Face::setTalking(bool talking) {
    isTalking = talking;
}

// Method to set whether the face is listening
void Face::setListening(bool listening) {
    isListening = listening;
}

// Method to stop the face thread
void Face::stop() {
    stopped = true;
    // Any necessary cleanup or joining of the thread goes here
}
