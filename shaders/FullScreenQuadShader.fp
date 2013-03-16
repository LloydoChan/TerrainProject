#version 330

uniform sampler2D texture;

in vec2 texCoord;
out vec3 color;

void main()
{
  float xFactor = 1.0/640 * gl_FragCoord.x;
  float yFactor = 1.0/480 * gl_FragCoord.y;
  xFactor *= yFactor;
  color = vec3(xFactor,xFactor,xFactor);
}