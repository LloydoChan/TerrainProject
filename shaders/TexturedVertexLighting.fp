//TexturedVertexLighting.fp
#version 330

in vec2 outTex;
in float lightingFactor;
uniform sampler2D texture;
uniform vec3 lightingColor;

out vec3 color;


void main()
{
  vec3 ambient = vec3(0.3,0.3,0.3);
  color =  lightingFactor * lightingColor * texture2D(texture,outTex).xyz + ambient * texture2D(texture,outTex).xyz;

}