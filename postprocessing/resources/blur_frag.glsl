#version 450 core 

layout(location = 0) uniform sampler2D tex;

uniform int horizontal;
uniform int glowMaskOnly;
uniform int activateBlur;
uniform int size;


layout(location = 0) out vec4 colortex;

in vec2 fragTex;

layout(std140,binding = 0) volatile buffer weights_ssbo
{ 
  float weights[1000];
};

void main()
{
	vec3 texturecolor = texture(tex, fragTex, 0).rgb;
    vec2 offset = vec2(1.0/640.0, 1.0/480.0);

	//float weights[5]= {0.198005, 0.200995, 0.202001, 0.200995, 0.198005};
	//int size = 5;

	//float weights[11] = {0.000003, 0.000229, 0.005977, 0.060598, 0.24173, 0.382925, 0.24173, 0.060598, 0.005977, 0.000229, 0.000003};
	//size = 11;

	float weights[9] = {0.000229, 0.005977, 0.060598, 0.241732, 0.382928, 0.241732, 0.060598, 0.005977, 0.000229};
	int size = 9;
	

    if (horizontal == 1) {
        for (int i = 0; i < size; ++i) {
            texturecolor.rgb += texture(tex, fragTex + vec2(float(i) * offset.x, 0.0), 0).rgb * weights[i];
            texturecolor.rgb += texture(tex, fragTex - vec2(float(i) * offset.x, 0.0), 0).rgb * weights[i];
        }
    } else {
        for (int i = 0; i < size; ++i) {
            texturecolor.rgb += texture(tex, fragTex + vec2(0.0, float(i) * offset.y), 0).rgb * weights[i];
            texturecolor.rgb += texture(tex, fragTex - vec2(0.0, float(i) * offset.y), 0).rgb * weights[i];
        }
    }

	if (activateBlur == 1) {
		colortex = vec4(texturecolor, 1.0);
	} else {
		colortex.rgb = texture(tex, fragTex, 0).rgb;	// render the glow mask only
	}

	colortex.a = 1.0;


}
