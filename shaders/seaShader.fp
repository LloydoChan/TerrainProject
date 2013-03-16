//seashader.fp
#version 330


uniform int winWidth;
uniform int winHeight;

uniform float noiseTile;
uniform float noiseFactor;
uniform float time;
uniform float waterShininess;
uniform mat4 viewMatrix;

uniform sampler2D texWaterReflection;
uniform sampler2D noiseTexture;

out vec4 col;

in vec3 pos;
in vec3 pixToEye;
in vec3 pixToLight;
in vec2 normalizedTex;
in vec4 transformedTex;


void main ()
{
	vec2 uvNormal0 = normalizedTex * noiseTile;
	//uvNormal0.s += time*0.01;
	//uvNormal0.t += time*0.01;

	vec2 uvNormal1 = normalizedTex * noiseTile;
	//uvNormal1.s -= time*0.01;
	//uvNormal1.t += time*0.01;

	vec3 normal0 = texture2D(noiseTexture, uvNormal0).rgb * 2.0 - 1.0;
	vec3 normal1 = texture2D(noiseTexture, uvNormal1).rgb * 2.0 - 1.0;
	vec3 normal = normalize(normal0+normal1);

	vec2 uvReflection = vec2(gl_FragCoord.x/winWidth, gl_FragCoord.y/winHeight);
	
	//vec2 uvFinal = uvReflection + noiseFactor*normal.xy;
	col = texture2D(texWaterReflection, uvReflection);

	vec3 N = normalize(vec3(viewMatrix * vec4(normal.x, normal.z, normal.y, 0.0)));
	vec3 L = normalize(pixToLight);
	vec3 V = normalize(pixToEye);
	float iSpecular = pow(clamp(dot(reflect(-L, N), V), 0.0, 1.0), waterShininess);

	col += iSpecular;

	col.a = 0.7;

}




