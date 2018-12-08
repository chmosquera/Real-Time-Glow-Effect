#version 450 core 

layout(location = 0) uniform sampler2D tex;
//layout(location = 1) uniform sampler2D tex2;

uniform int horizontal;


layout(location = 0) out vec4 colortex;

in vec2 fragTex;

layout(std140) uniform weights_ssbo
{ 
  float weights[1000];
};

void main()
{
	vec3 texturecolor = texture(tex, fragTex, 0).rgb;
	//vec3 texturecolor2 = texture(tex2, fragTex, 0).rgb;
	//float offset = 1.0/640.0;
    //vec2 offset = 1.0/ textureSize(tex, 0);
    vec2 offset = vec2(1.0/640.0, 1.0/480.0);

	// ----------
	// do blur
	// ----------
	/*texturecolor.r = pow(texturecolor.r, 1);
	texturecolor.g = pow(texturecolor.g, 1);
	texturecolor.b = pow(texturecolor.b, 1);
	*/

	//**********
	// smear
	//**********

	//for (int i = -50; i <= 50; i++) {
	//	if (i == 0) continue;
	//	vec3 col = texture(tex, fragTex + vec2(float(i) * offset.x, 0.0), 0).rgb;
	//	texturecolor += col * (1.0/float(i));
	//}


	//**********
	// gauss
	//**********

    //float weights[5] = {0.06136, 0.24477, 0.38774, 0.24477, 0.06136};
	//float weights[5] = {0.00135, 0.157305, 0.68269, 0.157305, 0.00135};
	//float weights[5] = {0.198005, 0.200995, 0.202001, 0.200995, 0.198005};
	
	float weights[9] = {0.000229, 0.005977, 0.060598, 0.241732, 0.382928, 0.241732, 0.060598, 0.005977, 0.000229};
	int size = 9;

    if (horizontal == 1) {
        for (int i = 0; i < size; ++i) {
            texturecolor.rgb += texture(tex, fragTex + vec2(float(i) * offset.x, 0.0), 0).rgb * weights[i];
            texturecolor.rgb += texture(tex, fragTex - vec2(float(i) * offset.x, 0.0), 0).rgb * weights[i];
			//texturecolor.rgb /= 2.0f;
        }
    } else {
        for (int i = 0; i < size; ++i) {
            texturecolor.rgb += texture(tex, fragTex + vec2(0.0, float(i) * offset.y), 0).rgb * weights[i];
            texturecolor.rgb += texture(tex, fragTex - vec2(0.0, float(i) * offset.y), 0).rgb * weights[i];
			//texturecolor.rgb /= 2.0f;
        }
    }

	//texturecolor = normalize(texturecolor);
	//texturecolor = texturecolor/2.0;
	colortex = vec4(texturecolor, 1.0);
	colortex.a = 1.0;


}
