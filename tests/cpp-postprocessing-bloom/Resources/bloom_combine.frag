varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform float exposure;

void main()
{
  const float gamma = 2.2;
  vec3 hdrColor = texture2D(CC_Texture0, v_texCoord).rgb;      
  vec3 bloomColor = texture2D(CC_Texture1, v_texCoord).rgb;
  hdrColor += bloomColor;
  
  vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
   
  result = pow(result, vec3(1.0 / gamma));
  gl_FragColor = vec4(result, 1.0);
}
