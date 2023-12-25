#ifndef MINE_CAMERA_HPP
#define MINE_CAMERA_HPP

#include <array>

#include <glm/glm.hpp>

namespace mine {
class camera {
    float radius;
public:
    float theta;
    float phi;
    float screen_width;
    float screen_height;
    float aspect_ratio;
    float FOV;

    glm::vec3 position;
    glm::vec3 center;
    glm::highp_mat4 view;

    camera();
    void set_screen(float screen_width, float screen_height);
    void set_data(float radius, float theta, float phi);
    void set_center(const std::array<int, 4>& bounds);
    void zoom(bool in, bool out);
    void update_angles(float theta, float phi);
private:
    void update_position();
};
}

#endif