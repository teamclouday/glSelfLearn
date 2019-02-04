#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
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
    glDepthFunc(GL_LESS);
    // enable stencil test
    glEnable(GL_STENCIL_TEST);

    // load program
    Shader program = Shader("ModelVert.glsl", "ModelFrag.glsl");
    if(!program.exits())
    {
        printf("Failed to load program!\n");
        glfwTerminate();
        return -2;
    }

    Model programModel("./Models/nanosuit/nanosuit.obj");
    if(!programModel.exists())
    {
        printf("Failed to load the model!\n");
        glfwTerminate();
        return -3;
    }
    
    // set object view
    glm::mat4 projection(1.0f);
    glm::mat4 view(1.0f);

    while(!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        do_movement();

        glClearColor(0.0f, 0.01f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        projection = glm::perspective(camera.Zoom, (GLfloat)WINDOW_WIDTH/(GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();

        program.Use();
        program.setMatrix4fv("projection", glm::value_ptr(projection));
        program.setMatrix4fv("view", glm::value_ptr(view));
        program.setf3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

        // add dirLight
        program.setf3("dirLight.direction", 0.2f, 0.3f, 0.4f);
        program.setf3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
        program.setf3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
        program.setf3("dirLight.specular", 1.0f, 1.0f, 1.0f);
        // add pointLights
        program.setf3("pointLights[0].position", -1.0f, 0.9f, -1.7f);
        program.setf1("pointLights[0].constant", 1.0f);
        program.setf1("pointLights[0].linear", 0.09);
        program.setf1("pointLights[0].quadratic", 0.032);
        program.setf3("pointLights[0].ambient", 0.1f, 0.1f, 0.1f);
        program.setf3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        program.setf3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        
        program.setf3("pointLights[1].position", -0.9f, 1.2f, 1.3f);
        program.setf1("pointLights[1].constant", 1.0f);
        program.setf1("pointLights[1].linear", 0.09);
        program.setf1("pointLights[1].quadratic", 0.032);
        program.setf3("pointLights[1].ambient", 0.1f, 0.1f, 0.1f);
        program.setf3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        program.setf3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);

        program.setf3("pointLights[2].position", 1.8f, -0.9f, 0.4f);
        program.setf1("pointLights[2].constant", 1.0f);
        program.setf1("pointLights[2].linear", 0.09);
        program.setf1("pointLights[2].quadratic", 0.032);
        program.setf3("pointLights[2].ambient", 0.1f, 0.1f, 0.1f);
        program.setf3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        program.setf3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        // add spotlight
        program.setf3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);
        program.setf3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);
        program.setf1("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        program.setf1("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
        program.setf3("spotLight.ambient", 0.1f, 0.1f, 0.1f);
        program.setf3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
        program.setf3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        program.setf1("spotLight.constant", 1.0f);
        program.setf1("spotLight.linear", 0.09f);
        program.setf1("spotLight.quadratic", 0.032f);

        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        program.setMatrix4fv("model", glm::value_ptr(model));
        programModel.draw(program);

        glfwSwapBuffers(window);
    }
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