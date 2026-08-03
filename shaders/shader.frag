#version 450

layout(location = 1) in vec3 fragColor; 
layout(location = 2) in vec2 uv;
layout(location = 3) in flat uint texIndex;

layout(set = 1, binding = 0) uniform sampler2D tex[16];

layout(location = 0) out vec4 outColor; 

void main() {
    vec3 color = texture(tex[texIndex],uv).xyz;
    outColor = vec4(color, 1.0f); 
}
