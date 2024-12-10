#include "face_renderer.h"

FaceRenderer::FaceRenderer(int resolutionX, int resolutionY) {
    // creating a renderable texture with the specified resolution
    if (!texture.create(resolutionX, resolutionY)) {
        throw std::runtime_error("Failed to create render texture");
    }
    texture.clear(sf::Color::White);
}


void FaceRenderer::render(sf::RenderWindow &window) {
    sf::Sprite sprite(texture.getTexture());
    window.draw(sprite);
}

void FaceRenderer::setFaceConfig(const std::map<std::string, sf::Vector2f> &config) {
    faceConfig = config;
}

void FaceRenderer::renderEye(const sf::Vector2f &center, float radiusX, float radiusY) {
    sf::CircleShape eye(radiusX);
    eye.setFillColor(sf::Color::White);
    eye.setOutlineColor(sf::Color::Black);
    eye.setOutlineThickness(2);
    eye.setPosition(center.x - radiusX, center.y - radiusY);
    eye.setScale(1.f, radiusY / radiusX);

    texture.draw(eye);
}

void FaceRenderer::renderPupil(const sf::Vector2f &center, float radius) {
    sf::CircleShape pupil(radius); // Create a circle for the pupil
    pupil.setFillColor(sf::Color::Black); // Set the pupil's color to black
    pupil.setPosition(center.x - radius, center.y - radius); // Position the pupil

    texture.draw(pupil); // Draw the pupil on the texture
}

void FaceRenderer::renderMouth(const std::vector<sf::Vector2f> &points) {
    sf::VertexArray mouth(sf::TrianglesFan, points.size()); // Create a triangle fan for the mouth

    for (size_t i = 0; i < points.size(); ++i) {
        mouth[i].position = points[i]; // Assign each point
        mouth[i].color = sf::Color::Red; // Set the mouth color to red
    }

    texture.draw(mouth); // Draw the mouth on the texture
}

void FaceRenderer::renderEyebrow(const std::vector<sf::Vector2f> &points) {
    sf::VertexArray eyebrow(sf::LinesStrip, points.size()); // Create a line strip for the eyebrow

    for (size_t i = 0; i < points.size(); ++i) {
        eyebrow[i].position = points[i]; // Assign each point
        eyebrow[i].color = sf::Color::Black; // Set the eyebrow color to black
    }

    texture.draw(eyebrow); // Draw the eyebrow on the texture
}
