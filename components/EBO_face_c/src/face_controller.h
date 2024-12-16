#ifndef FACE_CONTROLLER_H
#define FACE_CONTROLLER_H

#include "face_renderer.h"
#include <nlohmann/json.hpp>
#include <map>
#include <string>
#include <mutex>
#include <iostream>
#include <fstream>
#include <globals.h>

class FaceController {
public:
    explicit FaceController(FaceRenderer &renderer);
    /**
     * @brief Marks that an update is needed.
     *
     * This function sets the `needUpdate` flag to `true`, indicating that
     * the face configuration needs to be updated (e.g., after a change in emotion
     * or other parameters like pupil position, talking state, etc.).
     * It is typically called when changes are made to the face configuration
     * that require a visual update.
     */
    void requestUpdate();

    /**
     * @brief Disables the update flag.
     *
     * This function sets the `needUpdate` flag to `false`, indicating that no update
     * is required after the update process has been completed. This ensures that
     * the system only performs updates when necessary.
     * Typically, this function is called after the face rendering and interpolation
     * have been performed.
     */
    void disableUpdate();

    /**
     * @brief Checks if an update is needed.
     *
     * This function returns the current state of the `needUpdate` flag. It is used to
     * check whether the face configuration requires an update, such as when a new emotion
     * is set or when other parameters change (e.g., pupil position, talking state).
     *
     * @return `true` if an update is required, `false` otherwise.
     */
    [[nodiscard]] bool shouldUpdate() const;

    void setEmotion(const std::string &emotion);
    void setPupilPosition(float x, float y);
    void setPupilState(bool active);
    void setTalking(bool talking);
    void setListening(bool listening);

    /**
    * @brief Updates the animation state of the face, including interpolation, rendering, and additional logic for pupil and facial expressions.
    *
    * This method is responsible for updating the state of the face, including:
    * - **Interpolation**: Gradually transitions between the current and target configurations of the face (e.g., emotions or expressions). The interpolation is applied until the progress reaches 1.0f.
    * - **Face rendering**: Once interpolation is applied (if needed), the updated configuration is rendered to the screen.
    * - **Pupil rendering**: If the pupil is active, it renders the pupil at the specified position.
    * - **Talking and listening animations**: Placeholder for logic that animates the mouth and other features depending on the state of the face (e.g., if `isTalking` or `isListening` is set to true).
    *
    * The method ensures that the updates occur smoothly, with interpolation being applied incrementally. The progress of interpolation is controlled to avoid exceeding the value of 1.0f, ensuring that the face transitions gradually without abrupt changes.
    *
    * - **Interpolation Progress**: The progress of the interpolation is incremented gradually, using `Globals::progress_increment`, and capped at 1.0f. Once it reaches 1.0f, the interpolation stops.
    * - **Face Rendering**: After interpolation (if any), the updated face configuration is sent to the renderer to update the visual representation of the face.
    * - **Pupil Movement**: If the pupil is active (`pupilActive`), it is rendered at the position specified by `pupilPosition`.
    * - **Talking Animation**: If the face is in a "talking" state (`isTalking`), logic for mouth animation can be applied (this can be expanded later).
    * - **Listening Animation**: Similar to talking, if the face is in a "listening" state (`isListening`), animation logic for listening can be added here.
    *
    * @note This method must be called frequently (e.g., every frame) to ensure smooth transitions and updates of the face configuration.
    */
    void update();

    /**
     * @brief Loads configurations for all emotions from a directory containing JSON files.
     *
     * This method scans the specified directory for `.json` files, parses each file, and loads the
     * configuration for each emotion into the `emotionsConfig` map. The name of each emotion is
     * derived from the filename (without the `.json` extension).
     *
     * @param directoryPath Path to the directory containing emotion JSON files.
     *
     * @throws std::runtime_error If the directory does not exist or is not valid.
     * @throws std::ios_base::failure If there is an error opening a file.
     * @throws nlohmann::json::parse_error If a JSON file contains invalid data.
     */
    void loadAllEmotions(const std::string &directoryPath);

private:
    FaceRenderer &renderer;
    std::map<std::string, std::map<std::string, sf::Vector2f>> emotionsConfig;
    std::map<std::string, sf::Vector2f> currentConfig;
    std::map<std::string, sf::Vector2f> targetConfig;
    std::mutex configMutex;

    bool needUpdate = false;
    float interpolation_progress;
    bool isTalking;
    bool isListening;
    bool pupilActive;
    sf::Vector2f pupilPosition;

    /**
     * @brief Parses and loads the configuration for a single emotion from a JSON object.
     *
     * This method processes the attributes of a given emotion (e.g., centers, points, and radii)
     * and scales them using predefined scaling factors. The processed configuration is stored
     * in the `emotionsConfig` map under the specified emotion name.
     *
     * @param emotionName The name of the emotion being parsed (e.g., "anger", "joy").
     * @param jsonData The JSON object containing the configuration for the emotion.
     */
    void parseEmotionJSON(const std::string &emotionName, const nlohmann::json &jsonData);

    /**
     * @brief Smoothly interpolates between the current and target face configurations.
     *
     * This method adjusts the `currentConfig` values to transition smoothly towards
     * the `targetConfig` values by applying linear interpolation. Each feature's position
     * (x, y) in `currentConfig` is updated incrementally using a factor defined in
     * `Globals::interpolation_speed`. The interpolation ensures gradual changes in the face
     * configuration for animations or transitions.
     *
     * - If `Globals::interpolation_speed` is close to 1.0, the transition is almost instant.
     * - If `Globals::interpolation_speed` is close to 0.0, the transition is slower and smoother.
     *
     * @note The updates are applied directly to `currentConfig`, modifying its values in place.
     *
     * @see Globals::interpolation_speed
     */
    void applyInterpolation();
};

#endif // FACE_CONTROLLER_H