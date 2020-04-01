
// ------------------- By: TSURA @ -------------------
// ███╗   ██╗███████╗ ██████╗ ██╗  ██╗███████╗██╗  ██╗
// ████╗  ██║██╔════╝██╔═══██╗██║  ██║██╔════╝╚██╗██╔╝
// ██╔██╗ ██║█████╗  ██║   ██║███████║█████╗   ╚███╔╝
// ██║╚██╗██║██╔══╝  ██║   ██║██╔══██║██╔══╝   ██╔██╗
// ██║ ╚████║███████╗╚██████╔╝██║  ██║███████╗██╔╝ ██╗
// ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
// ---------------------- [.xyz] ---------------------

/**
 * @file    src/skeletonGL/utility/SGL_Renderer.cpp
 * @author  AlexHG
 * @date    9/4/2018
 * @version 1.0
 *
 * @brief Renders to the currently bound post processor (FBO)
 *
 * @section DESCRIPTION
 *
 * This file defines all of the framework's rendering entities
 */

#include "SGL_Renderer.hpp"

/**
 * @brief Constructor
 *
 * @param oglm The window's OpenGL manager
 * @param shaderLine The default shader for the line renderer
 * @param shaderPoint The default shader for the pixel renderer
 * @param shaderText The default shader for the text renderer
 * @param spriteShader The default shader for the sprite renderer
 * @return nothing
 */
SGL_Renderer::SGL_Renderer(std::shared_ptr<SGL_OpenGLManager> oglm, const SGL_Shader &shaderLine, const SGL_Shader &shaderPoint,
                           const SGL_Shader &shaderText, const SGL_Shader &spriteShader, const SGL_Shader &spriteBatchShader,
                           const SGL_Shader &pixelBatchShader, const SGL_Shader &lineBatchShader)
{
    SGL_Log("Configuring renderer...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    // Geometry shader configuration
    this->WMOGLM = oglm;
    this->pDefaultColor = {1.0f, 1.0f, 1.0f, 1.0f};
    this->pLineShader = shaderLine;
    this->pPixelShader = shaderPoint;
    this->pTextShader = shaderText;
    this->pSpriteShader = spriteShader;
    this->pSpriteBatchShader = spriteBatchShader;
    this->pPixelBatchShader = pixelBatchShader;
    this->pLineBatchShader = lineBatchShader;


    // Generic buffers: They hold vertex, color
    this->pLineVAO = "_SGL_Renderer_line_VAO";
    this->pLineVBO = "_SGL_Renderer_line_VBO";
    this->pPointVAO = "_SGL_Renderer_pixel_VAO";
    this->pPointVBO = "_SGL_Renderer_pixel_VBO";
    this->pTextVBO = "_SGL_Renderer_text_VBO";
    this->pTextVAO = "_SGL_Renderer_text_VAO";
    this->pSpriteVAO = "_SGL_Renderer_sprite_VAO";
    this->pSpriteVBO = "_SGL_Renderer_sprite_VBO";

    this->pSpriteBatchVBO = "_SGL_Renderer_spriteBatch_VBO";
    this->pSpriteBatchVAO = "_SGL_Renderer_spriteBatch_VAO";

    this->pPixelBatchVBO = "_SGL_Renderer_pixelBatch_VBO";
    this->pPixelBatchVAO = "_SGL_Renderer_pixelBatch_VAO";

    this->pLineBatchVAO = "_SGL_Renderer_lineBatch_VAO";
    this->pLineBatchVBO = "_SGL_Renderer_lineBatch_VBO";

    // Texture buffers: Simple buffer to hold UV data
    this->pTextureUVVBO = "_SGL_Renderer_textureCoords_VBO";


    // Instancing buffers: These VBO are used to allocate the memory required to
    // fit the batch / instances to be rendered, this same buffer is then recycled
    // with the actual data to be stored in the previously allocated GPU memory.
    this->pPixelBatchInstancesVBO = "_SGL_Renderer_pixelBatch_instances_VBO";
    this->pLineBatchInstancesVBO = "_SGL_Renderer_lineBatch_instances_VBO";
    this->pSpriteBatchInstancesVBO = "_SGL_Renderer_spriteBatch_instances_VBO";


    // VAOs
    WMOGLM->createVAO(this->pLineVAO);
    WMOGLM->createVAO(this->pPointVAO);
    WMOGLM->createVAO(this->pTextVAO);
    WMOGLM->createVAO(this->pSpriteVAO);
    WMOGLM->createVAO(this->pSpriteBatchVAO);
    WMOGLM->createVAO(this->pPixelBatchVAO);
    WMOGLM->createVAO(this->pLineBatchVAO);

    // VBOs
    WMOGLM->createVBO(this->pPointVBO);
    WMOGLM->createVBO(this->pLineVBO);
    WMOGLM->createVBO(this->pTextVBO);
    WMOGLM->createVBO(this->pSpriteVBO);
    WMOGLM->createVBO(this->pTextureUVVBO);
    WMOGLM->createVBO(this->pSpriteBatchVBO);
    WMOGLM->createVBO(this->pPixelBatchVBO);
    WMOGLM->createVBO(this->pLineBatchVBO);
    WMOGLM->createVBO(pPixelBatchInstancesVBO);
    WMOGLM->createVBO(pSpriteBatchInstancesVBO);
    WMOGLM->createVBO(pLineBatchInstancesVBO);

    this->generateFont(FOLDER_STRUCTURE::defaultFont);

    this->loadLineBuffers(shaderLine);
    this->loadPointBuffers(shaderPoint);
    this->loadSpriteBuffers(pSpriteShader);

    this->loadPixelBatchBuffers(pPixelBatchShader);
    this->loadSpriteBatchBuffers(pSpriteBatchShader);
    this->loadLineBatchBuffers(pLineBatchShader);

}

/**
 * @brief Destructor
 *
 * @return nothing
 */
SGL_Renderer::~SGL_Renderer()
{
    delete[] modelMatrices;

    // VAOs
    WMOGLM->deleteVAO(this->pLineVAO);
    WMOGLM->deleteVAO(this->pPointVAO);
    WMOGLM->deleteVAO(this->pTextVAO);
    WMOGLM->deleteVAO(this->pSpriteVAO);
    WMOGLM->deleteVAO(this->pSpriteBatchVAO);
    WMOGLM->deleteVAO(this->pPixelBatchVAO);
    WMOGLM->deleteVAO(this->pLineBatchVAO);

    // VBOs
    WMOGLM->deleteVBO(this->pLineVBO);
    WMOGLM->deleteVBO(this->pPointVBO);
    WMOGLM->deleteVBO(this->pTextVBO);
    WMOGLM->deleteVBO(this->pTextureUVVBO);
    WMOGLM->deleteVBO(this->pSpriteVBO);
    WMOGLM->deleteVBO(this->pSpriteBatchVBO);
    WMOGLM->deleteVBO(this->pPixelBatchVBO);
    WMOGLM->deleteVBO(this->pLineBatchVBO);
    WMOGLM->deleteVBO(this->pPixelBatchInstancesVBO);
    WMOGLM->deleteVBO(this->pLineBatchInstancesVBO);
    WMOGLM->deleteVBO(this->pSpriteBatchInstancesVBO);

}

/**
 * @brief Renders a pixel
 * @param pixel The Pixel object to render
 *
 * @return nothing
 */
void SGL_Renderer::renderPixel(const SGL_Pixel &pixel) const
{
    SGL_Shader activeShader;
    if (pixel.shader.shaderType != SHADER_TYPE::PIXEL)
        activeShader = pPixelShader;
    else
        activeShader = pixel.shader;

    // NOTE, coords are normalized, gotta deal with that
    GLfloat vertices[] = {
        pixel.position.x, pixel.position.y
    };
    WMOGLM->bindVAO(this->pPointVAO);
    WMOGLM->bindVBO(this->pPointVBO);
    activeShader.use(*WMOGLM);

    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(vertices), &vertices[0]);
    glm::vec4 assignedColor = {pixel.color.r, pixel.color.g, pixel.color.b, pixel.color.a};
    activeShader.setVector4f(*WMOGLM, "pointColor", assignedColor);
    activeShader.setFloat(*WMOGLM, "deltaTime", activeShader.renderDetails.deltaTime);
    WMOGLM->drawArrays(GL_POINTS, 0, 2);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    activeShader.unbind(*WMOGLM);
}


