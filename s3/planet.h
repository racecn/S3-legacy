// planet.h
#ifndef PLANET_H
#define PLANET_H

#include "spaceobject.h"

struct PlanetInfo {
    std::string name;
    float orbitalPeriod; // in Earth days
    float rotationPeriod; // in Earth days
    // Add more information as needed
};

class Planet : public SpaceObject {
public:
    Planet(const glm::vec3& position, float radius, const std::string& name, float distanceFromSun,
        const glm::vec3& color = glm::vec3(1.0f), const glm::vec3& specularColor = glm::vec3(1.0f),
        float shininess = 32.0f, const std::string& diffuseTexture = "",
        const std::string& specularTexture = "", const std::string& normalTexture = "",
        const std::string& emissionTexture = "")
        : SpaceObject(position, radius, name), distanceFromSun(distanceFromSun),
        color(color), specularColor(specularColor), shininess(shininess),
        diffuseTexture(diffuseTexture), specularTexture(specularTexture),
        normalTexture(normalTexture), emissionTexture(emissionTexture) {}


    bool isMouseOver(const glm::vec2& mousePos) const override {
        // Print object position and mouse position for debugging
        std::cout << "Object Position: (" << position.x << ", " << position.y << ")" << std::endl;
        std::cout << "Mouse Position: (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;

        // Calculate distance between object and mouse position
        float distance = glm::length(glm::vec2(position.x, position.y) - mousePos);

        // Print the calculated distance
        std::cout << "Distance: " << distance << std::endl;

        // Print the radius for debugging
        std::cout << "Radius: " << radius << std::endl;

        // Return whether the mouse is over the object
        return distance <= radius;
    }


    float getDistanceFromSun() const;
    const PlanetInfo& getInfo() const;


private:
    float distanceFromSun;
    glm::vec3 color;
    glm::vec3 specularColor;
    float shininess;
    std::string diffuseTexture;
    std::string specularTexture;
    std::string normalTexture;
    std::string emissionTexture;
};

#endif // PLANET_H
