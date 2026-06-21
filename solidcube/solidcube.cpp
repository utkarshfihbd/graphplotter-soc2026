#include "solidcube.hpp"
#include<cmath>
float points[] = {
  //face 1 triangle 1
  -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  //face 1 triangle 2
  0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  //face 2 triangle 1
  -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
  -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
  0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
  //face 2 triangle 2
  0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
  0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
  //face 3 triangle 1
  -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
  -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
  0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
  //face 3 triangle 2
  0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
  0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
  -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
  //face 4 triangle 1
  -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  //face 4 triangle 2
  0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  //face 5 triangle 1
  0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
  0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
  0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
  //face 5 triangle 2
  0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
  0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
  0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
  //face 6 triangle 1
  -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  //face 6 triangle 2
  -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  
};

float camYaw = 45.0f;  
float camPitch = 30.0f;  
float camRadius = 5.0f; 
GLuint shaderProgram, vbo, vao, uModelViewProjectMatrix;
glm::mat4 view_matrix;
glm::mat4 ortho_matrix;
glm::mat4 modelviewproject_matrix;

void initBuffersGL(void) {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1,&vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
  std::string vshader("vshader.glsl");
  std::string fshader("fshader.glsl");
  std::vector<GLuint> shaderList;
  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vshader));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fshader));
  shaderProgram = csX75::CreateProgramGL(shaderList);
  glUseProgram(shaderProgram);
  uModelViewProjectMatrix = glGetUniformLocation(shaderProgram, "ModelViewProjectMatrix");
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8*sizeof(float), NULL);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(4*(sizeof(float))));

}

 void renderGL(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  float camX = camRadius * cos(glm::radians(camPitch)) * sin(glm::radians(camYaw));
  float camY = camRadius * sin(glm::radians(camPitch));
  float camZ = camRadius * cos(glm::radians(camPitch)) * cos(glm::radians(camYaw));
  view_matrix = glm::lookAt(
    glm::vec3(camX, camY, camZ),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
  );
   glm::mat4 proj_matrix = glm::perspective(
        glm::radians(45.0f),
        1.0f,  
        0.1f,
        100.0f
    );
  modelviewproject_matrix = proj_matrix * view_matrix;
  glUniformMatrix4fv(uModelViewProjectMatrix, 1, GL_FALSE, glm::value_ptr(modelviewproject_matrix));
  glDrawArrays(GL_TRIANGLES, 0, 36);
 }

 int main(int argc, char** argv)
{
  GLFWwindow* window;
  glfwSetErrorCallback(csX75::error_callback);
  if (!glfwInit())
    return -1;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
  window = glfwCreateWindow(512, 512, "SolidCube", NULL, NULL);
  if (!window)
    {
      glfwTerminate();
      return -1;
    }
  
  glfwMakeContextCurrent(window);
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err)
    {
      std::cerr<<"GLEW Init Failed : %s"<<std::endl;
    }

  glfwSetKeyCallback(window, csX75::key_callback);
  glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  csX75::initGL();
  initBuffersGL();

  while (glfwWindowShouldClose(window) == 0)
    {
      renderGL();
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  
  glfwTerminate();
  return 0;
}