#include "face_renderer.h"

FaceRenderer::FaceRenderer(sf::RenderWindow &win) : window(win) {}

// Calculate a single Bézier point between two points
sf::Vector2f interpolate(const sf::Vector2f &p1, const sf::Vector2f &p2, float t) {
    return sf::Vector2f(
        p1.x + (p2.x - p1.x) * t,
        p1.y + (p2.y - p1.y) * t
    );
}

// De Casteljau algorithm for Bézier curve
std::vector<sf::Vector2f> FaceRenderer::calculateBezierCurve(const std::vector<sf::Vector2f> &controlPoints, int segments) {
    std::vector<sf::Vector2f> bezierPoints;

    for (int step = 0; step <= segments; ++step) {
        float t = static_cast<float>(step) / segments;
        std::vector<sf::Vector2f> tempPoints = controlPoints;

        // Apply De Casteljau's algorithm
        while (tempPoints.size() > 1) {
            std::vector<sf::Vector2f> nextLevel;
            for (size_t i = 0; i < tempPoints.size() - 1; ++i) {
                nextLevel.push_back(interpolate(tempPoints[i], tempPoints[i + 1], t));
            }
            tempPoints = nextLevel;
        }

        // The last point is the result for this value of t
        bezierPoints.push_back(tempPoints[0]);
    }

    return bezierPoints;
}

void FaceRenderer::render() {
    window.clear(backgroundColor);

    // Render each facial feature based on faceConfig
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
            std::vector<sf::Vector2f> points(6, sf::Vector2f(0.f, 0.f));
            for (int i = 1; i <= 6; ++i) {
                std::string pointKey = key.substr(0, key.find("_p")) + "_p" + std::to_string(i);
                if (faceConfig.find(pointKey) != faceConfig.end()) {
                    points[i - 1] = faceConfig.at(pointKey);
                }
            }
            renderMouth(points);
        }
    }
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
    eye.setOutlineThickness(1.5f);
    eye.setPosition(center.x - radiusX, center.y - radiusY);
    eye.setScale(1.f, radiusY / radiusX);

    window.draw(eye);
}

void FaceRenderer::renderPupil(const sf::Vector2f &center, float radius) {
    sf::CircleShape pupil(radius);
    pupil.setFillColor(sf::Color::Black);
    pupil.setPosition(center.x - radius, center.y - radius);

    window.draw(pupil);
}

void FaceRenderer::renderMouth(const std::vector<sf::Vector2f> &points) {
    if (points.size() < 4) return;

    // Generate smooth points using Bézier
    std::vector<sf::Vector2f> curve = calculateBezierCurve(points, 100);

    // Create a VertexArray to draw the curve
    sf::VertexArray mouth(sf::LineStrip, curve.size());
    for (size_t i = 0; i < curve.size(); ++i) {
        mouth[i].position = curve[i];
        mouth[i].color = sf::Color::Black;
    }

    window.draw(mouth);
}

void FaceRenderer::renderEyebrow(const std::vector<sf::Vector2f> &points) {
    if (points.size() < 3) return;

    std::vector<sf::Vector2f> curve = calculateBezierCurve(points, 30);

    sf::VertexArray eyebrow(sf::TrianglesStrip, curve.size() * 2);

    float thickness = 5.0f;  // Grosor de la ceja
    for (size_t i = 0; i < curve.size(); ++i) {
        sf::Vector2f direction = (i < curve.size() - 1) ?
                                 curve[i + 1] - curve[i] :
                                 curve[i] - curve[i - 1];

        sf::Vector2f normal(-direction.y, direction.x);
        float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
        normal /= length;  // Normaliza el vector

        eyebrow[i * 2].position = curve[i] + normal * thickness;
        eyebrow[i * 2 + 1].position = curve[i] - normal * thickness;
        eyebrow[i * 2].color = sf::Color::Black;
        eyebrow[i * 2 + 1].color = sf::Color::Black;
    }

    window.draw(eyebrow);
}