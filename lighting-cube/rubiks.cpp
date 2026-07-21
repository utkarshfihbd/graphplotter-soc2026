#include "rubiks.hpp"
#include <cmath>
#include <vector>

struct Cubie {
  glm::vec3 gridPos;
  glm::mat4 modelMatrix;
  GLuint vao, vbo;
};

std::vector<Cubie> cubies;

float camYaw = 45.0f;
float camPitch = 30.0f;
float camRadius = 8.0f;

GLuint shaderProgram;
GLuint uModel, uView, uProj, uNormalMatrix;
GLuint uLightPos, uViewPos, uLightColor;
glm::mat4 view_matrix;

glm::vec4 getFaceColor(int face, glm::vec3 pos) {
  glm::vec4 black  = {0.05f, 0.05f, 0.05f, 1.0f};
  glm::vec4 yellow = {1.0f,  1.0f,  0.0f,  1.0f};
  glm::vec4 green  = {0.0f,  0.8f,  0.0f,  1.0f};
  glm::vec4 white  = {1.0f,  1.0f,  1.0f,  1.0f};
  glm::vec4 blue   = {0.0f,  0.0f,  1.0f,  1.0f};
  glm::vec4 red    = {1.0f,  0.0f,  0.0f,  1.0f};
  glm::vec4 orange = {1.0f,  0.5f,  0.0f,  1.0f};

  if (face == 0 && pos.y == -1) return yellow;
  if (face == 1 && pos.z ==  1) return green;
  if (face == 2 && pos.y ==  1) return white;
  if (face == 3 && pos.z == -1) return blue;
  if (face == 4 && pos.x ==  1) return red;
  if (face == 5 && pos.x == -1) return orange;
  return black;
}

void buildCubieVAO(Cubie& c) {
  float geom[6][6][3] = {
    {{-0.5f,-0.5f,-0.5f},{-0.5f,-0.5f, 0.5f},{ 0.5f,-0.5f, 0.5f},
     { 0.5f,-0.5f, 0.5f},{ 0.5f,-0.5f,-0.5f},{-0.5f,-0.5f,-0.5f}},
    {{-0.5f,-0.5f, 0.5f},{-0.5f, 0.5f, 0.5f},{ 0.5f, 0.5f, 0.5f},
     { 0.5f, 0.5f, 0.5f},{ 0.5f,-0.5f, 0.5f},{-0.5f,-0.5f, 0.5f}},
    {{-0.5f, 0.5f,-0.5f},{-0.5f, 0.5f, 0.5f},{ 0.5f, 0.5f, 0.5f},
     { 0.5f, 0.5f, 0.5f},{ 0.5f, 0.5f,-0.5f},{-0.5f, 0.5f,-0.5f}},
    {{-0.5f,-0.5f,-0.5f},{-0.5f, 0.5f,-0.5f},{ 0.5f, 0.5f,-0.5f},
     { 0.5f, 0.5f,-0.5f},{ 0.5f,-0.5f,-0.5f},{-0.5f,-0.5f,-0.5f}},
    {{ 0.5f,-0.5f,-0.5f},{ 0.5f,-0.5f, 0.5f},{ 0.5f, 0.5f, 0.5f},
     { 0.5f, 0.5f, 0.5f},{ 0.5f, 0.5f,-0.5f},{ 0.5f,-0.5f,-0.5f}},
    {{-0.5f,-0.5f,-0.5f},{-0.5f,-0.5f, 0.5f},{-0.5f, 0.5f, 0.5f},
     {-0.5f, 0.5f, 0.5f},{-0.5f, 0.5f,-0.5f},{-0.5f,-0.5f,-0.5f}},
  };

  glm::vec3 faceNormal[6] = {
    { 0.0f,-1.0f, 0.0f},
    { 0.0f, 0.0f, 1.0f},
    { 0.0f, 1.0f, 0.0f},
    { 0.0f, 0.0f,-1.0f},
    { 1.0f, 0.0f, 0.0f},
    {-1.0f, 0.0f, 0.0f},
  };

  float vdata[36 * 11];
  int idx = 0;
  for (int face = 0; face < 6; face++) {
    glm::vec4 col = getFaceColor(face, c.gridPos);
    glm::vec3 n = faceNormal[face];
    for (int v = 0; v < 6; v++) {
      vdata[idx++] = geom[face][v][0];
      vdata[idx++] = geom[face][v][1];
      vdata[idx++] = geom[face][v][2];
      vdata[idx++] = 1.0f;
      vdata[idx++] = col.r;
      vdata[idx++] = col.g;
      vdata[idx++] = col.b;
      vdata[idx++] = col.a;
      vdata[idx++] = n.x;
      vdata[idx++] = n.y;
      vdata[idx++] = n.z;
    }
  }

  glGenVertexArrays(1, &c.vao);
  glBindVertexArray(c.vao);
  glGenBuffers(1, &c.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, c.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vdata), vdata, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 11*sizeof(float), NULL);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)(4*sizeof(float)));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float), (void*)(8*sizeof(float)));
  glBindVertexArray(0);
}

void rotateFace(int axis, int slice, int dir) {
  glm::vec3 axisVec(0.0f);
  axisVec[axis] = 1.0f;

  glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f * dir), axisVec);

  for (Cubie& c : cubies) {
    if ((int)glm::round(c.gridPos[axis]) != slice)
      continue;

    c.modelMatrix = rot * c.modelMatrix;
    glm::vec3 newPos = glm::vec3(rot * glm::vec4(c.gridPos, 1.0f));
    c.gridPos = glm::round(newPos);
    if (c.gridPos.x == 0.0f) c.gridPos.x = 0.0f;
    if (c.gridPos.y == 0.0f) c.gridPos.y = 0.0f;
    if (c.gridPos.z == 0.0f) c.gridPos.z = 0.0f;

  }
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

  for (int x = -1; x <= 1; x++)
    for (int y = -1; y <= 1; y++)
      for (int z = -1; z <= 1; z++) {
        Cubie c;
        c.gridPos = glm::vec3(x, y, z);
        c.modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z) * 1.05f);
        buildCubieVAO(c);
        cubies.push_back(c);
      }

  std::cout << "=== Rubik's Cube ===" << std::endl;
  std::cout << std::endl;
  std::cout << "  Camera controls:" << std::endl;
  std::cout << "    Arrow keys  -- camera orbiting" << std::endl;
  std::cout << "    W / S       -- zoom in / out" << std::endl;
  std::cout << std::endl;
  std::cout << "  Face rotations (Shift = reverse):" << std::endl;
  std::cout << "    U           -- top face" << std::endl;
  std::cout << "    D           -- bottom face" << std::endl;
  std::cout << "    R           -- right face" << std::endl;
  std::cout << "    L           -- left face" << std::endl;
  std::cout << "    F           -- front face" << std::endl;
  std::cout << "    B           -- back face" << std::endl;
  std::cout << std::endl;
  std::cout << "  ESC           -- quit" << std::endl;
  std::cout << std::endl;
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
  glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(view_matrix));
  glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(proj_matrix));
  glUniform3fv(uLightPos, 1, glm::value_ptr(camPos));
  glUniform3fv(uViewPos, 1, glm::value_ptr(camPos));
  glUniform3f(uLightColor, 1.0f, 1.0f, 1.0f);

  for (Cubie& c : cubies) {
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(c.modelMatrix)));
    glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(c.modelMatrix));
    glUniformMatrix3fv(uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glBindVertexArray(c.vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
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
  window = glfwCreateWindow(512, 512, " Phong Lighting", NULL, NULL);
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