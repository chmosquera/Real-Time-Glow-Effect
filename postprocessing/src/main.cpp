/* Lab 6 base code - transforms using local matrix functions
	to be written by students -
	based on lab 5 by CPE 471 Cal Poly Z. Wood + S. Sueda
	& Ian Dunn, Christian Eckhardt
*/
#include <iostream>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "camera.h"

#define _USE_MATH_DEFINES
#include <cmath>

#define M_PI 3.14159265358979323846

// used for helper in perspective
#include "glm/glm.hpp"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;

class Gaussian
{

public: 
	//double weights[9][1];
	//double weights[11];

	float * Calculate1DSampleKernel(double deviation, int size)
	{
		//double * weights = NULL;
		float * weights = new float[size];
		float sum = 0;

		int half = size / 2;
		for (int i = 0; i < size; i++)
		{
			weights[i] = 1 / (sqrt(2 * M_PI) * deviation) * exp(-(i - half) * (i - half) / (2 * deviation * deviation));
			sum += weights[i];
		}
		return weights;
	}

	//void ClearWeights() {
	//	int size = Size();
	//	for (int i = 0; i < size; i++) {
	//		weights[i] = 0;
	//	}
	//}

	//int Size() {
	//	return sizeof(weights) / sizeof(double);
	//}
	//double[, ] Calculate1DSampleKernel(double deviation)
	//{
	//	int size = (int)Math.Ceiling(deviation * 3) * 2 + 1;
	//	return Calculate1DSampleKernel(deviation, size);
	//}
	float * CalculateNormalized1DSampleKernel(double deviation, int size)
	{
		// clear weights
		//double * weights = new double[size];
		//for (int i = 0; i < size; i++) {
		//	weights[i] = 0;
		//}

		float * weights = Calculate1DSampleKernel(deviation, size);
		NormalizeMatrix(weights, size);
		return weights;
	}

