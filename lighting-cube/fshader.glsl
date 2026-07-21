#version 330

in vec4 color;
in vec3 fragPos;
in vec3 fragNormal;
out vec4 frag_color;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main()
{
  vec3 N = normalize(fragNormal);
  vec3 L = normalize(lightPos - fragPos);
  vec3 V = normalize(viewPos - fragPos);
  vec3 R = reflect(-L, N);
  float ambientStrength = 0.35;
  vec3 ambient = ambientStrength * lightColor;
  float diff = max(dot(N, L), 0.15);
  vec3 diffuse = diff * lightColor;
  float spec = pow(max(dot(R, V), 0.0), 32.0);
  vec3 specular = 0.5 * spec * lightColor;
  vec3 result = (ambient + diffuse + specular) * color.rgb;
  frag_color = vec4(result, color.a);
}