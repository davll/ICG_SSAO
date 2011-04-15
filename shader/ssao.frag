//#version 120

varying vec2 vTexCoord;

uniform sampler2D depthMap;
uniform sampler2D normalMap;
uniform sampler2D randomTex;
uniform vec2 pixelSize;
uniform vec2 cameraRange;
#define NEARZ cameraRange.x
#define FARZ cameraRange.y

uniform vec4 aoParameters;
uniform float aoWeight;
uniform float noiseInputWeight;

float aoCap = aoParameters.x;
float aoMultiplier = aoParameters.y;
float depthTolerance = aoParameters.z;
float aorange = aoParameters.w;

float depthToLinear2(float dval){
  return (2.0 * NEARZ ) / ( -(NEARZ+FARZ)+((dval))*(FARZ-NEARZ) );
}
float depthToLinear(float dval){
  return (2.0 * NEARZ * FARZ) / ( -(NEARZ+FARZ)+((dval)*2.0 - 1.0)*(FARZ-NEARZ) );
}
float linearToNormalized(float zval){
  return (-zval-NEARZ)/(FARZ-NEARZ);
}

vec4 sampleOnePixel(vec2 off){
  vec2 pos = vTexCoord + off;
  float depth = texture2D(depthMap, pos).x;
  return vec4(texture2D(normalMap, pos).xyz * 2.0 - vec3(1.0), depth);
}

float compareDepths(float depth1, float depth2){
  depth1 = depthToLinear2(depth1);
  depth2 = depthToLinear2(depth2);
  // units in space the AO effect extends to (this gets divided by the camera far range
  float diff = sqrt(clamp(1.0-(depth1-depth2) / (aorange/(FARZ-NEARZ)),0.0,1.0));
  float ao = min(aoCap,max(0.0,depth1-depth2-depthTolerance) * aoMultiplier) * diff;
  return ao;
}

void main(){
  vec4 centerData = sampleOnePixel(vec2(0.0));
  vec2 randomVec = normalize(texture2D(randomTex, 2.0 * vTexCoord * (1.0-centerData.w) / pixelSize).xy * 2.0 - vec2(1.0));
  float diskRadius = 4.0 * (1.5-centerData.w);
  //float depthRange = 0.5 * (1.0-centerData.w);
  //float diskRadius = 4.0;
  
  if(length(centerData.xyz) == 0.0)
    discard;
  
  vec4 sampleVec[2], sampleData[4];
  vec2 SAMPLE_VECTOR = (diskRadius * pixelSize * mix(normalize(vec2(1.0,1.0)), randomVec.xy, noiseInputWeight));
  //vec2 SAMPLE_VECTOR = (diskRadius * pixelSize);
  sampleVec[0] = vec4( SAMPLE_VECTOR*vec2(1.0, 1.0), SAMPLE_VECTOR*vec2(-1.0, 1.0) );
  sampleVec[1] = vec4( SAMPLE_VECTOR*vec2(3.0, 2.0), SAMPLE_VECTOR*vec2(-3.0, 2.0) );
  
  float ao = 0.0, sum = 0.0, fade = 1.0;
  for(int k=0;k<3;k++){
    for(int i=0;i<2;i++){
      sampleData[0] = sampleOnePixel(sampleVec[i].xy);
      sampleData[1] = sampleOnePixel(-sampleVec[i].xy);
      sampleData[2] = sampleOnePixel(sampleVec[i].zw);
      sampleData[3] = sampleOnePixel(-sampleVec[i].zw);
      
      float t = 0.0;
      for(int j=0;j<4;j++){
        float angleFactor = 1.0 - dot(centerData.xyz, sampleData[i].xyz);
        t += compareDepths(sampleData[j].w, centerData.w);
      }
      
      ao += t * fade;
      sampleVec[i] *= 2.0;
      sum += fade * 4.0;
    }
    fade *= 0.5;
  }
  ao /= sum;
  gl_FragColor = vec4( 1.0 - ao*aoWeight );
}
