//#version 120

varying vec2 vTexCoord;
uniform sampler2D sourceTex;

void main(){
  gl_FragData[0] = texture2D(sourceTex, vTexCoord.xy).xxxx;
}
