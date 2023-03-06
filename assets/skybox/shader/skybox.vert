#version 330 core

in vec3 aPos;

out vec3 textureDirection;

uniform mat4 VP;

void main() {
    vec4 position = VP * vec4(aPos, 1.0);
    
    // normalized device coordinates will then always have a z value equal to 1.0: the maximum depth value
    gl_Position = position.xyww;
    
    textureDirection = aPos;
}  