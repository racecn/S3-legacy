#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include "Sphere.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm.hpp"


// Constants
const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 2;

// Constructor
Sphere::Sphere(float radius, int sectors, int stacks, bool smooth, int up) :
    radius(radius), sectorCount(sectors), stackCount(stacks), smooth(smooth),
    up(up),
    VAO(0), VBO(0), EBO(0)
{
    if (sectors < MIN_SECTOR_COUNT) this->sectorCount = MIN_SECTOR_COUNT;
    if (stacks < MIN_STACK_COUNT) this->stackCount = MIN_STACK_COUNT;
    buildVerticesSmooth();
    setupSphere();
}

// Destructor
Sphere::~Sphere() {
    // Clean up buffers
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Sphere::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Sphere::buildVerticesSmooth() {
    // Constants
    const float PI = acos(-1.0f);
    const float lengthInv = 1.0f / radius;

    // Clear previous data
    clearArrays();

    // Variables for vertex position, normal, and texCoord
    float x, y, z, xy;
    float nx, ny, nz;
    float s, t;

    // Calculate step size for sectors and stacks
    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    // Reserve memory to avoid frequent reallocations
    vertices.reserve((sectorCount + 1) * (stackCount + 1) * 3);
    normals.reserve((sectorCount + 1) * (stackCount + 1) * 3);
    texCoords.reserve((sectorCount + 1) * (stackCount + 1) * 2);

    // Calculate vertices, normals, and texture coordinates
    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = -PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        y = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;

            // Calculate vertex position
            x = xy * cosf(sectorAngle);
            z = xy * sinf(sectorAngle);
            addVertex(x, y, z);

            // Calculate normalized vertex normal
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            addNormal(nx, ny, nz);

            // Calculate texture coordinates if smooth
            if (smooth) {
                s = 1.0f - static_cast<float>(j) / sectorCount;
                t = static_cast<float>(i) / stackCount;
                addTexCoord(s, t);
            }
        }
    }

    // Reserve memory for indices
    indices.reserve(stackCount * sectorCount * 6);

    // Calculate indices for triangles
    unsigned int k1, k2;
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                addIndices(k1, k2, k1 + 1);
            }

            if (i != (stackCount - 1)) {
                addIndices(k1 + 1, k2, k2 + 1);
            }
        }
    }

    // Build interleaved vertices
    buildInterleavedVertices();
}


void Sphere::set(float radius, int sectorCount, int stackCount, bool smooth, int up) {
    this->radius = radius;
    this->sectorCount = sectorCount;
    this->stackCount = stackCount;
    this->smooth = smooth;
    this->up = up;

    // Rebuild the sphere with the new parameters
    buildVerticesSmooth();
    setupSphere();
}

void Sphere::buildInterleavedVertices() {
    std::size_t i, j;
    std::size_t count = vertices.size();
    for (i = 0, j = 0; i < count; i += 3, j += 2) {
        interleavedVertices.push_back(vertices[i]);
        interleavedVertices.push_back(vertices[i + 1]);
        interleavedVertices.push_back(vertices[i + 2]);

        interleavedVertices.push_back(normals[i]);
        interleavedVertices.push_back(normals[i + 1]);
        interleavedVertices.push_back(normals[i + 2]);

        interleavedVertices.push_back(texCoords[j]);
        interleavedVertices.push_back(texCoords[j + 1]);
    }
}

void Sphere::setupSphere() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, interleavedVertices.size() * sizeof(float), &interleavedVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Sphere::addVertex(float x, float y, float z) {
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
}

void Sphere::addNormal(float nx, float ny, float nz) {
    normals.push_back(nx);
    normals.push_back(ny);
    normals.push_back(nz);
}

void Sphere::addTexCoord(float s, float t) {
    texCoords.push_back(s);
    texCoords.push_back(t);
}

void Sphere::addIndices(unsigned int i1, unsigned int i2, unsigned int i3) {
    indices.push_back(i1);
    indices.push_back(i2);
    indices.push_back(i3);
}

void Sphere::clearArrays() {
    vertices.clear();
    normals.clear();
    texCoords.clear();
    indices.clear();
    interleavedVertices.clear();
}

void Sphere::setSectorCount(int sectorCount)
{
    if (sectorCount != this->sectorCount)
        set(radius, sectorCount, stackCount, smooth, up);
}

void Sphere::setStackCount(int stackCount)
{
    if (stackCount != this->stackCount)
        set(radius, sectorCount, stackCount, smooth, up);
}

void Sphere::setSmooth(bool smooth) {
    if (smooth != this->smooth)
        set(radius, sectorCount, stackCount, smooth, up);
}

std::vector<float> Sphere::computeFaceNormal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) {
    const float EPSILON = 0.000001f;
    std::vector<float> normal(3, 0.0f);

    glm::vec3 v1(x2 - x1, y2 - y1, z2 - z1);
    glm::vec3 v2(x3 - x1, y3 - y1, z3 - z1);

    glm::vec3 result = glm::normalize(glm::cross(v1, v2));

    if (glm::length(result) > EPSILON) {
        normal[0] = result.x;
        normal[1] = result.y;
        normal[2] = result.z;
    }

    return normal;
}
