#version 450 core 

layout(location = 0) uniform sampler2D tex;
layout(location = 1) uniform sampler2D tex2;


out vec4 colortex;
in vec2 fragTex;

uniform int activateBlur;
uniform int glowMaskOnly;

void main()
{
	vec3 glowColor = texture(tex, fragTex, 0).rgb;		// glow
	vec3 texColor = texture(tex2, fragTex, 0).rgb;		// color
	const float gamma = 2.2;
	texColor = pow(texColor, vec3(gamma));				// color correcting

	if (glowMaskOnly == 0) {
		
		glowColor.rgb *= 0.00005;
		vec3 blendedColor = texColor + glowColor; // additive blending
		float exposure = 1.0f;

		colortex = vec4(blendedColor, 1.0);

	} else {		
		colortex.rgb = glowColor * 0.00005;
	}

	colortex.a = 1.0;



}
