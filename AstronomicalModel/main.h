//
//  main.h
//  AstronomicalModel
//
//  Created by Matthew McGuire on 5/22/15.
//  Copyright (c) 2015 Matthew McGuire. All rights reserved.
//

#ifndef AstronomicalModel_main_h
#define AstronomicalModel_main_h

#include "lib3D.h"
#include "BetterSphere.h"
#include "AstronObject.h"


// Sphere and Solar system objects are initialized
BetterSphere modelSphere(10,10,1.0);
AstroGroup solarSystem;

//**************************************************
/*@@##====--- OpenGL parameters (BEGIN) ---====##@@*/
const int numVAO = 5;
const int numBuffers = 14;
const int numUBuffs = 4;
GLuint VertexArrayID[numVAO];       //  Array of Vertex Array Objects
GLuint program[1];                  //  max. number of shader programs
GLuint shaderBuffer[numBuffers];    //  Array of ordinary shader buffers
GLuint attribLocation[8];           //  Array of shader attribute locations
GLint uniformLocation[9];           //  Array of uniform variable locations
GLuint textureName[2];
GLuint uBlockIndex[numUBuffs];      //  Array of Uniform buffer block names
GLint uBlockSize[numUBuffs];        //  Sizes of Uniform buffer blocks
GLuint uBlockBinding​[numUBuffs];
GLubyte * uBufferCamera;
enum PolygonModes {LINE, SURFACE, POINT};
PolygonModes polygonModeToggle = LINE;
/*@@##====--- OpenGL parameters (END) ---====##@@*/

//************************************************
/*@@##====--- GLFW parameters (BEGIN) ---====##@@*/
GLuint mainWinWidth = 850;
GLuint mainWinHeight = 850;
GLfloat halfWinWidth = mainWinWidth/2.0;
GLfloat halfWinHeight = mainWinHeight/2.0;
char windowName[50];
GLuint shaderProg;
GLFWwindow* mainWin;
GLdouble fps[2], nowFPS;
GLuint fpsCounter = 0;
/*@@##====--- GLFW parameters (END) ---====##@@*/

//************************************************************************
/*@@##====--- Model, View, and Interaction parameters (BEGIN) ---====##@@*/
matr4 modelvMatrix;
matr4 projMatrix;
enum {ModelvMatrix,ProjMatrix,numCameraBlockVars};
GLubyte * modelMatrixAddr;
GLubyte * projMatrixAddr;
size_t uVarMemorySize[numCameraBlockVars];

// the camera begins out on the positive z-axis, looking at the origin
GLfloat camEyeθ = M_PI;         // θ is measured around the x-z plane, begins at pi
GLfloat camEyeφ = (M_PI/2.0);   // φ is measured downward from the positive-y axis, begins at pi/2
GLfloat camEyeR = 4.0f;         // R is the 'radius': the distance from the origin to the camera
point3 camEye = euclidCamera(camEyeR,camEyeθ,camEyeφ);
point3 camRight = {cos(camEyeθ),0,-sin(camEyeθ)};   // to orient the camera, the vector directly to right
point3 camAt = point3(0.0f,0.0f,0.0f);
point3 camUp = point3(0.0f,1.0f,0.0f);

GLfloat frFOV =20.0;                // field of view for perspective frustrum
GLfloat frAspect = 1.0f;            // aspect ratio for perspective frustrum
GLfloat frNear = 0.1f;              // near side of perspective frustrum
GLfloat frFar = 6000.0f;            // far side of perspective frustrum

GLfloat accelFactor = 0.2f;
GLfloat zoomFactor = 0.15f;
GLfloat rShift;                 // magnitude of mouse scroll event
GLboolean donePausing = GL_TRUE;
GLdouble lastPauseEnd;
GLdouble pauseLength = 0.02;    // update wait duration (in seconds) for mouse-driven events

/*@@##====--- Model and View parameters (END) ---====##@@*/


