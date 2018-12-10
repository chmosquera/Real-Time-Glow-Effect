#version 450 core 

//out vec4 color;

in vec3 WorldPos;
in vec2 fragTex;
in vec3 fragNor;

uniform vec3 campos;
uniform float glowScale;
uniform vec3 minthresh;
uniform vec3 maxthresh;

layout(location = 0) uniform sampler2D tex;

layout(location = 0) out vec4 colortex;
layout(location = 1) out vec4 alphatex;



void main()
{
	vec3 normal = normalize(fragNor);
	vec3 texturecolor = texture(tex, fragTex).rgb;
	
	//diffuse light
	vec3 lp = vec3(100);
	vec3 ld = normalize(lp - WorldPos);
	float light = dot(ld,normal);	
	light = clamp(light,0,1);

	//colortex.rgb = texturecolor * light * 2.0;
	colortex.rgb = texturecolor;
	colortex.a=1;

	
	alphatex = vec4(0.0, 0.0, 0.0, 1.0);

	if ((texturecolor.r > minthresh.r && texturecolor.g > minthresh.g && texturecolor.b > minthresh.b) &&
		(texturecolor.r < maxthresh.r && texturecolor.g < maxthresh.g && texturecolor.b < maxthresh.b)){

		alphatex.rgb = colortex.rgb * glowScale;
	}
	
	
}
