#ifndef _GL_FRAMEWORK_HPP_
#define _GL_FRAMEWORK_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

extern float camYaw;
extern float camPitch;
extern float camRadius;
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

namespace csX75
{
  void initGL(void);
 
  void error_callback(int error, const char* description);
  void framebuffer_size_callback(GLFWwindow* window, int width, int height);
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif