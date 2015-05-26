#version 330
in vec3 vPosition;
uniform camera {
    mat4 modelvMatrix;
    mat4 projMatrix;
};
uniform mat4 objectTrans[9];

void main() {
    gl_Position = projMatrix * modelvMatrix * objectTrans[gl_InstanceID] * vec4(vPosition,1.0);
}