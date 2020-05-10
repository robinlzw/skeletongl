
// ------------------- By: TSURA @ -------------------
// ███╗   ██╗███████╗ ██████╗ ██╗  ██╗███████╗██╗  ██╗
// ████╗  ██║██╔════╝██╔═══██╗██║  ██║██╔════╝╚██╗██╔╝
// ██╔██╗ ██║█████╗  ██║   ██║███████║█████╗   ╚███╔╝
// ██║╚██╗██║██╔══╝  ██║   ██║██╔══██║██╔══╝   ██╔██╗
// ██║ ╚████║███████╗╚██████╔╝██║  ██║███████╗██╔╝ ██╗
// ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
// ---------------------- [.xyz] ---------------------

/**
 * @file    src/skeletonGL/utility/SGL_As.cpp
 * @author  TSURA @ NEOHEX.XYZ
 * @date    9/4/2018
 * @version 1.0
 *
 * @brief Loads and manages all assets, including compiling and linking shaders
 *
 * @section DESCRIPTION
 *
 * This class loads and converts asset files into usable resources.
 * Image files supported: PNG, JPG, BMP
 * Font formats supported: TTF
 *
 */
#ifndef SRC_SKELETONGL_ASSETS_ASSET_MANAGER_HPP
#define SRC_SKELETONGL_ASSETS_ASSET_MANAGER_HPP
// C++
#include <map>
#include <string>
#include <vector>
#include <memory>
// SkeletonGL
#include "../renderer/SGL_OpenGLManager.hpp"
#include "../renderer/SGL_Texture.hpp"
#include "../renderer/SGL_Shader.hpp"
#include "SGL_DataStructures.hpp"
#include "SGL_Utility.hpp"

/**
 *  @brief Manages all rendering resources
 */
class SGL_AssetManager
{
private:
    std::shared_ptr<SGL_OpenGLManager> WMOGLM;                    ///< Owned by the windowManager
    std::map<std::string, SGL_Shader> shaders;                    ///< Map of all available shaders
    std::map<std::string, SGL_Texture> textures;                  ///< Map of all available textures
    long long int pTextureGPUMemory;                              ///< How much GPU memory the program is using

    // Parses, compiles and links a shader, geometry shader is optional
    SGL_Shader loadShaderFromFile(SHADER_TYPE shaderType, const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr);
    // Creates an OpenGL texture from a supported image
    SGL_Texture loadTextureFromFile(const GLchar *file, GLboolean alpha);

public:
    // Constructor
    SGL_AssetManager(std::shared_ptr<SGL_OpenGLManager> oglm);
    // Destructor
    ~SGL_AssetManager();

    //loads and generates a shader program from a source file
    SGL_Shader loadShaders(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource, std::string name, SHADER_TYPE shaderType);
    //finds and retrieves a stored shader, throws on error (can't render without a shader)
    SGL_Shader getShader(std::string name);
    //loads and generates a texture from a file, throws SGL_Exception if shit happens
    SGL_Texture loadTexture(const GLchar *file, GLboolean alpha, std::string name);
    //finds and retrieves a stored texture
    SGL_Texture getTexture(std::string name);

    // This map contains an accessible container
    // with all the loaded shaders and their respective
    // types, this to differentiate between shaders
    // that the main camera's MVP matrix must update
    // (only non FBO shaders so far)
    std::map<std::string, SHADER_TYPE> shaderTypes;

    // Returns the total GPU memory allocated
    int getTextureMemoryGPU();
};
#endif // SRC_SKELETONGL_ASSETS_ASSET_MANAGER_HPP
