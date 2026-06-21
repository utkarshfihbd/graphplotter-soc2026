#include "shader_util.hpp"
#include "gl_framework.hpp"
float points[] = {
	// triangle 1
	0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.25f, 0.434f, 0.0f, 1.0f, 0.0f, 0.0f,
	// triangle 2
	0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.25f, 0.434f, 0.0f, 0.0f, 1.0f, 0.0f,
	-0.25f, 0.434f, 0.0f, 0.0f, 1.0f, 0.0f,
	// triangle 3
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	-0.25f, 0.434f, 0.0f, 0.0f, 0.0f, 1.0f,
	-0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	// triangle 4
	0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	-0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	-0.25f, -0.434f, 0.0f, 0.0f, 1.0f, 1.0f,
	// triangle 5
	0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	-0.25f, -0.434f, 0.0f, 1.0f, 0.0f, 1.0f,
	0.25f, -0.434f, 0.0f, 1.0f, 0.0f, 1.0f,
	// triangle 6
	0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	0.25f, -0.434f, 0.0f, 1.0f, 1.0f, 0.0f,
	0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f
};

GLuint shaderProgram, vbo, vao;
void initShadersGL(void) {
	std::string vertex_shader_file("hexagon_vs.glsl");
	std::string fragment_shader_file("hexagon_fs.glsl");
	std::vector<GLuint> shaderList;
	shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
	shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));
	shaderProgram = csX75::CreateProgramGL(shaderList);

}

void initVertexBufferGL(void) {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 108*sizeof(float), points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*(sizeof(float))));
}

void renderGL(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(shaderProgram);
  glBindVertexArray (vao);
  glDrawArrays(GL_TRIANGLES, 0, 18);
}

int main(int argc, char** argv) {
	  
	GLFWwindow* window;
  glfwSetErrorCallback(csX75::error_callback);
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

  window = glfwCreateWindow(640, 480, "HEXAGON", NULL, NULL);
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
  initShadersGL();
  initVertexBufferGL();

  while (glfwWindowShouldClose(window) == 0)
    {
      renderGL();
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  glfwTerminate();
  return 0;
}