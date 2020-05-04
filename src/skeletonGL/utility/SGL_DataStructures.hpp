
// ------------------- By: TSURA @ -------------------
// ███╗   ██╗███████╗ ██████╗ ██╗  ██╗███████╗██╗  ██╗
// ████╗  ██║██╔════╝██╔═══██╗██║  ██║██╔════╝╚██╗██╔╝
// ██╔██╗ ██║█████╗  ██║   ██║███████║█████╗   ╚███╔╝
// ██║╚██╗██║██╔══╝  ██║   ██║██╔══██║██╔══╝   ██╔██╗
// ██║ ╚████║███████╗╚██████╔╝██║  ██║███████╗██╔╝ ██╗
// ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
// ---------------------- [.xyz] ---------------------

/**
 * @file    src/skeletonGL/utility/SGL_DataStructures.cpp
 * @author  TSURA @ NEOHEX.XYZ
 * @date    9/4/2018
 * @version 1.0
 *
 * @brief Contains most of the data structures used by SkeletonGL
 *
 * @section DESCRIPTION
 *
 * This file houses almost all the used data structurs, for easy access and clarity since
 * most of them are related
 */

#ifndef SGL_DATA_STRUCTURES
#define SGL_DATA_STRUCTURES

// C++
#include <string>
// SDL2
#include <SDL2/SDL.h>
// GLEW
#include <GL/glew.h>
// GLM
#include "../deps/glm/glm.hpp"
#include "../deps/glm/gtc/matrix_transform.hpp"


// PRE-PROCESSOR COMPILATION OPTIONS
// #define SGL_OUTPUT_OPENGL_DETAILS true ///< Enables (extra) OpenGLManager output. Useful for debugging.

/**
 * @brief Common math constants
 */
namespace SGL
{
  const float PI = 3.14159265;
  const float PIx2 = 6.2831853;
  const float E = 2.7182818284;
}


/**
 * @brief Project's folder structure
 */
namespace FOLDER_STRUCTURE
{
    const std::string rootDir = "./";
    const std::string assetsDir = "/assets/skeletonGL/";

    const std::string audioDir = rootDir + assetsDir + "audio/";
    const std::string shadersDir = rootDir + assetsDir + "shaders/";
    const std::string imagesDir = rootDir + assetsDir + "textures/";
    const std::string fontsDir = rootDir + assetsDir + "fonts/";
    const std::string defaultFont = fontsDir + "defaultFont.ttf";
    const std::string defaultTexture = imagesDir + "defaultTexture.png";
    const std::string defaultDebugLog = rootDir + ".debugLog.txt";
}

/**
 * @brief Details for the renderer's custom UV values
 * @section DESCRIPTION
 *
 * Example format:
 * GLfloat UV[] = {
 *     0.0f, 1.0f, //top left
 *     1.0f, 0.0f, //bot right
 *     0.0f, 0.0f, //bot left

 *     0.0f, 1.0f, //top left
 *     1.0f, 1.0f, //top right
 *     1.0f, 0.0f, //bot right
 * };
 */
struct UV_Wrapper
{
    glm::vec2 UV_topLeft, UV_botRight, UV_botLeft, UV_topRight;
    UV_Wrapper() : UV_topRight(0,0), UV_botLeft(0,0), UV_botRight(0,0), UV_topLeft(0,0) {}
};

/**
 * @brief Houses all the initialization options for window creation
 * @section DESCRIPTION
 *
 * Dimension values will be used to calculate orthographic projection metrices
 * Note that .ini values will override the ones supplied by the programmer during window creation
 */
struct WindowCreationSpecs
{
    bool fullScreen, enableVSYNC;               ///< Enable fullscreen, VSYNC at startup
    bool activeVSYNC;                           ///< Actual VSYNC status
    bool cursorVisibility;                      ///< Toggles the system cursor on and off
    int displayID;                              ///< Helps when debugging on multiple displays
    std::string windowTitle;                    ///< The title of the window
    int windowPosX;                             ///< Initial window position for non fullscreen applications
    int windowPosY;                             ///< Initial window position for non fullscreen applications
    int minW, minH;                             ///< Smallest supported window size
    int maxW, maxH;                             ///< Maximum supported window size
    int currentX, currentY, currentW, currentH; ///< Window dimensions
    int internalW, internalH;                   ///< Internal orthographic projection dimensions
    int fullscreenW, fullscreenH;               ///< Default full screen resolution
    int gamepad1;                               ///< Available gamepad slots
    std::string iniFile;                        ///< Path to the config ini file (might want to rename the file like the executable)
    WindowCreationSpecs() : currentX(0), currentY(0), currentW(0), currentH(0), internalW(0), internalH(0),
                            windowPosX(SDL_WINDOWPOS_UNDEFINED), windowPosY(SDL_WINDOWPOS_UNDEFINED),
                            enableVSYNC(true), fullScreen(false), minW(640), minH(360), maxW(1920),
                            maxH(1080), fullscreenW(1920), fullscreenH(1080), windowTitle(""),
                            cursorVisibility(true), activeVSYNC(false), displayID(-1), iniFile("config.ini") {}
};

