#include "scenebasic_uniform.h"
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include "helper/texture.h"

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>
#include "helper/glutils.h"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

// Load a plane and the barrel object.
SceneBasic_Uniform::SceneBasic_Uniform() : tPrev(0), plane(30.0f, 30.0f, 1, 1, 10.0f, 10.0f) {
	barrel = ObjMesh::load("media/barrel.obj", false, true);
}

void SceneBasic_Uniform::initScene() {
	angle = 0.0;
	rotSpeed = 1.5f;
	compile();

	glEnable(GL_DEPTH_TEST);
	view = glm::lookAt(vec3(0.0f, 1.5f, 2.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = mat4(1.0f);

	// Array for quad
	GLfloat verts[] = {
	-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
	};
	GLfloat tc[] = {
	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	// Set up the buffers
	unsigned int handle[2];
	glGenBuffers(2, handle);
	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

	// Set up the vertex array object
	glGenVertexArrays(1, &quad);
	glBindVertexArray(quad);
	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0); // Vertex position
	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2); // Texture coordinates
	glBindVertexArray(0);
	
	setupFBO();

	prog.setUniform("Light.L", glm::vec3(0.4f));
	prog.setUniform("Light.Position", view * lightPos);

	prog.setUniform("Fog.MaxDist", 15.0f);
	prog.setUniform("Fog.MinDist", 5.0f);
	prog.setUniform("Fog.Colour", vec3(0.5f, 0.5f, 0.5f));
}

void SceneBasic_Uniform::setupFBO()
{
	GLuint depthBuf, posTex, normTex, colourTex;
	// Create and bind the FBO
	glGenFramebuffers(1, &deferredFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);
	// The depth buffer
	glGenRenderbuffers(1, &depthBuf);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	// Create the textures for position, normal and color
	createGBufTex(GL_TEXTURE0, GL_RGB32F, posTex); // Position
	createGBufTex(GL_TEXTURE1, GL_RGB32F, normTex); // Normal
	createGBufTex(GL_TEXTURE2, GL_RGB8, colourTex); // Color
	// Attach the textures to the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colourTex, 0);
	GLenum drawBuffers[] = { GL_NONE, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(4, drawBuffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::createGBufTex(GLenum texUnit, GLenum format, GLuint & texid)
{
	glActiveTexture(texUnit);
	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
}


void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

// the 'update' function is called each frame.
// It is used to update the position of the moving light source.
void SceneBasic_Uniform::update(float t)
{
	float deltaT = t - tPrev;
	if (tPrev == 0.0f) {
		deltaT = 0.0f;
	}

	tPrev = t;
	
	if (animating())
	{
		angle = glm::mod(angle + deltaT * rotSpeed, glm::two_pi<float>());
		lightPos.x = glm::cos(angle) * 7.0f;
		lightPos.y = 3.0f;
		lightPos.z = glm::sin(angle) * 7.0f;
	}
}

// This function will take a loaded texture file and bind it in OpenGL, and set it's texture parameters.
void BindandSetParams(GLuint tex, int index)
{
	// Choose which of the two samplers to use.
	switch (index)
	{
	case 3:
		glActiveTexture(GL_TEXTURE3);
		break;
	case 4:
		glActiveTexture(GL_TEXTURE4);
		break;
	}
	// Bind the texture file that is passed into the function.
	glBindTexture(GL_TEXTURE_2D, tex);
	// Set mipmapping to nearest.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// Set texture wrapping to repeat.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Generate mipmap textures.
	glGenerateMipmap(GL_TEXTURE_2D);
}

void SceneBasic_Uniform::drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	prog.setUniform("Light.Position", view * lightPos);

	prog.setUniform("Material.Rough", 0.9f);
	prog.setUniform("Material.Metal", 1);
	prog.setUniform("Material.Colour", glm::vec3(0.5f,0.5f, 0.5f));

	// Load the barrel and moss textures.
	GLuint barrelTex = Texture::loadTexture("media/texture/barrel.png");
	GLuint mossTex = Texture::loadTexture("media/texture/moss.png");
	//Then bind them and set their texture parameters.
	BindandSetParams(barrelTex, 3);
	BindandSetParams(mossTex, 4);

	// For loop to render 5 different barrels spaced apart.
	float distance = 0.0f;
	for (int i = 0; i < 5; i++) {
		model = mat4(1.0f);
		model = glm::translate(model, vec3(distance * 0.6f - 1.0f, 0.0f, -distance));
		
		setMatrices();
		barrel->render();
		
		distance += 3.0f;
	}

	// Load the ground texture, and pass it to the function to bind and set texture parameters.
	GLuint planeTexID = Texture::loadTexture("media/texture/ground.png");
	BindandSetParams(planeTexID, 3);

	model = mat4(1.0f);
	prog.setUniform("Material.Rough", 0.9f);
	prog.setUniform("Material.Metal", 0);
	prog.setUniform("Material.Colour", glm::vec3(0.5f));
	
	setMatrices();
	plane.render();
}

void SceneBasic_Uniform::pass1()
{
	prog.setUniform("Pass", 1);
	glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);
	drawScene();
	glFinish();
}

void SceneBasic_Uniform::pass2()
{
	prog.setUniform("Pass", 2);
	// Revert to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	view = mat4(1.0);
	model = mat4(1.0);
	projection = mat4(1.0);
	setMatrices();
	// Render the quad
	glBindVertexArray(quad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}


void SceneBasic_Uniform::render()
{
	pass1();
	pass2();
}

void SceneBasic_Uniform::resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
	mat4 mv = view * model;
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	prog.setUniform("MVP", projection * mv);
	prog.setUniform("ProjectioMatrix", projection);
}
