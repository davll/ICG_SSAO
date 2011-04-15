//#version 120

varying vec3 vVSNormal;
varying vec4 vFSCoord;
varying vec3 vDiffuseColor;
varying float vShininess;

uniform sampler2D aoMap;

float readAO(){
  return texture2D(aoMap, (vFSCoord.xy/vFSCoord.w + vec2(1.0))*0.5).x;
}

void main(){
  float aofactor = readAO();
  vec3 col = vDiffuseColor * 0.5;
  vec3 ambientColor = aofactor * col;
  vec3 color = col * max(dot((vVSNormal), normalize(vec3(1.0, 1.0, 1.0))),0.0);
  
  gl_FragData[0] = vec4(ambientColor + color, 0.0);
}
