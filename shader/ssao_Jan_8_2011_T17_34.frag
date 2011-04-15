#version 120

varying vec2 vTexCoord;

uniform sampler2D depthMap;
uniform sampler2D normalMap;
uniform sampler2D randomTex;
uniform vec2 pixelSize;
uniform vec2 cameraRange;

#define NEARZ cameraRange.x
#define FARZ cameraRange.y

uniform float totStrength = 1.38;
uniform float strength = 0.07;
uniform float offset = 18.0;
uniform float falloff = 0.000002;
uniform float rad = 0.006;

float readLinearZ(vec2 pos){
  float lz = (texture2D(depthMap, pos).x - 0.5) * 2.0;
  lz = (2.0 * NEARZ * FARZ) / (NEARZ + FARZ + lz * (NEARZ-FARZ));
  return (lz-NEARZ) / (FARZ - NEARZ);
}

vec3 readNormal(vec2 pos){
  return texture2D(normalMap, pos).xyz * 2.0 - vec3(1.0);
}

#define SAMPLES 16
const float invSamples = 1.0/16.0;

#define S1  vec3(0.53812504, 0.18565957, -0.43192)
#define S2  vec3(0.13790712, 0.24864247, 0.44301823)
#define S3  vec3(0.33715037, 0.56794053, -0.005789503)
#define S4  vec3(-0.6999805, -0.04511441, -0.0019965635)
#define S5  vec3(0.06896307, -0.15983082, -0.85477847)
#define S6  vec3(0.056099437, 0.006954967, -0.1843352)
#define S7  vec3(-0.014653638, 0.14027752, 0.0762037)
#define S8  vec3(0.010019933, -0.1924225, -0.034443386)
#define S9  vec3(-0.35775623, -0.5301969, -0.43581226)
#define S10 vec3(-0.3169221, 0.106360726, 0.015860917)
#define S11 vec3(0.010350345, -0.58698344, 0.0046293875)
#define S12 vec3(-0.08972908, -0.49408212, 0.3287904)
#define S13 vec3(0.7119986, -0.0154690035, -0.09183723)
#define S14 vec3(-0.053382345, 0.059675813, -0.5411899)
#define S15 vec3(0.035267662, -0.063188605, 0.54602677)
#define S16 vec3(-0.47761092, 0.2847911, -0.0271716)

void main(){
  vec3 pSphere[16]; // Due to bugs on GLSL of Mac, I cannot allocate array with initial values
  pSphere[0] = S1;
  pSphere[1] = S2;
  pSphere[2] = S3;
  pSphere[3] = S4;
  pSphere[4] = S5;
  pSphere[5] = S6;
  pSphere[6] = S7;
  pSphere[7] = S8;
  pSphere[8] = S9;
  pSphere[9] = S10;
  pSphere[10] = S11;
  pSphere[11] = S12;
  pSphere[12] = S13;
  pSphere[13] = S14;
  pSphere[14] = S15;
  pSphere[15] = S16;
  
  vec3 fres = normalize((texture2D(randomTex,vTexCoord*offset).xyz*2.0) - vec3(1.0));
  
  float currentPixelDepth = readLinearZ(vTexCoord);
  vec3 ep = vec3(vTexCoord.xy,currentPixelDepth);
  vec3 norm = readNormal(vTexCoord);
  
  float bl = 0.0;
  // adjust for the depth ( not shure if this is good..)
  float radD = rad/currentPixelDepth;
  
  vec3 ray, se;
  float occluderDepth, depthDifference, normDiff;
  
  for(int i=0; i<SAMPLES;++i){
	// get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
	ray = radD*reflect(pSphere[i],fres);
	// if the ray is outside the hemisphere then change direction
	se = ep + sign(dot(ray,norm) )*ray;
	
	vec3 occNorm = readNormal(se.xy);
	float occZ = readLinearZ(se.xy);
	
	// if depthDifference is negative = occluder is behind current fragment
	depthDifference = currentPixelDepth-occZ;
	// calculate the difference between the normals as a weight
	normDiff = (1.0-dot(occNorm,norm));
	// the falloff equation, starts at falloff and is kind of 1/x^2 falling
	bl += step(falloff,depthDifference)*normDiff*(1.0-smoothstep(falloff,strength,depthDifference));
  }
  
  float ao = 1.0-totStrength*bl*invSamples;
  gl_FragColor = vec4(ao);
}
