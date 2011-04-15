#version 120

varying vec2 vTexCoord;

uniform sampler2D depthMap;
uniform sampler2D normalMap;
uniform sampler2D randomTex;
uniform vec2 pixelSize;
uniform vec2 cameraRange;

#define NEARZ cameraRange.x
#define FARZ cameraRange.y
#define PI 3.14159265

#define near NEARZ
#define far FARZ
#define samples 5
#define rings 3
#define texCoord vTexCoord.xy

vec3 rand(vec2 coord){
  //return texture2D(randomTex, coord * gl_FragCoord.xy).xyz;
  float noiseX = (fract(sin(dot(coord ,vec2(12.9898,78.233))) * 43758.5453));
  float noiseY = (fract(sin(dot(coord ,vec2(12.9898,78.233)*2.0)) * 43758.5453));
  return vec3(noiseX,noiseY,0.0)*0.004;
}

float readDepth(vec2 coord){
  return (2.0 * near) / (far + near - texture2D(depthMap, coord).x * (far-near));
}

vec3 readNormal(vec2 pos){
  return texture2D(normalMap, pos).xyz * 2.0 - vec3(1.0);
}

float compareDepths(float depth1, float depth2){
  float aoCap = 1.0;
  float aoMultiplier = 100.0;
  float depthTolerance = 0.0000;
  float aorange = 60.0;
  // units in space the AO effect extends to (this gets divided by the camera far range
  float diff = sqrt(clamp(1.0-(depth1-depth2) / (aorange/(far-near)),0.0,1.0));
  float ao = min(aoCap,max(0.0,depth1-depth2-depthTolerance) * aoMultiplier) * diff;
  return ao;
}

void main(){
  float depth = readDepth(texCoord);
  float d;
  
  float width = 1.0 / pixelSize.x;
  float height = 1.0 / pixelSize.y;
  float aspect = width/height;
  vec2 noise = rand(texCoord).xy;
  
  float w = (1.0 / width)/clamp(depth,0.05,1.0)+(noise.x*(1.0-noise.x));
  float h = (1.0 / height)/clamp(depth,0.05,1.0)+(noise.y*(1.0-noise.y));
  float pw, ph, ao, s;
  float fade = 1.0;
  
  for(int i=0;i<rings;i++){
    fade *= 0.5;
	for(int j=0;j<samples*i;j++){
	  float step = PI*2.0 / (samples*float(i));
	  pw = (cos(float(j)*step)*float(i));
	  ph = (sin(float(j)*step)*float(i))*aspect;
	  d = readDepth( vec2(texCoord.s+pw*w,texCoord.t+ph*h));
	  ao += compareDepths(depth,d)*fade;
	  s += 1.0*fade;
	}
  }
  
  ao /= s;
  ao = 1.0-ao;
  
  gl_FragColor = vec4(ao);
}
