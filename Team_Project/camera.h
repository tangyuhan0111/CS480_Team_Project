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
    glm::vec3 GetCameraPos() const { return cameraPos; }
    float GetCurrentSpeed() const { return currentSpeed; }
    float GetMaxSpeed() const { return maxSpeed; }
    bool IsFirstPersonMode() const { return isFirstPersonToggled; }
	bool IsObservationMode() const { return isObservationModeToggled; }

    void Update();
    void ToggleFirstPerson(bool enabled);
	void ToggleObservationMode(bool enabled);

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
    void FirstPersonCameraView();
    void ObservationModeView();
	void SetObservationTarget(const glm::vec3& target);
    void UpdateObservationTargetPosition(const glm::vec3& target);
	void UpdateObservationCamera(float dt);
	void ResetObservationCamera();
    void RotateAroundPlanet();
  
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
    float thirdPersonDistancefromStarship = 3.3f;
    float thirdPersonHeightAboveStarship = 1.4f;

	//first person camera
    float firstPersonHeightAboveStarship = 0.8f;
    float firstPersonForwardOffset = 0.6f;

    float yaw = -90.0f; //turning of an object horizontally
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
    float starshipTurnSpeed = 50.0f; //turn speed 
    float rollSpeed = 50.0f; //roll speed

    //planetary observation mode 
    bool isFirstPersonToggled = false; 
	bool isObservationModeToggled = false; //not used yet but can be used to toggle between first person and observation mode
	bool isPlanetLockedOn = false; //not used yet but can be used to lock the camera onto a planet when in first person mode
	glm::vec3 PlanetTargetPos; //the position of the planet being observed
    float observationYaw;
	float observationPitch;
    float observationRadius;
    float defaultObservationYaw;
	float defaultObservationPitch;
	float defaultObservationRadius;

};

#endif /* CAMERA_H */
