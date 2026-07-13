#version 450

layout(location = 0) in vec3 vPos;  
layout(location = 1) in vec3 vColor; 

layout(location = 1) out vec3 outColor;

layout(binding = 0) uniform ArcBallCamera {
    mat4 view;
    mat4 proj;
};

void main() {
    gl_Position = proj * view * vec4(vPos,1.0);
    outColor = vColor;
}
