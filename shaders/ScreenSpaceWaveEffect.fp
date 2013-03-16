#version 330

uniform sampler2D texture;
uniform float time;

in vec2 texCoord;
out vec3 color;

void main()
{
  texCoord.x +=  sin(time)/90.0f;
  color = texture2D(texture,texCoord).xyz;
}