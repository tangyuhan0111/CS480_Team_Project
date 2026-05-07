#ifndef ENGINE_H
#define ENGINE_H


#include <assert.h>
#include "window.h"
#include "graphics.h"

static void cursorPositionCallBack(GLFWwindow*, double xpos, double ypos);

class Engine
{
  public:
    Engine(const char*  name, int width, int height);
    
    ~Engine();
    bool Initialize();
    void Run();
    void ProcessInput();
    unsigned int getDT();
    long long GetCurrentTimeMillis();
    void Display(GLFWwindow*, double);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
  
  private:
    // Window related variables
    Window *m_window;    
    const char* m_WINDOW_NAME;
    int m_WINDOW_WIDTH;
    int m_WINDOW_HEIGHT;
    bool m_FULLSCREEN;


    Graphics *m_graphics;

    bool m_running;
    bool firstMouse = true;
    float lastX = 0.0f;
    float lastY = 0.0f;
    float m_lastFrame = 0.0f;
};

#endif // ENGINE_H
