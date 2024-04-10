#include "glad.h"
#include <GLFW/glfw3.h>

#include "glm.hpp"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Sphere.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);


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

    // build and compile shaders
    // -------------------------
    Shader ourShader("lighting.vs", "lighting.fs");
    Shader earthShader("shaders/earth.vs", "shaders/earth.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");
    Shader sunShader("shaders/sun.vs", "shaders/sun.fs");
    Shader moonShader("shaders/sun.vs", "shaders/sun.fs");
    Shader marsShader("shaders/sun.vs", "shaders/sun.fs");

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

    glm::vec3 mardsEmissiveColor = glm::vec3(1.0f, 0.5f, 0.5f);
    float marsEmissiveIntensity = 1.0f;

    // shader configuration
    // --------------------


    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    

    // load models
    // -----------
    Sphere sphere(0.0465f, numSectors, numStacks, smoothShading, 3);

    // sun object
    Sphere sun(10.0f,36, 16, true, 3);
    glm::vec3 sunPosition = glm::vec3(214.08045977f, 0.0f, 0.0f);
    glm::mat4 sunModelMatrix = glm::translate(glm::mat4(1.0f), sunPosition);

    Sphere moon(0.01f, 24, 9, true, 3);
    glm::vec3 moonPosition = glm::vec3(1.395f, 0.0f, 0.0f);
    glm::mat4 moonModelMatrix = glm::translate(glm::mat4(1.0f), moonPosition);


    Sphere mars(0.04, 24, 9, true, 3);
    glm::vec3 marsPosition = glm::vec3(-20.0f, 0.0f, 0.0f);
    glm::mat4 marsModelMatrix = glm::translate(glm::mat4(1.0f), marsPosition);

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

        sunShader.use();
        sunShader.setMat4("model", sunModelMatrix);
        sunShader.setMat4("view", view);
        sunShader.setMat4("projection", projection);
        sunShader.setVec3("emissiveColor", sunEmissiveColor* sunEmissiveIntensity);
        sun.draw();

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
        marsShader.setVec3("emissiveColor", mardsEmissiveColor * marsEmissiveIntensity);
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

        // Change the actual OpenGL cull mode based on the selection
        if (currentCullModeIdx == 0) {
            setCullMode(CULL_BACK);
        }
        else if (currentCullModeIdx == 1) {
            setCullMode(CULL_FRONT);
        }

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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime / 2);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
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
