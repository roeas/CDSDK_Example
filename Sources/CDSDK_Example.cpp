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

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

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
    GLScene cdscene;
    cdscene.LoadModel("Models/scene.cdbin");
    cdscene.SetShader(pbrShader);

    float deltaTime = 0.0f;
    float lastFrameTime = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float crtTime = glfwGetTime();
        deltaTime = crtTime - lastFrameTime;
        lastFrameTime = crtTime;

        ProcessInput(window, deltaTime);

        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.m_zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model(1.0f);
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));

        pbrShader.Use();
        pbrShader.SetMat4("projection", projection);
        pbrShader.SetMat4("view", view);
        pbrShader.SetMat4("model", model);

        pbrShader.SetInt("u_lights[0].type", 0);
        pbrShader.SetVec3("u_lights[0].position", glm::vec3(4.0f, 0.0f, 0.0f));
        pbrShader.SetFloat("u_lights[0].intensity", 1024.0f);
        pbrShader.SetVec3("u_lights[0].color", glm::vec3(0.8f, 0.4f, 0.4f));
        pbrShader.SetFloat("u_lights[0].range", 1024.0f);

        pbrShader.SetInt("u_lights[1].type", 0);
        pbrShader.SetVec3("u_lights[1].position", glm::vec3(-4.0f, 0.0f, 0.0f));
        pbrShader.SetFloat("u_lights[1].intensity", 1024.0f);
        pbrShader.SetVec3("u_lights[1].color", glm::vec3(0.4f, 0.4f, 0.8f));
        pbrShader.SetFloat("u_lights[1].range", 1024.0f);

        pbrShader.SetInt("u_lights[2].type", 0);
        pbrShader.SetVec3("u_lights[2].position", glm::vec3(0.0f, -4.0f, 0.0f));
        pbrShader.SetFloat("u_lights[2].intensity", 1024.0f);
        pbrShader.SetVec3("u_lights[2].color", glm::vec3(0.4f, 0.8f, 0.4f));
        pbrShader.SetFloat("u_lights[2].range", 1024.0f);

        pbrShader.SetInt("u_lights[3].type", 1);
        pbrShader.SetFloat("u_lights[3].intensity", 4.0f);
        pbrShader.SetVec3("u_lights[3].color", glm::vec3(1.0f, 1.0f, 1.0f));
        pbrShader.SetFloat("u_lights[3].range", 1024.0f);
        pbrShader.SetVec3("u_lights[3].direction", glm::vec3(0.0f, 0.0f, -1.0f));

        cdscene.Draw(pbrShader);

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
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void MouseCallback(GLFWwindow* window, double crt_x, double crt_y) {
    static float last_x = crt_x;
    static float last_y = crt_y;

    float delta_x = crt_x - last_x;
    float delta_y = last_y - crt_y;

    last_x = crt_x;
    last_y = crt_y;

    camera.ProcessMouseMovement(delta_x, delta_y);
}

void ScrollCallback(GLFWwindow* window, double xIn, double yIn) {
    camera.ProcessMouseScroll(static_cast<float>(yIn));
}
