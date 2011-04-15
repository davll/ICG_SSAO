//#version 120

attribute vec4 inVertex;

varying vec2 vTexCoord;

void main(){
  gl_Position = inVertex;
  vTexCoord = (inVertex.xy + vec2(1.0))*0.5;
}
