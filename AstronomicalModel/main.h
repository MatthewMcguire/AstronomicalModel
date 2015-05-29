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

AstroGroup solarSystem(0.35);

//**************************************************
/*@@##====--- OpenGL parameters (BEGIN) ---====##@@*/
const int numVAO = 3;
const int numBuffers = 3;
const int numUBuffs = 3;
GLuint VertexArrayID[numVAO];       //  Array of Vertex Array Objects
GLuint program[1];                  //  max. number of shader programs
GLuint shaderBuffer[numBuffers];    //  Array of ordinary shader buffers
GLuint attribLocation[3];           //  Array of shader attribute locations
GLint uniformLocation[3];           //  Array of uniform variable locations
GLuint textureName[3];
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
GLdouble xCursorPos, yCursorPos;
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
GLfloat camEyeφ = (0.0);   // φ is measured downward from the positive-y axis, begins at pi/2
GLfloat camEyeR = 1200.0f;         // R is the 'radius': the distance from the origin to the camera
point3 camEye = euclidSpherical(camEyeR,camEyeθ,camEyeφ);
point3 camRight = {cos(camEyeθ),0,-sin(camEyeθ)};   // to orient the camera, the vector directly to right
point3 camAt = point3(0.0f,0.0f,0.0f);
point3 camUp = glm::cross(camEye,camRight);

GLfloat frFOV =20.0;                // field of view for perspective frustrum
GLfloat frAspect = 1.0f;            // aspect ratio for perspective frustrum
GLfloat frNear = 0.1f;              // near side of perspective frustrum
GLfloat frFar = 10020.0f;            // far side of perspective frustrum

GLfloat accelFactor = 0.2f;
GLfloat zoomFactor = 0.15f;
GLfloat rShift;                 // magnitude of mouse scroll event
/*@@##====--- Model and View parameters (END) ---====##@@*/

/*@@##====--- Simulation parameters (BEGIN) ---====##@@*/
float simulationSpeed = 1.0;
GLboolean donePausing = GL_TRUE;
GLdouble lastPauseEnd;
GLdouble pauseLength = 0.02;    // update wait duration (in seconds) for mouse-driven events
matr4 objTransforms[20];        // array of model transforms for each object
/*@@##====--- Simulation parameters (END) ---====##@@*/

//*********************************************************
/*@@##====--- General helper functions (BEGIN) ---====##@@*/
enum {simspeed,simscale};
void reportParam(int report)
{
    float hoursPerSecond;
    switch(report)
    {
        case simspeed:
            hoursPerSecond = simulationSpeed/pauseLength;         // number of simulation 'hours' per sec.
            hoursPerSecond = float(int(hoursPerSecond*100.0))/100.0;    // round to hundredths
            std::cout << "Simulation speed: " << hoursPerSecond << " hours per second" << std::endl;
            break;
        case simscale:
            std::cout << "Simulation Scale: " << solarSystem.currentScaleFactor() << std::endl;
            break;
    }
}
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
void moveCamera(void)
{
    // get current mouse cursor
    // normalize to [-1,1]
    glfwGetCursorPos(mainWin, &xCursorPos, &yCursorPos);
    GLdouble displacedHorizontal = (xCursorPos-halfWinWidth)/halfWinWidth;
    GLdouble displacedVertical = (yCursorPos-halfWinHeight)/halfWinHeight;
    
    // compute acceleration
    // modify polar angles
    camEyeθ += fabs(accelFactor*pow(displacedHorizontal,2.0))* sgn(displacedHorizontal);
    camEyeφ += fabs(accelFactor*pow(displacedVertical,2.0))* sgn(displacedVertical);
    camEyeθ = smallPiBound(camEyeθ);
    camEyeφ = smallPiBound(camEyeφ);
    camRight = {cos(camEyeθ),0,-sin(camEyeθ)};
    camUp = glm::cross(camEye,camRight);
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
            solarSystem.adjustScale(+0.01);
            reportParam(simscale);
        break;
        case GLFW_KEY_DOWN:
            solarSystem.adjustScale(-0.01);
            reportParam(simscale);
        break;
        case GLFW_KEY_LEFT:
            simulationSpeed -= 0.05;
            reportParam(simspeed);
        break;
        case GLFW_KEY_RIGHT:
            simulationSpeed += 0.05;
            reportParam(simspeed);
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
    // nothing to do here. I only poll the x and y pos when needed
    // but un-comment these should it be necessary:
    // xCursorPos = xpos;
    // yCursorPos = ypos;
}
void mouseFunc(GLFWwindow* window, int button, int action, int mods)
{
//    if (button == GLFW_MOUSE_BUTTON_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
//        moveCamera();
}
void scrollFunc(GLFWwindow* mainWin, double xOffset, double yOffset)
{
    rShift = sqrt(fabs(yOffset)) * sgn(yOffset) * accelFactor;
    camEyeR *= (1.0+rShift);
}
/*@@##====--- GLFW Callback functions (END) ---====##@@*/


