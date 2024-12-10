#ifndef FACE_RENDERER_H
#define FACE_RENDERER_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <stdexcept>

class FaceRenderer {
public:
    FaceRenderer(int res_x, int res_y);
    void render(sf::RenderWindow &window); // renders the entire face
    void setFaceConfig(const std::map<std::string, sf::Vector2f> &config);

    // rendering specific facial parts
    void renderEye(const sf::Vector2f &center, float radiusX, float radiusY);
    void renderPupil(const sf::Vector2f &center, float radius);
    void renderMouth(const std::vector<sf::Vector2f> &points);
    void renderEyebrow(const std::vector<sf::Vector2f> &points);

private:
    sf::RenderTexture texture;
    std::map<std::string, sf::Vector2f> faceConfig;
};

#endif // FACE_RENDERER_H