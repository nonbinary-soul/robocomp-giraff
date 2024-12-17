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

void FaceRenderer::renderFace() {
    // Clear the window with the background color
    window.clear(background_color);

    // Render each facial feature based on face_config
    for (const auto &[part_name, attribute] : face_config) {
        if (part_name.find("_center") != std::string::npos) {
            // eyes and pupils
            std::string base_key = part_name.substr(0, part_name.find("_center"));

            if (part_name.find("eye") != std::string::npos) {
                // render eyes if the radius values exist
                if (face_config.find(base_key + "_r1") != face_config.end() &&
                    face_config.find(base_key + "_r2") != face_config.end()) {
                    float radius_x = face_config.at(base_key + "_r1").x;
                    float radius_y = face_config.at(base_key + "_r2").x;
                    renderEye(attribute, radius_x, radius_y);
                }
            } else if (part_name.find("pupil") != std::string::npos) {
                // render pupils if the radius value exists
                if (face_config.find(base_key + "_r3") != face_config.end()) {
                    float radius = face_config.at(base_key + "_r3").x;
                    renderPupil(attribute, radius);
                }
            } else if (part_name.find("tongue") != std::string::npos) {
                if (face_config.find(base_key + "_r1") != face_config.end() &&
                    face_config.find(base_key + "_r2") != face_config.end()) {
                    float radius_x = face_config.at(base_key + "_r1").x;
                    float radius_y = face_config.at(base_key + "_r2").x;
                    renderTongue(attribute, radius_x, radius_y);
                }
            }
        } else if (part_name.find("eyebrow") != std::string::npos ||
                   part_name.find("eyelid") != std::string::npos ||
                   part_name.find("cheek") != std::string::npos ||
                   part_name.find("tongue") != std::string::npos
                   ) {
            // Collect points
            std::vector<sf::Vector2f> points;
            for (int i = 1; i <= 4; ++i) {
                std::string point_key = part_name.substr(0, part_name.find("_p")) + "_p" + std::to_string(i);
                if (face_config.find(point_key) != face_config.end()) {
                    points.push_back(face_config.at(point_key));
                }
            }

            // render the correct part
            if (!points.empty()) {
                if (part_name.find("eyebrow") != std::string::npos) renderEyebrow(points);
                else if (part_name.find("eyelid") != std::string::npos) renderEyelid(points);
                else if (part_name.find("cheek") != std::string::npos) renderCheek(points);
            }
        } else if (part_name.find("mouth") != std::string::npos) {
            // Collect points for the mouth
            std::vector<sf::Vector2f> points(6, sf::Vector2f(0.f, 0.f));
            for (int i = 1; i <= 6; ++i) {
                std::string point_key = part_name.substr(0, part_name.find("_p")) + "_p" + std::to_string(i);
                if (face_config.find(point_key) != face_config.end()) {
                    points[i - 1] = face_config.at(point_key);
                }
            }
            renderMouth(points);
        }
    }
}

void FaceRenderer::setBackgroundColor(const sf::Color &color) {
    background_color = color;
}

void FaceRenderer::setFaceConfig(const std::map<std::string, sf::Vector2f> &config) {
    face_config = config;
}

void FaceRenderer::renderEyebrow(const std::vector<sf::Vector2f> &points) {
    if (points.size() < 4) return;

    sf::ConvexShape eyebrow;
    eyebrow.setPointCount(points.size());

    // asigning points to ConvexShape
    for (size_t i = 0; i < points.size(); ++i) {
        eyebrow.setPoint(i, points[i]);
    }

    eyebrow.setFillColor(sf::Color::Black);

    // Dibujar la ceja en la ventana
    window.draw(eyebrow);
}

void FaceRenderer::renderEyelid(const std::vector<sf::Vector2f> &points) {
    if (points.size() < 4) return;

    // Calcular la curva Bézier
    std::vector<sf::Vector2f> curve = calculateBezierCurve(points, 20);

    // Usar VertexArray para dibujar una línea suave (LinesStrip)
    sf::VertexArray eyelid(sf::LinesStrip, curve.size());

    for (size_t i = 0; i < curve.size(); ++i) {
        eyelid[i].position = curve[i];
        eyelid[i].color = sf::Color::Black; // Color del párpado
    }

    window.draw(eyelid);
}

void FaceRenderer::renderEye(const sf::Vector2f &center, float radius_x, float radius_y) {
    sf::CircleShape eye(radius_x);
    eye.setFillColor(sf::Color::Black);
    eye.setOrigin(radius_x, radius_y);
    eye.setPosition(center);
    eye.setScale(1.f, radius_y / radius_x); // making the ellipse

    window.draw(eye);
}

void FaceRenderer::renderPupil(const sf::Vector2f &center, float radius) {
    sf::CircleShape pupil(radius);
    pupil.setFillColor(sf::Color::White);
    pupil.setOrigin(radius, radius);
    pupil.setPosition(center);

    window.draw(pupil);
}

void FaceRenderer::renderMouth(const std::vector<sf::Vector2f> &points) {
    if (points.size() != 6) return; // Verifica que haya exactamente 6 puntos

    sf::ConvexShape mouth;
    mouth.setPointCount(points.size());

    for (size_t i = 0; i < points.size(); ++i) {
        mouth.setPoint(i, points[i]);
    }

    mouth.setFillColor(sf::Color::Black);

    window.draw(mouth);
}

void FaceRenderer::renderTongue(const sf::Vector2f &center, float radius_x, float radius_y) {
    sf::CircleShape tongue(radius_x);

    tongue.setFillColor(sf::Color(255, 105, 180));
    tongue.setOrigin(radius_x, radius_x);
    tongue.setPosition(center);
    tongue.setScale(1.0f, radius_y / radius_x); // ellipse

    window.draw(tongue);
}

void FaceRenderer::renderCheek(const std::vector<sf::Vector2f> &points) {
    if (points.size() != 4) return;

    sf::ConvexShape cheek;
    cheek.setPointCount(points.size());

    for (size_t i = 0; i < points.size(); ++i) {
        cheek.setPoint(i, points[i]);
    }

    // color and transparency
    sf::Color cheekColor = sf::Color(255, 182, 193);
    cheekColor.a = 100;

    cheek.setFillColor(cheekColor);

    window.draw(cheek);
}
