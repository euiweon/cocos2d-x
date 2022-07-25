/**
 * Depth-of-field shader with bokeh
 * ported from GLSL shader by Martins Upitis
 * http://artmartinsh.blogspot.com/2010/02/glsl-lens-blur-filter-with-bokeh.html
 */

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform float maxblur;	// max blur amount
uniform float aperture; // aperture - bigger values for shallower depth of field

uniform float nearClip;
uniform float farClip;

uniform float focus;
uniform float aspect;

void main()
{
    vec2 aspectcorrect = vec2(1.0, aspect);

    float depth = texture2D(CC_Texture1, v_texCoord).x;
    
    // Convert perspective depth to ViewZ
    float viewZ = (nearClip * farClip) / ((farClip - nearClip) * depth - farClip);

    float factor = focus + viewZ; // viewZ is <= 0, so this is a difference equation

    vec2 dofblur = vec2(clamp(factor * aperture, -maxblur, maxblur));
    
    vec2 dofblur9 = dofblur * 0.9;
    vec2 dofblur7 = dofblur * 0.7;
    vec2 dofblur4 = dofblur * 0.4;

    vec4 col = vec4(0.0);

    col += texture2D(CC_Texture0, v_texCoord.xy);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.0, 0.4)     * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.15, 0.37)   * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.29, 0.29)   * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.37, 0.15)  * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.40, 0.0)    * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.37, -0.15)  * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.29, -0.29)  * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.15, -0.37) * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.0, -0.4)    * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.15, 0.37)  * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.29, 0.29)  * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.37, 0.15)   * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.4, 0.0)    * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.37, -0.15) * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.29, -0.29) * aspectcorrect) * dofblur);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.15, -0.37)  * aspectcorrect) * dofblur);

    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.15, 0.37)   * aspectcorrect) * dofblur9);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.37, 0.15)  * aspectcorrect) * dofblur9);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.37, -0.15)  * aspectcorrect) * dofblur9);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.15, -0.37) * aspectcorrect) * dofblur9);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.15, 0.37)  * aspectcorrect) * dofblur9);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.37, 0.15)   * aspectcorrect) * dofblur9);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.37, -0.15) * aspectcorrect) * dofblur9);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.15, -0.37)  * aspectcorrect) * dofblur9);

    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.29, 0.29)   * aspectcorrect) * dofblur7);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.40, 0.0)    * aspectcorrect) * dofblur7);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.29, -0.29)  * aspectcorrect) * dofblur7);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.0, -0.4)    * aspectcorrect) * dofblur7);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.29, 0.29)  * aspectcorrect) * dofblur7);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.4, 0.0)    * aspectcorrect) * dofblur7);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.29, -0.29) * aspectcorrect) * dofblur7);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.0, 0.4)     * aspectcorrect) * dofblur7);

    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.29, 0.29)   * aspectcorrect) * dofblur4);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.4, 0.0)     * aspectcorrect) * dofblur4);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.29, -0.29)  * aspectcorrect) * dofblur4);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.0, -0.4)    * aspectcorrect) * dofblur4);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.29, 0.29)  * aspectcorrect) * dofblur4);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.4, 0.0)    * aspectcorrect) * dofblur4);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(-0.29, -0.29) * aspectcorrect) * dofblur4);
    col += texture2D(CC_Texture0, v_texCoord.xy + (vec2(0.0, 0.4)     * aspectcorrect) * dofblur4);

    gl_FragColor = col / 41.0;
    gl_FragColor.a = 1.0;
}
