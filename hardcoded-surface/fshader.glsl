#version 330

in vec3 fragPos;
in vec3 fragNormal;
out vec4 frag_color;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float heightMin;
uniform float heightMax;

vec3 heightColor(float t)
{
  vec3 low  = vec3(0.10, 0.20, 0.90);
  vec3 mid  = vec3(0.10, 0.80, 0.20);
  vec3 high = vec3(0.90, 0.15, 0.10);

  if (t < 0.5)
    return mix(low, mid, t * 2.0);
  else
    return mix(mid, high, (t - 0.5) * 2.0);
}

void main()
{
  float t = clamp((fragPos.y - heightMin) / (heightMax - heightMin), 0.0, 1.0);
  vec3 objectColor = heightColor(t);

  vec3 N = normalize(fragNormal);
  vec3 L = normalize(lightPos - fragPos);
  vec3 V = normalize(viewPos - fragPos);
  vec3 R = reflect(-L, N);

  float ambientStrength = 0.35;
  vec3 ambient = ambientStrength * lightColor;

  float diff = max(dot(N, L), 0.15);
  vec3 diffuse = diff * lightColor;

  float spec = pow(max(dot(R, V), 0.0), 32.0);
  vec3 specular = 0.4 * spec * lightColor;

  vec3 result = (ambient + diffuse + specular) * objectColor;
  frag_color = vec4(result, 1.0);
}