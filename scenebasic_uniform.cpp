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
	rotSpeed = 3.0f;
	compile();

	glEnable(GL_DEPTH_TEST);
	view = glm::lookAt(vec3(0.0f, 1.5f, 2.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = mat4(1.0f);

	prog.setUniform("Fog.MaxDist", 15.0f);
	prog.setUniform("Fog.MinDist", 5.0f);
	prog.setUniform("Fog.Colour", vec3(0.5f, 0.5f, 0.5f));
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
		lightPos.x = glm::cos(angle) * 2.0f;
		lightPos.y = 2.0f;
		lightPos.z = glm::sin(angle) * 2.0f;
	}
}

// This function will take a loaded texture file and bind it in OpenGL, and set it's texture parameters.
void BindandSetParams(GLuint tex, int index)
{
	// Choose which of the two samplers to use.
	switch (index)
	{
	case 0:
		glActiveTexture(GL_TEXTURE0);
		break;
	case 1:
		glActiveTexture(GL_TEXTURE1);
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
	prog.setUniform("Light.L", glm::vec3(0.4f));
	prog.setUniform("Light.Position", view * lightPos);

	prog.setUniform("Material.Rough", 0.5f);
	prog.setUniform("Material.Metal", 1);
	prog.setUniform("Material.Colour", glm::vec3(1.0f, 1.0f, 1.0f));

	// Load the barrel and moss textures.
	GLuint barrelTex = Texture::loadTexture("media/texture/barrel.png");
	GLuint mossTex = Texture::loadTexture("media/texture/moss.png");
	//Then bind them and set their texture parameters.
	BindandSetParams(barrelTex, 0);
	BindandSetParams(mossTex, 1);

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
	BindandSetParams(planeTexID, 0);

	model = mat4(1.0f);
	prog.setUniform("Material.Rough", 0.9f);
	prog.setUniform("Material.Metal", 0);
	prog.setUniform("Material.Colour", glm::vec3(0.3f));
	
	setMatrices();
	plane.render();
}

void SceneBasic_Uniform::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawScene();
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
	prog.setUniform("ProjectionMatrix", projection);
}
