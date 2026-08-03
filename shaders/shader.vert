#version 450

layout(location = 0) in vec3 vPos;  
layout(location = 1) in vec3 vColor; 
layout(location = 2) in vec2 vUV;
layout(location = 3) in uint vTexIndex;

layout(location = 1) out vec3 outColor;
layout(location = 2) out vec2 outUV;
layout(location = 3) out flat uint outTexIndex;

layout(binding = 0) uniform ArcBallCamera {
    mat4 view;
    mat4 proj;
};

void main() {
    gl_Position = proj * view * vec4(vPos,1.0);
    outColor = vColor;
    outUV = vUV;
    outTexIndex = vTexIndex;
}
