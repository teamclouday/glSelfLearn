#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <SOIL/SOIL.h>

#include "./include/Shader.hpp"
#include "./include/Mesh.hpp"
#include "./include/Camera.hpp"
#include "./include/Model.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "LearnOpenGL"

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void do_movement();

Camera camera(glm::vec3(0.0f, 10.0f, 80.0f));
bool keys[1024];
bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastX = 400;
GLfloat lastY = 300;

int main()
{
    // init glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    // create window
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    glfwMakeContextCurrent(window);
    // set callback
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // modern OpenGL
    glewExperimental = GL_TRUE;
    glewInit();
    // set OpenGL rendering size
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); 
    // enable depth test
    glEnable(GL_DEPTH_TEST);

    // load program
    Shader program = Shader("ModelVert.glsl", "ModelFrag.glsl");

    // load models
    Model planet = Model("./Models/planet/planet.obj");
    Model rock = Model("./Models/rock/rock.obj");

    // Set the object data (buffers, vertex attributes)

    // set VAO

    // set object view
    glm::mat4 projection(1.0f);
    glm::mat4 view(1.0f);
    projection = glm::perspective(45.0f, (GLfloat)WINDOW_WIDTH/(GLfloat)WINDOW_HEIGHT, 0.1f, 1000.0f); 

    // setup model properties
    GLuint amount = 1000;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    std::srand(glfwGetTime());
    GLfloat radius = 50.0f;
    GLfloat offset = 2.5f;
    for(GLuint i = 0; i < amount; i++)
    {
        glm::mat4 model(1.0f);
        GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;

        GLfloat displacement = (std::rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        GLfloat x = std::sin(angle) * radius + displacement;

        displacement = (std::rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        GLfloat y = displacement * 0.4f;

        displacement = (std::rand() % (GLint)(2 * offset * 100)) / 100.0f - offset;
        GLfloat z = std::cos(angle) * radius + displacement;

        model = glm::translate(model, glm::vec3(x, y, z));
        GLfloat scale = (std::rand() % 20) / 100.0f + 0.05f;
        model = glm::scale(model, glm::vec3(scale));
        GLfloat rotAngle = (std::rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
        modelMatrices[i] = model;
    }

    while(!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        do_movement();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = camera.GetViewMatrix();

        program.Use();
        glUniformMatrix4fv(glGetUniformLocation(program.programID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(program.programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(5.0f));
        glUniformMatrix4fv(glGetUniformLocation(program.programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        planet.draw(program);
        for(GLuint i = 0; i < amount; i++)
        {
            glUniformMatrix4fv(glGetUniformLocation(program.programID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrices[i]));
            rock.draw(program);
        }

        
        glfwSwapBuffers(window);
    }

    delete modelMatrices;
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if(key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;
    }
    if(key == GLFW_KEY_R)
        camera.reset();
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset*0.05f);
}

void do_movement()
{
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
}