/**
 * @brief Renders a pixel
 * @param x1 X position of the pixel
 * @param y1 Y position of the pixel
 * @param color The pixel's color
 *
 * @return nothing
 */
void SGL_Renderer::renderPixel(float x1, float y1, SGL_Color color)
{
    // NOTE, coords are normalized, gotta deal with that
    GLfloat vertices[] = {
        x1, y1
    };
    WMOGLM->bindVAO(this->pPointVAO);
    WMOGLM->bindVBO(this->pPointVBO);
    pPixelShader.use(*WMOGLM);

    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(vertices), &vertices[0]);
    glm::vec4 assignedColor = {color.r, color.g, color.b, color.a};
    pPixelShader.setVector4f(*WMOGLM, "pointColor", assignedColor);
    pPixelShader.setFloat(*WMOGLM, "deltaTime", pPixelShader.renderDetails.deltaTime);

    WMOGLM->drawArrays(GL_POINTS, 0, 2);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    pPixelShader.unbind(*WMOGLM);
}


/**
 * @brief Renders a line
 * @param line SGL_Line to render
 *
 * @return nothing
 */
void SGL_Renderer::renderLine(const SGL_Line &line) const
{
    SGL_Shader activeShader;
    if (line.shader.shaderType != SHADER_TYPE::LINE)
        activeShader = pLineShader;
    else
        activeShader = line.shader;

    //NOTE, coords are normalized, gotta deal with that
    GLfloat vertices[] = {
        line.positionA.x, line.positionA.y, line.positionB.x, line.positionB.y
    };
    WMOGLM->bindVAO(this->pLineVAO);
    WMOGLM->bindVBO(this->pLineVBO);
    activeShader.use(*WMOGLM);

    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(vertices), &vertices[0]);
    glm::vec4 assignedColor = {line.color.r, line.color.g, line.color.b, line.color.a};
    activeShader.setVector4f(*WMOGLM, "lineColor", assignedColor);
    activeShader.setFloat(*WMOGLM, "deltaTime", activeShader.renderDetails.deltaTime);
    WMOGLM->drawArrays(GL_LINES, 0, 2);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    activeShader.unbind(*WMOGLM);
}

/**
 * @brief Renders a pixel
 * @param x1 First vector X position
 * @param y1 First vector Y position
 * @param x2 Second vector X position
 * @param y2 Second vector Y position
 * @param color The line's color
 *
 * @return nothing
 */
void SGL_Renderer::renderLine(float x1, float y1, float x2, float y2, SGL_Color color)
{
    // NOTE, coords are normalized, gotta deal with that
    GLfloat vertices[] = {
        x1, y1, x2, y2
    };
    WMOGLM->bindVAO(this->pLineVAO);
    WMOGLM->bindVBO(this->pLineVBO);
    pLineShader.use(*WMOGLM);

    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(vertices), &vertices[0]);
    glm::vec4 assignedColor = {color.r, color.g, color.b, color.a};
    pLineShader.setVector4f(*WMOGLM, "lineColor", assignedColor);
    pLineShader.setFloat(*WMOGLM, "deltaTime", pLineShader.renderDetails.deltaTime);
    WMOGLM->drawArrays(GL_LINES, 0, 2);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    pLineShader.unbind(*WMOGLM);
}

