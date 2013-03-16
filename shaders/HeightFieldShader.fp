//HeightFieldShader.fp
#version 330

in vec2 outTex;
in vec3 normal;
in vec3 position;
in float height;


uniform vec3 lightingVector;
uniform vec4 sunRaysColor;
uniform float sunCosY;
uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D sandTexture;

uniform sampler2DShadow depthMap;


out vec4 color;




void main()
{
  vec3 lighting = normalize(lightingVector);
  float lightingFactor = max(dot(normal,-lighting),0.0);
  if(lightingFactor > 1.0f) lightingFactor = 1.0f;


  float slope = normal.y;

  if(height < 10.0f)
    discard;

  vec4 baseColor;

  vec4 ambient = vec4(0.55,0.55,0.55,1.0);

  if(height > 64.0f)
  {
    if(slope > 0.6)
      baseColor = mix(texture2D(rockTexture,outTex),texture2D(grassTexture,outTex),slope);
    else
       baseColor = texture2D(rockTexture,outTex);

  }else{
     if(slope < 0.7)
       baseColor = texture2D(rockTexture,outTex);
	 else
       baseColor =  mix(texture2D(sandTexture,outTex),texture2D(grassTexture,outTex),height/64.0);
	 
  }

 
   color =  clamp(lightingFactor * baseColor,0.0,1.0) + clamp(ambient * baseColor,0.0,1.0);
  
 
}
