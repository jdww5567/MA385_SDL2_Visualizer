#include <mine/cameraHandler.hpp>
#include <cmath>
#include <glm/gtx/transform.hpp>

namespace mine {
cameraHandler::cameraHandler() {
    radius = 0;
    theta = 0;
    phi = 0;
    screenWidth = 0;
    screenHeight = 0;
    pos = glm::vec3();
    view = glm::highp_mat4();
}

void cameraHandler::setScreen(float screenWidth_, float screenHeight_) {
    screenWidth = screenWidth_;
    screenHeight = screenHeight_;
}

void cameraHandler::setData(float radius_, float theta_, float phi_) {
    theta = theta_;

    if (radius_ < 0.1f) {
        radius = 0.1f;
    } else {
        radius = radius_;
    }

    if (phi_ < 1.0f) {
        phi = 1.0f;
    } else if (phi_ > 179.0f) {
        phi = 179.0f;
    } else {
        phi = phi_;
    }

    updatePos();
}

void cameraHandler::zoom(bool in) {
    if (in) {
        radius -= 0.1f * radius;
        if (radius < 0.1f) {
            radius = 0.1f;
        }
    } else {
        radius += 0.1f * radius;
    }

    updatePos();
}

void cameraHandler::updateAngles(float theta_, float phi_) {
    theta = theta_;

    if (phi_ < 1.0f) {
        phi = 1.0f;
    } else if (phi_ > 179.0f) {
        phi = 179.0f;
    } else {
        phi = phi_;
    }

    updatePos();
}

void cameraHandler::updatePos() {
    pos = glm::vec3(
        radius * sin(glm::radians(phi)) * cos(glm::radians(theta)),
        radius * cos(glm::radians(phi)),
        radius * sin(glm::radians(phi)) * sin(glm::radians(theta))
    );

    glm::vec3 cameraUp = glm::vec3();

    if (pos.y > 0) {
        cameraUp = glm::normalize(glm::vec3(
            -pos.x,
            (powf(pos.x, 2) + powf(pos.z, 2)) / pos.y,
            -pos.z
        ));
    } else if (pos.y < 0) {
        cameraUp = glm::normalize(glm::vec3(
            pos.x,
            -(powf(pos.x, 2) + powf(pos.z, 2)) / pos.y,
            pos.z
        ));
    } else {
        cameraUp = glm::vec3(0, 1, 0);
    }

    view = glm::lookAt(pos, glm::vec3(0, 0, 0), cameraUp);

    view = glm::perspective(
        glm::radians(60.0f),
        screenWidth / screenHeight,
        0.01f,
        1000.0f
    ) * view;
}
}