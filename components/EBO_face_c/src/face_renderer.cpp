#include "face_renderer.h"

FaceRenderer::FaceRenderer(sf::RenderWindow &win) : window(win) {}

void FaceRenderer::render(sf::RenderWindow &window) {
    texture.clear(sf::Color::White); // Clear the texture before rendering

    // Render each facial part based on faceConfig
    for (const auto &[key, value] : faceConfig) {
        if (key.find("_center") != std::string::npos) {
            // Handle eyes or pupils
            std::string baseKey = key.substr(0, key.find("_center"));
            if (key.find("Eye") != std::string::npos) {
                if (faceConfig.find(baseKey + "_r1") != faceConfig.end() &&
                    faceConfig.find(baseKey + "_r2") != faceConfig.end()) {
                    float radiusX = faceConfig.at(baseKey + "_r1").x;
                    float radiusY = faceConfig.at(baseKey + "_r2").x;
                    renderEye(value, radiusX, radiusY);
                }
            } else if (key.find("Pupil") != std::string::npos) {
                if (faceConfig.find(baseKey + "_r3") != faceConfig.end()) {
                    float radius = faceConfig.at(baseKey + "_r3").x;
                    renderPupil(value, radius);
                }
            }
        } else if (key.find("Eyebrow") != std::string::npos || key.find("Eyelid") != std::string::npos) {
            // Collect points for eyebrows or eyelids
            std::vector<sf::Vector2f> points;
            for (int i = 1; i <= 4; ++i) {
                std::string pointKey = key.substr(0, key.find("_p")) + "_p" + std::to_string(i);
                if (faceConfig.find(pointKey) != faceConfig.end()) {
                    points.push_back(faceConfig.at(pointKey));
                }
            }
            if (!points.empty()) {
                if (key.find("Eyebrow") != std::string::npos) {
                    renderEyebrow(points);
                } else if (key.find("Eyelid") != std::string::npos) {
                    renderEyebrow(points); // Reuse eyebrow renderer for eyelids
                }
            }
        } else if (key.find("mouth") != std::string::npos) {
            // Collect points for the mouth
            std::vector<sf::Vector2f> points(6, sf::Vector2f(0.f, 0.f)); // Initialize with 6 points at (0, 0)
            for (int i = 1; i <= 6; ++i) {
                std::string pointKey = key.substr(0, key.find("_p")) + "_p" + std::to_string(i);
                if (faceConfig.find(pointKey) != faceConfig.end()) {
                    points[i - 1] = faceConfig.at(pointKey); // Assign existing points
                }
            }
            renderMouth(points);
        }
    }

    // Draw the final composed texture on the window
    sf::Sprite sprite(texture.getTexture());
    window.draw(sprite);
}

void FaceRenderer::setBackgroundColor(const sf::Color &color) {
    backgroundColor = color;
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
