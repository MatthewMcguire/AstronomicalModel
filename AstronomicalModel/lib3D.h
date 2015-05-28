//
//  lib3D.h
//  WGA visualization
//
//  Created by Matthew McGuire on 4/24/15.
//  Copyright (c) 2015 Matthew McGuire. All rights reserved.
//

#ifndef __WGA_visualization__lib3D__
#define __WGA_visualization__lib3D__

#ifdef __APPLE__
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include <OpenGL/gl3.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cmath>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/quaternion.hpp>
#include <GLM/gtx/quaternion.hpp>

#define GLFW_NO_GLU
#define  GLFW_INCLUDE_GL3

//  Define M_PI to an extraordinary accuracy
#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

// for readability
typedef glm::vec3 point3;
typedef glm::vec3 vec3;
typedef glm::vec4 point4;
typedef glm::vec4 vec4;
typedef glm::mat4 matr4;
typedef glm::mat3 matr3;
typedef glm::vec2 vec2;
typedef glm::vec2 point2;


// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )  ((GLvoid*) (offset))

namespace myOpenGl3D {
    
    glm::quat RotationBetweenVectors(vec3, vec3);
    //  Convenient figure to avoid division by zero errors
    const GLfloat DivideByZeroTolerance = GLfloat(1.0e-07);
    
    //  Degrees-to-radians constant
    const GLfloat DegreesToRadians = M_PI / 180.0;
    
    // Create a NULL-terminated string by reading the provided file
    
    //  function to load vertex and fragment shader files
    GLuint prepareShaders(const char* vertexShadr, const char* fragmentShadr);
    
    void loadTextureImg(const char * imagepath);
    
    /* Helper function to convert GLSL types to storage sizes */
    size_t TypeSize(GLenum type);
    
    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }
    
    // convert camera location from spherical to euclidean
    point3 euclidSpherical(GLfloat r,GLfloat th, GLfloat ph);
    
    // error callback function
    void errorCallb(int errcode, const char* desc);
    
    GLfloat smallPiBound(GLfloat);
    
    /*  (BEGIN) these several error-log helper functions are not my own,
        but are adapted from Gerdelan's OpenGL book */
    bool restart_gl_log();
    bool gl_log(const char* message, ...);
    bool gl_log_err(const char* message, ...);
    void log_gl_params(void);
    void _print_shader_info_log (GLuint);
    void _print_programme_info_log (GLuint);
    const char* GL_type_to_string (GLenum);
    void print_all (GLuint);
    
    /*  (END) these several error-log helper functions are not my own,
     but are adapted from Gerdelan's OpenGL book */
}
using namespace myOpenGl3D;


#endif /* defined(__WGA_visualization__lib3D__) */
