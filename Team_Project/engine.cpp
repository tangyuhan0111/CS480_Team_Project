#include "engine.h"
#include <limits>
#include "glm/ext.hpp"

Engine::Engine(const char* name, int width, int height)
{
  m_WINDOW_NAME = name;
  m_WINDOW_WIDTH = width;
  m_WINDOW_HEIGHT = height;

}


Engine::~Engine()
{
  delete m_window;
  delete m_graphics;
  m_window = NULL;
  m_graphics = NULL;
}

bool Engine::Initialize()
{
  // Start a window
  m_window = new Window(m_WINDOW_NAME, &m_WINDOW_WIDTH, &m_WINDOW_HEIGHT);
  if(!m_window->Initialize())
  {
    printf("The window failed to initialize.\n");
    return false;
  }

  // Start the graphics
  m_graphics = new Graphics();
  if(!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT))
  {
    printf("The graphics failed to initialize.\n");
    return false;
  }

  glfwSetWindowUserPointer(m_window->getWindow(), this);
  glfwSetCursorPosCallback(m_window->getWindow(), Engine::cursor_position_callback);
  glfwSetScrollCallback(m_window->getWindow(), Engine::scroll_callback);
  glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED); //disables mouse cursor so it can moved outside of window

  // No errors
  return true;
}

void Engine::Run()
{
  m_running = true;

  while (!glfwWindowShouldClose(m_window->getWindow()))
  {
      ProcessInput();
      Display(m_window->getWindow(), glfwGetTime());
      glfwPollEvents();
  }
  m_running = false;

}

void Engine::ProcessInput()
{
    float currentFrame = static_cast<float>(glfwGetTime());
    float deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window->getWindow(), true);

    Camera* camera = m_graphics->getCamera();

	//planetary observation mode
    bool tabPressed = glfwGetKey(m_window->getWindow(), GLFW_KEY_TAB) == GLFW_PRESS;
    if (tabPressed && !isTabPressed)
    {
        if (!camera->IsObservationMode())
        {
            glm::vec3 closestPlanetPos = m_graphics->getPlanetPosition("Earth"); //default to Earth if no planets are close enough
            float closestDistance = std::numeric_limits<float>::max();
            m_observationPlanetName = "Earth";

            for (const std::string& planetName : {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"}) 
            {
                glm::vec3 planetPos = m_graphics->getPlanetPosition(planetName);
                float distance = glm::length(planetPos - camera->GetStarshipPos());

                if (distance < closestDistance) 
                {
                    closestDistance = distance;
                    closestPlanetPos = planetPos;
                    m_observationPlanetName = planetName;
                }
            }
			camera->SetObservationTarget(closestPlanetPos);
        }
        else 
        {
            m_observationPlanetName.clear();
			camera->ToggleObservationMode(false);
        }
    }
    isTabPressed = tabPressed;

    if (camera->IsObservationMode())
    {
        if (!m_observationPlanetName.empty())
        {
            camera->UpdateObservationTargetPosition(m_graphics->getPlanetPosition(m_observationPlanetName));
        }

        camera->UpdateObservationCamera(deltaTime);

        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_R) == GLFW_PRESS)
        {
			camera->ResetObservationCamera();
		}

        //different key bindings to toggle the 8 different planets
        bool isKey1Pressed = glfwGetKey(m_window->getWindow(), GLFW_KEY_1) == GLFW_PRESS; //set to keybind 1
        if (isKey1Pressed && !isKeyPressed[0])
        {
            m_observationPlanetName = "Mercury";
        }

        bool isKey2Pressed = glfwGetKey(m_window->getWindow(), GLFW_KEY_2) == GLFW_PRESS; //set to keybind 2
        if (isKey2Pressed && !isKeyPressed[1])
        {
            m_observationPlanetName = "Venus";
        }

		bool isKey3Pressed = glfwGetKey(m_window->getWindow(), GLFW_KEY_3) == GLFW_PRESS; //set to keybind 3
        if (isKey3Pressed && !isKeyPressed[2])
        {
            m_observationPlanetName = "Earth";
        }

		bool isKey4Pressed = glfwGetKey(m_window->getWindow(), GLFW_KEY_4) == GLFW_PRESS; //set to keybind 4
        if (isKey4Pressed && !isKeyPressed[3])
        {
            m_observationPlanetName = "Mars";
        }

		bool isKey5Pressed = glfwGetKey(m_window->getWindow(), GLFW_KEY_5) == GLFW_PRESS; //set to keybind 5
        if (isKey5Pressed && !isKeyPressed[4])
        {
            m_observationPlanetName = "Jupiter";
        }

		bool isKey6Pressed = glfwGetKey(m_window->getWindow(), GLFW_KEY_6) == GLFW_PRESS; //set to keybind 6
        if (isKey6Pressed && !isKeyPressed[5])
        {
            m_observationPlanetName = "Saturn";
        }

		bool isKey7Pressed = glfwGetKey(m_window->getWindow(), GLFW_KEY_7) == GLFW_PRESS; //set to keybind 7
        if (isKey7Pressed && !isKeyPressed[6])
        {
            m_observationPlanetName = "Uranus";
        }

		bool isKey8Pressed = glfwGetKey(m_window->getWindow(), GLFW_KEY_8) == GLFW_PRESS; //set to keybind 8
        if (isKey8Pressed && !isKeyPressed[7])
        {
			m_observationPlanetName = "Neptune";
        }
        return;
	}

    //exploration mode
    camera->Exploration(m_window->getWindow(), deltaTime); //calls exploration controls

    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) //brakes
    {
        camera->Brake(deltaTime);
    }

    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) //accelerates
    {
        camera->Accelerate(deltaTime);
    }

    camera->UpdateMovement(deltaTime); //updates camera movement
}

void Engine::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
 
    //glfwGetCursorPos(window, &xpos, &ypos);
    //std::cout << "Position: (" << xpos << ":" << ypos << ")";
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

    if (engine->firstMouse) {
        engine->lastX = (float)xpos;
        engine->lastY = (float)ypos;
        engine->firstMouse = false;
    }

    float xoffset = (float)xpos - engine->lastX;
    float yoffset = engine->lastY - (float)ypos;

    engine->lastX = (float)xpos;
    engine->lastY = (float)ypos;

    engine->m_graphics->getCamera()->Rotate(xoffset, yoffset);
}

unsigned int Engine::getDT()
{
  //long long TimeNowMillis = GetCurrentTimeMillis();
  //assert(TimeNowMillis >= m_currentTimeMillis);
  //unsigned int DeltaTimeMillis = (unsigned int)(TimeNowMillis - m_currentTimeMillis);
  //m_currentTimeMillis = TimeNowMillis;
  //return DeltaTimeMillis;
    return glfwGetTime();
}

long long Engine::GetCurrentTimeMillis()
{
  //timeval t;
  //gettimeofday(&t, NULL);
  //long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
  //return ret;
    return (long long) glfwGetTime();
}

void Engine::Display(GLFWwindow* window, double time) {

    m_graphics->HierarchicalUpdate2(time);
    m_graphics->Render();
    m_window->Swap();
}

static void cursorPositionCallBack(GLFWwindow* window, double xpos, double ypos) {
    //cout << xpos << " " << ypos << endl;
}

void Engine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    engine->m_graphics->getCamera()->Zoom((float)yoffset);
}