/**
 * @brief Renders a string of text
 * @param line SGL_Text object to render
 *
 * @return nothing
 */
void SGL_Renderer::renderText(SGL_Text &text)
{
    SGL_Shader activeShader;
    if (text.shader.shaderType != SHADER_TYPE::TEXT)
        activeShader = pTextShader;
    else
        activeShader = text.shader;

    WMOGLM->faceCulling(true);
    // RESUME render state
    activeShader.use(*WMOGLM);
    glm::vec4 assignedColor = {text.color.r, text.color.g, text.color.b, text.color.a};
    activeShader.setVector4f(*WMOGLM, "textColor", assignedColor);
    activeShader.setFloat(*WMOGLM, "deltaTime", activeShader.renderDetails.deltaTime);
    WMOGLM->activeTexture(GL_TEXTURE0);
    WMOGLM->bindVAO(pTextVAO);

    // Iterate through all the characters in the map
    std::string::const_iterator c;
    for(c = text.message.begin(); c != text.message.end(); ++c)
    {
        Character ch = characters[*c];

        GLfloat xpos = text.position.x + ch.bearing.x * text.scale;
        // This y pos works when you do NOT flip the y axis in the ortho projection matrix
        // GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;
        // this one does
        GLfloat ypos = text.position.y + (this->characters['H'].bearing.y - ch.bearing.y) * text.scale;

        GLfloat w = ch.size.x * text.scale;
        GLfloat h = ch.size.y * text.scale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 1.0 },
            { xpos + w, ypos,       1.0, 0.0 },
            { xpos,     ypos,       0.0, 0.0 },

            { xpos,     ypos + h,   0.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 1.0 },
            { xpos + w, ypos,       1.0, 0.0 }
        };
        // Render glyph texture over the quad
        WMOGLM->bindTexture(GL_TEXTURE_2D, ch.textureID);
        // Update content of VBO memory
        WMOGLM->bindVBO(pTextVBO);
        WMOGLM->bufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        WMOGLM->unbindVBO();
        // Render the quad
        WMOGLM->drawArrays(GL_TRIANGLES, 0, 6);
        // Advance cursor for the next glyph (note that advance is number of 1/64 pixels)
        text.position.x += (ch.advance >> 6) * text.scale;
    }

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    activeShader.unbind(*WMOGLM);
    WMOGLM->bindTexture(GL_TEXTURE_2D, 0);
    WMOGLM->faceCulling(false);
}

/**
 * @brief Renders a string of text
 * @param text String to render
 * @param x Vvector Y position
 * @param y Vector Y position
 * @param scale Float that sets the size
 * @param color The text's color
 *
 * @return nothing
 */
void SGL_Renderer::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, SGL_Color color)
{
    WMOGLM->faceCulling(true);
    // RESUME render state
    this->pTextShader.use(*WMOGLM);
    this->pTextShader.setVector4f(*WMOGLM, "textColor", color.r, color.g, color.b, color.a);
    pTextShader.setFloat(*WMOGLM, "deltaTime", 0.0f);
    WMOGLM->activeTexture(GL_TEXTURE0);
    WMOGLM->bindVAO(pTextVAO);

    // Iterate through all the characters in the map
    std::string::const_iterator c;
    for(c = text.begin(); c != text.end(); ++c)
    {
        Character ch = characters[*c];

        GLfloat xpos = x + ch.bearing.x * scale;
        // This y pos works when you do NOT flip the y axis in the ortho projection matrix
        // GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;
        // this one does
        GLfloat ypos = y + (this->characters['H'].bearing.y - ch.bearing.y) * scale;

        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 1.0 },
            { xpos + w, ypos,       1.0, 0.0 },
            { xpos,     ypos,       0.0, 0.0 },

            { xpos,     ypos + h,   0.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 1.0 },
            { xpos + w, ypos,       1.0, 0.0 }
        };
        // Render glyph texture over the quad
        WMOGLM->bindTexture(GL_TEXTURE_2D, ch.textureID);
        // Update content of VBO memory
        WMOGLM->bindVBO(pTextVBO);
        WMOGLM->bufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        WMOGLM->unbindVBO();
        // Render the quad
        WMOGLM->drawArrays(GL_TRIANGLES, 0, 6);
        // Advance cursor for the next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale;
    }

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    this->pTextShader.unbind(*WMOGLM);
    WMOGLM->bindTexture(GL_TEXTURE_2D, 0);
    WMOGLM->faceCulling(false);
}

/**
 * @brief Renders a sprite
 * @param sprite SGL_Sprite to be rendered
 *
 * @return nothing
 */
