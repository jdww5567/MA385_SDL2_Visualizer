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
    aspectRatio = 0;
    fov = glm::radians(60.0f);
    pos = glm::vec3();
    center = glm::vec3();
    view = glm::highp_mat4();
}

void cameraHandler::setScreen(float screenWidth_, float screenHeight_) {
    screenWidth = screenWidth_;
    screenHeight = screenHeight_;
    aspectRatio = screenWidth / screenHeight;

    fov = glm::radians(60.0f);
    if (aspectRatio < 1) {
        fov = glm::atan(glm::tan(fov / 2.0f) / aspectRatio) * 2.0f;
    }

    updatePos();
}

void cameraHandler::setData(float radius_, float theta_, float phi_) {
    theta = theta_;

    if (radius_ < 0.1f) {
        radius = 0.1f;
    } else {
        radius = radius_;
    }

    if (phi_ < 2.0f) {
        phi = 2.0f;
    } else if (phi_ > 178.0f) {
        phi = 178.0f;
    } else {
        phi = phi_;
    }

    updatePos();
}

void cameraHandler::setCenter(float xNB, float xPB, float zNB, float zPB) {
    center.x = (xNB + xPB) / 2.0;
    center.z = (zNB + zPB) / 2.0;

    if (center.x == 0 || center.z == 0) {
        center = glm::vec3{0, center.y, 0};
    }

    updatePos();
}

void cameraHandler::setPlane(float y) {
    if (y > 10000.0f || y < -10000.0f) {
        return;
    }
    center.y = y;

    updatePos();
}

void cameraHandler::zoom(bool in, bool out) {
    if (in) {
        radius -= 0.1f * radius;
        if (radius < 0.1f) {
            radius = 0.1f;
        }
    } else if (out) {
        radius += 0.1f * radius;
        if (radius > 10000.0f) {
            radius = 10000.0f;
        }
    }

    updatePos();
}

void cameraHandler::updateAngles(float theta_, float phi_) {
    theta = theta_;

    if (phi_ < 2.0f) {
        phi = 2.0f;
    } else if (phi_ > 178.0f) {
        phi = 178.0f;
    } else {
        phi = phi_;
    }

    updatePos();
}

void cameraHandler::updatePos() {
    pos = glm::vec3(
        radius * sin(glm::radians(phi)) * cos(glm::radians(theta)) + center.x,
        radius * cos(glm::radians(phi)) + center.y,
        radius * sin(glm::radians(phi)) * sin(glm::radians(theta)) + center.z
    );

    glm::vec3 cameraUp = glm::vec3();

    if (pos.y > center.y) {
        cameraUp = glm::normalize(glm::vec3(
            -(pos.x - center.x),
            (glm::pow(pos.x - center.x, 2) + glm::pow(pos.z - center.z, 2)) / (pos.y - center.y),
            -(pos.z - center.z)
        ));
    } else if (pos.y < center.y) {
        cameraUp = glm::normalize(glm::vec3(
            pos.x - center.x,
            -(glm::pow(pos.x - center.x, 2) + glm::pow(pos.z - center.z, 2)) / (pos.y - center.y),
            pos.z - center.z
        ));
    } else {
        cameraUp = glm::vec3(0, 1, 0);
    }

    view = glm::lookAt(pos, center, cameraUp);
    
    view = glm::perspective(
        fov,
        aspectRatio,
        0.01f,
        100.0f
    ) * view;
}
}