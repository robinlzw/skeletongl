
// ------------------- By: TSURA @ -------------------
// ███╗   ██╗███████╗ ██████╗ ██╗  ██╗███████╗██╗  ██╗
// ████╗  ██║██╔════╝██╔═══██╗██║  ██║██╔════╝╚██╗██╔╝
// ██╔██╗ ██║█████╗  ██║   ██║███████║█████╗   ╚███╔╝
// ██║╚██╗██║██╔══╝  ██║   ██║██╔══██║██╔══╝   ██╔██╗
// ██║ ╚████║███████╗╚██████╔╝██║  ██║███████╗██╔╝ ██╗
// ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
// ---------------------- [.xyz] ---------------------


#include "particleGenerator.hpp"


ParticleGenerator::ParticleGenerator(std::string className, ShaderManager shader, Texture2D texture, GLuint amount) :
    pShader(shader), pTexture(texture), pAmount(amount), pClassName(className), pDefaultParticleSize(32.0f)
{
    pOGLM = new OGLManager();
    this->pVAO = this->pClassName + "_default_particleGenerator_pVAO";
    this->pVBO = this->pClassName + "_default_particleGenerator_pVBO";
    SGL_Log(pVBO);
    this->loadBuffers();
    this->pEnableDisperse = true;
    this->pEnableRotation = true;
}

ParticleGenerator::~ParticleGenerator()
{
    pOGLM->deleteVAO(this->pVAO);
    pOGLM->deleteVBO(this->pVBO);
    this->pShader.unbind(*pOGLM);
    delete pOGLM;
}

void ParticleGenerator::loadBuffers()
{
    GLfloat particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    pOGLM->createVAO(this->pVAO);
    pOGLM->createVBO(this->pVBO);
    pOGLM->bindVAO(this->pVAO);
    pOGLM->bindVBO(this->pVBO);
    pOGLM->bufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);

    this->pShader.use(*pOGLM);
    pOGLM->enableVertexAttribArray(0);
    pOGLM->vertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

    //create set amount of paticle instances
    for (GLuint i = 0; i < this->pAmount; ++i)
    {
        this->pParticles.push_back(Particle());
    }

    pOGLM->unbindVAO();
    pOGLM->unbindVBO();
    this->pShader.unbind(*pOGLM);
}

void ParticleGenerator::enableDispersion()
{
    this->pEnableDisperse = true;
}

void ParticleGenerator::disableDispersion()
{
    this->pEnableDisperse = false;
}

//this function works independently of the render() one, it renders through raw
//opengl instead of the spriteRenderer, it uses a special blend mode that make particles
//look shiny
void ParticleGenerator::draw()
{
    //SGL_Log("DRAW: " + pClassName);
    pOGLM->blending(true, BLENDING_TYPE::PARTICLE_RENDERING);
    this->pShader.use(*pOGLM);
    for (Particle particle : this->pParticles)
    {
        if (particle.life > 0.0f)
        {
            this->pShader.setVector2f(*pOGLM, "offset", particle.position);
            this->pShader.setVector4f(*pOGLM, "color", particle.color);
            this->pShader.setFloat(*pOGLM, "size", particle.size);
            this->pShader.use(*pOGLM);
            pOGLM->activeTexture(GL_TEXTURE0);
            pTexture.bind(*pOGLM);
            pOGLM->bindVAO(this->pVAO);
            pOGLM->drawArrays(GL_TRIANGLES, 0, 6);
            pOGLM->unbindVAO();
        }
    }
    this->pShader.unbind(*pOGLM);
    pOGLM->blending(true, BLENDING_TYPE::DEFAULT_RENDERING);
}

//renders the particles using a sprite and the sprite renderer, keeps all the textures native
//qualities
void ParticleGenerator::render(SpriteRenderer &renderer, PrimitiveRenderer &primitiveRenderer)
{
    Sprite particleSprite;
    particleSprite.texture = pTexture;
    for (Particle particle : this->pParticles)
    {
        if (particle.life > 0.0f)
        {
            particleSprite.color = SGL_Color(particle.color.x, particle.color.y, particle.color.z, particle.color.w);
            particleSprite.position = particle.position;
            particleSprite.size = glm::vec2(particle.size, particle.size);

            renderer.render(&particleSprite);
        }
    }

}

//index storage for the last unused particles

GLuint ParticleGenerator::firstUnusedParticle()
{
    //search from last used particle
    for (GLuint i = pLastUsedParticle; i < this->pAmount; ++i)
    {
        if (this->pParticles[i].life <= 0.0f)
        {
            pLastUsedParticle = i;
            return i;
        }
    }
    //if above fails, do a linear search
    for (GLuint i = 0; i < pLastUsedParticle; ++i)
    {
        if (this->pParticles[i].life <= 0.0f)
        {
            pLastUsedParticle = i;
            return i;
        }
    }
    //all particles are taken, overide the first one
    pLastUsedParticle = 0;
    return 0;
}

//size 0 particles will default to size 32 in the respawn function
void ParticleGenerator::update(GLfloat dt, glm::vec2 position, glm::vec2 velocity, GLuint newParticles,
                               float size, glm::vec2 offset, int decay)
{
    velocity *= 10.0f;
    //SGL_Log("UPDATE: " + pClassName);
    //add new particles
    for(GLuint i = 0; i < newParticles; ++i)
    {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->pParticles[unusedParticle], position, velocity, size, offset);
    }
    if (decay == 0)
        decay = 10;
    //update all particles
    for(GLuint i = 0; i < this->pAmount; ++i)
    {
        Particle &p = this->pParticles[i];
        p.life -= dt * decay;
        if (p.life > 0.0f)
        {
            if (pEnableDisperse)
            {
                p.position.x += ((p.velocity.x * p.disperseAngle.x) * dt);
                p.position.y += ((p.velocity.y * p.disperseAngle.y) * dt);
                p.color.w -= dt * decay;
            }
            else
            {
                p.position += p.velocity * dt;
                p.color.w -= dt * decay;
            }
        }
    }
}

void ParticleGenerator::respawnParticle(Particle &particle, glm::vec2 position, glm::vec2 velocity, float size, glm::vec2 offset)
{
    //srand(time(NULL));
    GLfloat random = ((rand() % 100) - 50) / 10.0f;
    GLfloat rColor = 0.5 + (rand() % 100) / 100.0f;

    particle.position = position + random + offset;
    //particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    particle.life = 1.0f;
    particle.velocity = velocity;
    if (size == 0.0f)
        particle.size = pDefaultParticleSize;
    else
        particle.size = size;

    if (pEnableDisperse)
    {
        int randomDegreeX = (rand() % 360);
        int randomDegreeY = (rand() % 360);
        float cosX = cos(randomDegreeX * 3.14159265 / 180.0);
        float sinY = sin(randomDegreeY * 3.14159265 / 180.0);

        particle.disperseAngle = glm::vec2(cosX, sinY);
    }
    else
        particle.disperseAngle = glm::vec2(0, 0);

    if (pEnableRotation)
    {
        particle.rotation = (rand() % 360);
    }
}

float ParticleGenerator::getTotalMemory() const
{
    return sizeof(std::vector<Particle>) + sizeof(Particle) * pParticles.size();
}