void SGL_Renderer::renderSprite(const SGL_Sprite &sprite)
{
    SGL_Shader activeShader;
    if (sprite.shader.shaderType != SHADER_TYPE::SPRITE)
        activeShader = pSpriteShader;
    else
        activeShader = sprite.shader;

    GLfloat UV[] = {
        sprite.uvCoords.UV_topLeft.x / sprite.texture.width, sprite.uvCoords.UV_topLeft.y / sprite.texture.height,
        sprite.uvCoords.UV_botRight.x / sprite.texture.width, sprite.uvCoords.UV_botRight.y / sprite.texture.height,
        sprite.uvCoords.UV_botLeft.x / sprite.texture.width, sprite.uvCoords.UV_botLeft.y / sprite.texture.height,

        sprite.uvCoords.UV_topLeft.x / sprite.texture.width, sprite.uvCoords.UV_topLeft.y / sprite.texture.height,
        sprite.uvCoords.UV_topRight.x / sprite.texture.width, sprite.uvCoords.UV_topRight.y / sprite.texture.height,
        sprite.uvCoords.UV_botRight.x / sprite.texture.width, sprite.uvCoords.UV_botRight.y / sprite.texture.height,
    };

    WMOGLM->bindVAO(this->pSpriteVAO);
    WMOGLM->bindVBO(this->pTextureUVVBO);
    activeShader.use(*WMOGLM);
    // Replace current texture coordinates
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(UV), &UV[0]);

    // If the user didn't specify a blending mode use the renderers default
    if (sprite.blending == 0)
        WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
    else
        WMOGLM->blending(true, sprite.blending);

    // Prepare transformations
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(sprite.position, 0.0f)); //move
    // Move origin of rotation specified in sprite's data
    model = glm::translate(model, glm::vec3(sprite.rotationOrigin.x, sprite.rotationOrigin.y, 0.0f));
    model = glm::rotate(model, sprite.rotation, glm::vec3(0.0f, 0.0f, 1.0f));//rotate
    model = glm::translate(model, glm::vec3(-sprite.rotationOrigin.x, -sprite.rotationOrigin.y, 0.0f)); //set the rotation origin back to origin
    model = glm::scale(model, glm::vec3(sprite.size, 1.0f)); //scale

    // Parse uniforms
    activeShader.setMatrix4(*WMOGLM, "model", model);
    // Render texture quad
    activeShader.setVector4f(*WMOGLM, "spriteColor", sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a);
    activeShader.setFloat(*WMOGLM, "deltaTime", activeShader.renderDetails.deltaTime);
    activeShader.setVector2f(*WMOGLM, "spriteDimensions", sprite.size.x, sprite.size.y);
    // Activate textures
    WMOGLM->activeTexture(GL_TEXTURE0);
    sprite.texture.bind(*WMOGLM);
    // Draw it
    WMOGLM->drawArrays(GL_TRIANGLES, 0, 6);

    activeShader.unbind(*WMOGLM);
    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    // restore default blending status
    WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
}


/**
 * @brief Renders a sprite batch using instanced rendering
 * @param sprite SGL_Sprite to be rendered
 * @param sprite SGL_Sprite to be rendered
 *
 * @return nothing
 */
void SGL_Renderer::renderSpriteBatch(const SGL_Sprite &sprite)
{
    SGL_Shader activeShader;
    if (sprite.shader.shaderType != SHADER_TYPE::SPRITE)
        activeShader = pSpriteBatchShader;
    else
        activeShader = sprite.shader;

    activeShader = pSpriteBatchShader;

    GLfloat UV[] = {
        sprite.uvCoords.UV_topLeft.x / sprite.texture.width, sprite.uvCoords.UV_topLeft.y / sprite.texture.height,
        sprite.uvCoords.UV_botRight.x / sprite.texture.width, sprite.uvCoords.UV_botRight.y / sprite.texture.height,
        sprite.uvCoords.UV_botLeft.x / sprite.texture.width, sprite.uvCoords.UV_botLeft.y / sprite.texture.height,

        sprite.uvCoords.UV_topLeft.x / sprite.texture.width, sprite.uvCoords.UV_topLeft.y / sprite.texture.height,
        sprite.uvCoords.UV_topRight.x / sprite.texture.width, sprite.uvCoords.UV_topRight.y / sprite.texture.height,
        sprite.uvCoords.UV_botRight.x / sprite.texture.width, sprite.uvCoords.UV_botRight.y / sprite.texture.height,
    };

    WMOGLM->bindVAO(this->pSpriteBatchVAO);
    WMOGLM->bindVBO(this->pTextureUVVBO);
    activeShader.use(*WMOGLM);
    // Replace current texture coordinates
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(UV), &UV[0]);
    // If the user didn't specify a blending mode use the renderers default
    if (sprite.blending == 0)
        WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
    else
        WMOGLM->blending(true, sprite.blending);


    // TESTING --
    for (int i = 0; i < pBatchAmount; ++i)
    {
        // Prepare transformations
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(i*80, i*15, 0.0f)); //move
        // // Move origin of rotation specified in sprite's data
        model = glm::translate(model, glm::vec3(sprite.rotationOrigin.x, sprite.rotationOrigin.y, 0.0f));
        model = glm::rotate(model, sprite.rotation, glm::vec3(0.0f, 0.0f, 1.0f));//rotate
        model = glm::translate(model, glm::vec3(-sprite.rotationOrigin.x, -sprite.rotationOrigin.y, 0.0f)); //set the rotation origin back to origin
        model = glm::scale(model, glm::vec3(sprite.size, 1.0f)); //scale

        // model = glm::translate(model, glm::vec3(i*3, .rotationOrigin.y, 0.0f));
        // model = glm::rotate(model, sprite.rotation, glm::vec3(0.0f, 0.0f, 1.0f));//rotate
        // model = glm::translate(model, glm::vec3(-sprite.rotationOrigin.x, -sprite.rotationOrigin.y, 0.0f)); //set the rotation origin back to origin
        // model = glm::scale(model, glm::vec3(32, 32, 1.0f)); //scale
        modelMatrices[i] = model;

        // SGL_Log("X: " + std::to_string(modelMatrices[i][0][0]));
    }


    // WMOGLM->checkForGLErrors();


    // SGL_Log("Reloading instance buffers.");
    WMOGLM->bindVBO(pSpriteBatchInstancesVBO);
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(glm::mat4) * pBatchAmount, &modelMatrices[0]);
    WMOGLM->unbindVBO();

    WMOGLM->bindVBO(this->pSpriteBatchVBO);
    // SGL_Log("X: " + std::to_string(modelMatrices[0][0][0]));
    // Prepare transformations
    // glm::mat4 model;
    // model = glm::translate(model, glm::vec3(sprite.position, 0.0f)); //move
    // // Move origin of rotation specified in sprite's data
    // model = glm::translate(model, glm::vec3(sprite.rotationOrigin.x, sprite.rotationOrigin.y, 0.0f));
    // model = glm::rotate(model, sprite.rotation, glm::vec3(0.0f, 0.0f, 1.0f));//rotate
    // model = glm::translate(model, glm::vec3(-sprite.rotationOrigin.x, -sprite.rotationOrigin.y, 0.0f)); //set the rotation origin back to origin
    // model = glm::scale(model, glm::vec3(sprite.size, 1.0f)); //scale

    // // Parse uniforms
    // activeShader.setMatrix4(*WMOGLM, "model", model);
    // Render texture quad
    activeShader.setVector4f(*WMOGLM, "spriteColor", sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a);
    activeShader.setFloat(*WMOGLM, "deltaTime", activeShader.renderDetails.deltaTime);
    activeShader.setVector2f(*WMOGLM, "spriteDimensions", sprite.size.x, sprite.size.y);
    // Activate textures
    WMOGLM->activeTexture(GL_TEXTURE0);
    sprite.texture.bind(*WMOGLM);
    // Draw it
    // WMOGLM->drawArrays(GL_TRIANGLES, 0, 6);
    // WMOGLM->checkForGLErrors();

    WMOGLM->drawArraysInstanced(GL_TRIANGLES, 0, 6, pBatchAmount);
    // WMOGLM->drawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, pBatchAmount);
    // WMOGLM->checkForGLErrors();

    activeShader.unbind(*WMOGLM);
    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    // restore default blending status
    WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
}



