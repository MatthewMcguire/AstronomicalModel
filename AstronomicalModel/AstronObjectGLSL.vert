#version 330
in vec3 vPosition;
in vec2 textureSTMap;
out vec2 textureSTMapFrag;
in vec3 vNormal;
uniform camera {
    mat4 modelvMatrix;
    mat4 projMatrix;
};
uniform mat4 objectTransform[9];
out vec4 colour;
varying int instanceID;

void main() {
    gl_Position = projMatrix * modelvMatrix * objectTransform[gl_InstanceID] * vec4(vPosition,1.0);
    instanceID =gl_InstanceID;
    if(gl_InstanceID == 0)
       textureSTMapFrag = vec2(textureSTMap.x, 0.5*textureSTMap.y);
    else
       textureSTMapFrag = vec2(textureSTMap.x, 0.5*textureSTMap.y+0.5);
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