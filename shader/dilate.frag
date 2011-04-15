varying vec2 vTexCoord;

uniform sampler2D srcMap;
uniform vec2 samples[4];

void main(){
  vec4 color = texture2D(srcMap, vTexCoord);
  for(int i=0;i<4;i++){
    color = min(color, texture2D(srcMap, vTexCoord+samples[i]));
	color = min(color, texture2D(srcMap, vTexCoord-samples[i]));
  }
  gl_FragData[0] = color;
}