/**
 * @brief Renders a sprite batch using instanced rendering
 * @param sprite SGL_Sprite to be rendered
 * @param sprite SGL_Sprite to be rendered
 *
 * @return nothing
 */
void SGL_Renderer::renderSpriteBatch(const SGL_Sprite &sprite, const std::vector<glm::vec2> *vectors)
{
    SGL_Shader activeShader;
    if (sprite.shader.shaderType != SHADER_TYPE::SPRITE)
        activeShader = pSpriteBatchShader;
    else
        activeShader = sprite.shader;

    activeShader = pSpriteBatchShader;

    GLfloat UV[] = {
        sprite.uvCoords.UV_topLeft.x / sprite.texture.width, sprite.uvCoords.UV_topLeft.y / sprite.texture.height,
        sprite.uvCoords.UV_botRight.x / sprite.texture.width, sprite.uvCoords.UV_botRight.y / sprite.texture.height,
        sprite.uvCoords.UV_botLeft.x / sprite.texture.width, sprite.uvCoords.UV_botLeft.y / sprite.texture.height,

        sprite.uvCoords.UV_topLeft.x / sprite.texture.width, sprite.uvCoords.UV_topLeft.y / sprite.texture.height,
        sprite.uvCoords.UV_topRight.x / sprite.texture.width, sprite.uvCoords.UV_topRight.y / sprite.texture.height,
        sprite.uvCoords.UV_botRight.x / sprite.texture.width, sprite.uvCoords.UV_botRight.y / sprite.texture.height,
    };

    WMOGLM->bindVAO(this->pSpriteBatchVAO);
    WMOGLM->bindVBO(this->pTextureUVVBO);
    activeShader.use(*WMOGLM);
    // Replace current texture coordinates
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(UV), &UV[0]);
    // If the user didn't specify a blending mode use the renderers default
    if (sprite.blending == 0)
        WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
    else
        WMOGLM->blending(true, sprite.blending);


    // TESTING --
    std::uint16_t index = 0;
    // for (int i = 0; i < vectors.size(); ++i)
    for (std::vector<glm::vec2>::const_iterator i = vectors->begin(); i < vectors->end(); ++i)
    {
        // Prepare transformations
        glm::mat4 model;
        model = glm::translate(model, glm::vec3((*i).x, (*i).y, 0.0f)); //move
        // // Move origin of rotation specified in sprite's data
        model = glm::translate(model, glm::vec3(sprite.rotationOrigin.x, sprite.rotationOrigin.y, 0.0f));
        model = glm::rotate(model, sprite.rotation, glm::vec3(0.0f, 0.0f, 1.0f));//rotate
        model = glm::translate(model, glm::vec3(-sprite.rotationOrigin.x, -sprite.rotationOrigin.y, 0.0f)); //set the rotation origin back to origin
        model = glm::scale(model, glm::vec3(sprite.size, 1.0f)); //scale

        // model = glm::translate(model, glm::vec3(i*3, .rotationOrigin.y, 0.0f));
        // model = glm::rotate(model, sprite.rotation, glm::vec3(0.0f, 0.0f, 1.0f));//rotate
        // model = glm::translate(model, glm::vec3(-sprite.rotationOrigin.x, -sprite.rotationOrigin.y, 0.0f)); //set the rotation origin back to origin
        // model = glm::scale(model, glm::vec3(32, 32, 1.0f)); //scale
        modelMatrices[index] = model;
        index++;
        // SGL_Log("X: " + std::to_string(modelMatrices[i][0][0]));
    }


    // WMOGLM->checkForGLErrors();

    // SGL_Log("Reloading instance buffers.");
    WMOGLM->bindVBO(pSpriteBatchInstancesVBO);
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(glm::mat4) * pBatchAmount, &modelMatrices[0]);
    WMOGLM->unbindVBO();

    WMOGLM->bindVBO(this->pSpriteBatchVBO);

    // Render texture quad
    activeShader.setVector4f(*WMOGLM, "spriteColor", sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a);
    activeShader.setFloat(*WMOGLM, "deltaTime", activeShader.renderDetails.deltaTime);
    activeShader.setVector2f(*WMOGLM, "spriteDimensions", sprite.size.x, sprite.size.y);

    // Activate textures
    WMOGLM->activeTexture(GL_TEXTURE0);
    sprite.texture.bind(*WMOGLM);

    // Render it
    WMOGLM->drawArraysInstanced(GL_TRIANGLES, 0, 6, pBatchAmount);

    activeShader.unbind(*WMOGLM);
    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    // restore default blending status
    WMOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
}




