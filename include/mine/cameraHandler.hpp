#ifndef MINE_CAMERAHANDLER_HPP
#define MINE_CAMERAHANDLER_HPP

#include <glm/glm.hpp>

namespace mine {
class cameraHandler {
    float radius;
    float screenWidth;
    float screenHeight;
public:
    float theta;
    float phi;
    glm::vec3 pos;
    glm::highp_mat4 view;

    cameraHandler();
    void setScreen(float screenWidth_, float screenHeight_);
    void setData(float radius_, float theta_, float phi_);
    void zoom(bool in);
    void updateAngles(float theta_, float phi_);
private:
    void updatePos();
};
}

#endif