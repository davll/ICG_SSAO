//#version 120

attribute vec4 inVertex;
attribute vec3 inNormal;
attribute vec2 inTexCoord;
attribute vec4 inColor;

varying vec3 vVSNormal;
varying vec4 vFSCoord;
varying vec2 vTexCoord;
varying vec3 vDiffuseColor;
varying float vShininess;
//varying float vLinearZ;

uniform mat4 projMatrix;
uniform mat4 mvMatrix;

void main(){
  vec4 vpos = projMatrix * mvMatrix * inVertex;
  gl_Position = vpos;
  
  vFSCoord = vpos;
  
  mat3 normMatrix = mat3(mvMatrix[0].xyz, mvMatrix[1].xyz, mvMatrix[2].xyz);
  vVSNormal = normalize(normMatrix * inNormal);
  
  vTexCoord = inTexCoord.xy;
  
  vDiffuseColor = inColor.xyz;
  vShininess = inColor.w;
  
  
  
}
