#include "glad.h"
#include <GLFW/glfw3.h>

#include "glm.hpp"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Sphere.h"

#include "spaceobject.h"
#include "sun.h"
#include "planet.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
std::vector<glm::vec3> generateCircleVertices(float radius, int numSegments, glm::vec3 offset);
bool RaySphereIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& sphereCenter, float sphereRadius);
void drawOrbitLine(float radius, int segments);
// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
bool wireframeMode = false;

enum CullMode {
    CULL_BACK = GL_BACK,
    CULL_FRONT = GL_FRONT
};

void setCullMode(CullMode mode) {
    glEnable(GL_CULL_FACE); // Enable face culling
    glCullFace(mode);       // Set the cull mode
}


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool cursorEnabled = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// sphere
int numStacks = 18;
int numSectors = 36;
bool smoothShading = true;
float marsOffset = 0;
float marsRotation = 90.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);


    std::vector<SpaceObject*> spaceObjects = {
        new Planet(glm::vec3(0.0f), 5.0f, "Mercury",true, 0.383f, glm::vec3(0.8f, 0.6f, 0.4f), glm::vec3(0.0f), 0.0f, "resources/textures/planets/mercury/mercury_diffuse.jpg", "", "", ""),
        new Planet(glm::vec3(0.0f), 9.0f, "Venus", true,0.949f, glm::vec3(0.9f, 0.8f, 0.6f), glm::vec3(0.0f), 0.0f, "resources/textures/planets/venus/venus_diffuse.jpg", "", "", ""),
        new Planet(glm::vec3(0.0f), 12.0f, "Earth", true,1.0f, glm::vec3(0.6f, 0.7f, 1.0f), glm::vec3(0.0f), 0.0f, "resources/textures/planets/earth/earth_diffuse.jpg", "", "", ""),
        new Planet(glm::vec3(0.0f), 15.0f, "Mars", true,0.532f, glm::vec3(0.9f, 0.5f, 0.2f), glm::vec3(0.0f), 0.0f, "resources/textures/planets/mars/mars_diffuse.jpg", "", "", ""),
        new Planet(glm::vec3(0.0f), 25.0f, "Jupiter", true,11.21f, glm::vec3(0.8f, 0.6f, 0.4f), glm::vec3(0.0f), 0.0f, "resources/textures/planets/jupiter/jupiter_diffuse.jpg", "", "", ""),
        new Planet(glm::vec3(0.0f), 35.0f, "Saturn", true,9.45f, glm::vec3(0.8f, 0.7f, 0.6f), glm::vec3(0.0f), 0.0f, "resources/textures/planets/saturn/saturn_diffuse.jpg", "", "", ""),
        new Planet(glm::vec3(0.0f), 45.0f, "Uranus", true,4.01f, glm::vec3(0.6f, 0.8f, 0.9f), glm::vec3(0.0f), 0.0f, "resources/textures/planets/uranus/uranus_diffuse.jpg", "", "", ""),
        new Planet(glm::vec3(0.0f), 55.0f, "Neptune", true,3.88f, glm::vec3(0.2f, 0.4f, 0.9f), glm::vec3(0.0f), 0.0f, "resources/textures/planets/neptune/neptune_diffuse.jpg", "", "", ""),
        // Add other planets here
        new Sun("Sun", 0.0f, 0, 10.0f, glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), 1.0f, "resources/textures/sun.jpg", "", "", "")
    };



    // build and compile shaders
    // -------------------------
    Shader ourShader("lighting.vs", "lighting.fs");
    Shader earthShader("shaders/earth.vs", "shaders/earth.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");
    Shader sunShader("shaders/sun.vs", "shaders/sun.fs");
    Shader moonShader("shaders/sun.vs", "shaders/sun.fs");
    Shader marsShader("shaders/sun.vs", "shaders/sun.fs");
    Shader circleShader("circle.vs", "circle.fs"); // Assuming you have a Shader class for loading shaders


    float skyboxVertices[] = {
        // positions          
        // Right face (+X)
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        // Left face (-X)
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        // Top face (+Y)
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        // Bottom face (-Y)
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,

         // Front face (+Z)
         1.0f, -1.0f,  1.0f,
         -1.0f, -1.0f,  1.0f,
         -1.0f,  1.0f,  1.0f,
         -1.0f,  1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,
          1.0f, -1.0f,  1.0f,

          // Back face (-Z)
          -1.0f, -1.0f, -1.0f,
           1.0f, -1.0f, -1.0f,
           1.0f,  1.0f, -1.0f,
           1.0f,  1.0f, -1.0f,
          -1.0f,  1.0f, -1.0f,
          -1.0f, -1.0f, -1.0f
    };


    //skybox VAO

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // load textures
// -------------

    unsigned int moonTexture = loadTexture("resources/textures/planets/moon/moon_diffuse.jpg");

    unsigned int earthTexture = loadTexture("resources/textures/planets/earth/earth_diffuse.jpg");
    unsigned int earthNormal = loadTexture("resources/textures/planets/earth/earth_normal.jpg");
    unsigned int earthCloudTexture = loadTexture("resources/textures/planets/earth/earth_clouds.jpg");
    unsigned int earthSpecular = loadTexture("resources/textures/planets/earth/earth_specular.jpg");

    // sun material
    // ------------
    glm::vec3 sunEmissiveColor = glm::vec3(1.0f, 1.0f, 0.2f);
    float sunEmissiveIntensity = 1.0f;

    glm::vec3 moonEmissiveColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float moonEmissiveIntensity = 0.5f;

    glm::vec3 marsEmissiveColor = glm::vec3(1.0f, 0.5f, 0.5f);
    float marsEmissiveIntensity = 1.0f;

    // shader configuration
    // --------------------


    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    

    // load models
    // -----------
    Sphere sphere(0.0465f, numSectors, numStacks, smoothShading, 3);

    // sun object
    Sphere sun(1.0f, 36, 16, true, 3);
    glm::vec3 sunPosition = glm::vec3(0.0f, 0.0f, 0.0f); // Position the sun at the center
    glm::mat4 sunModelMatrix = glm::translate(glm::mat4(1.0f), sunPosition);





    glm::vec3 moonPosition = glm::vec3(1.395f, 0.0f, 0.0f);
    Sphere moon(1.0f, 24, 9, true, 3);
    glm::mat4 moonModelMatrix = glm::translate(glm::mat4(1.0f), moonPosition);


    Sphere mars(1.0f, 24, 9, true, 3);
    glm::vec3 marsPosition = glm::vec3(10.0f, 0.0f, 0.0f);
    glm::mat4 marsModelMatrix = glm::translate(glm::mat4(1.0f), marsPosition );




    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::Spectrum::StyleColorsSpectrum();
    ImGui::Spectrum::LoadFont(18);

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::vector<float> frameTimes;
    float totalTime = 0.0f;
    const size_t maxFrames = 60;
    int currentCullModeIdx = 0; // 0 for GL_BACK, 1 for GL_FRONT

    unsigned int circleVAO, circleVBO;
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);
    glBindVertexArray(circleVAO);






    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Update the buffer
        frameTimes.push_back(deltaTime);
        totalTime += deltaTime;
        if (frameTimes.size() > maxFrames)
        {
            totalTime -= frameTimes.front();
            frameTimes.erase(frameTimes.begin());
        }

        setCullMode(CULL_BACK);
        // Calculate the average FPS
        float averageFPS = frameTimes.size() / totalTime;

        // input
        // -----
        processInput(window);
        




        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);

        switch (currentCullModeIdx) {
        case 0:
            glCullFace(GL_BACK);
            break;
        case 1:
            glCullFace(GL_FRONT);
            break;
        default:
            // You can set a default cull face or handle this case as needed
            glCullFace(GL_BACK);
            break;
        }

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5000.0f); // Increase zFar to 5000.0f

        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Convert to NDC
        float ndcX = (2.0f * mouseX) / SCR_WIDTH - 1.0f;
        float ndcY = 1.0f - (2.0f * mouseY) / SCR_HEIGHT;

        // Convert to world space
        glm::vec4 rayClip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
        glm::vec4 rayEye = glm::inverse(projection) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
        glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
        rayWorld = glm::normalize(rayWorld);

        // Render the ray
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex3f(camera.Position.x, camera.Position.y, camera.Position.z);
        glVertex3f(camera.Position.x + rayWorld.x * 100.0f, camera.Position.y + rayWorld.y * 100.0f, camera.Position.z + rayWorld.z * 100.0f);
            
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        //ourModel.Draw(ourShader);


        earthShader.use();
        earthShader.setMat4("projection", projection);
        earthShader.setMat4("view", view);
        earthShader.setMat4("model", model);
        earthShader.setInt("earthTexture", 0);
        earthShader.setInt("earthNormalMap", 1);
        earthShader.setInt("earthCloudTexture", 2);
        earthShader.setInt("earthSpecular", 3);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, earthTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, earthNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, earthCloudTexture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, earthSpecular);

        sphere.draw();


        // Update the sun's model matrix
        sunModelMatrix = glm::translate(glm::mat4(1.0f), sunPosition);

        // Render the sun
        sunShader.use();
        sunShader.setMat4("model", sunModelMatrix);
        sunShader.setMat4("view", view);
        sunShader.setMat4("projection", projection);
        sunShader.setVec3("emissiveColor", sunEmissiveColor * sunEmissiveIntensity);
        sun.draw();



        // Render the moon
        moonShader.use();
        moonShader.setMat4("model", moonModelMatrix);
        moonShader.setMat4("view", view);
        moonShader.setMat4("projection", projection);
        moonShader.setVec3("emissiveColor", moonEmissiveColor * moonEmissiveIntensity); // Set moon's emissive color
        moon.draw();

        marsShader.use();
        marsShader.setMat4("model", marsModelMatrix);
        marsShader.setMat4("view", view);
        marsShader.setMat4("projection", projection);
        marsShader.setVec3("emissiveColor", marsEmissiveColor * marsEmissiveIntensity);
        mars.draw();


        const char* cullModeItems[] = { "Front face", "Back Face" };

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        bool my_tool_active = true;


        // Render the ImGUI components
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::Begin("S3 Settings", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Simulation"))
            {
                if (ImGui::MenuItem("Start/Stop Simulation", "Space")) { /* Toggle simulation */ }
                if (ImGui::MenuItem("Speed Up", "Up Arrow")) { /* Increase simulation speed */ }
                if (ImGui::MenuItem("Slow Down", "Down Arrow")) { /* Decrease simulation speed */ }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Save", "Ctrl+S", false)) { /* Do stuff */ }
                if (ImGui::MenuItem("Close", "Escape")) { glfwSetWindowShouldClose(window, true); }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Toggle Labels")) { /* Show or hide labels */ }
                if (ImGui::MenuItem("Change Perspective")) { /* Switch viewing perspective */ }
                if (ImGui::MenuItem("Show Orbits")) { /* Show or hide orbits */ }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("GitHub Documentation", "Ctrl+H", false)) { /* do stuff to link to that here*/ }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // gui container for the camera and mouse position
        ImGui::BeginChild("Camera", ImVec2(0, 200), true);
        ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", camera.Position.x, camera.Position.y, camera.Position.z);
        ImGui::Text("Camera Front: (%.2f, %.2f, %.2f)", camera.Front.x, camera.Front.y, camera.Front.z);
        ImGui::Text("Mouse Position: (%.2f, %.2f)", mouseX, mouseY);
        ImGui::EndChild();


        // Set a size for the child window to ensure it's visible
        ImGui::BeginChild("Planets", ImVec2(0, 200), true);
        ImGui::Text("FPS: %.1f", averageFPS);
        ImGui::SliderInt("Stacks", &numStacks, 2, 50); // Adjust the min and max values as needed
        ImGui::SliderInt("Sectors", &numSectors, 3, 50);
        ImGui::Checkbox("Smooth shading", &smoothShading);
        ImGui::ColorEdit3("Emissive Color", glm::value_ptr(sunEmissiveColor));
        ImGui::SliderFloat("Emissive Intensity", &sunEmissiveIntensity, 0.0f, 10.0f);
        ImGui::EndChild();

        // Add some spacing between child windows
        ImGui::Spacing();

        // Set a size for the second child window
        ImGui::BeginChild("Program", ImVec2(0, 100), true);
        ImGui::Checkbox("Wireframe Mode", &wireframeMode);
        ImGui::Combo("Cull Mode", &currentCullModeIdx, cullModeItems, IM_ARRAYSIZE(cullModeItems));
        ImGui::SliderFloat("Mars orbit offset", &marsOffset, -5.0f, 5.0f);

        // Change the actual OpenGL cull mode based on the selection
        if (currentCullModeIdx == 0) {
            setCullMode(CULL_BACK);
        }
        else if (currentCullModeIdx == 1) {
            setCullMode(CULL_FRONT);
        }

        ImGui::EndChild();

        ImGui::BeginChild("Keybinds");


        ImGui::Text("W, A, S, D - Move camera");
        ImGui::Text("Hold Left Shift - Double camera speed");

        ImGui::EndChild();


        ImGui::End();


        static int prevStacks = numStacks;
        if (numStacks != prevStacks) {
            sphere.setStackCount(numStacks);
            sphere.buildVerticesSmooth();
            prevStacks = numStacks;
        }

        static int prevSectors = numSectors;
        if (numSectors != prevSectors) {
            sphere.setSectorCount(numSectors);
            sphere.buildVerticesSmooth();
            prevSectors = numSectors;
        }

        static bool prevShading = smoothShading;
        if (smoothShading != prevShading) {
            sphere.setSmooth(smoothShading);
            prevShading = smoothShading;
        }


        glm::vec3 rayOrigin = camera.Position; // Assuming camera.Position is the ray origin
        glm::vec3 rayDirection = rayWorld;     // Assuming rayWorld is the ray direction


        // Check for intersection with each planet
        for (const auto& planet : spaceObjects) {
            // Check if the current space object is a planet
            if (auto* p = dynamic_cast<Planet*>(planet)) {
                // Perform ray-sphere intersection test
                if (RaySphereIntersect(rayOrigin, rayDirection, p->getPosition(), p->getRadius())) {
                    // Collision detected, handle it (e.g., show information about the planet)
                    ImGui::OpenPopup(p->getName().c_str());
                    if (ImGui::BeginPopupModal(p->getName().c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        ImGui::Text("You are hovering over %s!", p->getName().c_str());
                        ImGui::EndPopup();
                    }
                }
            }
        }

        // for each planet that has orbiting enabled, draw the orbit line
        for (const auto& planet : spaceObjects) {
            if (auto* p = dynamic_cast<Planet*>(planet)) {
                if (p->getOrbiting()) {
					drawOrbitLine(p->getOrbitRadius(), 24);
				}
			}
		}
   


        // Rendering ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup ImGui
    // -------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}

void drawOrbitLine( float radius, int segments) {

    glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

    unsigned int orbitVAO, orbitVBO;

    std::vector<glm::vec3> orbitVertices = generateCircleVertices(radius, segments, center);
    
    glGenVertexArrays(1, &orbitVAO);
    glGenBuffers(1, &orbitVBO);
    glBindVertexArray(orbitVAO);
    glBindBuffer(GL_ARRAY_BUFFER, orbitVBO);
    glBufferData(GL_ARRAY_BUFFER, orbitVertices.size() * sizeof(glm::vec3), orbitVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);




    // Render the orbit circle
    glBindVertexArray(orbitVAO);
    glDrawArrays(GL_LINE_LOOP, 0, segments);

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{

    // Close the window when the user presses the Escape key
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // change the camera speed if the user holds the left shift key
    float cameraSpeed = camera.MovementSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cameraSpeed *= 2.0f; // Double the camera speed
    }


    // WASD movement controls
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, cameraSpeed);

    // Toggle the cursor when the user presses the Tab key
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        cursorEnabled = !cursorEnabled;
        glfwSetInputMode(window, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (!cursorEnabled) { // Only process mouse movement if the cursor is disabled
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        // Calculate the offset of the mouse position
        // This is done to prevent the camera from jumping when the cursor is disabled
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!cursorEnabled) { // Only process scrolling if the cursor is disabled
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}



// the loadTexture function is to load a texture from a file path into a texture object in OpenGL
unsigned int loadTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip the textures on the y-axis
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

std::vector<glm::vec3> generateCircleVertices(float radius, int numSegments, glm::vec3 offset) {
    std::vector<glm::vec3> vertices;

    // define the angle between each segment
    float angleIncrement = 2.0f * glm::pi<float>() / numSegments;

    // for each segment, calculate the x and z coordinates 
    // of the circle based on the radius and angle
    for (int i = 0; i < numSegments; ++i) {
        float angle = i * angleIncrement;
        float x = radius * glm::cos(angle) + offset.x;
        float y = offset.y;
        float z = radius * glm::sin(angle) + offset.z;
        vertices.push_back(glm::vec3(x, y, z));
    }
    return vertices;
}


bool RaySphereIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& sphereCenter, float sphereRadius)
{
    glm::vec3 L = sphereCenter - rayOrigin;
    float tca = glm::dot(L, rayDirection);
    float d2 = glm::dot(L, L) - tca * tca;
    float radius2 = sphereRadius * sphereRadius;
    if (d2 > radius2) return false;
    float thc = sqrt(radius2 - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;
    if (t0 < 0 && t1 < 0) return false; // Both intersections are behind the ray
    return true;
}
