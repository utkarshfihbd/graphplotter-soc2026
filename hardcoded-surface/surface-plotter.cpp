#include "surface-plotter.hpp"
#include <cmath>
#include <cstddef>
#include <vector>

float camYaw = 45.0f;
float camPitch = 30.0f;
float camRadius = 10.0f;

const int   GRID_N = 100;                 
const float PI_F    = 3.14159265358979323846f;
const float RANGE   = 2.0f * PI_F;        
const float HEIGHT_MIN = -1.0f;           
const float HEIGHT_MAX =  1.0f;

struct Vertex {
  glm::vec3 pos;
  glm::vec3 normal;
};

std::vector<Vertex> vertices;
std::vector<GLuint> indices;
GLuint surfVAO, surfVBO, surfEBO;
GLsizei numIndices;

GLuint shaderProgram;
GLuint uModel, uView, uProj, uNormalMatrix;
GLuint uLightPos, uViewPos, uLightColor;
GLuint uHeightMin, uHeightMax;
glm::mat4 view_matrix;

float heightFunc(float x, float z) {
  return sin(x) * cos(z);
}

glm::vec3 heightNormal(float x, float z) {
  float dhdx =  cos(x) * cos(z);
  float dhdz = -sin(x) * sin(z);
  return glm::normalize(glm::vec3(-dhdx, 1.0f, -dhdz));
}

void buildSurfaceMesh() {
  vertices.clear();
  indices.clear();

  for (int i = 0; i < GRID_N; i++) {
    for (int j = 0; j < GRID_N; j++) {
      float x = -RANGE + (2.0f * RANGE) * (float)i / (float)(GRID_N - 1);
      float z = -RANGE + (2.0f * RANGE) * (float)j / (float)(GRID_N - 1);
      float y = heightFunc(x, z);

      Vertex v;
      v.pos = glm::vec3(x, y, z);
      v.normal = heightNormal(x, z);
      vertices.push_back(v);
    }
  }

  for (int i = 0; i < GRID_N - 1; i++) {
    for (int j = 0; j < GRID_N - 1; j++) {
      GLuint tl = i * GRID_N + j;
      GLuint tr = tl + 1;
      GLuint bl = tl + GRID_N;
      GLuint br = bl + 1;

      indices.push_back(tl);
      indices.push_back(bl);
      indices.push_back(tr);

      indices.push_back(tr);
      indices.push_back(bl);
      indices.push_back(br);
    }
  }
  numIndices = (GLsizei)indices.size();

  glGenVertexArrays(1, &surfVAO);
  glBindVertexArray(surfVAO);

  glGenBuffers(1, &surfVBO);
  glBindBuffer(GL_ARRAY_BUFFER, surfVBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &surfEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surfEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

  glBindVertexArray(0);
}

void initBuffersGL(void) {
  std::string vshader("vshader.glsl");
  std::string fshader("fshader.glsl");
  std::vector<GLuint> shaderList;
  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vshader));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fshader));
  shaderProgram = csX75::CreateProgramGL(shaderList);
  glUseProgram(shaderProgram);

  uModel        = glGetUniformLocation(shaderProgram, "Model");
  uView         = glGetUniformLocation(shaderProgram, "View");
  uProj         = glGetUniformLocation(shaderProgram, "Proj");
  uNormalMatrix = glGetUniformLocation(shaderProgram, "NormalMatrix");
  uLightPos     = glGetUniformLocation(shaderProgram, "lightPos");
  uViewPos      = glGetUniformLocation(shaderProgram, "viewPos");
  uLightColor   = glGetUniformLocation(shaderProgram, "lightColor");
  uHeightMin    = glGetUniformLocation(shaderProgram, "heightMin");
  uHeightMax    = glGetUniformLocation(shaderProgram, "heightMax");

  glUniform1f(uHeightMin, HEIGHT_MIN);
  glUniform1f(uHeightMax, HEIGHT_MAX);

  buildSurfaceMesh();

  std::cout << "=== Surface Plotter: z = sin(x)*cos(y) ===" << std::endl;
  std::cout << std::endl;
  std::cout << "  Camera controls:" << std::endl;
  std::cout << "    Arrow keys  -- camera orbiting" << std::endl;
  std::cout << "    W / S       -- zoom in / out" << std::endl;
  std::cout << std::endl;
  std::cout << "  ESC           -- quit" << std::endl;
  std::cout << std::endl;
}

void rotateFace(int axis, int slice, int dir) {
  (void)axis; (void)slice; (void)dir; 
}

void renderGL(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float camX = camRadius * cos(glm::radians(camPitch)) * sin(glm::radians(camYaw));
  float camY = camRadius * sin(glm::radians(camPitch));
  float camZ = camRadius * cos(glm::radians(camPitch)) * cos(glm::radians(camYaw));
  glm::vec3 camPos(camX, camY, camZ);

  view_matrix = glm::lookAt(
    camPos,
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
  );
  glm::mat4 proj_matrix = glm::perspective(
    glm::radians(45.0f),
    1.0f,
    0.1f,
    100.0f
  );

  glm::mat4 model_matrix = glm::mat4(1.0f); 
  glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model_matrix)));

  glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(model_matrix));
  glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(view_matrix));
  glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj_matrix));
  glUniformMatrix3fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));

  // Headlamp light, same approach as Week 5's cube.
  glUniform3fv(uLightPos, 1, glm::value_ptr(camPos));
  glUniform3fv(uViewPos, 1, glm::value_ptr(camPos));
  glUniform3f(uLightColor, 1.0f, 1.0f, 1.0f);

  glBindVertexArray(surfVAO);
  glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
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
  window = glfwCreateWindow(700, 700, "Surface Plotter", NULL, NULL);
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
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
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