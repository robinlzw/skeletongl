
// ------------------- By: TSURA @ -------------------
// ███╗   ██╗███████╗ ██████╗ ██╗  ██╗███████╗██╗  ██╗
// ████╗  ██║██╔════╝██╔═══██╗██║  ██║██╔════╝╚██╗██╔╝
// ██╔██╗ ██║█████╗  ██║   ██║███████║█████╗   ╚███╔╝
// ██║╚██╗██║██╔══╝  ██║   ██║██╔══██║██╔══╝   ██╔██╗
// ██║ ╚████║███████╗╚██████╔╝██║  ██║███████╗██╔╝ ██╗
// ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
// ---------------------- [.xyz] ---------------------

 * @file    src/skeletonGL/utility/SGL_OpenGLManager.cpp
 * @author  AlexHG
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

#include "SGL_OpenGLManager.hpp"


/**
 * @brief Main and only constructor
 *
 * @return nothing
 */
SGL_OpenGLManager::SGL_OpenGLManager()
{
    currentTextureID = 0;
    currentShaderID = 0;
    initialErrorCap = 0;
}

/**
 * @brief Destructor
 *
 * @return nothing
 */
SGL_OpenGLManager::~SGL_OpenGLManager()
{
    for (auto &iter : VBO)
        deleteVBO(iter.second.name);
    for (auto &iter : EBO)
        deleteEBO(iter.second.name);
    for (auto &iter : VAO)
        deleteVAO(iter.second.name);
    for (auto &iter : FBO)
        deleteFBO(iter.second.name);

    VBO.clear();
    VAO.clear();
    EBO.clear();
    FBO.clear();
}

/**
 * @brief Creates a VBO and adds it to the VBOs storage map
 *
 * @param name The VBO identifier
 * @return nothing
 */
void SGL_OpenGLManager::createVBO(const std::string &name)
{
    if (VBO.count(name) > 0 )
    {
#ifdef SGL_OUTPUT_OPENGL_DETAILS
        SGL_Log("VBO already exists: " + name, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_CYAN);
#endif
    }
    else
    {
        GLuint newVBO;
        glGenBuffers(1, &newVBO);
        GLBuffer storeBuffer;
        storeBuffer.name = name;
        storeBuffer.ID = newVBO;
        VBO[name] = storeBuffer;
#ifdef SGL_OUTPUT_OPENGL_DETAILS
        SGL_Log("Created VBO [ "+name+" ] with ID: " + std::to_string(VBO[name].ID), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_CYAN);
#endif
    }
}


/**
 * @brief Creates an EBO and adds it to the EBOs storage map
 *
 * @param name The EBO identifier
 * @return nothing
 */
void SGL_OpenGLManager::createEBO(const std::string &name)
{
    if (EBO.count(name) > 0 )
    {
#ifdef SGL_OUTPUT_OPENGL_DETAILS
        SGL_Log("EBO already exists: " + name, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_CYAN);
#endif
    }
    else
    {
        GLuint newEBO;
        glGenBuffers(1, &newEBO);
        GLBuffer storeBuffer;
        storeBuffer.name = name;
        storeBuffer.ID = newEBO;
        VBO[name] = storeBuffer;
#ifdef SGL_OUTPUT_OPENGL_DETAILS
        SGL_Log("Created EBO [ "+name+" ] with ID: " + std::to_string(EBO[name].ID), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_CYAN);
#endif
    }
}


/**
 * @brief Creates a VAOs and adds it to the VAOs storage map
 *
 * @param name The VAO identifier
 * @return nothing
 */
void SGL_OpenGLManager::createVAO(const std::string &name)
{
    if (VAO.count(name) > 0 )
    {
#ifdef SGL_OUTPUT_OPENGL_DETAILS
        SGL_Log("VAO already exists: " + name, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_CYAN);
#endif

    }
    else
    {
        GLuint newVAO;
        glGenVertexArrays(1, &newVAO);
        // SGL_OpenGLManager::pLog->print("New VAO: ", newVAO);
        GLBuffer storeBuffer;
        storeBuffer.name = name;
        storeBuffer.ID = newVAO;
        VAO[name] = storeBuffer;
#ifdef SGL_OUTPUT_OPENGL_DETAILS
        SGL_Log("Created VAO [ "+name+" ] with ID: " + std::to_string(VAO[name].ID), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_CYAN);
#endif
    }
}


/**
 * @brief Creates a FBOs and adds it to the FBOs storage map
 *
 * @param name The FBO identifier
 * @return nothing
 */
void SGL_OpenGLManager::createFBO(const std::string &name)
{
    if (FBO.count(name) > 0 )
    {
#ifdef SGL_OUTPUT_OPENGL_DETAILS
        SGL_Log("FBO already exists: " + name, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_CYAN);
#endif
    }
    else
    {
        GLuint newFBO;
        glGenFramebuffers(1, &newFBO);
        // SGL_OpenGLManager::pLog->print("New VAO: ", newFBO);
        GLBuffer storeBuffer;
        storeBuffer.name = name;
        storeBuffer.ID = newFBO;
        FBO[name] = storeBuffer;
#ifdef SGL_OUTPUT_OPENGL_DETAILS
        SGL_Log("Created FBO [ "+name+" ] with ID: " + std::to_string(FBO[name].ID), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_CYAN);
#endif
    }
}



/**
 * @brief Binds a VBO
 *
 * @param name The VBO identifier
 * @return nothing
 */
void SGL_OpenGLManager::bindVBO(const std::string &name)
{
    if (VBO.find(name) == VBO.end())
    {
#ifdef SGL_OUTPUT_OPENGL_DETAILS
        SGL_Log("ERROR: VBO NOT FOUND: " + name, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_CYAN);
#endif
    }
    else
    {
        currentGLBuffers.currentVBO = VBO.at(name);
        //update currently bound VBO
        GLuint bufferToBind = SGL_OpenGLManager::currentGLBuffers.currentVBO.ID;
        glBindBuffer(GL_ARRAY_BUFFER, bufferToBind);
    }
}


/**
 * @brief Binds a EBO
 *
 * @param name The EBO identifier
 * @return nothing
 */
void SGL_OpenGLManager::bindEBO(const std::string &name)
{
    if (EBO.find(name) == EBO.end())
    {
#ifdef SGL_OUTPUT_OPENGL_DETAILS
        SGL_Log("ERROR: EBO NOT FOUND: " + name, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_CYAN);
#endif
    }
    else
    {
        currentGLBuffers.currentEBO = EBO.at(name);
        //update currently bound VBO
        GLuint bufferToBind = SGL_OpenGLManager::currentGLBuffers.currentEBO.ID;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferToBind);
    }
}


/**
 * @brief Binds a VAO
 *
 * @param name The VAO identifier
 * @return nothing
 */
void SGL_OpenGLManager::bindVAO(const std::string &name)
{
    if (VAO.find(name) == VAO.end())
    {
#ifdef SGL_OUTPUT_OPENGL_DETAILS
        SGL_Log("ERROR: VAO NOT FOUND: " + name, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_CYAN);
#endif
    }
    else
    {
        currentGLBuffers.currentVAO = VAO.at(name);
        //update currently bound VBO
        GLuint bufferToBind = SGL_OpenGLManager::currentGLBuffers.currentVAO.ID;
        glBindVertexArray(bufferToBind);
    }
}


/**
 * @brief Binds a FBO
 *
 * @param name The FBO identifier
 * @return nothing
 */
void SGL_OpenGLManager::bindFBO(const std::string &name, GLCONSTANTS::FBO type)
{
    if (FBO.find(name) == FBO.end())
    {
#ifdef SGL_OUTPUT_OPENGL_DETAILS
        SGL_Log("ERROR: FBO NOT FOUND: " + name, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_CYAN);
#endif
    }
    else
    {
        GLuint bufferToBind = FBO.at(name).ID;
        switch (type)
        {
        case GLCONSTANTS::RW:
            glBindFramebuffer(GL_FRAMEBUFFER, bufferToBind);
            currentGLBuffers.currentFBO_R = FBO.at(name);
            currentGLBuffers.currentFBO_W = FBO.at(name);
            break;
        case GLCONSTANTS::R:
            glBindFramebuffer(GL_READ_FRAMEBUFFER, bufferToBind);
            currentGLBuffers.currentFBO_R = FBO.at(name);
            break;
        case GLCONSTANTS::W:
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, bufferToBind);
            currentGLBuffers.currentFBO_W = FBO.at(name);
            break;
        }
    }
}

