//TextShader.fp
#version 330

in vec2 texCoords;
uniform sampler2D texture;
uniform vec3 inColor;


out vec4 color;

void main()
{
  color = texture2D(texture,texCoords);

}
