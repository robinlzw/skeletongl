
// ------------------- By: TSURA @ -------------------
// ███╗   ██╗███████╗ ██████╗ ██╗  ██╗███████╗██╗  ██╗
// ████╗  ██║██╔════╝██╔═══██╗██║  ██║██╔════╝╚██╗██╔╝
// ██╔██╗ ██║█████╗  ██║   ██║███████║█████╗   ╚███╔╝
// ██║╚██╗██║██╔══╝  ██║   ██║██╔══██║██╔══╝   ██╔██╗
// ██║ ╚████║███████╗╚██████╔╝██║  ██║███████╗██╔╝ ██╗
// ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
// ---------------------- [.xyz] ---------------------

#ifndef PARTICLEG_H
#define PARTICLEG_H

#include <GL/glew.h>
#include "../deps/glm/glm.hpp"
#include "../deps/glm/gtc/matrix_transform.hpp"
#include <vector>
#include <string>

#include "../debugging/consoleLog.hpp"
#include "texture2D.hpp"
#include "shaderManager.hpp"
#include "oglManager.hpp"
#include "../data/SGL_DataStructures.hpp"
#include "spriteRenderer.hpp"
#include "primitiveRenderer.hpp"

//single particle

struct Particle
{
    glm::vec2 position, velocity, disperseAngle;
    glm::vec4 color;
    float rotation, size;
    GLfloat life;

    Particle() : position(0.0f), size(0.0f), velocity(0.0f), color(1.0f), life(0.0f), disperseAngle(0.0f), rotation(0.0f) {}
};

class ParticleGenerator
{
private:
    //opengl function access
    OGLManager *pOGLM;

    std::string pClassName;
    std::vector<Particle> pParticles;
    GLuint pAmount;
    ShaderManager pShader;
    Texture2D pTexture;
    std::string pVAO, pVBO;
    //enable 360 degree particle disperse
    GLboolean pEnableDisperse;
    GLboolean pEnableRotation;
    GLuint pLastUsedParticle = 0;
    GLfloat pDefaultParticleSize;

    void loadBuffers();
    GLuint firstUnusedParticle();
    void respawnParticle(Particle &particle, glm::vec2 position, glm::vec2 velocity, float size, glm::vec2 offset = glm::vec2(0.0f, 0.0f));

    //disable all copying and moving to protect BO
    ParticleGenerator(const ParticleGenerator&) = delete;
    ParticleGenerator *operator = (const ParticleGenerator&) = delete;
    ParticleGenerator(ParticleGenerator &&) = delete;
    ParticleGenerator &operator = (ParticleGenerator &&) = delete;
public:
    ParticleGenerator(std::string className, ShaderManager shader, Texture2D texture, GLuint amount);
    ~ParticleGenerator();

    void update(GLfloat dt, glm::vec2 position, glm::vec2 velocity, GLuint newParticles, float size,
                glm::vec2 offset = glm::vec2(0.0f, 0.0f), int decay = 0);
    void draw();
    void render(SpriteRenderer &renderer, PrimitiveRenderer &primitiveRenderer);
    float getTotalMemory() const;
    void enableDispersion();
    void disableDispersion();
};

#endif