/**
 * @brief Generates a texture
 *
 * @param n Amount to create
 * @param textures Gluint to hold the ID
 * @return nothing
 */
void SGL_OpenGLManager::genTextures(GLsizei n, GLuint *textures)
{
    glGenTextures(n, textures);
}


/**
 * @brief Binds a texture
 *
 * @param target Texture target
 * @param textures Texture ID
 * @return nothing
 */
void SGL_OpenGLManager::bindTexture(GLenum target, GLuint texture)
{
    glBindTexture(target, texture);
}

/**
 * @brief Deletes a texture
 *
 * @param n Amount to delete
 * @param textures Texture ID
 * @return nothing
 */
void SGL_OpenGLManager::deleteTextures(GLsizei n, const GLuint *textures)
{
    glDeleteTextures(n, textures);
}

/**
 * @brief  Set pixel storage modes
 *
 * @param pname Parameter to be set
 * @param param Value for the pname argument
 * @return nothing
 */
void SGL_OpenGLManager::pixelStorei(GLenum pname, GLint param)
{
    glPixelStorei(pname, param);
}

/**
 * @brief  Set texture parameters
 *
 * @param target Value for the pname argument
 * @param pname Specifies the texture object name for glTextureParameter functions.
 * @param param For the scalar commands, specifies the value of pname.
 * @return nothing
 */
void SGL_OpenGLManager::texParameteri(GLenum target, GLenum pname, GLint param)
{
    glTexParameteri(target, pname, param);
}

/**
 * @brief  Set texture parameters
 *
 * @param target Value for the pname argument
 * @param pname Specifies the texture object name for glTextureParameter functions.
 * @param param For the scalar commands, specifies the value of pname.
 * @return nothing
 */
void SGL_OpenGLManager::texParameterf(GLenum target, GLenum pname, GLfloat param)
{
    glTexParameterf(target, pname, param);
}


/**
 * @brief  Set texture parameters
 *
 * @param target Specifies the target texture of the active texture unit.
 * @param level Specifies the level-of-detail number. Level 0 is the base image level.
 * @param internalFormat Specifies the internal format of the texture.
 * @param width Specifies the width of the texture image.
 * @param height Specifies the height of the texture image.
 * @param border Specifies the width of the border. Must be 0.
 * @param format Specifies the format of the texel data.  Must match internalformat.
 * @param type Specifies the data type of the texel data.
 * @param data Specifies a pointer to the image data in memory.
 *
 * @return nothing
 */
void SGL_OpenGLManager::texImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *data)
{
    glTexImage2D(target, level, internalformat, width, height, border, format, type, data);
}


/**
 * @brief Return the location of a uniform variable in a shader
 *
 * @param program Specifies the program object to be queried.
 * @param name Points to a null terminated string containing the name of the uniform variable whose location is to be queried.
 * @return GLint
 */
GLint SGL_OpenGLManager::getUniformLocation(GLuint program, const GLchar *name)
{
    return glGetUniformLocation(program, name);
}

/**
 * @brief Specify the value of a uniform variable for the current program object
 *
 * @param location Specifies the location of the uniform variable to be modified.
 * @param v0 Value to use
 * @return nothing
 */
void SGL_OpenGLManager::uniform1f(GLint location, GLfloat v0)
{
    glUniform1f(location, v0);
}

/**
 * @brief Specify the value of a uniform variable for the current program object
 *
 * @param location Specifies the location of the uniform variable to be modified.
 * @param v0 Value to use
 * @return nothing
 */
void SGL_OpenGLManager::uniform1i(GLint location, GLint v0)
{
    glUniform1i(location, v0);
}

/**
 * @brief Specify the value of a uniform variable for the current program object
 *
 * @param location Specifies the location of the uniform variable to be modified.
 * @param count Specifies the number of elements that are to be modified
 * @param value Specifies a pointer to an array of count values that will be used to update the specified uniform variable.
 * @return nothing
 */
