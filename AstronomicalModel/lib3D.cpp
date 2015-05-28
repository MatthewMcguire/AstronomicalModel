//
//  lib3D.cpp
//  WGA visualization
//
//  Created by Matthew McGuire on 4/24/15.
//  Copyright (c) 2015 Matthew McGuire. All rights reserved.
//

#include "lib3D.h"
#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

namespace myOpenGl3D {
    glm::quat RotationBetweenVectors(vec3 start, vec3 dest){
        start = normalize(start);
        dest = normalize(dest);
        
        float cosTheta = dot(start, dest);
        vec3 rotationAxis;
        
        if (cosTheta < -1 + 0.001f){
            // special case when vectors in opposite directions:
            // there is no "ideal" rotation axis
            // So guess one; any will do as long as it's perpendicular to start
            rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);
            if (glm::length2(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
                rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);
            
            rotationAxis = normalize(rotationAxis);
            return glm::angleAxis(180.0f, rotationAxis);
        }
        
        rotationAxis = cross(start, dest);
        
        float s = sqrt( (1+cosTheta)*2 );
        float invs = 1 / s;
        
        return glm::quat(
                    s * 0.5f,
                    rotationAxis.x * invs,
                    rotationAxis.y * invs,
                    rotationAxis.z * invs
                    );
        
    }
    
    GLuint shaderProgram;
    
    // Function: readShaderSource(const char*)
    // Create a NULL-terminated string comprised of the source file contents
    static char* readShaderSource(const char* shaderFile) {
        FILE* theFilePtr = fopen(shaderFile, "r");
        if ( theFilePtr == NULL ) { return NULL; }  // file absent? return NULL
        
        // calculate shader file size
        fseek(theFilePtr, 0L, SEEK_END);
        long filesize = ftell(theFilePtr);
        fseek(theFilePtr, 0L, SEEK_SET);
        
        // create a buffer sufficiently large, and copy the source into it
        char* fileBuffer = new char[filesize + 1];
        fread(fileBuffer, 1, filesize, theFilePtr);
        fileBuffer[filesize] = '\0';    // a NULL symbol to mark end of file
        
        fclose(theFilePtr);
        return fileBuffer;
    }
    
    // Function: prepareShaders (const char*, const char*)
    // Create a GLSL program object from vertex and fragment shader files
    GLuint prepareShaders(const char* vertexShadr, const char* fragmentShadr) {
        int numShaders = 2;
        struct Shader {
            const char*  filename;
            GLenum       type;
            GLchar*      source;
        };
        Shader shaders[numShaders];
        shaders[0]= {vertexShadr, GL_VERTEX_SHADER, NULL};
        shaders[1]= {fragmentShadr, GL_FRAGMENT_SHADER, NULL};
        
        shaderProgram = glCreateProgram();
        
        // read the shader source code, compile it, and attach to GLSL program
        for (int i = 0; i < numShaders; ++i) {
            Shader& s = shaders[i];
            s.source = readShaderSource(s.filename);
            if (shaders[i].source == NULL) {
                std::cerr << "It was not possible to read this shader file: "
                << s.filename << std::endl;
                exit(EXIT_FAILURE);
            }
            
            // create the shader, then compile it
            GLuint shader = glCreateShader(s.type);
            glShaderSource(shader, 1, (const GLchar**) &s.source, NULL);
            glCompileShader(shader);
            GLint compiledStatus;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiledStatus);
            
            // report any compilation problems
            if (!compiledStatus) {
                std::cerr << s.filename << " would not compile. "
                << "Error detail follows:" << std::endl;
                GLint logSize;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
                char* logMsg = new char[logSize];
                glGetShaderInfoLog(shader, logSize, NULL, logMsg);
                std::cerr << logMsg << std::endl;
                gl_log(logMsg);
                delete [] logMsg;
                exit(EXIT_FAILURE);
            }
            delete [] s.source;
            glAttachShader(shaderProgram, shader);
        }
        
