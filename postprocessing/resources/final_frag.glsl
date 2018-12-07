#version 450 core 

layout(location = 0) uniform sampler2D tex;
layout(location = 1) uniform sampler2D tex2;


out vec4 colortex;
in vec2 fragTex;

void main()
{
	vec3 glowColor = texture(tex, fragTex, 0).rgb;		// alpha
	vec3 texColor = texture(tex2, fragTex, 0).rgb;

	//glowColor += texColor;
	//glowColor/2.0;
	//colortex.rgb = glowColor;

	float t = 0.00002;
	colortex.rgb = (glowColor * t) + (texColor * (1.0-t));
	//colortex.rgb = texture(tex2, fragTex, 0).rgb;
	colortex.a = 1.0;

	
}