void SGL_OpenGLManager::uniform1fv(GLint location, GLsizei count, const GLfloat *value)
{
    glUniform1fv(location, count, value);
}

/**
 * @brief Specify the value of a uniform variable for the current program object
 *
 * @param location Specifies the location of the uniform variable to be modified.
 * @param count Specifies the number of elements that are to be modified
 * @param value Specifies a pointer to an array of count values that will be used to update the specified uniform variable.
 * @return nothing
 */
void SGL_OpenGLManager::uniform1iv(GLint location, GLsizei count, const GLint *value)
{
    glUniform1iv(location, count, value);
}

/**
 * @brief Specify the value of a uniform variable for the current program object
 *
 * @param location Specifies the location of the uniform variable to be modified.
 * @param count Specifies the number of elements that are to be modified
 * @param value Specifies a pointer to an array of count values that will be used to update the specified uniform variable.
 * @return nothing
 */
void SGL_OpenGLManager::uniform2fv(GLint location, GLsizei count, const GLfloat *value)
{
    glUniform2fv(location, count, value);
}

/**
 * @brief Specify the value of a uniform variable for the current program object
 *
 * @param location Specifies the location of the uniform variable to be modified.
 * @param count Specifies the number of elements that are to be modified
 * @param v1 Specifies a pointer to an array of count values that will be used to update the specified uniform variable.
 * @return nothing
 */
void SGL_OpenGLManager::uniform2f(GLint location, GLfloat v0, GLfloat v1)
{
    glUniform2f(location, v0, v1);
}


/**
 * @brief Specify the value of a uniform variable for the current program object
 *
 * @param location Specifies the location of the uniform variable to be modified.
 * @param v0 First value
 * @param v1 Second value
 * @param v2 Third value
 * @return nothing
 */
void SGL_OpenGLManager::uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    glUniform3f(location, v0, v1, v2);
}


/**
 * @brief Specify the value of a uniform variable for the current program object
 *
 * @param location Specifies the location of the uniform variable to be modified.
 * @param v0 First value
 * @param v1 Second value
 * @param v2 Third value
 * @param v3 Fourth value
 * @return nothing
 */
void SGL_OpenGLManager::uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    glUniform4f(location, v0, v1, v2, v3);
}


/**
 * @brief Specify the value of a uniform variable for the current program object
 *
 * @param location Specifies the location of the uniform variable to be modified.
 * @param count # of elements to modify
 * @param transpose For the matrix commands, specifies whether to transpose the matrix as the values are loaded into the uniform variable
 * @param value Pointer to the content
 * @return nothing
 */
void SGL_OpenGLManager::uniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    glUniformMatrix4fv(location, count, transpose, value);
}

/**
 * @brief Deletes a shader program
 *
 * @param program Shader program to delete
 * @return nothing
 */
void SGL_OpenGLManager::deleteProgram(GLuint program)
{
    glDeleteProgram(program);
}

/**
 * @brief Activates a shader program
 *
 * @param program Shader program to use
 * @return nothing
 */
void SGL_OpenGLManager::useProgram(GLuint program)
{
    currentShaderID = program;
    glUseProgram(program);
}

/**
 * @brief Specifies a shader source data
 *
 * @param shader Gluint ID use
 * @param count Amount of shaders
 * @param strin Shader source
 * @param length Shader source length
 * @return nothing
 */
void SGL_OpenGLManager::shaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length)
{
    glShaderSource(shader, count, string, length);
}

/**
 * @brief Creates a shader
 *
 * @param shaderType Type of shader to create
 * @return GLuint Created shader ID
 */
GLuint SGL_OpenGLManager::createShader(GLenum shaderType)
{
    return glCreateShader(shaderType);
}


/**
 * @brief Compiles a shader
 *
 * @param shader Shader ID
 * @return nothing
 */
void SGL_OpenGLManager::compileShader(GLuint shader)
{
    glCompileShader(shader);
}


/**
 * @brief Attaches a shader to a shader program
 *
 * @param program Shader program ID
 * @param shader Shader ID
 * @return nothing
 */
void SGL_OpenGLManager::attachShader(GLuint program, GLuint shader)
{
    glAttachShader(program, shader);
}