/**
 * @brief Renders a pixel batch using instanced rendering
 *
 * @return nothing
 */
void SGL_Renderer::renderPixelBatch(const SGL_Pixel &pixel)
{
    SGL_Shader activeShader;
    // if (pixel.shader.shaderType != SHADER_TYPE::PIXEL)
    //     activeShader = pPixelBatchShader;
    // else
    //     activeShader = pixel.shader;

    activeShader = pPixelBatchShader;

    // NOTE, coords are normalized, gotta deal with that
    GLfloat vertices[] = {
        pixel.position.x, pixel.position.y
    };
    WMOGLM->bindVAO(this->pPixelBatchVAO);
    WMOGLM->bindVBO(this->pPixelBatchVBO);
    activeShader.use(*WMOGLM);

    // Update UV data
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(vertices), &vertices[0]);

    WMOGLM->checkForGLErrors();

    // Update batch data
    glm::vec2 translations[pPixelBatchAmount];
    int index = 0;
    float offset = 0.1;
    for(int i = 0; i < pPixelBatchAmount; ++i)
    {
        glm::vec2 translation;
        translation.x = i + 32;
        translation.y = i + 35;
        translations[index++] = translation;
    }

    WMOGLM->bindVBO(pPixelBatchInstancesVBO);
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(glm::vec2) * pPixelBatchAmount, &translations[0]);
    WMOGLM->unbindVBO();

    WMOGLM->checkForGLErrors();
    glm::vec4 assignedColor = {pixel.color.r, pixel.color.g, pixel.color.b, pixel.color.a};
    activeShader.setVector4f(*WMOGLM, "pointColor", assignedColor);
    activeShader.setFloat(*WMOGLM, "deltaTime", activeShader.renderDetails.deltaTime);

    // Render instanced data
    // WMOGLM->drawArrays(GL_POINTS, 0, 2);
    WMOGLM->drawArraysInstanced(GL_POINTS, 0, 2, pPixelBatchAmount);

    WMOGLM->checkForGLErrors();

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    activeShader.unbind(*WMOGLM);

}


/**
 * @brief Renders a line batch
 *
 * @return nothing
 */
void SGL_Renderer::renderLineBatch(const SGL_Line &line)
{
    SGL_Shader activeShader;
    // if (pixel.shader.shaderType != SHADER_TYPE::PIXEL)
    //     activeShader = pPixelBatchShader;
    // else
    //     activeShader = line.shader;

    activeShader = pLineBatchShader;

    // NOTE, coords are normalized, gotta deal with that
        GLfloat vertices[] = {
        line.positionA.x, line.positionA.y, line.positionB.x, line.positionB.y
    };
    WMOGLM->bindVAO(this->pLineBatchVAO);
    WMOGLM->bindVBO(this->pLineBatchVBO);
    activeShader.use(*WMOGLM);

    // Update UV data
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(vertices), &vertices[0]);

    // WMOGLM->checkForGLErrors();

    // Update batch data
    glm::vec4 translations[pLineBatchAmount];
    int index = 0;
    for(int i = 0; i < pLineBatchAmount; ++i)
    {
        glm::vec4 translation;
        translation.x = i + 1;
        translation.y = i + 1;
        translation.z = i + 3;
        translation.w = i + 3;
        translations[index++] = translation;
    }
    WMOGLM->bindVBO(pLineBatchInstancesVBO);
    WMOGLM->bufferSubData(GL_ARRAY_BUFFER, NULL, sizeof(glm::vec4) * pLineBatchAmount, &translations[0]);
    WMOGLM->unbindVBO();

    WMOGLM->checkForGLErrors();
    glm::vec4 assignedColor = {line.color.r, line.color.g, line.color.b, line.color.a};
    activeShader.setVector4f(*WMOGLM, "lineColor", assignedColor);
    activeShader.setFloat(*WMOGLM, "deltaTime", activeShader.renderDetails.deltaTime);

    // Render instanced data
    // WMOGLM->drawArrays(GL_POINTS, 0, 2);
    WMOGLM->drawArraysInstanced(GL_LINES, 0, 2, pLineBatchAmount);

    WMOGLM->checkForGLErrors();

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    activeShader.unbind(*WMOGLM);
}


/**
 * @brief Load the line buffers
 * @param shader SGL_Shader to set as the line renderer default
 *
 * @return nothing
 */
void SGL_Renderer::loadLineBuffers(SGL_Shader shader)
{
    shader.use(*WMOGLM);

    GLfloat vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f
    };
    WMOGLM->bindVAO(this->pLineVAO);
    WMOGLM->bindVBO(this->pLineVBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    shader.unbind(*WMOGLM);
}

/**
 * @brief Load the line batcher buffers
 * @param shader SGL_Shader to set as the line BATCH (NOT THE SAME AS THE INDIVIDUAL LINE SHADER) renderer default
 *
 * @return nothing
 */
