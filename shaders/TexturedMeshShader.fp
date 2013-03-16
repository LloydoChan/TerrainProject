//TexturedMeshShader.fp
#version 330

in vec2 outTex;
in float checkYVal;
in vec2  lookupCoords;
in float lightingFactor;

uniform sampler2D texture;
uniform sampler2D heightMap;
uniform float texScaleWidth;
uniform float texScaleHeight;
out vec4 color;


void main()
{
  

  color = lightingFactor * texture2D(texture,outTex) + vec4(0.4,0.4,0.4,1.0) * texture2D(texture,outTex);

  if(color.a < 0.4)
    discard;

}