	void NormalizeMatrix(float* weights, int size)
	{
		//double[, ] ret = new double[matrix.GetLength(0), matrix.GetLength(1)];
		double sum = 0;
		//int size = Size();

		for (int i = 0; i < size; i++)
		{
//			for (int j = 0; j < cols; j++)
				sum += weights[i];
		}
		if (sum != 0)
		{
			for (int i = 0; i < size; i++)
			{
			//	for (int j = 0; j < cols; j++)
					weights[i] = weights[i] / sum;
			}
		}

		//return weights;
	}
};

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog, prog_blur, prog_final;

	// Shape to be used (from obj file)
	shared_ptr<Shape> shape;
	
	//camera
	camera mycam;

	//texture for sim
	GLuint TextureEarth, TextureMoon, TextureJupiter, TextureMercury, TextureVenus, TextureMars;
	GLuint TexColor, TexAlpha, BlurredTexAlpha[2] , fb, BlurFBO[2], depth_rb, depth_rb2;

	GLuint VertexArrayIDBox, VertexBufferIDBox, VertexBufferTex;
	
	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	// gaussian weights ssbo
	GLuint weights_ssbo;
	float * weights = NULL;
	int size = 9;

	// controls
	int activateBlur = 0;
	int glowMaskOnly = 0;
	float glowScale = 0.5f;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
		if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE){ activateBlur = !activateBlur;}
		if (key == GLFW_KEY_TAB && action == GLFW_RELEASE){ glowMaskOnly = !glowMaskOnly;}
		if (key == GLFW_KEY_UP && action == GLFW_PRESS) { glowScale += 0.05; };
		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) { glowScale -= 0.05; };
		if (key == GLFW_KEY_5 && action == GLFW_PRESS) { 
			size = 5; 
			Gaussian *gauss = new Gaussian();
			float * weights_t = gauss->CalculateNormalized1DSampleKernel(1.0, size);
			weights = new float[size];
			for (int i = 0; i < size; i++) {
				weights[i] = weights_t[i];
				cout << weights[i] << " ";
			} cout << endl;
		};
		if (key == GLFW_KEY_7 && action == GLFW_PRESS) { 
			size = 7; 
			Gaussian *gauss = new Gaussian();
			float * weights_t = gauss->CalculateNormalized1DSampleKernel(1.0, size);
			weights = new float[size];
			for (int i = 0; i < size; i++) {
				weights[i] = weights_t[i];
				cout << weights[i] << " ";
			} cout << endl;
		};
		if (key == GLFW_KEY_9 && action == GLFW_PRESS) { 
			size = 9; 
			Gaussian *gauss = new Gaussian();
			float * weights_t = gauss->CalculateNormalized1DSampleKernel(1.0, size);
			weights = new float[size];
			for (int i = 0; i < size; i++) {
				weights[i] = weights_t[i];
				cout << weights[i] << " ";
			} cout << endl;
		};
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{


		GLSL::checkVersion();

		
		// Set background color.
		//glClearColor(0.12f, 0.34f, 0.56f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		//culling:
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);

		//transparency
		glEnable(GL_BLEND);
		//next function defines how to mix the background color with the transparent pixel in the foreground. 
		//This is the standard:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		if (! prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("glowScale");
		prog->addUniform("campos");
		prog->addUniform("minthresh");
		prog->addUniform("maxthresh");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");


		prog_blur = make_shared<Program>();
		prog_blur->setVerbose(true);
		prog_blur->setShaderNames(resourceDirectory + "/vert.glsl", resourceDirectory + "/blur_frag.glsl");
		if (!prog_blur->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog_blur->init();
		prog_blur->addUniform("P");
		prog_blur->addUniform("V");
		prog_blur->addUniform("M");
		prog_blur->addUniform("weights_ssbo");
		prog_blur->addUniform("size");
		prog_blur->addUniform("horizontal");
		prog_blur->addUniform("glowMaskOnly");
		prog_blur->addUniform("activateBlur");
		prog_blur->addAttribute("vertPos");
		prog_blur->addAttribute("vertTex");

		
		prog_final = make_shared<Program>();
		prog_final->setVerbose(true);
		prog_final->setShaderNames(resourceDirectory + "/vert.glsl", resourceDirectory + "/final_frag.glsl");
		if (!prog_final->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog_final->init();
		prog_final->addUniform("P");
		prog_final->addUniform("V");
		prog_final->addUniform("M");
		prog_final->addUniform("activateBlur");
		prog_final->addUniform("glowMaskOnly");
		prog_final->addAttribute("vertPos");
		prog_final->addAttribute("vertTex");
	}

	void initGeom(const std::string& resourceDirectory)
	{
		//init rectangle mesh (2 triangles) for the post processing
		glGenVertexArrays(1, &VertexArrayIDBox);
		glBindVertexArray(VertexArrayIDBox);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDBox);

		GLfloat *rectangle_vertices = new GLfloat[18];
		// front
		int verccount = 0;

		rectangle_vertices[verccount++] = -1.0, rectangle_vertices[verccount++] = -1.0, rectangle_vertices[verccount++] = 0.0;
		rectangle_vertices[verccount++] = 1.0, rectangle_vertices[verccount++] = -1.0, rectangle_vertices[verccount++] = 0.0;
		rectangle_vertices[verccount++] = -1.0, rectangle_vertices[verccount++] = 1.0, rectangle_vertices[verccount++] = 0.0;
		rectangle_vertices[verccount++] = 1.0, rectangle_vertices[verccount++] = -1.0, rectangle_vertices[verccount++] = 0.0;
		rectangle_vertices[verccount++] = 1.0, rectangle_vertices[verccount++] = 1.0, rectangle_vertices[verccount++] = 0.0;
		rectangle_vertices[verccount++] = -1.0, rectangle_vertices[verccount++] = 1.0, rectangle_vertices[verccount++] = 0.0;


		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), rectangle_vertices, GL_STATIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferTex);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferTex);

		float t = 1. / 100.;
		GLfloat *rectangle_texture_coords = new GLfloat[12];
		int texccount = 0;
		rectangle_texture_coords[texccount++] = 0, rectangle_texture_coords[texccount++] = 0;
		rectangle_texture_coords[texccount++] = 1, rectangle_texture_coords[texccount++] = 0;
		rectangle_texture_coords[texccount++] = 0, rectangle_texture_coords[texccount++] = 1;
		rectangle_texture_coords[texccount++] = 1, rectangle_texture_coords[texccount++] = 0;
		rectangle_texture_coords[texccount++] = 1, rectangle_texture_coords[texccount++] = 1;
		rectangle_texture_coords[texccount++] = 0, rectangle_texture_coords[texccount++] = 1;

		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), rectangle_texture_coords, GL_STATIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(2);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// Initialize mesh.
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/sphere.obj");
		shape->resize(); 
		shape->init();
			
		
		int width, height, channels;
		char filepath[1000];

		//texture earth diffuse
		string str = resourceDirectory + "/earth.jpg";
		strcpy(filepath, str.c_str());		
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &TextureEarth);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureEarth);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		//width = 640; height = 480;

		//texture moon
		str = resourceDirectory + "/moon.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &TextureMoon);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureMoon);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//texture mercury
		str = resourceDirectory + "/mercury.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &TextureMercury);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureMercury);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//texture venus
		str = resourceDirectory + "/venus.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &TextureVenus);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureVenus);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		//texture mars
		str = resourceDirectory + "/mars.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &TextureMars);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureMars);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//texture jupiter
		str = resourceDirectory + "/jupiter.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &TextureJupiter);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureJupiter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		// [prog] Set texture units in cpu to samplers in gpu
		GLuint Tex1Location = glGetUniformLocation(prog->pid, "tex");//tex, tex2... sampler in the fragment shader
		GLuint Tex2Location = glGetUniformLocation(prog->pid, "tex2");
		// Then bind the uniform samplers to texture units:
		glUseProgram(prog->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);

		// [final prog] Set texture units in cpu to samplers in gpu
		Tex1Location = glGetUniformLocation(prog_final->pid, "tex");//tex, tex2... sampler in the fragment shader
		Tex2Location = glGetUniformLocation(prog_final->pid, "tex2");
		// Then bind the uniform samplers to texture units:
		glUseProgram(prog_final->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);

		/**************************************************************/
		/* --------- Frame Buffer - Color, Glow Source ---------------*/
		/**************************************************************/
		/* Color Texture */
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		//RGBA8 2D texture, 24 bit depth texture, 256x256
		glGenTextures(1, &TexColor);
		glBindTexture(GL_TEXTURE_2D, TexColor);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//NULL means reserve texture memory, but texels are undefined
		//**** Tell OpenGL to reserve level 0. Why? Defines the mipmap. 0 is highest res.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

		//You must reserve memory for other mipmaps levels as well either by making a series of calls to
		//glTexImage2D or use glGenerateMipmapEXT(GL_TEXTURE_2D).
		//Here, we'll use :
		glGenerateMipmap(GL_TEXTURE_2D);

		/* Brightness Texture */
		glGenTextures(1, &TexAlpha);
		glBindTexture(GL_TEXTURE_2D, TexAlpha);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
		glGenerateMipmap(GL_TEXTURE_2D);
		

		/* Frame Buffer */
		glGenFramebuffers(1, &fb);
		glBindFramebuffer(GL_FRAMEBUFFER, fb);

		//Attach texture(s) to this FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TexColor, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, TexAlpha, 0);

		//-------------------------
		glGenRenderbuffers(1, &depth_rb);
		glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

		//-------------------------
		//Attach depth buffer to FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);

		// Set texture units in cpu to samplers in gpu
		Tex1Location = glGetUniformLocation(prog_blur->pid, "tex");//tex, tex2... sampler in the fragment shader
		glUseProgram(prog_blur->pid);
		glUniform1i(Tex1Location, 0);
		/*****************************************************/
		/* --------- Frame Buffer - Blurring ---------------*/
		/****************************************************/
		//RGBA8 2D texture, 24 bit depth texture, 256x256

		glGenFramebuffers(2, BlurFBO);
		glGenTextures(2, BlurredTexAlpha);
		for (int i = 0; i < 2; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, BlurFBO[i]);
			glBindTexture(GL_TEXTURE_2D, BlurredTexAlpha[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
			glGenerateMipmap(GL_TEXTURE_2D);

			//Attach 2D texture to this FBO
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BlurredTexAlpha[i], 0);
		}
		//-------------------------
		glGenRenderbuffers(1, &depth_rb2);
		glBindRenderbuffer(GL_RENDERBUFFER, depth_rb2);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

		//-------------------------
		//Attach depth buffer to FBO
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb2);


		//-------------------------
		// 
			// testing gaus calculation
		Gaussian *gauss = new Gaussian();
		float* weights_low = gauss->CalculateNormalized1DSampleKernel(1.0, size);
		float* weights_mid = gauss->CalculateNormalized1DSampleKernel(0.5, size);
		float* weights_hi = gauss->CalculateNormalized1DSampleKernel(0.1, size);

		for (int i = 0; i < size; i++) {
			cout << weights_low[i] << " ";
			cout << weights_mid[i] << " ";
			cout << weights_hi[i] << " ";
			cout << endl;
		} cout << endl;

		weights = new float[size];
		for (int i = 0; i < size; i++) {
			weights[i] = (weights_low[i] + weights_mid[i] + weights_hi[i]) / 3.0;
			//weights[i] = weights_low[i];
			cout << weights[i] << " ";
		} cout << endl;

		glGenBuffers(1, &weights_ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, weights_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * size, &weights, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, weights_ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

		//GLuint weights_ssbo_id =  glGetUniformBlockIndex(prog_blur, &weights_ssbo);


		//-------------------------
		//Does the GPU support current FBO configuration?
		GLenum status;
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		switch (status)
		{
		case GL_FRAMEBUFFER_COMPLETE:
			cout << "status framebuffer: good";
			break;
		default:
			cout << "status framebuffer: bad!!!!!!!!!!!!!!!!!!!!!!!!!";
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	//*************************************
	double get_last_elapsed_time()
		{
		static double lasttime = glfwGetTime();
		double actualtime = glfwGetTime();
		double difference = actualtime - lasttime;
		lasttime = actualtime;
		return difference;
		}
	//*************************************
	
	void render_to_texture() // aka render to framebuffer
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };	// color and alpha
		glDrawBuffers(2, buffers);

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width / (float)height;
		glViewport(0, 0, width, height);

		glm::mat4 M, V, S, T, P;
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width / (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones
		V = mycam.process();


		// Clear framebuffer.
		// Why do you clear the framebuffer?
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//bind shader and copy matrices
		prog->bind();
		vec3 minthresh = vec3(0.2);
		vec3 maxthresh = vec3(0.7);

		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniform3fv(prog->getUniform("campos"), 1, &mycam.pos.x);
		glUniform1f(prog->getUniform("glowScale"), glowScale);
		glUniform3fv(prog->getUniform("minthresh"), 1, &minthresh.x);
		glUniform3fv(prog->getUniform("maxthresh"), 1, &maxthresh.x);


		float pih = -3.1415926 / 2.0;
		glm::mat4 Rx = glm::rotate(glm::mat4(1.f), pih, glm::vec3(1, 0, 0));

//	******		planet		******
		static float ven_angle = 0;
		ven_angle += 0.005;
		M = glm::translate(glm::mat4(1.f), glm::vec3(-3.0, 0, -2.5));
		mat4 Ry = glm::rotate(glm::mat4(1.f), ven_angle, glm::vec3(0, 1, 0));
		S = glm::scale(glm::mat4(1.f), glm::vec3(1.0));
		M = M * Ry * Rx * S;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureVenus);
		minthresh = vec3(0.5, 0.3, 0.0);
		glUniform3fv(prog->getUniform("minthresh"), 1, &minthresh.x);
		maxthresh = vec3(1.0, 1.0, 0.2);
		glUniform3fv(prog->getUniform("maxthresh"), 1, &maxthresh.x);

		shape->draw(prog);

		//	******		earth		******
		static float angle = 0;
		angle += 0.02;
		M = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -5));
		Ry = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 1, 0));
		M = M * Ry * Rx;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureEarth);
		minthresh = vec3(0.0, 0.0, 0.0);
		glUniform3fv(prog->getUniform("minthresh"), 1, &minthresh.x);
		maxthresh = vec3(1.0, 1.0, 0.2);
		glUniform3fv(prog->getUniform("maxthresh"), 1, &maxthresh.x);

		shape->draw(prog);	//draw earth

		//	******		moon		******
		static float moonangle = 0;
		moonangle += 0.005;
		M = glm::translate(glm::mat4(1.f), glm::vec3(-1.5, 0, 1.5));
		glm::mat4 Ryrad = glm::rotate(glm::mat4(1.f), moonangle, glm::vec3(0, 1, 0));
		T = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -5));
		S = glm::scale(glm::mat4(1.f), glm::vec3(0.25));
		M = T * Ryrad * M * Rx * S;
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureMoon);


		//done, unbind stuff
		prog->unbind();
	}

	void render_blur_1() // aka render to framebuffer
	{

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width / (float)height;
		glViewport(0, 0, width, height);

		// Setup MVP
		auto P = std::make_shared<MatrixStack>();
		P->pushMatrix();
		P->perspective(70., width, height, 0.1, 100.0f);
		glm::mat4 M, V, S, T;
		V = glm::mat4(1);
		M = glm::scale(glm::mat4(1), glm::vec3(1.2, 1, 1)) * glm::translate(glm::mat4(1), glm::vec3(-0.5, -0.5, -1));

		// Bind to shader
		prog_blur->bind();

		// calculate weights
		//weights = new float[size];
		//Gaussian *gauss = new Gaussian();
		//float * weights_t = gauss->CalculateNormalized1DSampleKernel(1.0, size);
		//weights = new float[size];
		//for (int i = 0; i < size; i++) {
		//	weights[i] = weights_t[i];
		//	cout << weights[i] << " ";
		//} cout << endl;

		// send weights to gpu
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, weights_ssbo);
		GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		memcpy(p, weights, sizeof(float) * size);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		
		GLuint block_index = 0;
		block_index = glGetProgramResourceIndex(prog_blur->pid, GL_SHADER_STORAGE_BLOCK, "weights_ssbo");
		GLuint ssbo_binding_point_index = 0;
		glShaderStorageBlockBinding(prog_blur->pid, block_index, ssbo_binding_point_index);			// THIS IS THE LINE AT WHICH IT CRASHES. 12:20PM
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, weights_ssbo);

		glUniformMatrix4fv(prog_blur->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(prog_blur->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog_blur->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform1i(prog_blur->getUniform("glowMaskOnly"), glowMaskOnly);
		glUniform1i(prog_blur->getUniform("activateBlur"), activateBlur);
		glUniform1i(prog_blur->getUniform("size"), size);
		glBindVertexArray(VertexArrayIDBox);


		int horizontal = 0;
		int amount = 12;		// test incr amount here
		bool firstIteration = true;
		for (int i = 0; i < amount; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, BlurFBO[horizontal]);
			GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };	// alpha
			glDrawBuffers(1, buffers);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUniform1i(prog_blur->getUniform("horizontal"), horizontal);			// Are we doing the second blur? no

			glBindTexture(
				GL_TEXTURE_2D, firstIteration ? TexAlpha : BlurredTexAlpha[!horizontal]
			);
		
			glDrawArrays(GL_TRIANGLES, 0, 6);

			horizontal = !horizontal;
			if (firstIteration) firstIteration = false;
	
		}

		prog_blur->unbind();
	}

	void render_to_screen()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0, 0.0, 0.0, 0.0);

		// Get current frame buffer size.
		double frametime = get_last_elapsed_time();

		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width / (float)height;
		glViewport(0, 0, width, height);

		auto P = std::make_shared<MatrixStack>();
		P->pushMatrix();
		P->perspective(70., width, height, 0.1, 100.0f);
		glm::mat4 M, V, S, T;

		V = glm::mat4(1);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		prog_final->bind();


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, BlurredTexAlpha[0]);
		//glBindTexture(GL_TEXTURE_2D, TexColor);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, TexColor);
		M = glm::scale(glm::mat4(1), glm::vec3(1.2, 1, 1)) * glm::translate(glm::mat4(1), glm::vec3(-0.5, -0.5, -1));
		glUniformMatrix4fv(prog_final->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
		glUniformMatrix4fv(prog_final->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(prog_final->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform1i(prog_final->getUniform("activateBlur"), activateBlur);
		glUniform1i(prog_final->getUniform("glowMaskOnly"), glowMaskOnly);
		glBindVertexArray(VertexArrayIDBox);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		prog_final->unbind();

		cout << "glowMaskOnly: " << glowMaskOnly << " | activateBlur: " << activateBlur << " | kernalSize: " << size << " | glowScale: " << glowScale << endl;

	}

};
//*********************************************************************************************************
int main(int argc, char **argv)
{

	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render_to_texture();
		application->render_blur_1();
		application->render_to_screen();
		
		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