void SGL_Renderer::loadLineBatchBuffers(SGL_Shader shader)
{
    SGL_Log("Configuring line batch renderer...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    shader.use(*WMOGLM);

    GLfloat vertices[] = {
        0.0f, 0.0f, 0.0f, 0.0f
    };
    WMOGLM->bindVAO(this->pLineBatchVAO);

    WMOGLM->bindVBO(this->pLineBatchVBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid *)0);

    WMOGLM->checkForGLErrors();

    this->pLineBatchAmount = 100;
    glm::vec4 memRes[pLineBatchAmount];

    // Pixel batching
    WMOGLM->bindVBO(pLineBatchInstancesVBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * pLineBatchAmount, &memRes[0], GL_STATIC_DRAW);

    // Line batching
    SGL_Log("Configuring line batch shader attrib pointer 3.");
    WMOGLM->enableVertexAttribArray(3);
    WMOGLM->checkForGLErrors();
    WMOGLM->vertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    WMOGLM->checkForGLErrors();
    WMOGLM->vertexAttribDivisor(3, 1);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    shader.unbind(*WMOGLM);
}

/**
 * @brief Load the pixel buffers
 * @param shader SGL_Shader to set as the pixel renderer default
 *
 * @return nothing
 */
void SGL_Renderer::loadPointBuffers(SGL_Shader shader)
{
    shader.use(*WMOGLM);

    GLfloat vertices[] = {
        0.0f, 0.0f
    };
    WMOGLM->bindVAO(this->pPointVAO);
    WMOGLM->bindVBO(this->pPointVBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    shader.unbind(*WMOGLM);
}

/**
 * @brief Load the pixel batch buffers
 * @param shader SGL_Shader to set as the pixel batch renderer default
 *
 * @return nothing
 */
void SGL_Renderer::loadPixelBatchBuffers(SGL_Shader shader)
{
    shader.use(*WMOGLM);

    GLfloat vertices[] = {
        0.0f, 0.0f
    };

    this->pPixelBatchAmount = 100;
    glm::vec2 translations[pPixelBatchAmount];
    int index = 0;
    float offset = 0.1;
    // for(int y = -10; y < pBatchAmount; y += 2)
    // {
    //     for(int x = -10; x < 10; x += 2)
    //     {
    //         glm::vec2 translation;
    //         translation.x = (float)x / 10.0f + offset;
    //         translation.y = (float)y / 10.0f + offset;
    //         translations[index++] = translation;
    //     }
    // }


    SGL_Log("Generating pixel batcher VBOs and VAOs.");

    WMOGLM->bindVAO(this->pPixelBatchVAO);

    // Pixel batching
    WMOGLM->bindVBO(pPixelBatchInstancesVBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * pPixelBatchAmount, &translations[0], GL_STATIC_DRAW);
    WMOGLM->unbindVBO();

    // Model coords
    WMOGLM->bindVBO(this->pPixelBatchVBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);

    // Pixel batching
    GLsizei vec2Size = sizeof(glm::vec2);
    SGL_Log("Configuring pixel batch shader attrib pointer 3.");
    WMOGLM->bindVBO(pPixelBatchInstancesVBO);
    WMOGLM->enableVertexAttribArray(3);
    WMOGLM->checkForGLErrors();
    WMOGLM->vertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    WMOGLM->checkForGLErrors();
    WMOGLM->vertexAttribDivisor(3, 1);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    shader.unbind(*WMOGLM);
}

/**
 * @brief Load the sprite buffers
 * @param shader SGL_Shader to set as the pixesprite renderer default
 *
 * @return nothing
 */
void SGL_Renderer::loadSpriteBuffers(SGL_Shader shader)
{
    GLfloat vertices[] = {
        // pos   tex
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };
    GLfloat UVCoords[] = {
        0.0f, 1.0f, // top left
        1.0f, 0.0f, // bot right
        0.0f, 0.0f, // bot left

        0.0f, 1.0f, // top left
        1.0f, 1.0f, // top right
        1.0f, 0.0f, // bot right
    };

    WMOGLM->bindVAO(this->pSpriteVAO);
    shader.use(*WMOGLM);
    // Parse buffers to GPU
    WMOGLM->bindVBO(this->pTextureUVVBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(UVCoords), UVCoords, GL_DYNAMIC_DRAW);
    WMOGLM->bindVBO(this->pSpriteVBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // Bind vertex data
    WMOGLM->bindVBO(this->pSpriteVBO);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    WMOGLM->bindVBO(this->pTextureUVVBO);
    WMOGLM->enableVertexAttribArray(1);
    WMOGLM->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    shader.unbind(*WMOGLM);
}

/**
 * @brief Load the sprite batch buffers
 * @param shader SGL_Shader to set as the pixesprite renderer default
 *               MUST COINCIDE WITH THE SAME ATTRIB DIVISOR USED IN SHADER
 * @return nothing
 */
void SGL_Renderer::loadSpriteBatchBuffers(SGL_Shader shader)
{
    GLfloat vertices[] = {
        // pos   tex
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };
    GLfloat UVCoords[] = {
        0.0f, 1.0f, // top left
        1.0f, 0.0f, // bot right
        0.0f, 0.0f, // bot left

        0.0f, 1.0f, // top left
        1.0f, 1.0f, // top right
        1.0f, 0.0f, // bot right
    };


    pBatchAmount = 300;
    modelMatrices = new glm::mat4[pBatchAmount];

    for (int i = 0; i < pBatchAmount; ++i)
    {
        // Prepare transformations
        glm::mat4 model;
        // model = glm::translate(model, glm::vec3(i*10, i*5, 0.0f)); //move
        // // Move origin of rotation specified in sprite's data
        // model = glm::translate(model, glm::vec3(sprite.rotationOrigin.x, sprite.rotationOrigin.y, 0.0f));
        // model = glm::rotate(model, sprite.rotation, glm::vec3(0.0f, 0.0f, 1.0f));//rotate
        // model = glm::translate(model, glm::vec3(-sprite.rotationOrigin.x, -sprite.rotationOrigin.y, 0.0f)); //set the rotation origin back to origin
        // model = glm::scale(model, glm::vec3(sprite.size, 1.0f)); //scale

        // model = glm::translate(model, glm::vec3(i*3, .rotationOrigin.y, 0.0f));
        // model = glm::rotate(model, sprite.rotation, glm::vec3(0.0f, 0.0f, 1.0f));//rotate
        // model = glm::translate(model, glm::vec3(-sprite.rotationOrigin.x, -sprite.rotationOrigin.y, 0.0f)); //set the rotation origin back to origin
        // model = glm::scale(model, glm::vec3(32, 32, 1.0f)); //scale
        modelMatrices[i] = model;

        // SGL_Log("X: " + std::to_string(modelMatrices[i][0][0]));
    }


    WMOGLM->checkForGLErrors();

    WMOGLM->bindVAO(this->pSpriteBatchVAO);
    shader.use(*WMOGLM);



    SGL_Log("Generating instance buffers.");
    WMOGLM->checkForGLErrors();
    WMOGLM->bindVBO(pSpriteBatchInstancesVBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * pBatchAmount, &modelMatrices[0], GL_STATIC_DRAW);
    WMOGLM->unbindVBO();
    // Parse buffers to GPU
    WMOGLM->bindVBO(this->pTextureUVVBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(UVCoords), UVCoords, GL_DYNAMIC_DRAW);
    WMOGLM->bindVBO(this->pSpriteBatchVBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // Bind vertex data
    WMOGLM->bindVBO(this->pSpriteBatchVBO);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    WMOGLM->bindVBO(this->pTextureUVVBO);
    WMOGLM->enableVertexAttribArray(1);
    WMOGLM->vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    // Set instancing data
    GLsizei vec4Size = sizeof(glm::vec4);
    SGL_Log("Configuring shader attrib pointer 3.");
    WMOGLM->bindVBO(pSpriteBatchInstancesVBO);
    WMOGLM->enableVertexAttribArray(3);
    WMOGLM->checkForGLErrors();
    WMOGLM->vertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, vec4Size * 4, (void*)0);
    WMOGLM->checkForGLErrors();

    SGL_Log("Configuring shader attrib pointer 4.");
    WMOGLM->enableVertexAttribArray(4);
    WMOGLM->vertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, vec4Size * 4, (void*)(vec4Size));
    WMOGLM->checkForGLErrors();

    WMOGLM->enableVertexAttribArray(5);
    WMOGLM->vertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, vec4Size * 4, (void*)(2 * vec4Size));
    WMOGLM->checkForGLErrors();

    WMOGLM->enableVertexAttribArray(6);
    WMOGLM->vertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, vec4Size * 4, (void*)(3 * vec4Size));

    WMOGLM->vertexAttribDivisor(3, 1);
    WMOGLM->vertexAttribDivisor(4, 1);
    WMOGLM->vertexAttribDivisor(5, 1);
    WMOGLM->vertexAttribDivisor(6, 1);
    WMOGLM->unbindVBO();

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    shader.unbind(*WMOGLM);
}




