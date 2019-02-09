#version 330 core
in vec3 Normal;
in vec3 Position;

out vec4 color;

uniform vec3 cameraPos;
uniform samplerCube skybox;

float ratio = 1.00 / 1.52;

void main()
{
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    color = texture(skybox, R);
}