//*********************************************************
/*@@##====--- General helper functions (BEGIN) ---====##@@*/
void togglePolyMode(void)
{
    switch(polygonModeToggle) {
        case LINE:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        polygonModeToggle = SURFACE;
        break;
        case SURFACE:
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        polygonModeToggle = POINT;
        break;
        case POINT:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        polygonModeToggle = LINE;
        break;
    }
}
/*@@##====--- General helper functions (END) ---====##@@*/

//********************************************************
/*@@##====--- GLFW Callback functions (BEGIN) ---====##@@*/
// Quit callback (window or program termination)
void quitApp(GLFWwindow *mainWin)
{
    glfwDestroyWindow(mainWin);
    glfwTerminate();
    exit(0);
}
// Keyboard callback (ascii input)
void asciiTyping(GLFWwindow* mainWin, unsigned int key)
{
    switch (key) {
        case 'Q':
        case 'q':
        quitApp(mainWin);
        break;
        case '+':
        case '=':
        break;
        case '_':
        case '-':
        break;
        case 'w':
        break;
        case 'e':
        break;
        case 's':
        break;
        case 'd':
        break;
        case 'x':
        break;
        case 'c':
        break;
        case 'm':
        togglePolyMode();
        break;
        case 'r':
        break;
        default:
        break;
    }
}
// Keyboard callback (special non-ascii keys)
void specialKeyTyping(GLFWwindow* mainWin, int key, int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE) { // only take action when the key is released
        return;
    }
    switch (key) {
        case GLFW_KEY_ESCAPE:
        quitApp(mainWin);
        break;
        case GLFW_KEY_UP:
        break;
        case GLFW_KEY_DOWN:
        break;
        case GLFW_KEY_LEFT:
        break;
        case GLFW_KEY_RIGHT:
        break;
        case GLFW_KEY_SPACE:
        default:
        break;
    }
}
// Window size change callback
void windowReshape(GLFWwindow* window, int width, int height)
{
    mainWinWidth = width;
    mainWinHeight = height;
    halfWinHeight = mainWinWidth/2.0;
    halfWinHeight = mainWinHeight/2.0;
}
void screenCursor(GLFWwindow* mainWin, double xpos, double ypos)
{

}
void mouseFunc(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS);
}
void scrollFunc(GLFWwindow* mainWin, double xOffset, double yOffset)
{
    rShift = sqrt(fabs(yOffset)) * sgn(yOffset) * accelFactor;
    camEyeR *= (1.0+rShift);
}
/*@@##====--- GLFW Callback functions (END) ---====##@@*/

void moveCamera(void)
{
//    // get current mouse cursor
//    // normalize to [-1,1]
//    GLfloat displacedHorizontal = (currentCursor[0]-halfWinWidth)/halfWinWidth;
//    GLfloat displacedVertical = (currentCursor[1]-halfWinHeight)/halfWinHeight;
//
//    // compute acceleration
//    // modify polar angles
//    camEyeθ += fabs(accelFactor*pow(displacedHorizontal,2.0))* sgn(displacedHorizontal);
//    camEyeφ += fabs(accelFactor*pow(displacedVertical,2.0))* sgn(displacedVertical);
//    camEyeθ = smallPiBound(camEyeθ);
//    camEyeφ = smallPiBound(camEyeφ);
//    camRight = {cos(camEyeθ),0,-sin(camEyeθ)};
//    camUp = glm::cross(camEye,camRight);
}

