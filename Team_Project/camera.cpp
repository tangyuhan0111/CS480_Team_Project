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
                                 500.0f); //Distance to the far plane, 
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

    if (isObservationModeToggled && isPlanetLockedOn)
    {
		ObservationModeView(); //toggle observation mode view if observation mode is toggled on, otherwise toggle third person view
    }
    else if (isFirstPersonToggled)
    {
        FirstPersonCameraView();
    }
    else 
    {
		ThirdPersonCameraView();
    }
}

void Camera::ToggleFirstPerson(bool enabled)
{
	//first person goes into position of the starship and looks in the direction the starship is facing
    isFirstPersonToggled = enabled;

    if (enabled)
    {
		isPlanetLockedOn = false; //disable planet lock-on when toggling to first person mode
		isObservationModeToggled = false; //disable observation mode when toggling to first person mode
    }

	//observation mode goes into position of the starship and looks at the planet being observed
    Update();
}

void Camera::ToggleObservationMode(bool enabled)
{
    isObservationModeToggled = enabled;

    if (enabled)
    {
        //isPlanetLockedOn = true;
		isFirstPersonToggled = false; //disable first person mode when toggling to observation mode
    }
    else
    {
		isPlanetLockedOn = false;
    }

    Update();
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

    if(isObservationModeToggled && isPlanetLockedOn)
    {
        observationYaw += xoffset;
        observationPitch += yoffset;

        if (observationPitch > 89.0f) 
        {
            observationPitch = 89.0f;
        }

        if (observationPitch < -89.0f) 
        {
            observationPitch = -89.0f;
        }
		ObservationModeView();
        return;
	}

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }
    updateCameraVectors();
}

void Camera::Zoom(float yoffset) {

    if (isObservationModeToggled && isPlanetLockedOn)
    {
		//update observationRadius based on yoffset
        observationRadius -= yoffset * 0.5f; // Adjust the zoom speed for observation mode

        if (observationRadius < 2.0f)
        {
            observationRadius = 2.0f; // Minimum distance from the planet
        }
        if (observationRadius > 200.0f)
        {
            observationRadius = 200.0f; // Maximum distance from the planet
        }
        ObservationModeView();
        return;
    }

    fov -= yoffset;

    if (fov < 1.0f) 
    {
        fov = 1.0f;
    }
    if (fov > 90.0f) 
    {
        fov = 90.0f;
    }

    Update();
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

    if(isObservationModeToggled && isPlanetLockedOn)
    {
        return; //don't update movement if in observation mode with planet lock-on
	}

    starshipPos += cameraFront * currentSpeed * dt; //moves the starship forward

    Update();
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

    Update();
}

void Camera::ThirdPersonCameraView() 
{
    cameraPos = starshipPos - cameraFront * thirdPersonDistancefromStarship + glm::vec3(0.0f, thirdPersonHeightAboveStarship, 0.0f);

    glm::vec3 thirdPersonLookTarget = starshipPos + cameraFront * 2.0f;
    view = glm::lookAt(cameraPos, thirdPersonLookTarget, cameraUp); // positions the camera close behind the starship while looking slightly ahead

    projection = glm::perspective(glm::radians(fov), aspectRatio, 0.01f, 500.0f);
}

void Camera::FirstPersonCameraView()
{
    cameraPos = starshipPos + cameraUp * firstPersonHeightAboveStarship + cameraFront * firstPersonForwardOffset;

    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); //positions the camera at the starship's position

	projection = glm::perspective(glm::radians(fov), aspectRatio, 0.01f, 500.0f);
}

void Camera::ObservationModeView() //might not even be correct but it is supposed to position the camera at a fixed distance from the planet and allow the user to rotate around the planet while keeping the camera focused on the planet
{ 
    float x = PlanetTargetPos.x + observationRadius * cos(glm::radians(observationYaw)) * cos(glm::radians(observationPitch)); //position the camera at a fixed distance from the planet
    float y = PlanetTargetPos.y + observationRadius * sin(glm::radians(observationPitch));
    float z = PlanetTargetPos.z + observationRadius * sin(glm::radians(observationYaw)) * cos(glm::radians(observationPitch));

    cameraPos = glm::vec3(x, y, z);

    view = glm::lookAt(cameraPos, PlanetTargetPos, glm::vec3(0.0f, 1.0f, 0.0f)); //positions the camera at the starship's position
    projection = glm::perspective(glm::radians(fov), aspectRatio, 0.01f, 500.0f);
}

void Camera::SetObservationTarget(const glm::vec3& target) 
{
    PlanetTargetPos = target;

	glm::vec3 offset = cameraPos - PlanetTargetPos;
    observationRadius = glm::length(offset);

    if(observationRadius < 0.001f)
    {
        observationRadius = 10.0f; // Minimum distance from the planet
		offset = glm::vec3(0.0f, 0.0f, observationRadius); // Default offset if the camera is too close to the planet
	}

    observationYaw = glm::degrees(glm::atan(offset.z, offset.x));
    observationPitch = glm::degrees(glm::asin(offset.y / observationRadius));

    defaultObservationYaw = observationYaw;
    defaultObservationPitch = observationPitch;
	defaultObservationRadius = observationRadius;

	isPlanetLockedOn = true;
	isObservationModeToggled = true;
	isFirstPersonToggled = false; //disable first person mode when setting observation target
    
	ObservationModeView();
}

void Camera::UpdateObservationTargetPosition(const glm::vec3& target)
{
    if (!isObservationModeToggled || !isPlanetLockedOn)
    {
        return;
    }

    PlanetTargetPos = target;
    ObservationModeView();
}

void Camera::UpdateObservationCamera(float dt) 
{
 //   if (isPlanetLockedOn)
 //   {
 //       observationYaw += starshipTurnSpeed * dt; //rotate around the planet
 //   }

 //   float x = PlanetTargetPos.x + observationRadius * cos(glm::radians(observationYaw)) * cos(glm::radians(observationPitch));
 //   float y = PlanetTargetPos.y + observationRadius * sin(glm::radians(observationPitch));
 //   float z = PlanetTargetPos.z + observationRadius * sin(glm::radians(observationYaw)) * cos(glm::radians(observationPitch));

 //   cameraPos = glm::vec3(x, y, z);
	//view = glm::lookAt(cameraPos, PlanetTargetPos, cameraUp);

    if(!isObservationModeToggled || !isPlanetLockedOn)
    {
        return; //don't update observation camera if not in observation mode or if planet lock-on is not enabled
	}
	ObservationModeView();
}

void Camera::ResetObservationCamera()
{
    observationYaw = defaultObservationYaw;
    observationPitch = defaultObservationPitch;
	observationRadius = defaultObservationRadius;

    ObservationModeView();
}
void Camera::RotateAroundPlanet()
{
    observationYaw += starshipTurnSpeed * 0.01f; //rotate around the planet
    /*float x = PlanetTargetPos.x + observationRadius * cos(glm::radians(observationYaw)) * cos(glm::radians(observationPitch));
    float y = PlanetTargetPos.y + observationRadius * sin(glm::radians(observationPitch));
    float z = PlanetTargetPos.z + observationRadius * sin(glm::radians(observationYaw)) * cos(glm::radians(observationPitch));
    cameraPos = glm::vec3(x, y, z);
	view = glm::lookAt(cameraPos, PlanetTargetPos, cameraUp);*/

	ObservationModeView();
}
