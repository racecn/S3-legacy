#version 330 core

// Vertex attributes
layout (location = 0) in vec3 aPos;      // Vertex position
layout (location = 1) in vec3 aNormal;   // Vertex normal

// Outputs to the fragment shader
out vec3 Normal;     // Normal vector in world space
out vec3 Position;   // Position vector in world space

// Uniforms (matrices)
uniform mat4 model;        // Model matrix: local space -> world space
uniform mat4 view;         // View matrix: world space -> camera space
uniform mat4 projection;   // Projection matrix: camera space -> clip space

void main()
{
    // Transform the normal vector from local space to world space
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Transform the position vector from local space to world space
    Position = vec3(model * vec4(aPos, 1.0));

    // Transform the vertex position through all transformation matrices
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
