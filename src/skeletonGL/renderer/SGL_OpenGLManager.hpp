
// ------------------- By: TSURA @ -------------------
// ███╗   ██╗███████╗ ██████╗ ██╗  ██╗███████╗██╗  ██╗
// ████╗  ██║██╔════╝██╔═══██╗██║  ██║██╔════╝╚██╗██╔╝
// ██╔██╗ ██║█████╗  ██║   ██║███████║█████╗   ╚███╔╝
// ██║╚██╗██║██╔══╝  ██║   ██║██╔══██║██╔══╝   ██╔██╗
// ██║ ╚████║███████╗╚██████╔╝██║  ██║███████╗██╔╝ ██╗
// ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
// ---------------------- [.xyz] ---------------------

/**
 * @file    src/skeletonGL/utility/SGL_OpenGLManager.hpp
 * @author  TSURA @ NEOHEX.XYZ
 * @date    9/4/2018
 * @version 1.0
 *
 * @brief Encapsulates ALL OpenGL related function calls and manages its resources
 *
 * @section DESCRIPTION
 *
 * This OpenGL Manager is designed to keep tabs on the current state of OpenGL's state machine
 * and allow for a much easier debugging experience. Any and all OpenGL functions that may be
 * required in the future should be added here.
 */

#ifndef SRC_SKELETONGL_RENDERER_OPENGLMANAGER_HPP
#define SRC_SKELETONGL_RENDERER_OPENGLMANAGER_HPP

// C++
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
// Dependencies
#include <GL/glew.h>
// SkeletonGL
#include "../utility/SGL_DataStructures.hpp"
#include "../utility/SGL_Utility.hpp"

/**
 * @brief Encapsulates an OpenGL context.
 * @section DESCRIPTION
 *
 * This class encapsulates all OpenGL related functions and keeps a record
 * of the current state of the internal OpenGL state machine to eased debugging.
 */
class SGL_OpenGLManager
{
private:
    CurrentSettings currentGLSettings;        ///< OpenGL rendering settings
    CurrentBuffers currentGLBuffers;          ///< Currently active buffers

    std::map<std::string, GLBuffer>VBO;       ///< All the context VBOs
    std::map<std::string, GLBuffer>EBO;       ///< All the context EBOs
    std::map<std::string, GLBuffer>VAO;       ///< All the context VAOs
    std::map<std::string, GLBuffer>FBO;       ///< All the context FBOs
    std::map<std::string, GLSetting>Settings; ///< Supported OpenGL render settings
    GLuint currentShaderID, currentTextureID; ///< Bound shader and texture IDs
    SGL_Color currentClearColor;              ///< Clear screen color

    // Currently, an opengl error arises when checking for
    // errors after initializing the GL context, doesn't
    // seem to affect rendering at all
    uint8_t initialErrorCap;

public:
    // Constructor
    SGL_OpenGLManager();

    // Destructor
    ~SGL_OpenGLManager();

    // Create a VBO
    void createVBO(const std::string &name);
    // Create an EBO
    void createEBO(const std::string &name);
    // Create a VAO
    void createVAO(const std::string &name);
    // Create a FBO
    void createFBO(const std::string &name);
    // Create a RBO
    void createRBO(const std::string &name);
    // Bind the selected buffer
    void bindVBO(const std::string &name);
    // Bind the selected buffer
    void bindEBO(const std::string &name);
    // Bind the selected VAO
    void bindVAO(const std::string &name);
    // Bind the selected FBO
    void bindFBO(const std::string &name, GLCONSTANTS::FBO type = GLCONSTANTS::RW);
    // Unbind the current buffer
    void unbindVBO();
    // Unbind the current EBO
    void unbindEBO();
    // Unbind the current VAO
    void unbindVAO();
    // Unbind the current FBO
    void unbindFBO();
    // Cleans all buffer storage
    void clearBuffers();
    // Deletes all the VBO
    void deleteVBO(const std::string &name);
    // Deletes all the EBO
    void deleteEBO(const std::string &name);
    // Deletes all the VAO
    void deleteVAO(const std::string &name);
    // Deletes all the FBO
    void deleteFBO(const std::string &name);

