#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout(location = 0) in vec2 inPosition[];
layout(location = 1) in vec2 inHalfExtent[];
layout(location = 2) in vec4 inColor[];

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outTexCoord;

void main() {
    vec2 position = inPosition[0];

    // top left
    gl_Position = vec4(position - inHalfExtent[0], 0.0, 1.0);
    outColor = inColor[0];
    outTexCoord = vec2(0.0, 0.0);
    EmitVertex();

    // top right
    gl_Position = vec4(position.x + inHalfExtent[0].x, position.y - inHalfExtent[0].y, 0.0, 1.0);
    outColor = inColor[0];
    outTexCoord = vec2(1.0, 0.0);
    EmitVertex();

    // bottom left
    gl_Position = vec4(position.x - inHalfExtent[0].x, position.y + inHalfExtent[0].y, 0.0, 1.0);
    outColor = inColor[0];
    outTexCoord = vec2(0.0, 1.0);
    EmitVertex();

    // bottom right
    gl_Position = vec4(position + inHalfExtent[0], 0.0, 1.0);
    outColor = inColor[0];
    outTexCoord = vec2(1.0, 1.0);
    EmitVertex();

    EndPrimitive();
}