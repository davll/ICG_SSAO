//#version 120

varying vec2 vTexCoord;

uniform sampler2D depthMap;
uniform sampler2D normalMap;
uniform vec2 pixelSize;
uniform vec2 cameraRange;

#define S1 vec2(1.0,0.0)
#define S2 vec2(-1.0,0.0)
#define S3 vec2(0.0,1.0)
#define S4 vec2(0.0,-1.0)
#define S5 vec2(-0.707107,-0.707107)
#define S6 vec2(-0.707107,0.707107)
#define S7 vec2(0.707107,-0.707107)
#define S8 vec2(0.707107,0.707107)

#define NEARZ cameraRange.x
#define FARZ cameraRange.y
#define Z_RANGE 0.01

float readDepth(vec2 pos){
  //return (texture2D(depthMap, pos).x);
  return (texture2D(depthMap, pos).x - 0.5) * 2.0;
}

float depth2z(float dval){
  return (2.0 * NEARZ * FARZ) / (NEARZ + FARZ + dval * (NEARZ-FARZ));
}

vec3 readNormal(vec2 pos){
  return texture2D(normalMap, pos).xyz * 2.0 - vec3(1.0);
}

vec2 sampleDepth(vec2 offset, vec2 zlimit){
  float zval = depth2z(readDepth(offset + vTexCoord));
  float valid = 1.0;
  return vec2((zlimit.y - zval), (zlimit.y-zlimit.x)) * valid;
}

float sampleVolume(vec2 zlimit, float sample_size){
  vec2 vol = vec2(0.0);
  vec2 po = sample_size * pixelSize;
  vec2 off;
  
  off = po * S1;
  for(int i=0;i<3;i++, off*=2.0) vol += sampleDepth(off, zlimit);
  off = po * S2;
  for(int i=0;i<3;i++, off*=2.0) vol += sampleDepth(off, zlimit);
  off = po * S3;
  for(int i=0;i<3;i++, off*=2.0) vol += sampleDepth(off, zlimit);
  off = po * S4;
  for(int i=0;i<3;i++, off*=2.0) vol += sampleDepth(off, zlimit);
  off = po * S5;
  for(int i=0;i<3;i++, off*=2.0) vol += sampleDepth(off, zlimit);
  off = po * S6;
  for(int i=0;i<3;i++, off*=2.0) vol += sampleDepth(off, zlimit);
  off = po * S7;
  for(int i=0;i<3;i++, off*=2.0) vol += sampleDepth(off, zlimit);
  off = po * S8;
  for(int i=0;i<3;i++, off*=2.0) vol += sampleDepth(off, zlimit);
  
  return vol.x / vol.y;
}

void main(){
  float cdepth = readDepth(vTexCoord);
  vec3 cnormal = readNormal(vTexCoord);
  
  float sample_size =  2.0 * (1.0 - (cdepth + 1.0) * 0.5);
  if(sample_size < 0.01)
    discard;
  //float sample_size = 2.0;
  
  float czval = depth2z(cdepth);
  vec2 zlimit = clamp(vec2(czval - Z_RANGE, czval + Z_RANGE), vec2(NEARZ), vec2(FARZ));
  
  float occlusion = sampleVolume(zlimit, sample_size);
  float aofactor = 1.5 - occlusion;
  gl_FragColor = vec4( aofactor );
}
