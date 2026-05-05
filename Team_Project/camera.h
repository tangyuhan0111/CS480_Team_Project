#ifndef CAMERA_H
#define CAMERA_H

#include "graphics_headers.h"

class Camera
{
  public:
    Camera();
    ~Camera();
    bool Initialize(int w, int h);
    glm::mat4 GetProjection();
    glm::mat4 GetView();

    void Update();

    void MoveForward(float amount);
    void MoveBackward(float amount);
    void MoveLeft(float amount);
    void MoveRight(float amount);
    void Rotate(float xoffset, float yoffset);
    void Zoom(float yoffset);
  
  private:

    double x = 0.0;
    double y = 10.0;
    double z = -16.0;
    glm::mat4 projection;
    glm::mat4 view;

    glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, -12.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = 90.0f;
    float pitch = 0.0f;
    float fov = 40.0f;
    float aspectRatio = 1.0f;
};

#endif /* CAMERA_H */
