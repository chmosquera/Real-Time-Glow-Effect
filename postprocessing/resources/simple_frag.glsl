#version 450 core 

//out vec4 color;

in vec3 WorldPos;
in vec2 fragTex;
in vec3 fragNor;


uniform vec3 campos;

layout(location = 0) uniform sampler2D tex;

layout(location = 0) out vec4 colortex;
layout(location = 1) out vec4 alphatex;



void main()
{
	vec3 normal = normalize(fragNor);
	vec3 texturecolor = texture(tex, fragTex).rgb;
	
	//diffuse light
	vec3 lp = vec3(0);
	vec3 ld = normalize(lp - WorldPos);
	float light = dot(ld,normal);	
	light = clamp(light,0,1);

	//specular light
	vec3 camvec = normalize(campos - WorldPos);
	vec3 h = normalize(camvec+ld);
	float spec = pow(dot(h,normal),5);
	spec = clamp(spec,0,1);
		
	//colortex.rgb = texturecolor * light +vec3(1, 1, 1)*spec;		// with spec
	colortex.rgb = texturecolor * light * 2.0;
	colortex.rgb = texturecolor;
	colortex.a=1;

	
	alphatex = vec4(0.0, 0.0, 0.0, 1.0);
	float thresh = 0.1;


	//if (texturecolor.r > thresh || texturecolor.g > thresh || texturecolor.b > thresh) {		//mask the brightest pixels
	//if (texturecolor.r < thresh && texturecolor.g < thresh && texturecolor.b > thresh) {		// mask the blue pixels
	if (texturecolor.r < 0.3 && texturecolor.g < 0.3 && texturecolor.b > 0.1) {		// mask the blue pixels

		//alphatex.rgb = vec3((colortex.r + colortex.g + colortex.b)/3.0);		// b&w average
		alphatex.rgb = colortex.rgb * 4.0;	// in color (note that the color will get multiplied as it runs through blur frag x times
		//alphatex.rgb = vec3(colortex.b);
		alphatex.a = 1;
	}
	
	
}
