#ifndef FACE_RENDERER_H
#define FACE_RENDERER_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <mutex>


class FaceRenderer {
public:
    explicit FaceRenderer(sf::RenderWindow &window);
    void renderFace();
    void setBackgroundColor(const sf::Color &color);

    void setFaceConfig(const std::map<std::string, sf::Vector2f> &config);

    // rendering specific facial parts
    void renderEyebrow(const std::vector<sf::Vector2f> &points);
    void renderEyelid(const std::vector<sf::Vector2f> &points);
    void renderEye(const sf::Vector2f &center, float radius_x, float radius_y);
    void renderPupil(const sf::Vector2f &center, float radius);
    void renderMouth(const std::vector<sf::Vector2f> &points);
    void renderTongue(const sf::Vector2f &center, float radius_x, float radius_y);
    void renderCheek(const std::vector<sf::Vector2f> &points);

private:
    sf::RenderWindow &window;
    sf::Color background_color = sf::Color::White;

    std::map<std::string, sf::Vector2f> face_config;
    std::mutex face_config_mutex;

    /**
     * @brief Calculates a single point on a linear Bézier segment between two points.
     *
     * This function interpolates linearly between two points (`p1` and `p2`) based on
     * the given interpolation factor. It is used as a helper function for constructing
     * Bézier curves.
     *
     * @param p1 The starting point of the segment.
     * @param p2 The ending point of the segment.
     * @param interpolationFactor A value between 0 and 1 that determines the relative position
     *        between `p1` and `p2`.
     *        - 0 corresponds to `p1`.
     *        - 1 corresponds to `p2`.
     * @return sf::Vector2f The interpolated point between `p1` and `p2`.
     */
    sf::Vector2f interpolate(const sf::Vector2f &p1, const sf::Vector2f &p2, float interpolationFactor);

    /**
     * @brief Calculates a Bézier curve using the De Casteljau algorithm.
     *
     * This function computes a series of points along a Bézier curve defined by a set
     * of control points. The curve is subdivided into a specified number of segments,
     * and each segment is calculated using linear interpolation at multiple levels.
     *
     * @param controlPoints A vector of control points defining the Bézier curve.
     *        Must contain at least 2 points.
     * @param segments The number of segments to divide the curve into. Higher values
     *        result in a smoother curve.
     * @return std::vector<sf::Vector2f> A vector of points representing the Bézier curve.
     *         The number of points will be `segments + 1`.
     */
    std::vector<sf::Vector2f> calculateBezierCurve(const std::vector<sf::Vector2f> &controlPoints, int segments);
};

#endif // FACE_RENDERER_H