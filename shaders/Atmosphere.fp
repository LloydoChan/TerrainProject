//atmosphere.fp
#version 330

uniform vec2 viewport;
uniform mat4 invProj;
uniform mat3 invViewRot;
uniform vec3 lightDir;

uniform float rayleighBrightness,mieBrightness,spotBrightness,scatterStrength,rayleighStrength,mieStrength;
uniform float rayleighCollectionPower,mieCollectionPower,mieDistribution;

float surfaceHeight = 0.99;
float range = 0.01;
float intensity = 1.8;
const int stepCount = 16;

out vec3 col;

vec3  GetWorldNormal();
float Phase(float alpha,float g);
float AtmosphericDepth(vec3 pos,vec3 dir);
float HorizonExtinction(vec3 pos,vec3 dir,float radius);
vec3  Absorb(float dist,vec3 col,float factor);

vec3 Kr = vec3(0.18867780436772762,0.4978442963618773,0.661606558641713);


void main(){
  vec3 eyeDir = GetWorldNormal();
  float alpha = dot(eyeDir,lightDir);

  float rayleighFactor = Phase(alpha,-0.01) * rayleighBrightness;
  float mieFactor = Phase(alpha,mieDistribution) * mieBrightness;
  float spot = smoothstep(0.0,15.0,Phase(alpha,0.995)) * spotBrightness;

  vec3 eyePos      = vec3(0.0,surfaceHeight,0.0);
  float eyeDepth   = AtmosphericDepth(eyePos,eyeDir);
  float stepLength = eyeDepth/float(stepCount);

  float eyeExtinction = HorizonExtinction(eyePos,eyeDir,surfaceHeight-0.15);

  vec3 rayleighCollected = vec3(0.0,0.0,0.0);
  vec3 mieCollected = vec3(0.0,0.0,0.0);

  for(int step = 0; step < stepCount; step++){
    float sampleDist = stepLength * float(step);
	vec3  pos = eyePos + eyeDir*sampleDist;
	float extinct = HorizonExtinction(pos,lightDir,surfaceHeight-0.35);
	float sampleDepth = AtmosphericDepth(pos,lightDir);
	vec3 inFlux = Absorb(sampleDepth,vec3(intensity),scatterStrength) * extinct;

    rayleighCollected += Absorb(sampleDist,Kr*inFlux,rayleighStrength);
    mieCollected += Absorb(sampleDist,inFlux,mieStrength);
  }

 

  rayleighCollected = (rayleighCollected * eyeExtinction * pow(eyeDepth,rayleighCollectionPower))/float(stepCount);
  mieCollected = (mieCollected * eyeExtinction * pow(eyeDepth,mieCollectionPower))/float(stepCount);

  col = vec3(spot * mieCollected + mieFactor * mieCollected + rayleighFactor * rayleighCollected);
}


vec3 GetWorldNormal(){
  vec2 fragCoord = gl_FragCoord.xy/viewport;
  fragCoord = (fragCoord-0.5) * 2.0;

  vec4 deviceNormal = vec4(fragCoord,0.0,1.0);
  vec3 eyeNormal    = normalize((invProj * deviceNormal).xyz);
  vec3 worldNormal  = normalize(invViewRot * eyeNormal);
  return worldNormal;
}

float Phase(float alpha, float g){
  float a = 3.0 * (1.0 - g*g);
  float b = 2.0 * (2.0 + g*g);
  float c = 1.0 + alpha * alpha;
  float d = pow(1.0+g*g-2.0*g*alpha,1.5);
  return (a/b) * (c/d);
}

float AtmosphericDepth(vec3 pos,vec3 dir){
  float a = dot(dir,dir);
  float b = 2.0*dot(dir,pos);
  float c = dot(pos,pos) - 1.0;
  float det = b*b -4.0*a*c;
  float detSqrt = sqrt(det);
  float q = (-b - detSqrt)/2.0;
  float t1 = c/q;
  return t1;
}

float HorizonExtinction(vec3 pos,vec3 dir,float radius){
  float u = dot(dir,-pos);

  if(u<0.0){
    return 1.0;
  }

  vec3 near = pos + u*dir;

  if(length(near) < radius){
	return 0.0;
  }else{
    vec3 v2 = normalize(near) * radius- pos;
	float diff = acos(dot(normalize(v2),dir));
	return smoothstep(0.0,1.0,pow(diff*2.0,3.0));
  }


}

vec3  Absorb(float dist,vec3 col,float factor){
  return col-col*pow(Kr,vec3(factor/dist));
}
