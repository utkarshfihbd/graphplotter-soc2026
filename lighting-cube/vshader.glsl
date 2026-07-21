#version 330

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec3 vNormal;

out vec4 color;
out vec3 fragPos;
out vec3 fragNormal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;
uniform mat3 NormalMatrix;

void main()
{
  vec4 worldPos = Model * vPosition;
  gl_Position = Proj * View * worldPos;
  fragPos = worldPos.xyz;
  fragNormal = normalize(NormalMatrix * vNormal);
  color = vColor;
}