void initGLFW()
{
    glfwSetErrorCallback(errorCallb);
    if (!glfwInit()) {
        std::cout << "GLFW failed to initialize!\n";
        exit(4);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    glfwWindowHint(GLFW_SAMPLES, 16);
    strcpy(windowName,"Solar System");
    mainWin = glfwCreateWindow(mainWinWidth, mainWinHeight, windowName, NULL, NULL);
    if (!mainWin) {
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(mainWin);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //    /* With Retina on Mac OS X, GLFW's FramebufferSize dne WindowSize */
    //    glfwGetFramebufferSize(wind, &fbwidth, &fbheight);
    //
    //    // call the reshape first, since it is not called at window creation
    //    windowReshape(wind, fbwidth, fbheight);  // shape view port
    //--- register callbacks with GLFW ----
    //    /* function to handle window resizes */
    //    glfwSetFramebufferSizeCallback(wind, fbreshape);
    glfwSetWindowSizeCallback(mainWin, windowReshape);
    glfwSetWindowCloseCallback(mainWin,quitApp);  // for window close
    glfwSetKeyCallback(mainWin, specialKeyTyping);
    glfwSetCharCallback(mainWin, asciiTyping);
    glfwSetCursorPosCallback(mainWin, screenCursor);
    glfwSetScrollCallback(mainWin, scrollFunc);
    glfwSetMouseButtonCallback(mainWin, mouseFunc);
    glfwSwapInterval(0);    // allows more than 60 FPS
}
void initOpenGL()
{
    glEnable(GL_DEPTH_TEST);
    program[0] = prepareShaders("AstronObjectGLSL.vert", "AstronObjectGLSL.frag");

    /*--- (BEGIN) Create VAO and buffer stuff  ---*/
    glGenBuffers(numBuffers,shaderBuffer);
    glGenVertexArrays(numVAO,VertexArrayID);
    /*--- (END) Create VAO and buffer stuff  ---*/
//
//    /*--- (BEGIN) Sphere Preparation  ---*/
//    // program 0 has shaders that do perspective and lighting model
//    // program 0 will use VAO 0
//    glUseProgram(program[0]);
//    // Sphere Indices and Vertices
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaderBuffer[0]);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(calcio.indices[0]) * calcio.numIndices),&calcio.indices.front(),GL_STATIC_DRAW);
//    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[1]);
//    glBufferData(GL_ARRAY_BUFFER, ((sizeof(calcio.vertices[0])+sizeof(calcio.norms[0])) * calcio.numVertices),NULL,GL_STATIC_DRAW);
//    glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(calcio.vertices[0]) * calcio.numVertices), &calcio.vertices.front());
//    glBufferSubData(GL_ARRAY_BUFFER, (sizeof(calcio.vertices[0]) * calcio.numVertices), (sizeof(calcio.norms[0]) * calcio.numVertices), &calcio.norms.front());
//
//    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[6]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(calcio.stMap[0])*calcio.numVertices,&calcio.stMap.front(), GL_STATIC_DRAW);
//
//    // Vertex Array Object 0 is for the Sphere
//    glBindVertexArray(VertexArrayID[0]);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaderBuffer[0]);
//    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[1]);
//    attribLocation[0] = glGetAttribLocation(program[0], "vPosition");
//    glVertexAttribPointer(attribLocation[0],3,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));
//
//    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[6]);
//    attribLocation[4] = glGetAttribLocation(program[0], "stMap");
//    glVertexAttribPointer(attribLocation[4],2,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));
//    glBindVertexArray(0);
//    /*--- (END) Sphere Preparation  ---*/
//
//    /*--- (BEGIN) Panel Preparation  ---*/
//    // program 1 has shaders that draw a textured panel without perspective
//    // program 1 will use VAO 1
//    glUseProgram(program[1]);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaderBuffer[2]);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Histo.bgIndices[0])*Histo.numBgIndices,&Histo.bgIndices.front(),GL_STATIC_DRAW);
//    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[3]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Histo.bgVertices[0])*Histo.numBgVertices,&Histo.bgVertices.front(),GL_STATIC_DRAW);
//
//    // Vertex Array Object 1 is for the Panel
//    glBindVertexArray(VertexArrayID[1]);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaderBuffer[2]);
//    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[3]);
//    attribLocation[1] = glGetAttribLocation(program[1], "vPosition");
//    glVertexAttribPointer(attribLocation[1],2,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));
//
//    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[5]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Histo.stMap[0])*Histo.numBgVertices,&Histo.stMap.front(), GL_STATIC_DRAW);
//    attribLocation[3] = glGetAttribLocation(program[1], "stMap");
//    glVertexAttribPointer(attribLocation[3],2,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));
//    glBindVertexArray(0);
//    /*--- (END) Panel Preparation  ---*/
//
//    /*--- (BEGIN) Slider Preparation  ---*/
//    // program 2 has shaders that draw panel sliders without perspective
//    // program 2 will use VAO 2
//    glUseProgram(program[2]);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaderBuffer[7]);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Histo.sliderIndices[0])*Histo.numSliderIndices,&Histo.sliderIndices.front(),GL_STATIC_DRAW);
//    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[8]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Histo.sliderVertices[0])*Histo.numSliderVertices,&Histo.sliderVertices.front(),GL_STATIC_DRAW);
//
//    // Vertex Array Object 2 is for the Sliders
//    glBindVertexArray(VertexArrayID[2]);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaderBuffer[7]);
//    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[8]);
//    attribLocation[2] = glGetAttribLocation(program[2], "vPosition");
//    glVertexAttribPointer(attribLocation[2],2,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));
//
//    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[11]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Histo.stMapSlider[0])*Histo.numSliderVertices,&Histo.stMapSlider.front(), GL_STATIC_DRAW);
//    attribLocation[6] = glGetAttribLocation(program[2], "stMapSlider");
//    glVertexAttribPointer(attribLocation[6],2,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));
//    uniformLocation[3] = glGetUniformLocation(program[2], "whichSlider");
//    uniformLocation[4] = glGetUniformLocation(program[2], "sliderOffsetsHorizontal");
//    glUniform2f(uniformLocation[4], sliderOffset[0],sliderOffset[1]);
//    glBindVertexArray(0);
//    /*--- (END) Slider Preparation  ---*/
//
//    /*==-- (BEGIN) Histogram preparation        --==*/
//    // program 3 has shaders that draw the histogram without perspective
//    // program 3 will use VAO 3
//    glUseProgram(program[3]);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaderBuffer[9]);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Histo.histoIndices[0])*Histo.numHistoIndices,&Histo.histoIndices.front(),GL_STATIC_DRAW);
//    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[10]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Histo.histoVertices[0])*Histo.numHistoVertices,&Histo.histoVertices.front(),GL_STATIC_DRAW);
//
//    // Vertex Array Object 2 is for the Histogram
//    glBindVertexArray(VertexArrayID[3]);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaderBuffer[9]);
//    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[10]);
//    attribLocation[5] = glGetAttribLocation(program[3], "vPosition");
//    glVertexAttribPointer(attribLocation[5],2,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));
//    uniformLocation[5] = glGetUniformLocation(program[3], "whichElement");
//    glBindVertexArray(0);
//    /*==-- (END) Histogram preparation        --==*/
//
//    /*--- (BEGIN) Highlights preparation  ---*/
//    glUseProgram(program[4]);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaderBuffer[12]);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(countryContrib.indices[0])*countryContrib.numIndices,&countryContrib.indices.front(),GL_STATIC_DRAW);
//    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[13]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(countryContrib.vertices[0])*countryContrib.numVertices,&countryContrib.vertices.front(),GL_STATIC_DRAW);
//
//    // Vertex Array Object 4 is for the Highlights
//    glBindVertexArray(VertexArrayID[4]);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaderBuffer[12]);
//    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[13]);
//    attribLocation[7] = glGetAttribLocation(program[4], "vPosition");
//    glVertexAttribPointer(attribLocation[7],3,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));
//    glBindVertexArray(0);
//
//    // connect Highlight spec matrices
//    uniformLocation[8] = glGetUniformLocation(program[4], "lightSpec");
//    uniformLocation[7] = glGetUniformLocation(program[4], "blobSpec");
//
//    startv = glm::normalize(vec3(0.0,0.0,1.0));
//
//    // my goodness, I really need to replace this sequence with a simple function and array
//    vec3 endv = glm::normalize(vec3(-.2,1.3 ,-1.24));
//    glm::mat4 rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[0] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.16,1.85 ,-1.51));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[1] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.47,1.79 ,-1.52));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[2] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(.01,1.04 ,-1.17));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[3] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.22,1.93 ,-1.34));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[4] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.13,1.88 ,-1.44));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[5] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(.02, 1.45,-1.12));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[6] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.48,2.12 ,-0.95));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[7] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.02, 1.3,-1.12));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[8] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.07,1.23 ,-1.18));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[9] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.25,1.86 ,-1.44));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[10] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.59,1.25 ,-1.26));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[11] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.44,1.49 ,-1.37));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[12] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(0.17,1.50 ,-1.12));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[13] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.3,1.13 ,-1.24));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[14] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.13,1.88 ,-1.44));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[15] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-0.17,2.22 ,-1.24));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[16] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.51,2.07 ,-1.52));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[17] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(0.17,0.98 ,-1.12));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[18] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.88,2.14 ,-1.22));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[19] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(.08, 1.95,-1.22));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[20] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(0.08,0.96,-1.15));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[21] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.33,2.11 ,-1.24));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[22] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    endv = glm::normalize(vec3(-.17,1.24 ,-1.17));
//    rotateFirst = glm::mat4_cast(RotationBetweenVectors(startv, endv));
//    blobSpec[23] = glm::translate(glm::mat4(1.0),endv)*rotateFirst;
//
//    countryContrib.updateHighlightColors(works.getAllPercs());
//    for(int i = 0; i<24; ++i)
//    lightSpec[i] = countryContrib.colorSpec[i];
//    glUniformMatrix4fv(uniformLocation[7], 24, GL_FALSE, glm::value_ptr(blobSpec[0]));
//    glUniform4fv(uniformLocation[8], 24, glm::value_ptr(lightSpec[0]));
//    for(int i = 0; i<24; ++i) std::cout << i << " " << lightSpec[i].x << "," << lightSpec[i].y
//    <<"," << lightSpec[i].z << "," <<lightSpec[i].w << std::endl;
//
//    /*--- (END)  Highlights preparation  ---*/
//
//    /*--- (BEGIN) Uniform block: Camera  ---*/
//    glUseProgram(program[0]);
//    uBlockIndex[0] = glGetUniformBlockIndex(program[0], "camera");
//    if (uBlockIndex[0] == GL_INVALID_INDEX) {
//        std::cout << "Unable to find 'camera' uniform block in the program." << std::endl; exit(EXIT_FAILURE);
//    } ;
//    glGetActiveUniformBlockiv(program[4], uBlockIndex[0], GL_UNIFORM_BLOCK_DATA_SIZE, &uBlockSize[0]);
//    uBufferC = (GLubyte *) malloc(uBlockSize[0]);
//    if (uBufferC==NULL) {
//        std::cout << "Failed while allocating uniform block buffer." << std::endl; exit(EXIT_FAILURE);
//    }
//    const char* uVarNames[numCameraBlockVars] = {"modelvMatrix","projMatrix","fixedPanelMatrix"};
//    GLuint uVarIndices[numCameraBlockVars];
//    GLint uVarSize[numCameraBlockVars];
//    GLint uVarOffset[numCameraBlockVars];
//    GLint uVarType[numCameraBlockVars];
//    glGetUniformIndices(program[0], numCameraBlockVars,uVarNames,uVarIndices);
//    glGetActiveUniformsiv(program[0], numCameraBlockVars, uVarIndices, GL_UNIFORM_OFFSET, uVarOffset);
//    glGetActiveUniformsiv(program[0], numCameraBlockVars, uVarIndices, GL_UNIFORM_SIZE, uVarSize);
//    glGetActiveUniformsiv(program[0], numCameraBlockVars, uVarIndices, GL_UNIFORM_TYPE, uVarType);
//
//    modelMatrixAddr = uBufferC+uVarOffset[ModelvMatrix];
//    uVarMemorySize[0] = uVarSize[ModelvMatrix]*TypeSize(uVarType[ModelvMatrix]) ;
//    projMatrixAddr =uBufferC+uVarOffset[ProjMatrix];
//    uVarMemorySize[1] =uVarSize[ProjMatrix]*TypeSize(uVarType[ProjMatrix]);
//    fixedPanelMatrixAddr =uBufferC+uVarOffset[FixedPanelMatrix];
//    uVarMemorySize[2] =uVarSize[FixedPanelMatrix]*TypeSize(uVarType[FixedPanelMatrix]);
//
//    memcpy(modelMatrixAddr,&modelvMatrix, uVarMemorySize[0]);
//    memcpy(projMatrixAddr,&projMatrix, uVarMemorySize[1]);
//    memcpy(fixedPanelMatrixAddr,&fixedPanelMatrix, uVarMemorySize[2]);
//    uBlockBinding​[0]= 1;
//    glUniformBlockBinding(program[0], uBlockIndex[0],uBlockBinding​[0]);
//    glBindBufferBase(GL_UNIFORM_BUFFER, uBlockBinding​[0], shaderBuffer[4]);
//    glBindBuffer(GL_UNIFORM_BUFFER, shaderBuffer[4]);
//    glBufferData(GL_UNIFORM_BUFFER, uBlockSize[0], uBufferC, GL_STATIC_DRAW);
//
//    uBlockBinding​[1]= 2;
//    glUniformBlockBinding(program[4], uBlockIndex[0],uBlockBinding​[1]);
//    glBindBufferBase(GL_UNIFORM_BUFFER, uBlockBinding​[1], shaderBuffer[4]);
//    glBindBuffer(GL_UNIFORM_BUFFER, shaderBuffer[4]);
//    glBufferData(GL_UNIFORM_BUFFER, uBlockSize[0], uBufferC, GL_STATIC_DRAW);
//    //    --- (END) Uniform block: Camera  ---
//
//    //    attribLocation[1] = glGetAttribLocation(program[0], "vNormal");
//    //    glVertexAttribPointer(attribLocation[1],3,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));
//
//    glClearColor(bgClearColor[0],bgClearColor[1], bgClearColor[2], 1);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
//    //     glEnable(GL_CULL_FACE);

}
void initTextures()
{
//    /*--- (BEGIN) Texture preparation: Earth map  ---*/
//    glGenTextures(2, textureName);
//
//    glUseProgram(program[0]);
//    glBindVertexArray(VertexArrayID[0]);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, textureName[0]);
//    loadTextureImg("equEarth-2048.gif");
//
//    glGenerateMipmap(GL_TEXTURE_2D);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    uniformLocation[1] = glGetUniformLocation(program[0], "samplePanel");
//    glUniform1i(uniformLocation[1], 0);
//    /*--- (END) Texture preparation: Earth map  ---*/
//
//    /*--- (BEGIN) Texture preparation: Panel and sliders  ---*/
//    glUseProgram(program[1]);
//    glBindVertexArray(VertexArrayID[1]);
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, textureName[1]);
//    loadTextureImg("timelineTexture-annun.gif");
//    glGenerateMipmap(GL_TEXTURE_2D);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    uniformLocation[2] = glGetUniformLocation(program[1], "samplePanel");
//    glUniform1i(uniformLocation[2], 1);
//
//    glUseProgram(program[2]);
//    glBindVertexArray(VertexArrayID[2]);
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, textureName[1]);
//    uniformLocation[6] = glGetUniformLocation(program[2], "sampleSlider");
//    glUniform1i(uniformLocation[2], 1);
//    /*--- (END) Texture preparation: Panel and sliders  ---*/
}

void updateCamera(void)
{
//    //    glUseProgram(program[0]);
//    camEye = euclidCamera(camEyeR,camEyeθ,camEyeφ);
//    modelvMatrix = glm::lookAt(camEye,camAt,camUp);
//    projMatrix = glm::perspective(frFOV,frAspect,frNear,frFar);
//    //    projMatrix = glm::ortho(prLeft, prRight, prBottom, prTop, prNear, prFar);
//
//    memcpy(modelMatrixAddr,&modelvMatrix, uVarMemorySize[0]);
//    memcpy(projMatrixAddr,&projMatrix, uVarMemorySize[1]);
//    memcpy(fixedPanelMatrixAddr,&fixedPanelMatrix, uVarMemorySize[2]);
//    glBindBuffer(GL_UNIFORM_BUFFER, shaderBuffer[4]);
//    glBufferData(GL_UNIFORM_BUFFER, uBlockSize[0], uBufferC, GL_STATIC_DRAW);
}


#endif
