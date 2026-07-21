#include "surface-plotter.hpp"
#include "parser.hpp"
#include <cmath>
#include <cstddef>
#include <vector>
#include <iostream>

float camYaw = 45.0f;
float camPitch = 30.0f;
float camRadius = 10.0f;

void rotateFace(int axis, int slice, int dir) {
  (void)axis; (void)slice; (void)dir; 
}

const int   GRID_N = 100;
const float PI_F    = 3.14159265358979323846f;
const float RANGE   = 2.0f * PI_F;
const float FD_EPS  = 1e-3f;              

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
GLuint uHeightMin, uHeightMax, uFlatShading;
glm::mat4 view_matrix;

ExprPtr surfaceExpr;
float currentHeightMin = -1.0f;
float currentHeightMax =  1.0f;
float simTime = 0.0f;
bool  paused = false;
float speedMultiplier = 1.0f;

enum class RenderMode { Wireframe, Flat, Phong };
RenderMode renderMode = RenderMode::Phong;

float sampleHeight(float x, float z, float t) {
  return (float)surfaceExpr->eval((double)x, (double)z, (double)t);
}

glm::vec3 sampleNormal(float x, float z, float t) {
  float dhdx = (sampleHeight(x + FD_EPS, z, t) - sampleHeight(x - FD_EPS, z, t)) / (2.0f * FD_EPS);
  float dhdz = (sampleHeight(x, z + FD_EPS, t) - sampleHeight(x, z - FD_EPS, t)) / (2.0f * FD_EPS);
  return glm::normalize(glm::vec3(-dhdx, 1.0f, -dhdz));
}
void regenerateMesh(float t) {
  float minY =  1e9f;
  float maxY = -1e9f;

  for (int i = 0; i < GRID_N; i++) {
    for (int j = 0; j < GRID_N; j++) {
      float x = -RANGE + (2.0f * RANGE) * (float)i / (float)(GRID_N - 1);
      float z = -RANGE + (2.0f * RANGE) * (float)j / (float)(GRID_N - 1);
      float y = sampleHeight(x, z, t);

      Vertex& v = vertices[i * GRID_N + j];
      v.pos = glm::vec3(x, y, z);
      v.normal = sampleNormal(x, z, t);

      minY = std::min(minY, y);
      maxY = std::max(maxY, y);
    }
  }

  if (maxY - minY < 1e-4f) { minY -= 0.5f; maxY += 0.5f; }
  currentHeightMin = minY;
  currentHeightMax = maxY;

  glBindBuffer(GL_ARRAY_BUFFER, surfVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
}

void buildSurfaceTopology() {
  vertices.resize(GRID_N * GRID_N);
  indices.clear();

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
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

  glGenBuffers(1, &surfEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surfEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

  glBindVertexArray(0);
}

void handleInput(GLFWwindow* window, float dt) {
  static bool prevSpace = false, prevM = false;

  bool space = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
  if (space && !prevSpace) paused = !paused;
  prevSpace = space;

  bool mKey = glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;
  if (mKey && !prevM) {
    switch (renderMode) {
      case RenderMode::Wireframe: renderMode = RenderMode::Flat;      break;
      case RenderMode::Flat:      renderMode = RenderMode::Phong;     break;
      case RenderMode::Phong:     renderMode = RenderMode::Wireframe; break;
    }
  }
  prevM = mKey;
  if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) speedMultiplier += 1.0f * dt;
  if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) speedMultiplier -= 1.0f * dt;
  if (speedMultiplier < 0.1f) speedMultiplier = 0.1f;
  if (speedMultiplier > 5.0f) speedMultiplier = 5.0f;
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
  uFlatShading  = glGetUniformLocation(shaderProgram, "flatShading");
  surfaceExpr = loadExpressionFromFile("equation.txt");

  buildSurfaceTopology();
  regenerateMesh(simTime);

  std::cout << "=== Time-Varying Surface Plotter ===" << std::endl;
  std::cout << std::endl;
  std::cout << "  Equation loaded from equation.txt" << std::endl;
  std::cout << std::endl;
  std::cout << "  Camera controls:" << std::endl;
  std::cout << "    Arrow keys  -- camera orbiting" << std::endl;
  std::cout << "    W / S       -- zoom in / out" << std::endl;
  std::cout << std::endl;
  std::cout << "  Animation controls:" << std::endl;
  std::cout << "    SPACE       -- play / pause" << std::endl;
  std::cout << "    =           -- speed up" << std::endl;
  std::cout << "    -           -- slow down" << std::endl;
  std::cout << "    M           -- cycle wireframe / flat / phong" << std::endl;
  std::cout << std::endl;
  std::cout << "  ESC           -- quit" << std::endl;
  std::cout << std::endl;
}

void renderGL(void) {
  glPolygonMode(GL_FRONT_AND_BACK, renderMode == RenderMode::Wireframe ? GL_LINE : GL_FILL);
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

  glUniform3fv(uLightPos, 1, glm::value_ptr(camPos));
  glUniform3fv(uViewPos, 1, glm::value_ptr(camPos));
  glUniform3f(uLightColor, 1.0f, 1.0f, 1.0f);

  glUniform1f(uHeightMin, currentHeightMin);
  glUniform1f(uHeightMax, currentHeightMax);
  glUniform1i(uFlatShading, renderMode == RenderMode::Flat ? 1 : 0);

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
  window = glfwCreateWindow(700, 700, "Time-Varying Surface Plotter", NULL, NULL);
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

  try {
    initBuffersGL();
  } catch (const std::exception& e) {
    std::cerr << "Failed to initialize: " << e.what() << std::endl;
    glfwTerminate();
    return -1;
  }

  double lastFrameTime = glfwGetTime();

  while (glfwWindowShouldClose(window) == 0)
    {
      double now = glfwGetTime();
      float dt = (float)(now - lastFrameTime);
      lastFrameTime = now;

      handleInput(window, dt);
      if (!paused) simTime += dt * speedMultiplier;
      regenerateMesh(simTime);

      renderGL();
      glfwSwapBuffers(window);
      glfwPollEvents();
    }

  glfwTerminate();
  return 0;
}