/**
 * @brief Deletes a shader
 *
 * @param shader Shader ID
 * @return nothing
 */
void SGL_OpenGLManager::deleteShader(GLuint shader)
{
    glDeleteShader(shader);
}


/**
 * @brief Creates a shader program
 *
 * @return nothing
 */
GLuint SGL_OpenGLManager::createProgram()
{
    return glCreateProgram();
}

/**
 * @brief Links a shader program
 *
 * @return nothing
 */
void SGL_OpenGLManager::linkProgram(GLuint program)
{
    glLinkProgram(program);
}

/**
 * @brief Return a parameter from a shader object
 *
 * @param program Shader program ID
 * @param pname Specifies the object parameter.
 * @param params Returns the requested object parameter.
 * @return nothing
 */
void SGL_OpenGLManager::getShaderiv(GLuint shader, GLenum pname, GLint *params)
{
    glGetShaderiv(shader, pname, params);
}

/**
 * @brief Return the information log for a shader object
 *
 * @param shader Shader program ID
 * @param maxLength Specifies the size of the character buffer for storing the returned information log.
 * @param length Returns the length of the string returned in infoLog (excluding the null terminator).
 * @param infoLog Specifies an array of characters that is used to return the information log.
 * @return nothing
 */
void SGL_OpenGLManager::getShaderInfoLog(GLuint shader,  GLsizei maxLength, GLsizei *length, GLchar *infoLog)
{
    glGetShaderInfoLog(shader, maxLength, length, infoLog);
}


/**
 * @brief Return a parameter from a shader object
 *
 * @param program Shader program ID
 * @param pname Specifies the object parameter.
 * @param params Returns the requested object parameter.
 * @return nothing
 */
void SGL_OpenGLManager::getProgramiv(GLuint shader, GLenum pname, GLint *params)
{
    glGetProgramiv(shader, pname, params);
}

/**
 * @brief Return the information log for a shader program_invocation_name object
 *
 * @param shader Shader program ID
 * @param maxLength Specifies the size of the character buffer for storing the returned information log.
 * @param length Returns the length of the string returned in infoLog (excluding the null terminator).
 * @param infoLog Specifies an array of characters that is used to return the information log.
 * @return nothing
 */
void SGL_OpenGLManager::getProgramInfoLog(GLuint shader,  GLsizei maxLength, GLsizei *length, GLchar *infoLog)
{
    glGetProgramInfoLog(shader, maxLength, length, infoLog);
}


/**
 * @brief Checks the FBO status
 * @param target FBOs to check on
 * @return nothing
 */
GLenum SGL_OpenGLManager::checkFrameBufferStatus(GLenum target)
{
    return glCheckFramebufferStatus(target);
}

/**
 * @brief Attach a texture image to a framebuffer object
 *
 * @param target Specifies the framebuffer target. The symbolic constant must be GL_FRAMEBUFFER.
 * @param attachmentSpecifies the attachment point to which an image from texture should be attached.
 * @param textarget Specifies the texture target.
 * @param texture Specifies the texture object whose image is to be attached.
 * @param level Specifies the mipmap level of the texture image to be attached, which must be 0.
 * @return nothing
 */
void SGL_OpenGLManager::frameBufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    glFramebufferTexture2D(target, attachment, textarget, texture, level);
}

/**
 * @brief Copy a block of pixels from one framebuffer object to another
 *
 * @param srcX0 Specify the bounds of the source rectangle within the read buffer of the read framebuffer.
 * @param srcY0 Specify the bounds of the source rectangle within the read buffer of the read framebuffer.
 * @param srcX1 Specify the bounds of the source rectangle within the read buffer of the read framebuffer.
 * @param srcY1 Specify the bounds of the source rectangle within the read buffer of the read framebuffer.
 * @param dstX0 Specify the bounds of the source rectangle within the read buffer of the read framebuffer.
 * @param dstY0 Specify the bounds of the source rectangle within the read buffer of the read framebuffer.
 * @param dstX1 Specify the bounds of the source rectangle within the read buffer of the read framebuffer.
 * @param dstY1 Specify the bounds of the source rectangle within the read buffer of the read framebuffer.
 * @param mask The bitwise OR of the flags indicating which buffers are to be copied. T
 * @param filter  Specifies the interpolation to be applied if the image is stretched. Must be GL_NEAREST or GL_LINEAR. 
 * @return nothing
 */
