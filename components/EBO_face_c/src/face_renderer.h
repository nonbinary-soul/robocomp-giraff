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
    void render(); // renders the entire face
    void setBackgroundColor(const sf::Color &color);

    void setFaceConfig(const std::map<std::string, sf::Vector2f> &config);
    // rendering specific facial parts
    void renderEye(const sf::Vector2f &center, float radiusX, float radiusY);
    void renderPupil(const sf::Vector2f &center, float radius);
    void renderMouth(const std::vector<sf::Vector2f> &points);
    void renderEyebrow(const std::vector<sf::Vector2f> &points);

private:
    sf::RenderWindow &window;
    sf::Color backgroundColor = sf::Color::White;

    std::map<std::string, sf::Vector2f> faceConfig;
    std::vector<sf::Vector2f> calculateBezierCurve(const std::vector<sf::Vector2f> &controlPoints, int segments);
};

#endif // FACE_RENDERER_H