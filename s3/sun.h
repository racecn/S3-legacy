// sun.h
#ifndef SUN_H
#define SUN_H

#include "spaceobject.h"

class Sun : public SpaceObject {
public:
    Sun(const std::string& name, float radius, float orbitalPeriod, float distanceFromSun, const glm::vec3& diffuseColor, const glm::vec3& specularColor, float shininess, const std::string& diffuseTexture, const std::string& specularTexture, const std::string& normalTexture, const std::string& emissionTexture)
        : SpaceObject(glm::vec3(0.0f), radius, name), orbitalPeriod(orbitalPeriod), distanceFromSun(distanceFromSun), diffuseColor(diffuseColor), specularColor(specularColor), shininess(shininess), diffuseTexture(diffuseTexture), specularTexture(specularTexture), normalTexture(normalTexture), emissionTexture(emissionTexture) {}


    bool isMouseOver(const glm::vec2& mousePos) const override {
        float distance = glm::length(glm::vec2(position.x, position.y) - mousePos);
        return distance <= radius;
    }
private:
    float orbitalPeriod;
    float distanceFromSun;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    float shininess;
    std::string diffuseTexture;
    std::string specularTexture;
    std::string normalTexture;
    std::string emissionTexture;
};
#endif // SUN_H