void SGL_OpenGLManager::blitFrameBuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
    glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}


/**
 * @brief Unbinds active VBO and EBO
 * @return nothing
 */
void SGL_OpenGLManager::clearBuffers()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/**
 * @brief Deletes a VBO from the map
 * @param name the VBO identifier
 * @return nothing
 */
void SGL_OpenGLManager::deleteVBO(const std::string &name)
{
    if (!VBO.empty() && VBO.find(name) == VBO.end())
    {
        GLBuffer byeBuffer = VBO.at(name);
        glDeleteBuffers(1, &byeBuffer.ID);
        VBO.erase(name);
    }
}


/**
 * @brief Deletes a EBO from the map
 * @param name the EBO identifier
 * @return nothing
 */
void SGL_OpenGLManager::deleteEBO(const std::string &name)
{
    if (!EBO.empty() && EBO.find(name) == EBO.end())
    {
        GLBuffer byeBuffer = EBO.at(name);
        glDeleteBuffers(1, &byeBuffer.ID);
        EBO.erase(name);
    }
}

/**
 * @brief Deletes a VAO from the map
 * @param name the VAO identifier
 * @return nothing
 */
void SGL_OpenGLManager::deleteVAO(const std::string &name)
{
    if (!VAO.empty() && VAO.find(name) == VAO.end())
    {
        GLBuffer byeBuffer = VAO.at(name);
        glDeleteBuffers(1, &byeBuffer.ID);
        VAO.erase(name);
    }
}


/**
 * @brief Deletes a FBO from the map
 * @param name the FBO identifier
 * @return nothing
 */
void SGL_OpenGLManager::deleteFBO(const std::string &name)
{
    if (!FBO.empty() && FBO.find(name) == FBO.end())
    {
        GLBuffer byeBuffer = FBO.at(name);
        glDeleteBuffers(1, &byeBuffer.ID);
        FBO.erase(name);
    }
}

/**
 * @brief Unbind VBO
 * @return nothing
 */
void SGL_OpenGLManager::unbindVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    currentGLBuffers.currentVBO.name = "NO_VBO";
    currentGLBuffers.currentVBO.ID = 0;
}


/**
 * @brief Unbind EBO
 * @return nothing
 */
void SGL_OpenGLManager::unbindEBO()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    currentGLBuffers.currentEBO.name = "NO_EBO";
    currentGLBuffers.currentEBO.ID = 0;
}


/**
 * @brief Unbind VAO
 * @return nothing
 */
void SGL_OpenGLManager::unbindVAO()
{
    glBindVertexArray(0);
    currentGLBuffers.currentVAO.name = "NO_VAO";
    currentGLBuffers.currentVAO.ID = 0;

}

/**
 * @brief Unbind FBO
 * @return nothing
 */
void SGL_OpenGLManager::unbindFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    currentGLBuffers.currentFBO_W.name = "NO_WRITE_FBO";
    currentGLBuffers.currentFBO_R.name = "NO_READ_FBO";

    currentGLBuffers.currentFBO_R.ID = 0;
    currentGLBuffers.currentFBO_W.ID = 0;

}



/**
 * @brief Toggles depth testing
 * @param value Enable depth testing?
 * @return nothing
 */
void SGL_OpenGLManager::depthTesting(bool value)
{
    if (value)
    {
        glEnable(GL_DEPTH_TEST);
        currentGLSettings.depth.active = true;
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
        currentGLSettings.depth.active = false;
    }
}


/**
 * @brief Toggles depth mask
 * @param value Enable depth mask?
 * @return nothing
 */
void SGL_OpenGLManager::depthMask(bool value)
{
    if (value)
    {
        glDepthMask(GL_TRUE);
        currentGLSettings.depthMask.active = true;
    }
    else
    {
        glDepthMask(GL_FALSE);
        currentGLSettings.depthMask.active = false;
    }
}