        // link the shaders and report linking problems
        glLinkProgram(shaderProgram);
        GLint linkedStatus;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkedStatus);
        if (!linkedStatus) {
            std::cerr << "Shader program failed to link" << std::endl;
            GLint logSize;
            glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logSize);
            char* logMsg = new char[logSize];
            glGetProgramInfoLog(shaderProgram, logSize, NULL, logMsg);
            gl_log(logMsg);
            std::cerr << logMsg << std::endl;
            delete [] logMsg;
            exit(EXIT_FAILURE);
        }
        
        // and return its ID

        return shaderProgram;
    }
    
    void loadTextureImg(const char * imagepath)
    {
        // Create one OpenGL texture

        // Read the image file
        GLint width=0, height=0, imgComponents=0;
        unsigned char *data = stbi_load(imagepath, &width, &height, &imgComponents, 4);
        // we ask for 4 components for pixel, expecting RGBA
        
        // flip the image right side up, so to speak
        int widthBytes = width * 4;
        unsigned char *top = NULL;
        unsigned char *bottom = NULL;
        unsigned char temp = 0;
        int halfHeight = height/2;
        for (int row = 0; row < halfHeight; row++) {
            top = data + row * widthBytes;
            bottom = data + (width - row - 1) * widthBytes;
            for (int col = 0; col < widthBytes; col++) {
                temp = *top;
                *top = *bottom;
                *bottom = temp;
                top++;
                bottom++;
            }
        }

        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, data);
        
    }
    
    /* Helper function to convert GLSL types to storage sizes */
    size_t TypeSize(GLenum type) {
        size_t size;
#define CASE(Enum, Count, Type) \
case Enum: size = Count * sizeof(Type); break
        switch (type) {
                CASE(GL_FLOAT, 1, GLfloat);
                CASE(GL_FLOAT_VEC2, 2, GLfloat);
                CASE(GL_FLOAT_VEC3, 3, GLfloat);
                CASE(GL_FLOAT_VEC4, 4, GLfloat);
                CASE(GL_INT, 1, GLint);
                CASE(GL_INT_VEC2, 2, GLint);
                CASE(GL_INT_VEC3, 3, GLint);
                CASE(GL_INT_VEC4, 4, GLint);
                CASE(GL_UNSIGNED_INT, 1, GLuint);
                CASE(GL_UNSIGNED_INT_VEC2, 2, GLuint);
                CASE(GL_UNSIGNED_INT_VEC3, 3, GLuint);
                CASE(GL_UNSIGNED_INT_VEC4, 4, GLuint);
                CASE(GL_BOOL, 1, GLboolean);
                CASE(GL_BOOL_VEC2, 2, GLboolean);
                CASE(GL_BOOL_VEC3, 3, GLboolean);
                CASE(GL_BOOL_VEC4, 4, GLboolean);
                CASE(GL_FLOAT_MAT2, 4, GLfloat);
                CASE(GL_FLOAT_MAT2x3, 6, GLfloat);
                CASE(GL_FLOAT_MAT2x4, 8, GLfloat);
                CASE(GL_FLOAT_MAT3, 9, GLfloat);
                CASE(GL_FLOAT_MAT3x2, 6, GLfloat);
                CASE(GL_FLOAT_MAT3x4, 12, GLfloat);
                CASE(GL_FLOAT_MAT4, 16, GLfloat);
                CASE(GL_FLOAT_MAT4x2, 8, GLfloat);
                CASE(GL_FLOAT_MAT4x3, 12, GLfloat);
#undef CASE
            default:
                fprintf(stderr, "Unknown type: 0x%x\n", type);
                exit(EXIT_FAILURE);
                break;
        }
        return size;
    }
    
    // convert camera location from spherical to euclidean
    point3 euclidSpherical(GLfloat r,GLfloat th, GLfloat ph) {
        return point3(r*sin(ph)*sin(th), r*cos(ph), r*cos(th)*sin(ph));
    }
    
    // error callback function
    void errorCallb(int errcode, const char* description) {
        fprintf(stderr, "%d: %s\n", errcode, description);
        gl_log_err
        (" GLFW ERROR: code %i msg: %s\n", errcode, description);
    }
    
    GLfloat smallPiBound(GLfloat a) {
        if (a < -1*M_PI) a += M_PI*2.0;
        if (a > M_PI) a -= M_PI*2.0;
        return a;
    }
    
    #define GL_LOG_FILE "gl.log"
    /* start a new log file. put the time and date at the top */
    bool restart_gl_log ()
    {
        time_t now;
        char* date;
        FILE* file = fopen (GL_LOG_FILE, "w");
        
        if (!file)
        {
            fprintf (
                     stderr,
                     "ERROR: could not open GL_LOG_FILE log file %s for writing\n",
                     GL_LOG_FILE
                     );
            return false;
        }
        now = time (NULL);
        date = ctime (&now);
        fprintf (file, "GL_LOG_FILE log. local time %s", date);
        fprintf (file, "build version: %s %s\n\n", __DATE__, __TIME__);
        fclose (file);
        return true;
    }

    /* add a message to the log file. arguments work the same way as printf() */
    bool gl_log (const char* message, ...)
    {
        va_list argptr;
        FILE* file = fopen (GL_LOG_FILE, "a");
        if (!file)
        {
            fprintf
            (
                     stderr,
                     "ERROR: could not open GL_LOG_FILE %s file for appending\n",
                     GL_LOG_FILE
                     );
            return false;
        }
        va_start (argptr, message);
        vfprintf (file, message, argptr);
        va_end (argptr);
        fclose (file);
        return true;
    }
    
    /* same as gl_log except also prints to stderr */
    bool gl_log_err (const char* message, ...)
    {
        va_list argptr;
        FILE* file = fopen (GL_LOG_FILE, "a");
        if (!file)
        {
            fprintf (
                     stderr,
                     "ERROR: could not open GL_LOG_FILE %s file for appending\n",
                     GL_LOG_FILE
                     );
            return false;
        }
        va_start (argptr, message);
        vfprintf (file, message, argptr);
        va_end (argptr);
        va_start (argptr, message);
        vfprintf (stderr, message, argptr);
        va_end (argptr);
        fclose (file);
        return true;
    }
    
    /* we can use a function like this to print some GL capabilities of our adapter
     to the log file. handy if we want to debug problems on other people's computers
     */
    void log_gl_params ()
    {
        int i;
        int v[2];
        unsigned char s = 0;
        GLenum params[] =
        {
            GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
            GL_MAX_CUBE_MAP_TEXTURE_SIZE,
            GL_MAX_DRAW_BUFFERS,
            GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
            GL_MAX_TEXTURE_IMAGE_UNITS,
            GL_MAX_TEXTURE_SIZE,
            GL_MAX_VARYING_FLOATS,
            GL_MAX_VERTEX_ATTRIBS,
            GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
            GL_MAX_VERTEX_UNIFORM_COMPONENTS,
            GL_MAX_VIEWPORT_DIMS,
            GL_STEREO,
        };
        const char* names[] =
        {
            "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
            "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
            "GL_MAX_DRAW_BUFFERS",
            "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
            "GL_MAX_TEXTURE_IMAGE_UNITS",
            "GL_MAX_TEXTURE_SIZE",
            "GL_MAX_VARYING_FLOATS",
            "GL_MAX_VERTEX_ATTRIBS",
            "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
            "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
            "GL_MAX_VIEWPORT_DIMS",
            "GL_STEREO",
        };
        gl_log ("GL Context Params:\n");
        // integers - only works if the order is 0-10 integer return types
        for (i = 0; i < 10; i++) {
            int v = 0;
            glGetIntegerv (params[i], &v);
            gl_log ("%s %i\n", names[i], v);
        }
        // others
        v[0] = v[1] = 0;
        glGetIntegerv (params[10], v);
        gl_log ("%s %i %i\n", names[10], v[0], v[1]);
        glGetBooleanv (params[11], &s);
        gl_log ("%s %i\n", names[11], (unsigned int)s);
        gl_log ("-----------------------------\n");
    }
    
    /* print errors in shader compilation */
    void _print_shader_info_log (GLuint shader_index) {
        int max_length = 2048;
        int actual_length = 0;
        char log[2048];
        glGetShaderInfoLog (shader_index, max_length, &actual_length, log);
        printf ("shader info log for GL index %i:\n%s\n", shader_index, log);
    }
    
    /* print errors in shader linking */
    void _print_programme_info_log (GLuint sp) {
        int max_length = 2048;
        int actual_length = 0;
        char log[2048];
        glGetProgramInfoLog (sp, max_length, &actual_length, log);
        printf ("program info log for GL index %i:\n%s", sp, log);
    }
    
    const char* GL_type_to_string (GLenum type) {
        switch (type) {
            case GL_BOOL: return "bool";
            case GL_INT: return "int";
            case GL_FLOAT: return "float";
            case GL_FLOAT_VEC2: return "vec2";
            case GL_FLOAT_VEC3: return "vec3";
            case GL_FLOAT_VEC4: return "vec4";
            case GL_FLOAT_MAT2: return "mat2";
            case GL_FLOAT_MAT3: return "mat3";
            case GL_FLOAT_MAT4: return "mat4";
            case GL_SAMPLER_2D: return "sampler2D";
            case GL_SAMPLER_3D: return "sampler3D";
            case GL_SAMPLER_CUBE: return "samplerCube";
            case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
            default: break;
        }
        return "other";
    }
    
    /* print absolutely everything about a shader - only useful if you get really
     stuck wondering why a shader isn't working properly */
    void print_all (GLuint sp) {
        int params = -1;
        int i;
        
        printf ("--------------------\nshader programme %i info:\n", sp);
        glGetProgramiv (sp, GL_LINK_STATUS, &params);
        printf ("GL_LINK_STATUS = %i\n", params);
        
        glGetProgramiv (sp, GL_ATTACHED_SHADERS, &params);
        printf ("GL_ATTACHED_SHADERS = %i\n", params);
        
        glGetProgramiv (sp, GL_ACTIVE_ATTRIBUTES, &params);
        printf ("GL_ACTIVE_ATTRIBUTES = %i\n", params);
        
        for (i = 0; i < params; i++) {
            char name[64];
            int max_length = 64;
            int actual_length = 0;
            int size = 0;
            GLenum type;
            glGetActiveAttrib (sp, i, max_length, &actual_length, &size, &type, name);
            if (size > 1) {
                int j;
                for (j = 0; j < size; j++) {
                    char long_name[64];
                    int location;
                    
                    sprintf (long_name, "%s[%i]", name, j);
                    location = glGetAttribLocation (sp, long_name);
                    printf ("  %i) type:%s name:%s location:%i\n",
                            i, GL_type_to_string (type), long_name, location);
                }
            } else {
                int location = glGetAttribLocation (sp, name);
                printf ("  %i) type:%s name:%s location:%i\n",
                        i, GL_type_to_string (type), name, location);
            }
        }
        
        glGetProgramiv (sp, GL_ACTIVE_UNIFORMS, &params);
        printf ("GL_ACTIVE_UNIFORMS = %i\n", params);
        for (i = 0; i < params; i++) {
            char name[64];
            int max_length = 64;
            int actual_length = 0;
            int size = 0;
            GLenum type;
            glGetActiveUniform (sp, i, max_length, &actual_length, &size, &type, name);
            if (size > 1) {
                int j;
                for (j = 0; j < size; j++) {
                    char long_name[64];
                    int location;
                    
                    sprintf (long_name, "%s[%i]", name, j);
                    location = glGetUniformLocation (sp, long_name);
                    printf ("  %i) type:%s name:%s location:%i\n",
                            i, GL_type_to_string (type), long_name, location);
                }
            } else {
                int location = glGetUniformLocation (sp, name);
                printf ("  %i) type:%s name:%s location:%i\n",
                        i, GL_type_to_string (type), name, location);
            }
        }
        
        _print_programme_info_log (sp);
    }
   
    

}


