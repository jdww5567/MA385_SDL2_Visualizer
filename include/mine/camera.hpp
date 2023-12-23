#ifndef MINE_CAMERA_HPP
#define MINE_CAMERA_HPP

#include <glm/glm.hpp>

namespace mine {
class camera {
    float radius;
public:
    float screenWidth;
    float screenHeight;
    float aspectRatio;
    float fov;
    float theta;
    float phi;
    glm::vec3 pos;
    glm::vec3 center;
    glm::highp_mat4 view;

    camera();
    void setScreen(float screenWidth_, float screenHeight_);
    void setData(float radius_, float theta_, float phi_);
    void setCenter(float xNB, float xPB, float zNB, float zPB);
    void zoom(bool in, bool out);
    void updateAngles(float theta_, float phi_);
private:
    void updatePos();
};
}

#endif