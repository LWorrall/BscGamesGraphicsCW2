#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include <glad/glad.h>
#include "helper/glslprogram.h"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include <glm/glm.hpp>


class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;

    GLSLProgram solidProg;
    GLuint shadowFBO, pass1Index, pass2Index;

    int shadowMapWidth, shadowMapHeight;
    glm::mat4 lightPV, shadowBias;
    Frustum lightFrustum;

    Plane plane;
    std::unique_ptr<ObjMesh> barrel;
    
    glm::vec4 lightPos;

    float angle;
    float rotSpeed;
    float tPrev;
    void compile();
public:
    SceneBasic_Uniform();
    void initScene();
    void update( float t );
    void render();
    void drawScene();
    void resize(int, int);
    void setMatrices();

    void setupFBO();
    void spitOutDepthBuffer();
};

#endif // SCENEBASIC_UNIFORM_H
