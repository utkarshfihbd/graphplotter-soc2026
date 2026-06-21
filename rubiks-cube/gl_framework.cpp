#include "gl_framework.hpp"

extern float camYaw;
extern float camPitch;
extern float camRadius;
void rotateFace(int axis, int slice, int dir);

namespace csX75
{
  void initGL(void)
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
  }

  //!GLFW Error Callback
  void error_callback(int error, const char* description)
  {
    std::cerr<<description<<std::endl;
  }

  //!GLFW framebuffer resize callback
  void framebuffer_size_callback(GLFWwindow* window, int width, int height)
  {
    glViewport(0, 0, width, height);
  }

  //!GLFW keyboard callback
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);

    // camera orbit
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
      camYaw -= 5.0f;
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
      camYaw += 5.0f;
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
      camPitch += 5.0f;
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
      camPitch -= 5.0f;
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
      camRadius -= 0.5f;
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
      camRadius += 0.5f;

    if (camPitch >  89.0f) camPitch =  89.0f;
    if (camPitch < -89.0f) camPitch = -89.0f;
    if (camRadius <  1.0f) camRadius =  1.0f;

    // U = uppper layer, D = down, R = right, L = left, F = front, B = back 
    // shift = reverse  
    if (action == GLFW_PRESS) {
      int dir = (mods & GLFW_MOD_SHIFT) ? -1 : 1;
      if (key == GLFW_KEY_U) rotateFace(1,  1,  dir);
      if (key == GLFW_KEY_D) rotateFace(1, -1, -dir);
      if (key == GLFW_KEY_R) rotateFace(0,  1,  dir);
      if (key == GLFW_KEY_L) rotateFace(0, -1, -dir);
      if (key == GLFW_KEY_F) rotateFace(2,  1,  dir);
      if (key == GLFW_KEY_B) rotateFace(2, -1, -dir);
    }
  }
};