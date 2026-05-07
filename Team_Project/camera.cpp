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
    //Update();

  aspectRatio = float(w) / float(h);
  updateCameraVectors();

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
    updateCameraVectors();
    /*glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(direction);

    Update();*/
}

void Camera::Zoom(float yoffset) {
    fov -= yoffset;

    if (fov < 1.0f) {
        fov = 1.0f;
    }
    if (fov > 90.0f) {
        fov = 90.0f;
    }
    ThirdPersonCameraView();
}

void Camera::Exploration(GLFWwindow* window, float dt)
{
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) //turn left
    {
        yaw -= starshipTurnSpeed * dt;
    }
       
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) //turn right
    {
        yaw += starshipTurnSpeed * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) //starship turns and points upwards
    {
        pitch += starshipTurnSpeed * dt;
    }
    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) //starship turns and points downwards
    {
        pitch -= starshipTurnSpeed * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        roll -= rollSpeed * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        roll += rollSpeed * dt;
    }

    ProcessMouseMovement(0.0f, 0.0f);
}

void Camera::Accelerate(float dt) 
{
    currentSpeed += acceleration * dt;

    if (currentSpeed >= maxSpeed)
    {
        currentSpeed = maxSpeed;
    }
}

void Camera::Brake(float dt)
{
    currentSpeed -= braking * dt; //starts to slow down

    if (currentSpeed < 0.0f) {
        currentSpeed = 0.0f; //Stop
    }
}

void Camera::UpdateMovement(float dt) 
{
    starshipPos += cameraFront * currentSpeed * dt; //moves the starship forward

    ThirdPersonCameraView();
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) 
{
    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }

    if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    glm::vec3 direction;

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(direction);

    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::cross(cameraFront, worldUp);

    if (glm::length(right) < 0.001f)
    {
        right = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    else
    {
        right = glm::normalize(right);
    }

    glm::vec3 baseUp = glm::normalize(glm::cross(right, cameraFront));
    glm::mat4 rollRotation = glm::rotate(glm::mat4(1.0f), glm::radians(roll), cameraFront);
    cameraUp = glm::normalize(glm::vec3(rollRotation * glm::vec4(baseUp, 0.0f)));

    ThirdPersonCameraView();
}

void Camera::ThirdPersonCameraView() 
{
    cameraPos = starshipPos - cameraFront * thirdPersonDistancefromStarship + glm::vec3(0.0f, thirdPersonHeightAboveStarship, 0.0f);

    view = glm::lookAt(cameraPos, starshipPos, cameraUp); //positions the camera behind and above the starship

    projection = glm::perspective(glm::radians(fov), aspectRatio, 0.01f, 100.0f);
}