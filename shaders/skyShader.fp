#version 330
in vec3 ray;

uniform samplerCube CubeMapTex;

out vec4 color;

void main()
{
  color = texture(CubeMapTex,ray);
  color.a = 0.0f;
}