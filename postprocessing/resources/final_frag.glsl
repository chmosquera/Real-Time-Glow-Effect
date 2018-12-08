#version 450 core 

layout(location = 0) uniform sampler2D tex;
layout(location = 1) uniform sampler2D tex2;


out vec4 colortex;
in vec2 fragTex;

uniform int activateBlur;

void main()
{
	const float gamma = 2.2;
	vec3 glowColor = texture(tex, fragTex, 0).rgb;		// alpha
	vec3 texColor = texture(tex2, fragTex, 0).rgb;
	texColor = pow(texColor, vec3(gamma));

	//glowColor += texColor;
	//glowColor/2.0;
	//colortex.rgb = glowColor;

	if (activateBlur == 1) {
		vec3 combinedColor = vec3(1.0);

		// *********** linear blending ****************
		float t = 0.00005;
		combinedColor = (glowColor * t) + (texColor * (1.0-t));


		// ********** another blending ***************
		glowColor.rgb *= 0.00005;
		combinedColor = texColor + glowColor; // additive blending
		float exposure = 1.0f;

		// tone mapping
		//combinedColor = vec3(1.0) - exp(-combinedColor * exposure);

		colortex = vec4(combinedColor, 1.0);
	} else {
		
		colortex.rgb = texColor;
	}

	colortex.a = 1.0;
}
