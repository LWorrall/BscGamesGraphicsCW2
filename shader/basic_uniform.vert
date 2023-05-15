#version 460

// The vertex shader processes the vertices of 3D models and prepares them for rendering.

// Declare vertices.
layout (location = 0) in vec3 VertexPosition;   // Vertex position vector.
layout (location = 1) in vec3 VertexNormal;     // Vertex normal vector used for shading.
layout (location = 2) in vec2 VertexTexCoord;   // Texture coordinates for the vertex.


// Declare output variables. These will be used for the next stage in the shader.
out vec3 Position;  // 4D vector for position of the vertex in camera space.
out vec3 Normal;    // 3D vector for the normal of the vertex in camera space.
out vec2 TexCoord;  // 2D vector for texture coordinates of the vertex.

out vec4 ShadowCoord;


// Declare uniform variables.
// These will transform the vertices from model space, to camera space, and then to screen space.
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform mat4 ProjectionMatrix;

uniform mat4 ShadowMatrix;


// Set output variables and calculate the final position of the vertex.
// Pass the vertex to the next stage in the pipeline.
void main() {
    
    ShadowCoord = MVP * vec4(VertexPosition, 1.0);

    TexCoord = VertexTexCoord;
    Normal = normalize( NormalMatrix * VertexNormal );
    Position = (ModelViewMatrix * vec4(VertexPosition, 1.0) ).xyz;
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}