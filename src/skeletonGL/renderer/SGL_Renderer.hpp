
// ------------------- By: TSURA @ -------------------
// ███╗   ██╗███████╗ ██████╗ ██╗  ██╗███████╗██╗  ██╗
// ████╗  ██║██╔════╝██╔═══██╗██║  ██║██╔════╝╚██╗██╔╝
// ██╔██╗ ██║█████╗  ██║   ██║███████║█████╗   ╚███╔╝
// ██║╚██╗██║██╔══╝  ██║   ██║██╔══██║██╔══╝   ██╔██╗
// ██║ ╚████║███████╗╚██████╔╝██║  ██║███████╗██╔╝ ██╗
// ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
// ---------------------- [.xyz] ---------------------

/**
 * @file    src/skeletonGL/utility/SGL_Renderer.hpp
 * @author  TSURA @ NEOHEX.XYZ
 * @date    9/4/2018
 * @version 1.0
 *
 * @brief Renders to the currently bound post processor (FBO)
 *
 * @section DESCRIPTION
 *
 * This file defines all of the framework's rendering entities
 */

#ifndef SRC_SKELETONGL_RENDERER_RENDERER_HPP
#define SRC_SKELETONGL_RENDERER_RENDERER_HPP

// C++
#include <iostream>
#include <string>
#include <memory>
#include <map>

// GLM
#include "../deps/glm/glm.hpp"
#include "../deps/glm/gtc/matrix_transform.hpp"

// LIB FREETYBE
#include <ft2build.h>
#include FT_FREETYPE_H

// SkeletonGL
#include "SGL_OpenGLManager.hpp"
#include "SGL_Shader.hpp"
#include "../utility/SGL_DataStructures.hpp"
#include "../utility/SGL_Utility.hpp"
#include "SGL_Texture.hpp"

/**
 * @brief Defnies a sprite to be rendered
 * @section DESCRIPTION
 *
 * All the sprite's rendering details are to be set in here
 */
struct SGL_Sprite
{
public:
    glm::vec2 position, size, rotationOrigin; ///< Positioning, scaling and rotation origin
    GLfloat rotation;                         ///< Rotation in radians (degrees deprecated in glm::rotate since 0.9.6)
    SGL_Color color;                          ///< Sprite color
    UV_Wrapper uvCoords;                      ///< Texture coordinates
    SGL_Texture texture;                      ///< Sprite texture
    bool enableCustomUV;                      ///< Does the sprite require custom UV values
    SGL_Shader shader;                        ///< Sprite shader
    BLENDING_TYPE blending;                   ///< Blending type

    // Reset the UV coordinates to show the full texture
    void resetUVCoords()
        {
            // Empty texture
            if (texture.width <= 0)
                return;
            else
                this->changeUVCoords(0, 0, texture.width, texture.height);
        }

    // Specify a custom quad as UV coordinates
    void changeUVCoords(int x, int y, int w, int h)
        {
            // stb_image can automatically flip loaded textures on their y-axis
            // to conform with opengl's flipped y-axis, this must be taken into
            // account when processing UV values

            // Check for empty texture
            if (texture.width <= 0)
                return;

            // Top left
            this->uvCoords.UV_topLeft.x = x;
            this->uvCoords.UV_topLeft.y = texture.height - (y + h);
            // Top right
            this->uvCoords.UV_topRight.x = x + w;
            this->uvCoords.UV_topRight.y = this->uvCoords.UV_topLeft.y;
            // Bottom right
            this->uvCoords.UV_botRight.x = x + w;
            this->uvCoords.UV_botRight.y = texture.height - y;
            this->uvCoords.UV_botLeft.x = x;
            this->uvCoords.UV_botLeft.y = this->uvCoords.UV_botRight.y;
        }


    SGL_Sprite() : enableCustomUV(false), position(0.0f), size(0.0f), rotationOrigin(0.0f), rotation(0.0f), blending(DEFAULT_RENDERING) {}
};


/**
 * @brief Defines a single pixel
 * @section DESCRIPTION
 *
 * Represents an individual pixel on the screen
 */
struct SGL_Pixel
{
    glm::vec2 position;             ///< Pixel position
    SGL_Color color;                ///< Pixel color
    SGL_Shader shader;              ///< Shader to process the pixel (because why the fuck not)
};

/**
 * @brief Defines a line
 * @section DESCRIPTION
 *
 * Represents an individual line on the screen
 */
struct SGL_Line
{
    glm::vec2 positionA, positionB; ///< Vertices for both ends of the line
    SGL_Color color;                ///< Line color
    SGL_Shader shader;              ///< Line shader
};


/**
 * @brief Encapsulates a string to be rendered by a TTF generated font
 * @section DESCRIPTION
 *
 * Represents a TTF rendered string on the screen
 */
struct SGL_Text
{
    glm::vec2 position;             ///< Text position
    float scale;                    ///< Text size
    std::string message;            ///< The text string to be rendered
    SGL_Color color;                ///< Text color
    SGL_Shader shader;              ///< Text shader
};

