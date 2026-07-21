#version 330

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;

out vec3 fragPos;
out vec3 fragNormal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;
uniform mat3 NormalMatrix;

void main()
{
  vec4 worldPos = Model * vec4(vPosition, 1.0);
  gl_Position = Proj * View * worldPos;
  fragPos = worldPos.xyz;
  fragNormal = normalize(NormalMatrix * vNormal);
}