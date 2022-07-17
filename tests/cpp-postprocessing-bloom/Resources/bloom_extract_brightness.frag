varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform float threshold;

void main()
{
	vec4 color = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));

	if(brightness > threshold)
			gl_FragColor = vec4(color.rgb, 1.0);
	else
			gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