/**
 * @brief Toggles face culling
 * @param value Enable face culling?
 * @return nothing
 */
void SGL_OpenGLManager::faceCulling(bool value)
{
    if (value)
    {
        glEnable(GL_CULL_FACE);
        currentGLSettings.faceCulling.active = true;
    }
    else
    {
        glDisable(GL_CULL_FACE);
        currentGLSettings.faceCulling.active = false;
    }
}


/**
 * @brief Toggles blending modes
 * @param value Enable blending mode??
 * @param type Blending mode to use
 * @return nothing
 */
void SGL_OpenGLManager::blending(bool value, BLENDING_TYPE type)
{
    if (value)
    {
        glEnable(GL_BLEND);
        currentGLSettings.blending.active = true;

        switch(type)
        {
        case NONE:
            break;
        case DEFAULT_RENDERING:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case TEST_RENDERING:
            glBlendFunc(GL_SRC_COLOR,  GL_DST_ALPHA);
            break;
        case TEXT_RENDERING:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case PARTICLE_RENDERING:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        case LIGHT_RENDERING:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;

        case LIGHT_SRC:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case DARKNESS:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            // glBlendFunc(GL_SRC_ALPHA,  GL_ONE_MINUS_DST_COLOR); // color inverter
            break;
        }
    }
    else
    {
        glDisable(GL_BLEND);
        currentGLSettings.blending.active = false;
    }
}

/**
 * @brief Resize the view port
 * @param x New x value
 * @param y New y value
 * @param w New w value
 * @param h New h value
 * @return nothing
 */
void SGL_OpenGLManager::resizeGL(int x, int y, int w, int h)
{
    glViewport(x, y, w, h);
    // SGL_OpenGLManager::pLog->print("New window width: ", w);
    // SGL_OpenGLManager::pLog->print("New window heigth: ", h);
}

/**
 * @brief Sets the clear screen color
 * @param newColor The new clear screen color
 * @return nothing
 */
void SGL_OpenGLManager::setClearColor(SGL_Color &newColor)
{
    glClearColor(newColor.r, newColor.g, newColor.b, newColor.a);

}


/**
 * @brief Clears the color buffer
 * @return nothing
 */
void SGL_OpenGLManager::clearColorBuffer()
{
    glClear(GL_COLOR_BUFFER_BIT);
}


/**
 * @brief Clears the depth buffer
 * @return nothing
 */
void SGL_OpenGLManager::clearDepthBuffer()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}


/**
 * @brief Clears both the color and depth buffer
 * @return nothing
 */
void SGL_OpenGLManager::clearColorAndDepthBuffers()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


/**
 * @brief Creates and initializes a buffer object's data store
 *
 * @param buffer Specifies the name of the buffer object for glNamedBufferData function.
 * @param size Specifies the size in bytes of the buffer object's new data store.
 * @param data Specifies a pointer to data that will be copied into the data store for initialization, or NULL if no data is to be copied.
 * @param usage Specifies the expected usage pattern of the data store.
 * @return nothing
 */
void SGL_OpenGLManager::bufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
    glBufferData(target, size, data, usage);
}

/**
 * @brief Updates a subset of a buffer object's data store
 *
 * @param buffer Specifies the name of the buffer object for glNamedBufferData function.
 * @param offset Specifies the offset into the buffer object's data store where data replacement will begin, measured in bytes.
 * @param size Specifies the size in bytes of the buffer object's new data store.
 * @param data Specifies a pointer to data that will be copied into the data store for initialization, or NULL if no data is to be copied.
 * @return nothing
 */
void SGL_OpenGLManager::bufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    glBufferSubData(target, offset, size, data);
}


/**
 * @brief Activates a texture
 *
 * @param texture Texture ID to activate
 * @return nothing
 */
void SGL_OpenGLManager::activeTexture(GLenum texture)
{
    currentTextureID = texture;
    glActiveTexture(texture);
}


/**
 * @brief Render primitives from array data
 *
 * @param mode Specifies what kind of primitives to render.
 * @param first Specifies the starting index in the enabled arrays.
 * @param count Specifies the number of indices to be rendered.
 * @return nothing
 */
