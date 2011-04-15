varying vec2 vTexCoord;

uniform sampler2D srcMap;
uniform float kernelWeights[5];
uniform vec2 kernelSamples[4];

void main(){
  vec4 color = texture2D(srcMap, vTexCoord) * kernelWeights[0];
  float weights = kernelWeights[0];
  for(int i=0;i<4;i++){
    color += texture2D(srcMap, vTexCoord+kernelSamples[i]) * kernelWeights[i+1];
	color += texture2D(srcMap, vTexCoord-kernelSamples[i]) * kernelWeights[i+1];
	weights += kernelWeights[i+1] * 2.0;
  }
  gl_FragData[0] = color / weights;
}
