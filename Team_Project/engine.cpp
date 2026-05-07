

#include "engine.h"
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

  //glfwSetCursorPosCallback(m_window->getWindow(), cursorPositionCallBack);
  glfwSetWindowUserPointer(m_window->getWindow(), this);
  glfwSetCursorPosCallback(m_window->getWindow(), Engine::cursor_position_callback);
  glfwSetScrollCallback(m_window->getWindow(), Engine::scroll_callback);

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


    //exploration mode

    m_graphics->getCamera()->Exploration(m_window->getWindow(), deltaTime); //calls exploration controls

    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) //brakes
    {
        m_graphics->getCamera()->Brake(deltaTime);
    }

    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) //accelerates
    {
        m_graphics->getCamera()->Accelerate(deltaTime);
    }

    m_graphics->getCamera()->UpdateMovement(deltaTime); //updates camera movement (commented for now since the camera will be moving forward out of view)

    //planetary observation mode

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