void SGL_OpenGLManager::drawArrays(GLenum mode, GLint first, GLsizei count)
{
    glDrawArrays(mode, first, count);
}

/**
 * @brief Draw multiple instances of a range of elements
 *
 * @param mode Specifies what kind of primitives to render.
 * @param first Specifies the starting index in the enabled arrays.
 * @param count Specifies the number of indices to be rendered.
 * @param instancecount Specifies the number of instances of the specified range of indices to be rendered
 * @return nothing
 */
void SGL_OpenGLManager::drawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
    glDrawArraysInstanced(mode, first, count, instancecount);
}

/**
 * @brief Draw multiple instances of a range of elements
 *
 * @param mode Specifies what kind of primitives to render.
 * @param count Specifies the number of elements to be rendered.
 * @param type Specifies the type of the values in indices. Must be one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT.
 * @param indices Specifies the number of instances of the specified range of indices to be rendered
 * @param instancecount  Specifies the number of instances of the specified range of indices to be rendered.
 * @return nothing
 */
void SGL_OpenGLManager::drawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{
    glDrawElementsInstanced(mode, count, type, indices, instancecount);
}

/**
 * @brief Enable or disable a generic vertex attribute array
 * @param index Specifies the index of the generic vertex attribute to be enabled or disabled.
 * @return nothing
 */
void SGL_OpenGLManager::enableVertexAttribArray(GLuint index)
{
    glEnableVertexAttribArray(index);
}

/**
 * @brief Modify the rate at which generic vertex attributes advance during instanced rendering
 * @param index Specifies the index of the generic vertex attribute to be enabled or disabled.
 * @param divisor  Specify the number of instances that will pass between updates of the generic attribute at slot index.
 * @return nothing
 */
void SGL_OpenGLManager::vertexAttribDivisor(GLuint index, GLint divisor)
{
    glVertexAttribDivisor(index, divisor);
}


/**
 * @brief Define an array of generic vertex attribute data
 *
 * @param index Specifies the index of the generic vertex attribute to be modified.
 * @param size Specifies the number of components per generic vertex attribute.
 * @param type Specifies the data type of each component in the array.
 * @param normalized For glVertexAttribPointer, specifies whether fixed-point data values should be normalized (GL_TRUE) or converted directly as fixed-point values (GL_FALSE) when they are accessed.
 * @param stride Specifies the byte offset between consecutive generic vertex attributes.
 * @param pointer Specifies a offset of the first component of the first generic vertex attribute in the array in the data store of the buffer currently bound to the GL_ARRAY_BUFFER target. The initial value is 0.
 * @return nothing
 */
void SGL_OpenGLManager::vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer)
{
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}


/**
 * @brief Check for anyhow internal OpenGL errors
 * @return nothing
 */
void SGL_OpenGLManager::checkForGLErrors()
{
    GLenum errCode;
    const GLubyte *errString;

    if ((errCode = glGetError()) != GL_NO_ERROR)
    {
        if (initialErrorCap == 0)
        {
            initialErrorCap++;
            return;
        }

        errString = gluErrorString(errCode);
        std::stringstream ss;
        ss << errString;
        SGL_Log("SkeletonGL OpenGL Error: " + ss.str(), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_RED);
        SGL_Log("Current shader ID: " + std::to_string(currentShaderID), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_RED);
        SGL_Log("Current texture ID: " + std::to_string(currentTextureID), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_RED);
        SGL_Log("Current VAO: " + currentGLBuffers.currentVAO.name + " " + std::to_string(currentGLBuffers.currentVAO.ID), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_RED);
        //assert(false);
    }
}

/**
 * @brief Returns the total active VBOs
 * @return int
 */
int  SGL_OpenGLManager::totalVBO()
{
    return VBO.size();
}

/**
 * @brief Returns the total active VAOs
 * @return int
 */
int SGL_OpenGLManager::totalVAO()
{
    return VAO.size();
}

/**
 * @brief Returns the total active EBOs
 * @return int
 */
int SGL_OpenGLManager::totalEBO()
{
    return EBO.size();
}

/**
 * @brief Returns the total active FBOs
 * @return int
 */
int SGL_OpenGLManager::totalFBO()
{
    return FBO.size();
}
