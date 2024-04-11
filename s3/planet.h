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
    Planet(const glm::vec3& position, float radius, const std::string& name, bool isOrbiting, float distanceFromSun,
        const glm::vec3& color = glm::vec3(1.0f), const glm::vec3& specularColor = glm::vec3(1.0f),
        float shininess = 32.0f, const std::string& diffuseTexture = "",
        const std::string& specularTexture = "", const std::string& normalTexture = "",
        const std::string& emissionTexture = "")
        : SpaceObject(position, radius, name), distanceFromSun(distanceFromSun),
        isOrbiting(isOrbiting),
        color(color), specularColor(specularColor), shininess(shininess),
        diffuseTexture(diffuseTexture), specularTexture(specularTexture),
        normalTexture(normalTexture), emissionTexture(emissionTexture),
        rotationSpeed(rotationSpeed), rotationAngle(0.0f) {}



    void update(float deltaTime) override {
        // Increment the rotation angle based on the rotation speed
        rotationAngle += rotationSpeed * deltaTime;
        // Wrap the rotation angle to keep it within 0 to 360 degrees
        if (rotationAngle > 360.0f) {
            rotationAngle -= 360.0f;
        }
    }

    glm::mat4 getModelMatrix(const glm::mat4& initialModelMatrix) const override {
        glm::mat4 model = initialModelMatrix;
        model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around the y-axis

        return model;
    }


    bool isMouseOver(const glm::vec2& mousePos) const override {

        // Calculate distance between object and mouse position
        float distance = glm::length(glm::vec2(position.x, position.y) - mousePos);

        // Return whether the mouse is over the object
        return distance <= radius;
    }


    float getDistanceFromSun() const {
        return distanceFromSun;
    }
    const PlanetInfo& getInfo() const;

    bool getOrbiting() const {
        return isOrbiting;
    }
    float getOrbitRadius() const {
        return distanceFromSun;
    }



private:
    bool isOrbiting;
    float distanceFromSun;
    glm::vec3 color;
    glm::vec3 specularColor;
    float shininess;
    std::string diffuseTexture;
    std::string specularTexture;
    std::string normalTexture;
    std::string emissionTexture;
    float rotationSpeed;
    float rotationAngle;
};

#endif // PLANET_H
