varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform int horizontal;
uniform float weight[5];
uniform vec2 tex_size;

void main()
{             
    vec2 tex_offset = 1.0 / tex_size;
    vec3 result = texture2D(CC_Texture0, v_texCoord).rgb * weight[0];

    if(horizontal > 0)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture2D(CC_Texture0, v_texCoord + vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
            result += texture2D(CC_Texture0, v_texCoord - vec2(tex_offset.x * float(i), 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture2D(CC_Texture0, v_texCoord + vec2(0.0, tex_offset.y * float(i))).rgb * weight[i];
            result += texture2D(CC_Texture0, v_texCoord - vec2(0.0, tex_offset.y * float(i))).rgb * weight[i];
        }
    }
    
    gl_FragColor = vec4(result, 1.0);
}