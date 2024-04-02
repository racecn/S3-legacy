#pragma once

#include <vector>

class Sphere
{
public:
    // Constructor/Destructor
    Sphere(float radius = 1.0f, int sectorCount = 36, int stackCount = 18, bool smooth = true, int up = 3);
    ~Sphere();

    // Getters
    float getRadius() const { return radius; }
    int getSectorCount() const { return sectorCount; }
    int getStackCount() const { return stackCount; }
    bool isSmooth() const { return smooth; }
    int getUp() const { return up; }

    // Setters
    void set(float radius, int sectorCount, int stackCount, bool smooth = true, int up = 3);
    void setRadius(float radius);
    void setSectorCount(int sectorCount);
    void setStackCount(int stackCount);
    void setSmooth(bool smooth);
    void setUp(int up);

    void buildVerticesSmooth();

    // Drawing functions
    void draw() const;

protected:
    // Helper functions
    void buildInterleavedVertices();
    void setupSphere();
    void clearArrays();
    void addVertex(float x, float y, float z);
    void addNormal(float nx, float ny, float nz);
    void addTexCoord(float s, float t);
    void addIndices(unsigned int i1, unsigned int i2, unsigned int i3);
    std::vector<float> computeFaceNormal(float x1, float y1, float z1,
        float x2, float y2, float z2,
        float x3, float y3, float z3);

private:
    // Member variables
    float radius;
    int sectorCount;
    int stackCount;
    bool smooth;
    int up;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;
    std::vector<float> interleavedVertices;
    unsigned int VAO, VBO, EBO; // OpenGL buffers
};