/**
 * @brief Represents a single character to be processed by the TTF renderer
 */
struct Character
{
    GLuint textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    GLuint advance;
};

/**
 * @brief Normalized color used by SkeletonGL
 */
struct SGL_Color
{
    GLfloat r, g, b, a; ///< min 0.0 to max 1.0
    SGL_Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    SGL_Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
    // Helper function to change color, may be used later for further range validation
    void setColor(float r, float g, float b, float a)
        {
            this->r = r; this->b = b;
            this->g = g; this->a = a;
        }
};

/**
 * @brief Represents the entire input state (and some helpful events) of a single frame
 */
struct SGL_InputFrame
{
    GLboolean up, down, left, right;                                                   ///< Arrow keys
    GLboolean q,w,e,r,t,y,u,i,o,p,a,s,d,f,g,h,j,k,l,z,x,c,v,b,n,m;                     ///< Letter keys
    GLboolean num1, num2, num3, num4, num5, num6, num7, num8, num9;                    ///< Upper numbers
    GLboolean mouseRightPressed, mouseRightReleased, mouseLeftPressed, mouseLeftReleased;   ///< Mouse buttons
    GLint rawMousePosX, rawMousePosY, normalizedMousePosX, normalizedMousePosY;        ///< Mouse position + normalized to internal resolution
    GLboolean sdlInternalQuit;                                                         ///< Internal SDL exit event
    GLboolean esc, space, enter, backspace, ctrl, shift, alt;                          ///< Modifiers
    GLboolean mouseFocus, keyboardFocus, windowFocus, windowMinimized, windowRestored; ///< Window state
    SGL_InputFrame() : q(false), w(false), e(false), r(false), t(false), y(false), u(false), i(false), o(false), p(false), a(false), s(false), d(false), f(false), g(false), h(false), j(false), k(false), l(false), z(false), x(false), c(false), v(false), b(false), n(false), m(false), num1(false), num2(false), num3(false), num4(false), num5(false), num6(false), num7(false), num8(false), num9(false), up(false), down(false), left(false), right(false), esc(false), space(false), enter(false), backspace(false), ctrl(false), shift(false),  alt(false), sdlInternalQuit(false), mouseLeftPressed(false), mouseLeftReleased(false), mouseRightPressed(false), mouseRightReleased(false), mouseFocus(false),  keyboardFocus(false), windowFocus(false), windowMinimized(false), windowRestored(false) {}
};

/**
 * @brief Represents a single value and key from the .ini file
 */
struct SGL_IniField
{
    std::string title, key, value;
};

/**
 * @brief Contains an individual frame's time data
 */

struct SGL_FrameData
{
    double time;
    std::uint16_t ticks;
    std::uint16_t updateCycles;

    SGL_FrameData() : time(0.0f), ticks(0), updateCycles(0) {}
    void reset()
        {
            time = 0.0f;
            ticks = 0;
        }
};


/**
 * @brief Represents an OpenGL buffer
 */
struct GLBuffer
{
    std::string name;
    GLuint ID;
    GLBuffer() : name(""), ID(0) {}
};

/**
 * @brief Represents an OpenGL setting
 */
struct GLSetting
{
    std::string name;
    GLenum value;
    bool active;
    GLSetting() : name(""), value(0), active(false) {}
};

/**
 * @brief Collection of the currently active buffers and other settings
 */
struct CurrentBuffers
{
    GLBuffer currentVBO, currentEBO, currentVAO, currentFBO_R, currentFBO_W;
};


/**
 * @brief Collection of the currently active buffers and other settings
 */
struct CurrentSettings
{
    GLSetting depth, faceCulling, blending, depthMask;
};


/**
 * @brief Helps with priority sorting
 */
enum LOG_LEVEL
{
    NO_LOG_LEVEL,
    DEBUG,    // Non critical output, for non SGL debugging
    ERROR,    // Outputs errors only, for production builds
    SGL_DEBUG // This one is exclusvely used by the internal SGL code!
};


/**
 * @brief Terminal colors, requires a compatible terminal! (URXVT works fine)
 */
enum LOG_COLOR
{
    TERM_DEFAULT,
    TERM_RED,
    TERM_GREEN,
    TERM_BLUE,
    TERM_BLACK,
    TERM_YELLOW,
    TERM_MAGENTA,
    TERM_CYAN,
    TERM_WHITE
};


/**
 * @brief What blending mode should the render use?
 */
enum BLENDING_TYPE
{
    NONE,
    DEFAULT_RENDERING,
    SPRITE_RENDERING,
    TEXT_RENDERING,
    PARTICLE_RENDERING,
    // All these ones are up for testing
    LIGHT_RENDERING,
    TEST_RENDERING,
    LIGHT_SRC,
    DARKNESS,
};


/**
 * @brief Switch between the two avilable camera modes
 */
enum CAMERA_MODE
{
    DEFAULT, // Apply camera offset to all shaders
    OVERLAY // Render everything relative to the screen (UI mode)
};


/**
 * @brief Used by the post-processor's framebuffer
 */
namespace GLCONSTANTS
{
    enum FBO
    {
        R,
        W,
        RW
    };
}


#endif