    // Activates the specified shader program
    void useProgram(GLuint program);
    // Creates a shader
    GLuint createShader(GLenum shaderType);
    // Compile a shader program
    void compileShader(GLuint shader);
    // Specify the shader data
    void shaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
    // Link shaders into a program
    void attachShader(GLuint program, GLuint shader);
    // Delete a shader
    void deleteShader(GLuint shader);
    // Create the shader program
    GLuint createProgram();
    // Link a compiled shader program
    void linkProgram(GLuint program);
    // Get shader data
    void getShaderiv(GLuint shader, GLenum pname, GLint *params);
    // Gets the shader info log
    void getShaderInfoLog(GLuint shader,  GLsizei maxLength, GLsizei *length, GLchar *infoLog);
    // Gets them program data
    void getProgramiv(GLuint shader, GLenum pname, GLint *params);
    // Gets the program log
    void getProgramInfoLog(GLuint shader,  GLsizei maxLength, GLsizei *length, GLchar *infoLog);

    // Sets a uniform location from the specified shader
    GLint getUniformLocation(GLuint program, const GLchar *name);
    // Sets a float uniform location from the specified shader
    void uniform1f(GLint location, GLfloat v0);
    // Sets an integer location from the specified shader
    void uniform1i(GLint location, GLint v0);
    // Sets a two floats uniforms from the specified shader
    void uniform2f(GLint location, GLfloat v0, GLfloat v1);
    // Sets a three float uniforms from the specified shader
    void uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
    // Sets a four float uniforms from the specified shader
    void uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    // Sets a 4x4 float uniform matrix from the specified shader
    void uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    // Sets a 2fv from the specified shader
    void uniform2fv(GLint location, GLsizei count, const GLfloat *value);
    // Sets a 1iv from the specified shader
    void uniform1iv(GLint location, GLsizei count, const GLint *value);
    // Sets a 1fv from the specified shader
    void uniform1fv(GLint location, GLsizei count, const GLfloat *value);

    // Sets the clear screen color
    void setClearColor(SGL_Color &newColor);
    // Clears the color buffer
    void clearColorBuffer();
    // Clears the depth buffer (UNUSED AS OF v 1.0)
    void clearDepthBuffer();
    // Clears both color and depth buffers
    void clearColorAndDepthBuffers();
    // Check for depth testing?
    void depthTesting(bool value);
    // Enable depth mask?
    void depthMask(bool value);
    // Enable face culling?
    void faceCulling(bool value);
    // Set blending type
    void blending(bool value, BLENDING_TYPE type);
    // Generate a texture
    void genTextures(GLsizei n, GLuint *textures);
    // Bind texture
    void bindTexture(GLenum target, GLuint texture);
    // Activates a texture
    void activeTexture(GLenum texture);
    // Deletes textures
    void deleteTextures(GLsizei n, const GLuint *textures);
    // Texture pixel format
    void pixelStorei(GLenum pname, GLint param);
    // Texture creation parameters
    void texParameteri(GLenum target, GLenum pname, GLint param);
    // Texture creation parameters
    void texParameterf(GLenum target, GLenum pname, GLfloat param);
    // Creates a 2D texture
    void texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *data);
    // Draws loaded state machine
    void drawArrays(GLenum mode, GLint first, GLsizei count);
    // Draw multiple instances of a range of elements
    void drawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
    // Draw multiple instances of a set of elements
    void drawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
    // Add data to buffer
    void bufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
    // Replace data to buffer
    void bufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
    // Enable or disable a generic vertex attribute array
    void enableVertexAttribArray(GLuint index);
    // Define an array of generic vertex attribute data
    void vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
    // modify the rate at which generic vertex attributes advance during instanced rendering
    void vertexAttribDivisor(GLuint index, GLint divisor);
    // Deletes a compiled and linked shader program
    void deleteProgram(GLuint program);
    // Generate framebuffer
    void frameBufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    // Render framebuffer
    void blitFrameBuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
    // Checks the current FBO status
    GLenum checkFrameBufferStatus(GLenum target);
    // Resize the viewport
    void resizeGL(int x, int y, int w, int h);
    // Error reporting
    void checkForGLErrors();
    // Total amount of active VBO for this OpenGL context
    int totalVBO();
    // Total amount of active VAOs for this OpenGL context
    int totalVAO();
    // Total amount of active EBOs for this OpenGL context
    int totalEBO();
    // Total amount of active FBO for this OpenGL context
    int totalFBO();
};

#endif // SRC_SKELETONGL_RENDERER_OPENGLMANAGER_HPP
