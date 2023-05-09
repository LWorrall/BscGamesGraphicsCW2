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
};

#endif // SCENEBASIC_UNIFORM_H
