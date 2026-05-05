#include "camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{

}

bool Camera::Initialize(int w, int h)
{
  //--Init the view and projection matrices
  //  if you will be having a moving camera the view matrix will need to more dynamic
  //  ...Like you should update it before you render more dynamic 
  //  for this project having them static will be fine
  //view = glm::lookAt( glm::vec3(x, y, z), //Eye Position
  //                    glm::vec3(0.0, 0.0, 0.0), //Focus point
  //                    glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up
  
  //view = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);
    Update();

    aspectRatio = float(w) / float(h);
  projection = glm::perspective( glm::radians(fov), //the FoV typically 90 degrees is good which is what this is set to
                                 //float(w)/float(h), //Aspect Ratio, so Circles stay Circular
                                 aspectRatio,
                                 0.01f, //Distance to the near plane, normally a small value like this
                                 100.0f); //Distance to the far plane, 
  return true;
}

glm::mat4 Camera::GetProjection()
{
  return projection;
}

glm::mat4 Camera::GetView()
{
  return view;
}

void Camera::Update() {
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    projection = glm::perspective(glm::radians(fov), aspectRatio, 0.01f, 100.0f);
}

void Camera::MoveForward(float amount) {
    cameraPos += cameraFront * amount;
    Update();
}

void Camera::MoveBackward(float amount) {
    cameraPos -= cameraFront * amount;
    Update();
}

void Camera::MoveLeft(float amount)
{
    glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
    cameraPos -= right * amount;
    Update();
}

void Camera::MoveRight(float amount)
{
    glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
    cameraPos += right * amount;
    Update();
}

void Camera::Rotate(float xoffset, float yoffset)
{
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(direction);

    Update();
}

void Camera::Zoom(float yoffset) {
    fov -= yoffset;
    if (fov < 1.0f) {
        fov = 1.0f;
    }
    if (fov > 90.0f) {
        fov = 90.0f;
    }
    Update();
}