struct SGL_Bitmap_Text
{
    glm::vec2 position;
    std::string text;
    SGL_Texture texture;
    SGL_Shader shader;
    SGL_Color color;
    std::uint8_t scale;
};


/**
 * @brief Manages the rendering process and setup
 * @section DESCRIPTION
 *
 * This class encapsulates all the actual rendering functions
 * and manages all the relevant buffers. Comes with default
 * shaders.

 */
class SGL_Renderer
{
private:
    std::shared_ptr<SGL_OpenGLManager> WMOGLM; ///< Window's OpenGL context
    SGL_Color pDefaultColor;
    std::string pLineVAO, pLineVBO, pPointVAO,
        pPointVBO, pTextVAO, pTextVBO, pSpriteVAO,
        pSpriteVBO, pTextureUVVBO;             ///< All the required OpenGL buffers
    SGL_Shader pLineShader, pPixelShader;      ///< Shader for the line and pixel renderers
    std::map<GLchar, Character> characters;    ///< Storage the loaded cahracters in a map
    std::map<char, glm::vec4> pBitmapCharacters; ///< Map storing a character's represntation in the bitmap font texture
    SGL_Shader pTextShader;                    ///< Text rendering shader
    SGL_Shader pSpriteShader;                  ///< Sprite shader
    SGL_Shader pSpriteBatchShader;             ///< Sprite batch shader
    SGL_Shader pPixelBatchShader;              ///< Pixel batch shader
    SGL_Shader pLineBatchShader;              ///< Pixel batch shader
    UV_Wrapper pDefaultUV;                     ///< Default UV values

    // EXPERIMENTAL
    // SPRITE BATCHING
    std::string pSpriteBatchVBO, pSpriteBatchVAO, pSpriteBatchInstancesVBO;
    std::uint32_t pBatchAmount;
    glm::mat4 *modelMatrices;

    // PIXEL BATCHING
    std::string pPixelBatchVBO, pPixelBatchVAO, pPixelBatchInstancesVBO;
    std::uint32_t pPixelBatchAmount;
    glm::vec2 *posVectors;

    // Line BATCHING
    std::string pLineBatchVBO, pLineBatchVAO, pLineBatchInstancesVBO;
    std::uint32_t pLineBatchAmount;
    glm::vec4 *pLineVectors;                   ///< Pointer to the data to be batched


    // Its important to point out that the individual pixel, line and sprite renderers
    // have independent VBO, VAO and shaders

    // Load all the required line buffers
    void loadLineBuffers(SGL_Shader shader);
    void loadLineBatchBuffers(SGL_Shader shader);

    // Load all the required pixel buffers
    void loadPointBuffers(SGL_Shader shader);
    void loadPixelBatchBuffers(SGL_Shader shader);

    // Load all the required sprite buffers
    void loadSpriteBuffers(SGL_Shader shader);
    void loadSpriteBatchBuffers(SGL_Shader shader);

    // Load and generate the ttf font
    void generateFont(const std::string fontPath);

    // Generate the lookup table for bitmap characters
    void generateBitmapFont();

    // Disable all copy and move constructors
    SGL_Renderer(const SGL_Renderer&) = delete;
    SGL_Renderer *operator = (const SGL_Renderer&) = delete;
    SGL_Renderer(SGL_Renderer &&) = delete;
    SGL_Renderer &operator = (SGL_Renderer &&) = delete;

public:
    // Constructor
    SGL_Renderer(std::shared_ptr<SGL_OpenGLManager> oglm, const SGL_Shader &shaderLine, const SGL_Shader &shaderPoint,
                 const SGL_Shader &shaderText, const SGL_Shader &spriteShader, const SGL_Shader &spriteBatchShader, const SGL_Shader &pixelBatchShader, const SGL_Shader &lineBatchShader);
    // Destructor
    ~SGL_Renderer();

    // Render a line
    void renderLine(const SGL_Line &line) const;
    // Render a line
    void renderLine(float x1, float y1, float x2, float y2, SGL_Color color);

    // Render a pixel
    void renderPixel(float x1, float y1, SGL_Color color);
    // Render a pixel
    void renderPixel(const SGL_Pixel &pixel) const;

    // Render a string
    void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, SGL_Color color);
    // Render a string
    void renderText(SGL_Text &text);

    // Renders text as bitmap characters
    void renderBitmapText(SGL_Bitmap_Text &text);
    // The bitmap font renderer requires a default texture
    // void renderBitmapText(std::string text, GLfloat x, GLfloat y, GLfloat scale, SGL_Color color);

    // Render a sprite
    void renderSprite(const SGL_Sprite &sprite);

    // EXPERIMENTAL
    void renderSpriteBatch(const SGL_Sprite &sprite);
    void renderPixelBatch(const SGL_Pixel &pixel);
    void renderLineBatch(const SGL_Line &line);

    void renderSpriteBatch(const SGL_Sprite &sprite, const std::vector<glm::vec2> *vectors);
};

#endif //SRC_SKELETONGL_RENDERER_RENDERER_HPP
