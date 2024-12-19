#include "face_renderer.h"

FaceRenderer::FaceRenderer(sf::RenderWindow &win) : window(win) {}

sf::Vector2f FaceRenderer::interpolate(const sf::Vector2f &p1, const sf::Vector2f &p2, float interpolationFactor) {
    return sf::Vector2f(
        p1.x + (p2.x - p1.x) * interpolationFactor,
        p1.y + (p2.y - p1.y) * interpolationFactor
    );
}

std::vector<sf::Vector2f> FaceRenderer::calculateBezierCurve(const std::vector<sf::Vector2f> &controlPoints, int segments) {
    std::vector<sf::Vector2f> bezierPoints;
    bezierPoints.reserve(segments + 1);

    for (int step = 0; step <= segments; ++step) {
        float curvePosition = static_cast<float>(step) / segments;

        std::vector<sf::Vector2f> tempPoints = controlPoints;

        for (size_t level = 1; level < controlPoints.size(); ++level) {
            for (size_t i = 0; i < tempPoints.size() - 1; ++i) {
                tempPoints[i] = interpolate(tempPoints[i], tempPoints[i + 1], curvePosition);
            }
            tempPoints.resize(tempPoints.size() - 1);
        }

        bezierPoints.push_back(tempPoints[0]);
    }

    return bezierPoints;
}

void FaceRenderer::renderFace() {
    std::lock_guard<std::mutex> lock(face_config_mutex);

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
                //else if (part_name.find("eyelid") != std::string::npos) renderEyelid(points);
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
    std::lock_guard<std::mutex> lock(face_config_mutex);
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

    std::vector<sf::Vector2f> curve = calculateBezierCurve(points, 20);

    sf::VertexArray eyelid(sf::LinesStrip, curve.size());

    for (size_t i = 0; i < curve.size(); ++i) {
        eyelid[i].position = curve[i];
        eyelid[i].color = sf::Color::Black;
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

void FaceRenderer::renderMouth(const std::vector<sf::Vector2f> &controlPoints) {
    if (controlPoints.size() != 6) return;

    const int segments = 50;
    std::vector<sf::Vector2f> bezierPoints = calculateBezierCurve(controlPoints, segments);

    // Crear un ConvexShape para dibujar la boca rellena
    sf::ConvexShape mouth;
    mouth.setPointCount(bezierPoints.size());

    // Asignar los puntos calculados a la forma
    for (size_t i = 0; i < bezierPoints.size(); ++i) {
        mouth.setPoint(i, bezierPoints[i]);
    }

    // Configurar el color de relleno
    mouth.setFillColor(sf::Color::Black);

    // Dibujar la boca
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
