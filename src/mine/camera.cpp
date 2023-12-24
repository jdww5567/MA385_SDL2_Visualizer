#include <mine/camera.hpp>

#include <cmath>

#include <glm/gtx/transform.hpp>

#include <mine/enums.hpp>

namespace mine {
camera::camera() : 
    radius{}, theta{}, phi{}, screen_width{}, screen_height{}, aspect_ratio{}, 
    fov{glm::radians(60.0f)}, position{}, center{}, view{} 
{}

void camera::set_screen(float screen_width, float screen_height) {
    this->screen_width = screen_width;
    this->screen_height = screen_height;
    aspect_ratio = this->screen_width / this->screen_height;

    if (aspect_ratio < 1) {
        fov = glm::atan(glm::tan(fov / 2.0f) / aspect_ratio) * 2.0f;
    } else {
        fov = glm::radians(60.0f);
    }

    update_position();
}

void camera::set_data(float radius, float theta, float phi) {
    this->theta = theta;

    if (radius < 0.1f) {
        this->radius = 0.1f;
    } else if (radius > 10000.0f) {
        this->radius = 10000.0f;
    } else {
        this->radius = radius;
    }

    if (phi < 2.0f) {
        this->phi = 2.0f;
    } else if (phi > 178.0f) {
        this->phi = 178.0f;
    } else {
        this->phi = phi;
    }

    update_position();
}

void camera::set_center(const std::array<int, 4>& bounds) {
    center.x = (bounds[NEG_X_BOUND] + bounds[POS_X_BOUND]) / 2.0f;
    center.z = (bounds[NEG_Z_BOUND] + bounds[POS_Z_BOUND]) / 2.0f;

    update_position();
}

void camera::zoom(bool in, bool out) {
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

    update_position();
}

void camera::update_angles(float theta, float phi) {
    this->theta += theta;
    this->phi += phi;

    if (this->theta > 360.0f) {
        this->theta = this->theta - 360.0f;
    } else if (this->theta < -360.0f) {
        this->theta = this->theta + 360.0f;
    }

    if (this->phi < 2.0f) {
        this->phi = 2.0f;
    } else if (phi > 178.0f) {
        this->phi = 178.0f;
    }

    update_position();
}

void camera::update_position() {
    position = glm::vec3(
        radius * sin(glm::radians(phi)) * cos(glm::radians(theta)) + center.x,
        radius * cos(glm::radians(phi)) + center.y,
        radius * sin(glm::radians(phi)) * sin(glm::radians(theta)) + center.z
    );

    glm::vec3 camera_up{};

    if (position.y > center.y) {
        camera_up = glm::normalize(glm::vec3(
            -(position.x - center.x),
            (glm::pow(position.x - center.x, 2) + glm::pow(position.z - center.z, 2)) / (position.y - center.y),
            -(position.z - center.z)
        ));
    } else if (position.y < center.y) {
        camera_up = glm::normalize(glm::vec3(
            position.x - center.x,
            -(glm::pow(position.x - center.x, 2) + glm::pow(position.z - center.z, 2)) / (position.y - center.y),
            position.z - center.z
        ));
    } else {
        camera_up = glm::vec3(0, 1, 0);
    }

    view = glm::lookAt(position, center, camera_up);
    
    view = glm::perspective(
        fov,
        aspect_ratio,
        0.01f,
        50.0f
    ) * view;
}
}