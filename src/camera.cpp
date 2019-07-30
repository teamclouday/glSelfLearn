#include "camera.hpp"

extern SDL_Window *myWindow;

Camera::Camera(glm::vec3 position, glm::vec3 up,
               GLfloat yaw, GLfloat pitch)
    : MovementSpeed(SPEED), MouseSenditivity(SENSITIVITY),
      Zoom(ZOOM), Front(glm::vec3(0.0f, 0.0f, -1.0f)), focus(false),
      keyMap(4, false), mousePos(2, 0)
{
    this->Position = position;
    this->WorldUp = up;
    this->Yaw = yaw;
    this->Pitch = pitch;
    this->updateCameraVectors();
}

Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ,
               GLfloat upX, GLfloat upY, GLfloat upZ,
               GLfloat yaw, GLfloat pitch)
    : MovementSpeed(SPEED), MouseSenditivity(SENSITIVITY),
      Zoom(ZOOM), Front(glm::vec3(0.0f, 0.0f, -1.0f)), focus(false),
      keyMap(4, false), mousePos(2, 0)
{
    this->Position = glm::vec3(posX, posY, posZ);
    this->WorldUp  = glm::vec3(upX, upY, upZ);
    this->Yaw   = yaw;
    this->Pitch = pitch;
    this->updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

void Camera::update(GLfloat deltaT)
{
    if(this->keyMap[0])
        this->ProcessKeyboard(FORWARD, deltaT);
    if(this->keyMap[1])
        this->ProcessKeyboard(LEFT, deltaT);
    if(this->keyMap[2])
        this->ProcessKeyboard(BACKWARD, deltaT);
    if(this->keyMap[3])
        this->ProcessKeyboard(RIGHT, deltaT);
    int x, y;
    SDL_GetMouseState(&x, &y);
    float xoffset = (float)(x - this->mousePos[0]);
    float yoffset = (float)(y - this->mousePos[1]);
    this->ProcessMouseMovement(xoffset, yoffset, GL_TRUE);
    SDL_GetWindowSize(myWindow, &x, &y);
    this->mousePos = {(int)(x/2), (int)(y/2)};
    SDL_WarpMouseInWindow(myWindow, this->mousePos[0], this->mousePos[1]);
}

void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
{
    GLfloat velocity = this->MovementSpeed * deltaTime;
    switch(direction)
    {
        case FORWARD:
            this->Position += this->Front * velocity;
            break;
        case BACKWARD:
            this->Position -= this->Front * velocity;
            break;
        case LEFT:
            this->Position -= this->Right * velocity;
            break;
        case RIGHT:
            this->Position += this->Right * velocity;
            break;
    }
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
    xoffset *= this->MouseSenditivity;
    yoffset *= this->MouseSenditivity;

    this->Yaw   += xoffset;
    this->Pitch -= yoffset;

    if(constrainPitch)
    {
        if(this->Pitch > 89.0f)
            this->Pitch = 89.0f;
        if(this->Pitch < -89.0f)
            this->Pitch = -89.0f;
    }

    this->updateCameraVectors();
}

void Camera::ProcessMouseScroll(GLfloat yoffset)
{
    if(this->Zoom >= 1.0f && this->Zoom <= 45.0f)
        this->Zoom -= yoffset;
    if(this->Zoom <= 1.0f)
        this->Zoom = 1.0f;
    if(this->Zoom >= 45.0f)
        this->Zoom = 45.0f;
}

void Camera::reset()
{
    this->Position = glm::vec3(0.0f, 1.0f, 5.0f);
    this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    this->Yaw = YAW;
    this->Pitch = PITCH;
    this->Zoom = ZOOM;
    this->MovementSpeed = SPEED;
    this->MouseSenditivity = SENSITIVITY;
    this->updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    front.y = sin(glm::radians(this->Pitch));
    front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    this->Front = glm::normalize(front);
    this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
    this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
}