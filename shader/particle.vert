#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

layout(location = 5) in vec3 instancePosition;
layout(location = 6) in vec4 instanceColor;
layout(location = 7) in float instanceSize;

layout(set = 0, )

layout(location = 0) out vec4 fragColor;

void main() {
    vec3 worldPos = (inPosition * instanceSize) + instancePosition;
    gl_Position = ubo.projection * ubo.view * vec4(worldPos, 1.0);
    fragColor = instanceColor;
}