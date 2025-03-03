// spaceobject.h
#ifndef SPACEOBJECT_H
#define SPACEOBJECT_H

#include "glm.hpp"
#include <string>

class SpaceObject {
public:
    SpaceObject(const glm::vec3& position, float radius, const std::string& name)
        : position(position), radius(radius), name(name) {}


    virtual ~SpaceObject() {}



    virtual void update(float deltaTime) {}
    virtual glm::mat4 getModelMatrix(const glm::mat4& initialModelMatrix) const = 0;
    virtual float getOrbitRadius() const = 0;
    virtual bool isMouseOver(const glm::vec2& mousePos) const = 0;

    const glm::vec3& getPosition() const { return position; }
    float getRadius() const { return radius; }
    const std::string& getName() const { return name; }

protected:
    glm::vec3 position;
    float radius;
    std::string name;
};

#endif // SPACEOBJECT_H