void initGLFW()
{
    assert (restart_gl_log ());             // simple log of graphics startup data
    gl_log (" starting GLFW\n% s\n", glfwGetVersionString ());

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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // set transparency mode
    //    /* With Retina on Mac OS X, GLFW's FramebufferSize dne WindowSize */
    //    glfwGetFramebufferSize(wind, &fbwidth, &fbheight);
    //
    //    // call the reshape first, since it is not called at window creation
    //    windowReshape(wind, fbwidth, fbheight);  // shape view port
    //--- register callbacks with GLFW ----
    //    /* function to handle window resizes */
    //    glfwSetFramebufferSizeCallback(wind, fbreshape);
    glfwSetWindowSizeCallback(mainWin, windowReshape);
    glfwSetWindowCloseCallback(mainWin,quitApp);
    glfwSetKeyCallback(mainWin, specialKeyTyping);
    glfwSetCharCallback(mainWin, asciiTyping);
    glfwSetCursorPosCallback(mainWin, screenCursor);
    glfwSetScrollCallback(mainWin, scrollFunc);
    glfwSetMouseButtonCallback(mainWin, mouseFunc);
    glfwSwapInterval(0);    // allows more than 60 FPS (though screen tearing is a risk)
    log_gl_params();
}
void initOpenGL()
{
    glEnable(GL_DEPTH_TEST);
    program[0] = prepareShaders("AstronObjectGLSL.vert", "AstronObjectGLSL.frag");

    /*--- Create VAO and buffer stuff  ---*/
    glGenBuffers(numBuffers,shaderBuffer);
    glGenVertexArrays(numVAO,VertexArrayID);

    /*--- (BEGIN) Sphere Preparation  ---*/
    // program 0 draws the astronomical objects as instances of a single sphere
    // program 0 will use VAO 0
    glUseProgram(program[0]);
    // Sphere Indices and Vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaderBuffer[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (sizeof(solarSystem.s.theSphere.indices[0]) * solarSystem.s.theSphere.numIndices),
                 &solarSystem.s.theSphere.indices.front(),GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[1]);
    glBufferData(GL_ARRAY_BUFFER,
                 ((sizeof(solarSystem.s.theSphere.vertices[0])+sizeof(solarSystem.s.theSphere.norms[0]))
                  * solarSystem.s.theSphere.numVertices),NULL,GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    (sizeof(solarSystem.s.theSphere.vertices[0]) * solarSystem.s.theSphere.numVertices),
                    &solarSystem.s.theSphere.vertices.front());
    glBufferSubData(GL_ARRAY_BUFFER,
                    (sizeof(solarSystem.s.theSphere.vertices[0]) * solarSystem.s.theSphere.numVertices),
                    (sizeof(solarSystem.s.theSphere.norms[0]) * solarSystem.s.theSphere.numVertices),
                    &solarSystem.s.theSphere.norms.front());

    glBindVertexArray(VertexArrayID[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaderBuffer[0]);
    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[1]);
    attribLocation[0] = glGetAttribLocation(program[0], "vPosition");
    glVertexAttribPointer(attribLocation[0],3,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));

    attribLocation[1] = glGetAttribLocation(program[0], "vNormal");
    glVertexAttribPointer(attribLocation[1],3,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));

    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[2]);
    attribLocation[2] = glGetAttribLocation(program[0], "textureSTMap");
    glVertexAttribPointer(attribLocation[2],2,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(0));

    // connect sphere specification matrices
    uniformLocation[0] = glGetUniformLocation(program[0], "objectTransform");


    // scale each sphere to proper size
    for (int i=0; i < solarSystem.numObjects; i++)
        objTransforms[i] = solarSystem.montum[i].modelOrientation *
        solarSystem.montum[i].absLocationMatrix *
        solarSystem.montum[i].modelScale;
    glUniformMatrix4fv(uniformLocation[0], solarSystem.numObjects,
                       GL_FALSE, glm::value_ptr(objTransforms[0]));

    /*--- (BEGIN) Uniform block: Camera  ---*/
    glUseProgram(program[0]);
    uBlockIndex[0] = glGetUniformBlockIndex(program[0], "camera");
    if (uBlockIndex[0] == GL_INVALID_INDEX) {
        std::cout << "Unable to find 'camera' uniform block in the program." << std::endl;
        exit(EXIT_FAILURE);
    } ;
    glGetActiveUniformBlockiv(program[0], uBlockIndex[0], GL_UNIFORM_BLOCK_DATA_SIZE, &uBlockSize[0]);
    uBufferCamera = (GLubyte *) malloc(uBlockSize[0]);
    if (uBufferCamera==NULL) {
        std::cout << "Failed while allocating uniform block buffer.\n\n"; exit(EXIT_FAILURE);
    }
    const char* uVarNames[numCameraBlockVars] = {"modelvMatrix","projMatrix"};
    GLuint uVarIndices[numCameraBlockVars];
    GLint uVarSize[numCameraBlockVars];
    GLint uVarOffset[numCameraBlockVars];
    GLint uVarType[numCameraBlockVars];
    glGetUniformIndices(program[0], numCameraBlockVars,uVarNames,uVarIndices);
    glGetActiveUniformsiv(program[0], numCameraBlockVars, uVarIndices, GL_UNIFORM_OFFSET, uVarOffset);
    glGetActiveUniformsiv(program[0], numCameraBlockVars, uVarIndices, GL_UNIFORM_SIZE, uVarSize);
    glGetActiveUniformsiv(program[0], numCameraBlockVars, uVarIndices, GL_UNIFORM_TYPE, uVarType);

    modelMatrixAddr = uBufferCamera+uVarOffset[ModelvMatrix];
    uVarMemorySize[0] = uVarSize[ModelvMatrix]*TypeSize(uVarType[ModelvMatrix]) ;
    projMatrixAddr =uBufferCamera+uVarOffset[ProjMatrix];
    uVarMemorySize[1] =uVarSize[ProjMatrix]*TypeSize(uVarType[ProjMatrix]);

    memcpy(modelMatrixAddr,&modelvMatrix, uVarMemorySize[0]);
    memcpy(projMatrixAddr,&projMatrix, uVarMemorySize[1]);
    uBlockBinding​[0]= 1;
    glUniformBlockBinding(program[0], uBlockIndex[0],uBlockBinding​[0]);
    glBindBufferBase(GL_UNIFORM_BUFFER, uBlockBinding​[0], shaderBuffer[2]);
    glBindBuffer(GL_UNIFORM_BUFFER, shaderBuffer[2]);
    glBufferData(GL_UNIFORM_BUFFER, uBlockSize[0], uBufferCamera, GL_STATIC_DRAW);
    //    --- (END) Uniform block: Camera  ---

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
    reportParam(simspeed);
}
void initTextures()
{
    /*--- (BEGIN) Texture preparation: Map of Luna and Venus ---*/
    glGenTextures(1, textureName);

    glUseProgram(program[0]);
    glBindVertexArray(VertexArrayID[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureName[0]);
    loadTextureImg("LunaVenus.gif");

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    uniformLocation[1] = glGetUniformLocation(program[0], "sample01");
    glUniform1i(uniformLocation[1], 0);
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
    camEye = euclidSpherical(camEyeR,camEyeθ,camEyeφ);
    modelvMatrix = glm::lookAt(camEye,camAt,camUp);
    projMatrix = glm::perspective(frFOV,frAspect,frNear,frFar);

    memcpy(modelMatrixAddr,&modelvMatrix, uVarMemorySize[0]);
    memcpy(projMatrixAddr,&projMatrix, uVarMemorySize[1]);
    glBindBuffer(GL_UNIFORM_BUFFER, shaderBuffer[2]);
    glBufferData(GL_UNIFORM_BUFFER, uBlockSize[0], uBufferCamera, GL_STATIC_DRAW);
}
void modelAnimate(void)
{
    solarSystem.updateMontum(60.0 * simulationSpeed);
    for (int i=0; i < solarSystem.numObjects; i++) {
        objTransforms[i] = solarSystem.montum[i].modelOrientation *
        solarSystem.montum[i].absLocationMatrix *
        solarSystem.montum[i].modelScale;
    }
    glUniformMatrix4fv(uniformLocation[0], solarSystem.numObjects, GL_FALSE, glm::value_ptr(objTransforms[0]));
}

void drawObjects(void)
{
    glBindVertexArray(VertexArrayID[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureName[0]);
    glEnableVertexAttribArray(attribLocation[0]);
    glBindBuffer(GL_ARRAY_BUFFER, shaderBuffer[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shaderBuffer[0]);
    solarSystem.drawMontum();
    glDisableVertexAttribArray(attribLocation[0]);
}


#endif
