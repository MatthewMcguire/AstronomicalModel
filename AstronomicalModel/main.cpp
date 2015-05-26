//
//  main.cpp
//  AstronomicalModel
//
//  Created by Matthew McGuire on 5/22/15.
//  Copyright (c) 2015 Matthew McGuire. All rights reserved.
//

#include "main.h"
#include <iostream>

/* Primary GLFW display loop */
void updateDisplay() {
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT );
    if (donePausing==GL_TRUE) {
        lastPauseEnd = glfwGetTime();
        donePausing = GL_FALSE;
        updateCamera();
    }
    // draw scene

    nowFPS = glfwGetTime();
    if(nowFPS > fps[1] + 1.0) {
        char newTitle[50];
        std::strcpy(newTitle, ("Solar System (" + std::to_string(fpsCounter) + " fps)").c_str());
        glfwSetWindowTitle(mainWin,newTitle );

        fpsCounter = 0;
        fps[1]=nowFPS;
    }
    else fpsCounter++;
    glfwSwapBuffers(mainWin);
    glfwMakeContextCurrent(mainWin);
    if(donePausing==GL_FALSE && (glfwGetTime()-lastPauseEnd)> pauseLength) donePausing = GL_TRUE;

}


int main(int argc, const char * argv[]) {
    std::cout << "Hello, Worlds!\n";
    fps[0] = glfwGetTime();
    assert (restart_gl_log ());
    gl_log (" starting GLFW\n% s\n", glfwGetVersionString ());

    /* Initialize graphics environment */
    solarSystem.updateMontum(60*24*365.25/8.0);
    initGLFW();
    log_gl_params();
    initOpenGL();
    glClearColor(.15,.15,.15, 1);
    initTextures();
    fps[1] = glfwGetTime();
    std::cout << "it took " << fps[1]-fps[0] << " s. to get started.\n";

    /* Enter the main interactive display loop*/
    do{
        updateDisplay();
        glfwPollEvents();
    } while (!glfwWindowShouldClose(mainWin));

    return 0;
}