/**
 * @brief Generates a character texture based on the font to load
 * @param fontPath String to the font
 *
 * @return nothing
 */
void SGL_Renderer::generateFont(const std::string fontPath)
{
    this->pTextShader.use(*WMOGLM);
    FT_Library ft;
    if(FT_Init_FreeType(&ft))
    {
        SGL_Log("ERROR::FREETYPE: Could not init FreeType Library", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_RED);
        throw SGL_Exception("SGL_Renderer::generateFont | Could not open the FreeType library.");
    }

    FT_Face face;
    if(FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        SGL_Log("ERROR::FREETYPE: Failed to load font" + fontPath, LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_RED);
        throw SGL_Exception("SGL_Renderer::generateFont | Error generating font: ");
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    // Set OpenGL options
    WMOGLM->faceCulling(true);
    WMOGLM->blending(true, BLENDING_TYPE::TEXT_RENDERING);
    WMOGLM->pixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load the first 128 characters of the ASCII set
    for(GLubyte c = 0; c < 128; ++c)
    {
        // Load a glyph
        if(FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            SGL_Log("ERROR::FREETYPE: Failed to load glyph", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_RED);
            continue;
        }
        // Generate texture, due to the nature of the textures, avoid the resource manager
        GLuint texture;
        WMOGLM->genTextures(1, &texture);
        WMOGLM->bindTexture(GL_TEXTURE_2D, texture);
        WMOGLM->texImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,
                          0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        // Texture options
        WMOGLM->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        WMOGLM->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        WMOGLM->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        WMOGLM->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Store it in the map
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        characters.insert(std::pair<GLchar,Character>(c, character));
    }
    WMOGLM->bindTexture(GL_TEXTURE_2D, 0);
    // Destroy ft
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Configure VBO and VAO
    // VBO will be filled with glSubData during render
    WMOGLM->bindVAO(this->pTextVAO);
    WMOGLM->bindVBO(this->pTextVBO);
    WMOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    WMOGLM->enableVertexAttribArray(0);
    WMOGLM->vertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    WMOGLM->unbindVAO();
    WMOGLM->unbindVBO();
    WMOGLM->faceCulling(false);
}
