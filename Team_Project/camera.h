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
    glm::vec3 GetStarshipPos() const { return starshipPos; }
    glm::vec3 GetStarshipFront() const { return cameraFront; }
    glm::vec3 GetStarshipUp() const { return cameraUp; }

    void Update();

    void MoveForward(float amount);
    void MoveBackward(float amount);
    void MoveLeft(float amount);
    void MoveRight(float amount);
    void Rotate(float xoffset, float yoffset);
    void Zoom(float yoffset);
    void Exploration(GLFWwindow* window, float dt);
    void Accelerate(float dt);
    void Brake(float dt);
    void UpdateMovement(float dt);
    void ProcessMouseMovement(float xoffset, float yoffset);
    void updateCameraVectors();
    void ThirdPersonCameraView();
  
  private:

    double x = 0.0;
    double y = 10.0;
    double z = -16.0;
    glm::mat4 projection;
    glm::mat4 view;

    glm::vec3 cameraPos = glm::vec3(0.0f, 4.0f, 2.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    //third person camera
    glm::vec3 starshipPos = glm::vec3(0.0f, 0.0f, 12.0f);
    float thirdPersonDistancefromStarship = 10.0f;
    float thirdPersonHeightAboveStarship = 4.0f;

    float yaw = 90.0f; //turning of an object horizontally
    float pitch = 0.0f; //tilts an object upward or downward
    float fov = 40.0f;
    float aspectRatio = 1.0f;

    //acceleration and braking
    float currentSpeed = 0.0f;
    float acceleration = 8.0f;
    bool isBraking;
    float braking = 10.0f;
    float maxSpeed = 15.0f;

    //exploration mode (yaw, pitch and roll)
    float roll = 0.0f; //tilts the wings of the starship 
    float starshipTurnSpeed = 60.0f; //turn speed 
    float rollSpeed = 50.0f; //roll speed
};

#endif /* CAMERA_H */
