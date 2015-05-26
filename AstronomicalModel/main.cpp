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
        if(glfwGetMouseButton(mainWin,GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
            moveCamera();
        updateCamera();
        modelAnimate();
    }
    // draw scene
    drawObjects();

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
    fps[0] = glfwGetTime();                 // begin to measure 'time to initialize'
    
    initGLFW();
    initOpenGL();
    initTextures();
    
    std::cout << "it took " << glfwGetTime()-fps[0] << " s. to get started.\n";
    /* Enter the main interactive display loop*/
    do{
        updateDisplay();
        glfwPollEvents();
    } while (!glfwWindowShouldClose(mainWin));

    return 0;
}
