//#version 120

varying vec4 vFSCoord;
varying vec3 vVSNormal;
//varying float vLinearZ;

void main(){
  
  vec3 norm = normalize(vVSNormal);
  norm = norm * 0.5 + vec3(0.5);
  
  
  gl_FragData[0] = vec4(norm, 0.0);
}
