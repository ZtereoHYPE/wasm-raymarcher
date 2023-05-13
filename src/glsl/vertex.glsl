#version 300 es

// specify 32 bit floats
precision highp float;

in vec2 a_position;

out vec2 v_texCoord;

void main() {
    v_texCoord = vec2(a_position.x, -a_position.y) * 0.5 + 0.5;
    gl_Position = vec4(a_position, 0.0, 1.0);
}