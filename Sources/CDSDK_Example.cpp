#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "scene.h"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow* window, float deltaTime);
void MouseCallback(GLFWwindow* window, double xIn, double yIn);
void ScrollCallback(GLFWwindow* window, double xIn, double yIn);

Camera g_camera;

std::string GetUniformName(const uint32_t index, const char *member) {
    std::stringstream ss;
    ss << "u_lights[" << index << "]." << member;
    return ss.str();
}

void SetupCamera(const cd::SceneDatabase *pScene) {
    if(pScene->GetCameraCount()) {
        const auto &sceneCamera = pScene->GetCamera(0);
        const auto &position = sceneCamera.GetEye();
        const auto &lookAt = sceneCamera.GetLookAt();

        g_camera.m_position = { position.x(), position.y(), position.z() };
        g_camera.LookAt({ lookAt.x(), lookAt.y(), lookAt.z() });
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(800, 600, "CDSKD_Example", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader pbrShader("Shaders/vs_PBR.glsl", "Shaders/fs_PBR.glsl");
    GLScene scene;
    scene.LoadModel("Models/scene.cdbin");
    scene.SetShader(pbrShader);

    SetupCamera(scene.GetSene());

    float deltaTime = 0.0f;
    float lastFrameTime = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        // printf("\nCamera pos: x: %f", g_camera.m_position.x);
        // printf(" y: %f", g_camera.m_position.y);
        // printf(" z: %f", g_camera.m_position.z);

        float crtTime = glfwGetTime();
        deltaTime = crtTime - lastFrameTime;
        lastFrameTime = crtTime;

        ProcessInput(window, deltaTime);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(g_camera.m_zoom), 800.0f / 600.0f, 0.001f, 10000.0f);
        glm::mat4 view = g_camera.GetViewMatrix();
        glm::mat4 model(1.0f);
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

        pbrShader.Use();
        pbrShader.SetMat4("projection", projection);
        pbrShader.SetMat4("view", view);
        pbrShader.SetMat4("model", model);
        pbrShader.SetVec3("u_cameraPos", g_camera.m_position);

        const cd::SceneDatabase *pScene = scene.GetSene();
        if(pScene->GetLightCount()) {
            uint32_t index = 0;
            for(const auto &light : pScene->GetLights()) {
                const int type = static_cast<int>(light.GetType());
                const glm::vec3 position = { light.GetPosition().x(), light.GetPosition().y(), light.GetPosition().z() };
                const float intensity = light.GetIntensity();
                const glm::vec3 clolr = { light.GetColor().x(), light.GetColor().y(), light.GetColor().z() };
                const float range = light.GetRange();
                const glm::vec3 direction = { light.GetDirection().x(), light.GetDirection().y() , light.GetDirection().z() };

                pbrShader.SetInt(GetUniformName(index, "type"), type);
                pbrShader.SetVec3(GetUniformName(index, "position"), position);
                pbrShader.SetFloat(GetUniformName(index, "intensity"), intensity);
                pbrShader.SetVec3(GetUniformName(index, "color"), clolr);
                pbrShader.SetFloat(GetUniformName(index, "range"), range);
                pbrShader.SetVec3(GetUniformName(index, "direction"), direction);

                ++index;
            }
        }

        scene.Draw(pbrShader);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        g_camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        g_camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        g_camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        g_camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        g_camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        g_camera.ProcessKeyboard(DOWN, deltaTime);
}

void MouseCallback(GLFWwindow* window, double crt_x, double crt_y) {
    static float last_x = crt_x;
    static float last_y = crt_y;

    float delta_x = crt_x - last_x;
    float delta_y = last_y - crt_y;

    last_x = crt_x;
    last_y = crt_y;

    g_camera.ProcessMouseMovement(delta_x, delta_y);
}

void ScrollCallback(GLFWwindow* window, double xIn, double yIn) {
    g_camera.ProcessMouseScroll(static_cast<float>(yIn));
}
