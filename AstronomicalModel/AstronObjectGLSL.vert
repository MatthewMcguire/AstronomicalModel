#version 330
in vec3 vPosition;
uniform camera {
    mat4 modelvMatrix;
    mat4 projMatrix;
};
uniform mat4 objectTrans[9];
out vec4 colour;

void main() {
    gl_Position = projMatrix * modelvMatrix * objectTrans[gl_InstanceID] * vec4(vPosition,1.0);
    switch(gl_InstanceID)
    {
        case 0: // yellow
            colour = vec4(0.96,0.929,0.02,1.0);
            break;
        case 1: // red
            colour = vec4(0.8,0.1,0.1,1.0);
            break;
        case 2: // green
            colour = vec4(0.1,0.8,0.1,1.0);
            break;
        case 3: // blue
            colour = vec4(0.1,0.1,0.8,1.0);
            break;
        case 4: // whitish
            colour = vec4(0.8,0.8,0.8,1.0);
            break;
        case 5: //darker red
            colour = vec4(0.4,0.05,0.05,1.0);
            break;
        default: // grey
            colour = vec4(0.3,0.3,0.3,1.0);
            break;
    }
}