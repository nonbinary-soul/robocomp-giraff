#ifndef FACE_RENDERER_H
#define FACE_RENDERER_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>
#include <cmath>


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

    std::vector<sf::Vector2f> calculateBezierCurve(const std::vector<sf::Vector2f> &controlPoints, int segments);
};

#endif // FACE_RENDERER_H