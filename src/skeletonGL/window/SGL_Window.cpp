
// ------------------- By: TSURA @ -------------------
// ███╗   ██╗███████╗ ██████╗ ██╗  ██╗███████╗██╗  ██╗
// ████╗  ██║██╔════╝██╔═══██╗██║  ██║██╔════╝╚██╗██╔╝
// ██╔██╗ ██║█████╗  ██║   ██║███████║█████╗   ╚███╔╝
// ██║╚██╗██║██╔══╝  ██║   ██║██╔══██║██╔══╝   ██╔██╗
// ██║ ╚████║███████╗╚██████╔╝██║  ██║███████╗██╔╝ ██╗
// ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
// ---------------------- [.xyz] ---------------------


/**
 * @file    src/skeletonGL/window/SGL_Window.cpp
 * @author  AlexHG
 * @date    9/4/2018
 * @version 1.0
 *
 * @brief Main window manager, acts as the SGL framework's interface
 *
 * @section DESCRIPTION
 *
 * SGL_Window wraps around most of the library's functionality, it provides the programmer
 * with the user's input, sets up the main window, controls the simulation's time, initializes and
 * configures the rendering engine, sets up renders. etc. This is the primary access to SkeletonGL.
 *
 */

#include "SGL_Window.hpp"


/**
 * @brief Main and only constructor
 *
 * @param
 * @return nothing
 */
SGL_Window::SGL_Window()
{
    pAlreadyInitialized = false;
}

/**
 * @brief Destructor, must orderly free resources
 * @return nothing
 */
SGL_Window::~SGL_Window()
{
    SGL_Log("Exiting SkeletonGL.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);
    SGL_Log("( ˘ ³˘)♥ Cheers cunt.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);
    this->destroyEverything();
    // Destroy SDL
    SDL_GL_DeleteContext(pGLContext);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
}


/**
 * @brief Destroys local resources
 * @return nothing
 */
void SGL_Window::destroyEverything()
{
    // If the gamepad is set, destroy it
    if (pGamepad_1 != NULL)
        SDL_JoystickClose(pGamepad_1);

    delete pIniParser;
    delete pCamera;
    delete pPostProcessorFBO;
}

/**
 * @brief Configure SkeletonGL, initializes all internal objects and subsystems
 *
 * @param x Window x position
 * @param y Window y position
 * @param w Window width (size, not internal resolution)
 * @param h Window height (size, not internal resolution)
 * @param internalW Internal rendering resolution width
 * @param internalH Internal rendering resolution height
 * @param windowTitle Limited to 50 characters, will truncate otherwise
 * @param fullscreen Should the window ne launched in fullscreen mode?
 * @param FPS Max FPS limit
 * @param iniFile .ini file to process, WILL OVERRIDE THESE VALUES
 * @return nothing
 */
void SGL_Window::initializeWindow(int x, int y, int w, int h, int internalW, int internalH,
                                  const std::string &windowTitle, bool fullscreen, std::string iniFile)
{
    pWindowCreationSpecs.currentX = x;
    pWindowCreationSpecs.currentY = y;
    pWindowCreationSpecs.currentW = w;
    pWindowCreationSpecs.currentH = h;
    pWindowCreationSpecs.internalW = internalW;
    pWindowCreationSpecs.internalH = internalH;
    pWindowCreationSpecs.windowTitle = windowTitle;
    pWindowCreationSpecs.fullScreen = fullscreen;
    pWindowCreationSpecs.enableVSYNC = true;
    // Process the ini file
    if (iniFile != "")
    {
        SGL_Log("Parsing configuration file: " + iniFile);
        pWindowCreationSpecs.iniFile = iniFile;
        processIniFile(iniFile);
    }
    this->start();
}

/**
 * @brief Parses and processes the .ini file
 *
 * @param path std::string, absolute path to the .ini file
 * @return nothing
 * @section DESCRIPTION
 *
 * Custom ini settings should be added here, the following are reserved for obvious internal use.
 * resolution, vsync, fullscreen, screen_id, gamepad_id
 */
void SGL_Window::processIniFile(std::string path)
{
    // --- INI LOADING ---
    // Load the ini configuration file
    pIniParser = new SGL_IniParser(pWindowCreationSpecs.iniFile);

    // [video]
    // resolution
    std::string resolutionIni = pIniParser->getRawValue("[VIDEO]", "resolution");
    SGL_Log("INI FILE resolution = " + resolutionIni);
    // Resoultion format must be WIDTHxHEIGHT (ex. 1920x1080)
    std::size_t posX = resolutionIni.find("x");
    if (resolutionIni != "" && posX != std::string::npos)
    {
        try
        {
            pWindowCreationSpecs.currentW = std::stoi(resolutionIni.substr(0, posX));
            pWindowCreationSpecs.currentH = std::stoi(resolutionIni.substr(posX + 1));
            //pWindowCreationSpecs.currentH = std::stoi(std::string(resolutionIni.substr(resolutionIni.find_last_of("x") + 1)));
        }
        catch (std::out_of_range &e)
        {
            SGL_Log(e.what());
        }
        SGL_Log("INI FILE resolution W = " + std::to_string(pWindowCreationSpecs.currentW));
        SGL_Log("INI FILE resolution H = " + std::to_string(pWindowCreationSpecs.currentH));
    }

    // Vsync
    std::string vsyncIni = pIniParser->getRawValue("[VIDEO]", "vsync");
    SGL_Log("INI FILE vsync = " + vsyncIni);
    if (!vsyncIni.empty())
    {
        if (vsyncIni == "0")
            pWindowCreationSpecs.enableVSYNC = false;
        else
            pWindowCreationSpecs.enableVSYNC = true;
    }
    // Fullscreen
    std::string fullscreenIni = pIniParser->getRawValue("[VIDEO]", "fullscreen");
    SGL_Log("INI FILE fullscreen = " + fullscreenIni);
    if (fullscreenIni == "0")
        pWindowCreationSpecs.fullScreen = false;
    else
        pWindowCreationSpecs.fullScreen = true;
    // Display ID
    int display_idIni = pIniParser->getIntValue("[VIDEO]", "display_id");
    SGL_Log("INI FILE display_id = " + std::to_string(display_idIni));
    pWindowCreationSpecs.displayID = display_idIni;

    // [INPUT]
    // Gamepad ID shouldn't be higher than 4
    int main_gamepad_id = pIniParser->getIntValue("[INPUT]", "main_gamepad_id");
    if (main_gamepad_id > 4)
    {
        SGL_Log("INI FILE main_gamepad_id: " + std::to_string(main_gamepad_id) + " INVALID ID. Setting to 0.");
        pWindowCreationSpecs.gamepad1 = 0;
    }
    else
    {
        SGL_Log("INI FILE main_gamepad_id = " + std::to_string(main_gamepad_id));
        pWindowCreationSpecs.gamepad1 = main_gamepad_id;
    }
}



/**
 * @brief Turns VSYNC on and off
 * @return nothing
 * @section DESCRIPTION
 *
 *
*/

void SGL_Window::toggleVSYNC(bool enable)
{
    // VSYNC support
    if (!enable)
    {
        if (SDL_GL_SetSwapInterval(0) < 0)
            throw SGL_Exception("int SGL_Window::toggleVSYNC | Error disabling VSync \n ");

        pWindowCreationSpecs.activeVSYNC = false;
        SGL_Log("VSync disabled.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
    else
    {
        if (SDL_GL_SetSwapInterval(1) < 0)
            throw SGL_Exception("int SGL_Window::start | Error enabling VSync \n ");

        pWindowCreationSpecs.activeVSYNC = true;
        SGL_Log("VSync enabled.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
}

/**
 * @brief Initializes dependencies, creates window and configures the renderer and asset manager modules
 * @return nothing
 * @section DESCRIPTION
 *
 * This functions loads their SkeletonGL library, it reads the .ini file (which will overwrite any settings
 * provided by initializeWindow), starts all dependencies, configures the orthographic matrix, loads and
 * generates all necessary assets, configures the renderer and postprocessor and creates a window.
*/
void SGL_Window::start()
{
    if (pAlreadyInitialized)
        return;

    SGL_Log("(•̀ᴗ•́)و< \Initializing SkeletonGL ver. " + std::to_string(pSGLVERSION), LOG_LEVEL::SGL_DEBUG,
             LOG_COLOR::TERM_GREEN);
    SGL_Log("      <--o-- By AlexHG --o-->", LOG_LEVEL::SGL_DEBUG,
        LOG_COLOR::TERM_GREEN);

    pHasMouseFocus = false;
    pHasKeyboardFocus = false;
    pIsMinimized = false;

    // --- SDL2 & WINDOW CREATION ---
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
    // SGL_Log("Initializing SDL2....", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER |
                SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER |
                SDL_INIT_HAPTIC | SDL_INIT_NOPARACHUTE) < 0)
    {
        SGL_Log(SDL_GetError(), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        throw SGL_Exception("int SGL_Window::start | Error starting SDL2 \n ");
    }
    // Testing other SDL2 modules, not needed atm
    // if(TTF_Init() < 0)
    // {
    //     SGL_Log(TTF_GetError());
    //     throw SGL_Exception("EXCEPTION IN: int SGL_Window::start | Error starting SDL_TTF \n ");
    // }
    // if(IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG) < 0)
    // {
    //     SGL_Log(IMG_GetError());
    //     throw SGL_Exception("EXCEPTION IN: int SGL_Window::start | Error starting SDL_Image \n ");
    // }
    // if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0)
    // {
    //     SGL_Log(Mix_GetError());
    //     throw SGL_Exception("EXCEPTION IN: int SGL_Window::start | Error starting SDL_Mixer \n ");
    // }

    // Create the window
    this->pWindow = SDL_CreateWindow(pWindowCreationSpecs.windowTitle.c_str(),
                                     pWindowCreationSpecs.windowPosX,
                                     pWindowCreationSpecs.windowPosY,
                                     pWindowCreationSpecs.currentW,
                                     pWindowCreationSpecs.currentH,
                                     SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL |
                                     SDL_INIT_JOYSTICK);

    if ( pWindowCreationSpecs.fullScreen != 0 )
        this->toggleFullScreen(true);
    if (pWindow == NULL)
    {
        SGL_Log("ERROR initializing an SDL2 window: " +  std::string(SDL_GetError()), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        throw SGL_Exception("int SGL_Window::start | Error creating window \n ");
    }
    SGL_Log("SDL2 initialized.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    // --- OPENGL & GLEW  ---
    // SGL_Log("Initializing OpenGL...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // hardware accel: set to 1 to require hardware acceleration set
    // to 0 to force software rendering; defaults to allow either
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    this->pGLContext = SDL_GL_CreateContext(pWindow);
    if(this->pGLContext == NULL)
    {
        SGL_Log("ERROR initializing OpenGL context.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        throw SGL_Exception("int SGL_Window::start | Error creating opengl context \n ");
    }
    const unsigned char *version = glGetString(GL_VERSION);
    if(version == NULL)
    {
        SGL_Log("ERROR retrieving opengl version", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        throw SGL_Exception("int SGL_Window::start | Error retrieving opengl version \n ");
    }
    // Must create a context and make it current before calling glewinit
    glewExperimental = GL_TRUE;
    GLenum glew_status = glewInit();
    if(glew_status != GLEW_OK)
    {
        std::stringstream ss;
        const unsigned char *errStr = glewGetErrorString(glew_status);
        ss << errStr;
        SGL_Log("GLEW ERROR: " + ss.str(), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        throw SGL_Exception("int SGL_Window::start | Error starting GLEW \n ");
    }
    // Make the created context current
    SDL_GL_MakeCurrent(pWindow, pGLContext);
    SDL_SetWindowMinimumSize(pWindow, pWindowCreationSpecs.minW, pWindowCreationSpecs.minH);
    SDL_SetWindowMaximumSize(pWindow, pWindowCreationSpecs.maxW, pWindowCreationSpecs.maxH);
    SGL_Log("Window created. W: " + std::to_string(pWindowCreationSpecs.currentW) + " H: " + std::to_string(pWindowCreationSpecs.currentH), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    // VSYNC support
    toggleVSYNC(pWindowCreationSpecs.enableVSYNC);

    pOGLM = std::make_shared<SGL_OpenGLManager>();

    // Invalid enumerant error as there is yet details to configure, its normal
    pOGLM->checkForGLErrors();
    SGL_Log("OpenGL initialized.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    // --- HARDWARE PROBE ---
    // Load host system info
    this->hostData = std::make_unique<SGL_DeviceInfo>();
    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

#ifdef __linux__
    hostData->hostOS = "Linux";
#endif

#ifdef __WIN32__
    hostData->hostOS = "Windows";
#endif

#ifdef __APPLE__
    hostData->hostOS = "Apple";
#endif

#ifdef __FreeBSD__
    hostData->hostOS = "FreeBSD";
#endif

    hostData->majorSDLVer = static_cast<int>(compiled.major);
    hostData->minorSDLVer = static_cast<int>(compiled.minor);
    hostData->majorSDLVer = static_cast<int>(compiled.patch);
    hostData->linkedMajorSDLVer = static_cast<int>(linked.major);
    hostData->linkedMinorSDLVer = static_cast<int>(linked.minor);
    hostData->linkedPatchSDLVer = static_cast<int>(linked.patch);
    hostData->glewVer = glewGetString(GLEW_VERSION);
    hostData->glVer = glGetString(GL_VERSION);
    hostData->glVendor = glGetString(GL_VENDOR);
    hostData->glRenderer = glGetString(GL_RENDERER);
    hostData->glslVer = glGetString(GL_SHADING_LANGUAGE_VERSION);
    hostData->glMaxVer = glGetString(GL_VERSION);

    hostData->printDeviceInfo();

    SGL_Log("<--- Display data --->", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);
    // Display mode data
    int currentDisplay = pWindowCreationSpecs.displayID, displayModeCount;
    u_int32_t format;
    SDL_DisplayMode dMode;
    SGL_Log("Display(s) available: " + std::to_string(SDL_GetNumVideoDisplays()), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    displayModeCount = SDL_GetNumDisplayModes(currentDisplay);
    if (displayModeCount < 1)
        throw SGL_Exception("int SGL_Window::start | Failed to retrieve display mode data. \n ");

    SGL_Log("Total display modes: " + std::to_string(displayModeCount), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    for (int i = 0; i < displayModeCount; ++i)
    {
        if (SDL_GetDisplayMode(currentDisplay, i,  &dMode) != 0)
            throw SGL_Exception("int SGL_Window::start | Failed to get display. \n ");
        format = dMode.format;
        SGL_Log("Display mode: " + std::to_string(i) + "bpp: " + std::to_string(SDL_BITSPERPIXEL(format)) + " format name: " + SDL_GetPixelFormatName(format) +
                    " mode width: " + std::to_string(dMode.w) + " mode height: " +
                    std::to_string(dMode.h), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
    // Current video display
    for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i)
    {
        if (SDL_GetCurrentDisplayMode(i, &dMode) != 0)
            throw SGL_Exception("int SGL_Window::start | Failed to get video display mode. \n ");
        else
        {
            pWindowCreationSpecs.maxW = dMode.w;
            pWindowCreationSpecs.maxH = dMode.h;
            if (i == currentDisplay)
            {
                pWindowCreationSpecs.fullscreenW = dMode.w;
                pWindowCreationSpecs.fullscreenH = dMode.h;
            }
            SGL_Log("Display #" + std::to_string(i) +
                        " current display mode is " + std::to_string(dMode.w) +
                        " x " + std::to_string(dMode.h) +
                        "@ " + std::to_string(dMode.refresh_rate) + "hz", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        }
    }
    SGL_Log("Using display #" + std::to_string(currentDisplay), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    pWindowCreationSpecs.displayID = currentDisplay;

    // --- DEFAULT ASSET LOADING ---
    this->checkGamepads(pWindowCreationSpecs.gamepad1);
    this->pLoadDefaultAssets();

    // --- RENDERER & CAMERA CONFIGURATION ----
    SGL_Log("<--- Engine configuration --->", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);
    // Initialize the FBO
    // NOTE: if this function is skipped, it may still work since the resizeGL function reloads the FBO
    // however this only works under certain compilers that correctly load the FBO pointer, if run in gdb
    // it will always crash
    this->startFBO(assetManager->getShader("postProcessor"));
    // Set internal opengl viewport size and reconfigure the framebuffer shader
    this->resizeGL(pWindowCreationSpecs.currentW, pWindowCreationSpecs.currentH);
    // Initialize the camera module (required for the projection matrix that be will parsed to the shaders)
    this->pCamera = new SGL_Camera(pOGLM);
    this->pCamera->initialize(getWindowCreationSpecs().internalW, getWindowCreationSpecs().internalH);
    // Update orthographic shaders
    this->setCameraMode(CAMERA_MODE::DEFAULT);
    SGL_Log("Orthographic shader width: " + std::to_string(this->getWindowCreationSpecs().internalW), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    SGL_Log("Orthographic shader height: " + std::to_string(this->getWindowCreationSpecs().internalH), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    SGL_Log("Camera and orthographic shader configured.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    // Configure renderer
    this->pClearScreen = {1.0f, 1.0f, 1.0f, 1.0f};
    renderer = std::make_unique<SGL_Renderer>(pOGLM, assetManager->getShader("line"), assetManager->getShader("point"), assetManager->getShader("text"), assetManager->getShader("spriteUV"), assetManager->getShader("spriteBatchUV"), assetManager->getShader("pixelBatch"),
    assetManager->getShader("lineBatch"));
    SGL_Log("Renderer configured.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    // Setup the post processor
    this->startFBO(pDefaultPPShader);
    pPostProcessorFBO->setClearColor(pClearScreen);
    SGL_Log("Post-processor initialized.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    // Set the default window icon
    Uint16 pixels[16*16] = { // Will later upgrado to use images too
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0aab, 0x0789, 0x0bcc, 0x0eee, 0x09aa, 0x099a, 0x0ddd,
        0x0fff, 0x0eee, 0x0899, 0x0fff, 0x0fff, 0x1fff, 0x0dde, 0x0dee,
        0x0fff, 0xabbc, 0xf779, 0x8cdd, 0x3fff, 0x9bbc, 0xaaab, 0x6fff,
        0x0fff, 0x3fff, 0xbaab, 0x0fff, 0x0fff, 0x6689, 0x6fff, 0x0dee,
        0xe678, 0xf134, 0x8abb, 0xf235, 0xf678, 0xf013, 0xf568, 0xf001,
        0xd889, 0x7abc, 0xf001, 0x0fff, 0x0fff, 0x0bcc, 0x9124, 0x5fff,
        0xf124, 0xf356, 0x3eee, 0x0fff, 0x7bbc, 0xf124, 0x0789, 0x2fff,
        0xf002, 0xd789, 0xf024, 0x0fff, 0x0fff, 0x0002, 0x0134, 0xd79a,
        0x1fff, 0xf023, 0xf000, 0xf124, 0xc99a, 0xf024, 0x0567, 0x0fff,
        0xf002, 0xe678, 0xf013, 0x0fff, 0x0ddd, 0x0fff, 0x0fff, 0xb689,
        0x8abb, 0x0fff, 0x0fff, 0xf001, 0xf235, 0xf013, 0x0fff, 0xd789,
        0xf002, 0x9899, 0xf001, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0xe789,
        0xf023, 0xf000, 0xf001, 0xe456, 0x8bcc, 0xf013, 0xf002, 0xf012,
        0x1767, 0x5aaa, 0xf013, 0xf001, 0xf000, 0x0fff, 0x7fff, 0xf124,
        0x0fff, 0x089a, 0x0578, 0x0fff, 0x089a, 0x0013, 0x0245, 0x0eff,
        0x0223, 0x0dde, 0x0135, 0x0789, 0x0ddd, 0xbbbc, 0xf346, 0x0467,
        0x0fff, 0x4eee, 0x3ddd, 0x0edd, 0x0dee, 0x0fff, 0x0fff, 0x0dee,
        0x0def, 0x08ab, 0x0fff, 0x7fff, 0xfabc, 0xf356, 0x0457, 0x0467,
        0x0fff, 0x0bcd, 0x4bde, 0x9bcc, 0x8dee, 0x8eff, 0x8fff, 0x9fff,
        0xadee, 0xeccd, 0xf689, 0xc357, 0x2356, 0x0356, 0x0467, 0x0467,
        0x0fff, 0x0ccd, 0x0bdd, 0x0cdd, 0x0aaa, 0x2234, 0x4135, 0x4346,
        0x5356, 0x2246, 0x0346, 0x0356, 0x0467, 0x0356, 0x0467, 0x0467,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff
    };
    this->setWindowIcon(pixels);

    pDeltaTimeMS = 0.0f;
    SGL_Log("<--- SkeletonGL rendering engine successfully loaded and configured --->", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);
    pAlreadyInitialized = true;
}

/**
 * @brief Ask the camera to update, won't if unnecessary
 * @param forceUpdate Forces the update to take place
 * @return nothing
 */
void SGL_Window::updateCamera(bool forceUpdate)
{
    this->pCamera->updateCamera(forceUpdate);
}

/**
 * @brief Set the camera position
 * @param pos glm::vec2 position vector
 * @return nothing
 */
void SGL_Window::setCameraPosition(glm::vec2 pos)
{
    this->pCamera->setPosition(pos);
}


/**
 * @brief Scale the camera to a new size
 * @param newScale Values outside 1.0 and -1.0 will cause visual glitches
 * @return nothing
 */
void SGL_Window::setCameraScale(float newScale)
{
    this->pCamera->setScale(newScale);
}

/**
 * @brief Toggles the camera mode between DEFAULT and OVERLAY and updates the PP shaders
 * @param mode Mode to be implemented
 * @return nothing
 */
void SGL_Window::setCameraMode(CAMERA_MODE mode)
{
    /*  Camera is active, apply the offset to the projection matrix */
    // Attempt to iterate all the required shaders, the PP doesn't require any MVP transformations
    // since it simply blits the final FBO texture to the screen
    for (auto iter = assetManager->shaderTypes.begin(); iter != assetManager->shaderTypes.end(); ++iter)
    {
        if (assetManager->getShader((*iter).first).shaderType != SHADER_TYPE::POST_PROCESSOR)
        {
            this->pCamera->cameraMode(assetManager->getShader((*iter).first), mode);
            //SGL_Log("Updating shader: " + (*iter).first);
        }
    }
}


/**
 * @brief Returns the horizontal position of the camera
 * @param mode Mode to be implemented
 * @return int Horizontal camera position
 */
int SGL_Window::getCameraPosX()
{
    return static_cast<int>(this->pCamera->getPosition().x);
}


/**
 * @brief Returns the vertical position of the camera
 * @return int Vertical camera position
 */
int SGL_Window::getCameraPosY()
{
    return static_cast<int>(this->pCamera->getPosition().y);
}


/**
 * @brief Returns the width of the camera
 * @return int Camera's width
 */
int SGL_Window::getCameraPosW()
{
    return static_cast<int>(this->pCamera->getPosition().z);
}

/**
 * @brief Returns the height of the camera
 * @return int Camera's height
 */
int SGL_Window::getCameraPosH()
{
    return static_cast<int>(this->pCamera->getPosition().w);
}

/**
 * @brief Set the OpenGL clear screen color
 * @param color The new clear screen color (alpha is ignored)
 */
void SGL_Window::setClearColor(const SGL_Color &color)
{
    pClearScreen = {color.r, color.g, color.b, 1.0f};
    pPostProcessorFBO->setClearColor(pClearScreen);
}


/**
 * @brief Change the Window title
 * @param newTitle String to be used
 */
void SGL_Window::setWindowTitle(const std::string &newTitle)
{
    SDL_SetWindowTitle(pWindow, newTitle.c_str());
}


/**
 * @brief Get the current WCS (note that these may be different to the one originally submitted)
 * @return WindowCreationSpecs Note the .ini file settings have priority
 */
WindowCreationSpecs SGL_Window::getWindowCreationSpecs()
{
    return this->pWindowCreationSpecs;
}

/**
 * @brief Change the program's icon (displayed in taskbars and window menu bars)
 * @param pixelMatrix Must be exactly 255 bytes long
 * @return nothing
 */
void SGL_Window::setWindowIcon(Uint16 pixelMatrix[16*16])
{
    SDL_Surface *surface;
    surface = SDL_CreateRGBSurfaceFrom(pixelMatrix,16,16,16,16*2,0x0f00,0x00f0,0x000f,0xf000);
    SDL_SetWindowIcon(this->pWindow, surface);
    SDL_FreeSurface(surface);
}

//NOTE: if the the new window size isn't internally rescaled the camera must
//be adjusted

/**
 * @brief Scales the main window, unavailable in fullscreen, scaling internal resolution resizes the glViewPort
 * @param w New window width
 * @param h New window height
 * @param scaleInternalResolution Resize the glViewPort?
 * @return nothing
 */
void SGL_Window::resizeWindow(int w, int h, bool scaleInternalResolution)
{
    if (w >= pWindowCreationSpecs.minW && h >= pWindowCreationSpecs.minH)
    {
        if (pWindowCreationSpecs.fullScreen)
        {
            SGL_Log("Can't resize window on fullscreen mode.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
            return;
        }
        SGL_Log("Resized window to w: " + std::to_string(w) +
                    " h: " + std::to_string(h));
        pWindowCreationSpecs.currentW = w;
        pWindowCreationSpecs.currentH = h;
        SDL_SetWindowSize(this->pWindow, w, h);
        if (scaleInternalResolution)
            this->resizeGL(w, h);
    }
    else
    {
        SGL_Log("Window can't be smaller than w: " + std::to_string(pWindowCreationSpecs.minW) +
                    " h: " + std::to_string(pWindowCreationSpecs.minH), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
}

/**
 * @brief Does the window have the user's keyboard focus?
 * @return bool
 */
bool SGL_Window::hasKeyboardFocus()
{
    return pHasKeyboardFocus;
}


/**
 * @brief Does the window have the user's mouse focus?
 * @return bool
 */
bool SGL_Window::hasMouseFocus()
{
    return pHasMouseFocus;
}

// Check if there are any gamepads available

/**
 * @brief Check if there are any gamepads available
 * @param gamepadID ID of the gamepad to use as default
 * @return nothing
 */
void SGL_Window::checkGamepads(int gamepadID)
{
    pGamepad_1 = NULL;
    SGL_Log("Checking for available gamepads...");
    int totalGamepadsFound = SDL_NumJoysticks();
    if( totalGamepadsFound == 0 )
    {
        SGL_Log("No gamepads detected. " + std::string(SDL_GetError()));
        return;
    }
    else if (gamepadID != 0)
    {
        for (int i = 0; i < totalGamepadsFound; ++i)
        {
            SDL_Joystick *tmpGamepad = SDL_JoystickOpen(i);
            if (tmpGamepad)
            {
                SDL_JoystickGUID guid = SDL_JoystickGetGUID(tmpGamepad);
                char guidStr[1024];
                SDL_JoystickGetGUIDString(guid, guidStr, sizeof(guidStr));
                const char *gamepadName = SDL_JoystickName(tmpGamepad);
                std::string guidString {guidStr};
                std::string nameString {gamepadName};

                int axes = SDL_JoystickNumAxes(tmpGamepad);
                int buttons = SDL_JoystickNumButtons(tmpGamepad);
                int hats = SDL_JoystickNumHats(tmpGamepad);
                int balls = SDL_JoystickNumBalls(tmpGamepad);

                SGL_Log("Gamepad #" + std::to_string(i+1) + " name: " + nameString + " ID:" + guidString + "  axes: " + std::to_string(axes) +
                        " buttons: " + std::to_string(buttons) +
                        " hats: " + std::to_string(hats) +
                        " balls: " + std::to_string(balls));
                SDL_JoystickClose(tmpGamepad);
                // We know gamepadID isn't 0, however SDL starts counting found gamepads from 0
                if ((gamepadID - 1) == i)
                {
                    this->pGamepad_1 = SDL_JoystickOpen(i);
                    if (pGamepad_1 == NULL)
                        SGL_Log("Failed to load gamepad.");
                    else
                        SGL_Log("Gamepad #" + std::to_string(gamepadID) + " loaded.");

                    return;
                }
            }
        }
    }
}

/**
 * @brief Get the input polled by this particular frame
 * @return SDL_InputFrame Contains all the collective user input
 */
SGL_InputFrame SGL_Window::getFrameInput()
{
    // Feel free to remove all the input your program won't need from the checklist
    SGL_InputFrame input;

    // -- MOUSE --
    // Mouse position relative to the window dimensions (RAW)
    SDL_GetMouseState(&input.rawMousePosX, &input.rawMousePosY);
    // Mouse position relative to the internal rendering resolution
    // Get the position in percent relative to the current resolution
    float xPos = input.rawMousePosX / static_cast<float>(pWindowCreationSpecs.currentW);
    float yPos = input.rawMousePosY / static_cast<float>(pWindowCreationSpecs.currentH);
    // Get the position relative to the internal resoultion using the percent
    input.normalizedMousePosX = std::floor(xPos * static_cast<float>(pWindowCreationSpecs.internalW));
    input.normalizedMousePosY = std::floor(yPos * static_cast<float>(pWindowCreationSpecs.internalH));

    while(SDL_PollEvent(&pEvent) != 0)
    {
        // Note about SDL_QUIT:
        // This event can raise from a variety of different sources and is OS dependant, in UNIX systems
        // all signals (SIGINT, SIGTERM etc.) that aren't directly addressed will also throw an SDL_QUIT
        // signal. SDL_WINDOWEVENT_CLOSE is a reliable alternative to handling user requested program
        // termination.

        if( pEvent.type == SDL_QUIT ) //{ input.sdlInternalQuit = true; }
            SGL_Log("SDL_QUIT event received.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        if( pEvent.type == SDL_WINDOWEVENT )
        {
            switch(pEvent.window.event) // --- WINDOW EVENTS ---
            {
            case SDL_WINDOWEVENT_RESIZED: // Resize the specs and the glViewPort
                pWindowCreationSpecs.currentW = pEvent.window.data1;
                pWindowCreationSpecs.currentH = pEvent.window.data2;
                this->resizeGL(pEvent.window.data1, pEvent.window.data2);
                // this->calculateProjections();
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
                SGL_Log("Window minimized: " + std::to_string(static_cast<int>(pEvent.window.windowID)), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                pIsMinimized = true;
                break;
            case SDL_WINDOWEVENT_RESTORED:
                SGL_Log("Window restored: " + std::to_string(static_cast<int>(pEvent.window.windowID)), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                input.windowMinimized = false;
                input.windowRestored = true;
                pIsMinimized = false;
                break;
            case SDL_WINDOWEVENT_ENTER:
                SGL_Log("Window gained mouse focus: " + std::to_string(static_cast<int>(pEvent.window.windowID)), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                pHasMouseFocus = true;
                break;
            case SDL_WINDOWEVENT_LEAVE:
                SGL_Log("Window lost mouse focus: " + std::to_string(static_cast<int>(pEvent.window.windowID)), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                pHasMouseFocus = false;
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                SGL_Log("Window gained keyboard focus: " + std::to_string(static_cast<int>(pEvent.window.windowID)), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                pHasKeyboardFocus = true;
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                SGL_Log("Window lost keyboard focus: " + std::to_string(static_cast<int>(pEvent.window.windowID)), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                pHasKeyboardFocus = false;
                break;
            case SDL_WINDOWEVENT_CLOSE:
                SGL_Log("Window has requested to exit: " + std::to_string(static_cast<int>(pEvent.window.windowID)), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
                input.sdlInternalQuit = true;
                break;
            }
        }
        // --- MOUSE --
        if (pEvent.button.button == SDL_BUTTON_LEFT && pEvent.button.state == SDL_PRESSED) { input.mouseLeftPressed = true; }
        if (pEvent.button.button == SDL_BUTTON_LEFT && pEvent.button.state == SDL_RELEASED) { input.mouseLeftReleased = true; }
        if (pEvent.button.button == SDL_BUTTON_RIGHT && pEvent.button.state == SDL_PRESSED) { input.mouseRightPressed = true; }
        if (pEvent.button.button == SDL_BUTTON_RIGHT && pEvent.button.state == SDL_RELEASED) { input.mouseRightReleased = true; }

        // --- KEYBOARD ---
        //NOTE: Its best to place keys that shouldn't be held down like a toggle button
        if(pEvent.type == SDL_KEYDOWN && pEvent.key.repeat == 0)
        {
            switch(pEvent.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                input.esc = true;
                break;
            case SDLK_SPACE:
                input.space = true;
                break;
            case SDLK_RETURN: // Scancode for the enter key don't seem to work
                input.enter = true;
                break;
            case SDLK_RETURN2:
                input.enter = true;
                break;
            case SDLK_F1:
                SDL_ShowCursor(1);
                break;
            case SDLK_F2:
                SDL_ShowCursor(0);
                break;
            }
        }
    }

    // Keyboard key states
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    // Alphabet (qwerty)
    // First row
    if (state[SDL_SCANCODE_Q]) { input.q = true; }
    if (state[SDL_SCANCODE_W]) { input.w = true; }
    if (state[SDL_SCANCODE_E]) { input.e = true; }
    if (state[SDL_SCANCODE_R]) { input.r = true; }
    if (state[SDL_SCANCODE_T]) { input.t = true; }
    if (state[SDL_SCANCODE_Y]) { input.y = true; }
    if (state[SDL_SCANCODE_U]) { input.u = true; }
    if (state[SDL_SCANCODE_I]) { input.i = true; }
    if (state[SDL_SCANCODE_O]) { input.o = true; }
    if (state[SDL_SCANCODE_P]) { input.p = true; }
    // Second row
    if (state[SDL_SCANCODE_A]) { input.a = true; }
    if (state[SDL_SCANCODE_S]) { input.s = true; }
    if (state[SDL_SCANCODE_D]) { input.d = true; }
    if (state[SDL_SCANCODE_F]) { input.f = true; }
    if (state[SDL_SCANCODE_G]) { input.g = true; }
    if (state[SDL_SCANCODE_H]) { input.h = true; }
    if (state[SDL_SCANCODE_J]) { input.j = true; }
    if (state[SDL_SCANCODE_K]) { input.k = true; }
    if (state[SDL_SCANCODE_L]) { input.l = true; }
    // Third row
    if (state[SDL_SCANCODE_Z]) { input.z = true; }
    if (state[SDL_SCANCODE_X]) { input.x = true; }
    if (state[SDL_SCANCODE_C]) { input.c = true; }
    if (state[SDL_SCANCODE_V]) { input.v = true; }
    if (state[SDL_SCANCODE_B]) { input.b = true; }
    if (state[SDL_SCANCODE_N]) { input.n = true; }
    if (state[SDL_SCANCODE_M]) { input.m = true; }
    // Arrow keys
    if (state[SDL_SCANCODE_UP]) { input.up = true; }
    if (state[SDL_SCANCODE_DOWN]) { input.down = true; }
    if (state[SDL_SCANCODE_LEFT]) { input.left = true; }
    if (state[SDL_SCANCODE_RIGHT]) { input.right = true; }
    // Numbers
    if (state[SDL_SCANCODE_1]) { input.num1 = true; }
    if (state[SDL_SCANCODE_2]) { input.num2 = true; }
    if (state[SDL_SCANCODE_3]) { input.num3 = true; }
    if (state[SDL_SCANCODE_4]) { input.num4 = true; }
    if (state[SDL_SCANCODE_5]) { input.num5 = true; }
    if (state[SDL_SCANCODE_6]) { input.num6 = true; }
    if (state[SDL_SCANCODE_7]) { input.num7 = true; }
    if (state[SDL_SCANCODE_8]) { input.num8 = true; }
    if (state[SDL_SCANCODE_9]) { input.num9 = true; }
    // Modifiers
    if (state[SDL_SCANCODE_BACKSPACE]) { input.backspace = true; }
    // if (state[SDL_SCANCODE_ESCAPE]) { input.esc = true; }
    if (state[SDL_SCANCODE_LCTRL]) { input.ctrl = true; }
    if (state[SDL_SCANCODE_LALT]) { input.alt = true; }
    if (state[SDL_SCANCODE_LSHIFT]) { input.shift = true; }
    if (state[SDL_SCANCODE_KP_ENTER]) { input.enter = true; }
    if (state[SDL_SCANCODE_KP_SPACE]) { input.space = true; }
    // if (state[SDL_SCANCODE_ENTER || SDL_SCANCODE_ENTER2]) { input.enter = true; }
    if (state[SDL_SCANCODE_RETURN || SDL_SCANCODE_RETURN2]) { input.enter = true; }

    // Save the current input as delta to compare
    pDeltaInput = input;
    return input;
}

/**
 * @brief Update the glViewPort and post-processor framebuffer dimensions
 * @param width New internal rendering width
 * @param height New internal rendering height
 * @return nothing
 */
void SGL_Window::resizeGL(int width, int heigth)
{
    pOGLM->resizeGL(0, 0, width, heigth);
    if (this->pPostProcessorFBO != nullptr)
    {
        SGL_Log("Reloading main frame buffer...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
        this->pPostProcessorFBO->reload(width, heigth);
        SGL_Log("Done.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }
}


/**
 * @brief Configure and instantiate the post-processor
 * @param SGL_Shader The framebuffer shader
 * @return nothing
 */
void SGL_Window::startFBO(const SGL_Shader &shader)
{
    // Destroy previous FBO if it exists
    SGL_Log("Starting the post-processor FBO.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    this->checkForErrors();
    // Start the FBO
    this->pPostProcessorFBO = new SGL_PostProcessor(pOGLM, shader);
    this->pPostProcessorFBO->reload(pWindowCreationSpecs.currentW, pWindowCreationSpecs.currentH);
    this->checkForErrors();
    SGL_Log("Done", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
}


/**
 * @brief Get time since program initialization (provided by SDL)
 * @return uint32_t Time since the program started
 */
uint32_t SGL_Window::getTickCount()
{
    return SDL_GetTicks();
}


/**
 * @brief Take an initial timestamp of the frame to be processed and reload the post processor
 * @return nothing
 */
void SGL_Window::startFrame()
{
    // SDL2 based profiler
    pT0 = SDL_GetPerformanceCounter();
    pTick0 = SDL_GetTicks();
    // C++11 (chrono) based timer
    // pChrono0 = std::chrono::high_resolution_clock::now();
    pChrono0 = std::chrono::steady_clock::now();

    // Begin FBO operations
    this->pPostProcessorFBO->beginRender();
}

/**
 * @brief Sleeps the main thread
 * @param double Time to sleep in milliseconds
 * @return nothing
 */
void SGL_Window::sleep(std::uint32_t ms)
{
    SDL_Delay(ms);
}

/**
 * @brief Calculates delta times, renders the current state of the post processor and swaps the renderer's window
 * @return nothing
 */
void SGL_Window::endFrame()
{
    // End all rendering operations
    this->pPostProcessorFBO->endRender();

    // pChrono1 = std::chrono::high_resolution_clock::now();
    pChrono1 = std::chrono::steady_clock::now();
    pT1 = SDL_GetPerformanceCounter();
    pTick1 = SDL_GetTicks();
    // pDeltaTimeMS = ((pT1 - pT0)) / static_cast<double>(SDL_GetPerformanceFrequency() * 1000.0f);
    pDeltaTimeMS = (pTick1 - pTick0) / 1000.0f;

    auto timeDiff = std::chrono::duration_cast<std::chrono::nanoseconds>(pChrono1 - pChrono0);
    pChronoDeltaTime = timeDiff.count();
    // Nanoseconds to seconds
    pChronoDeltaTime /= 1000000000;

    pDefaultPPShader.renderDetails.deltaTime = pChronoDeltaTime;
    pDefaultPPShader.renderDetails.mousePosX = pDeltaInput.normalizedMousePosX;
    pDefaultPPShader.renderDetails.mousePosY = pDeltaInput.normalizedMousePosY;

    // Render final FBO texture
    this->pPostProcessorFBO->render(pDefaultPPShader);
    // Finally, swap the buffers
    SDL_GL_SwapWindow(this->pWindow);
}


/**
 * @brief Returns the time it took to process the frame
 * @return double Delta time
 */
double SGL_Window::getDeltaTime()
{
    //return pDeltaTimeMS;
    return pChronoDeltaTime;
}

/**
 * @brief Toggle the screen shake post processor effect
 * @param shakeit Bool to enable / disable
 * @return nothing
 */
void SGL_Window::setShakeScreen(bool shakeit)
{
    if (shakeit)
        this->pPostProcessorFBO->shake = GL_TRUE;
    else
        this->pPostProcessorFBO->shake = GL_FALSE;
}

/**
 * @brief Toggle the screen chaos post processor effect
 * @param chaos Bool to enable / disable
 * @return nothing
 */
void SGL_Window::setChaosEffect(bool chaos)
{
    if (chaos)
        this->pPostProcessorFBO->chaos = GL_TRUE;
    else
        this->pPostProcessorFBO->chaos = GL_FALSE;
}

/**
 * @brief Toggle the screen confuse post processor effect
 * @param confuse Bool to enable / disable
 * @return nothing
 */
void SGL_Window::setConfuseEffect(bool confuse)
{
    if (confuse)
        this->pPostProcessorFBO->confuse = GL_TRUE;
    else
        this->pPostProcessorFBO->confuse = GL_FALSE;
}

/**
 * @brief Check for any potential errors, useful to debug a particular render or gl process
 * @return nothing
 */
void SGL_Window::checkForErrors()
{
    pOGLM->checkForGLErrors();
}

/**
 * @brief SGL internal media is to be loaded here, place your custom assets elsewhere to prevent mistakes
 * @return nothing
 */
void SGL_Window::pLoadDefaultAssets()
{
    SGL_Log("<--- Default assets --->", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_GREEN);
    assetManager = std::make_unique<SGL_AssetManager>(pOGLM);

    // The default texture will replace any texture not found by the loadTexture
    // function, it must be loaded before the AssetManager can properly load textures
    std::string defaultTextureName = "defaultTexture.png";
    std::string blankSquare = FOLDER_STRUCTURE::imagesDir + "blank_square.png";
    std::string bmpFont = FOLDER_STRUCTURE::imagesDir + "default_bitmap_font.png";

    assetManager->loadTexture(FOLDER_STRUCTURE::defaultTexture.c_str(), GL_FALSE, defaultTextureName);
    assetManager->loadTexture(blankSquare.c_str(), GL_TRUE, "blankSquare");
    assetManager->loadTexture(bmpFont.c_str(), GL_TRUE, "defaultBitmapFont");

    SGL_Log("Default textures loaded.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    pOGLM->checkForGLErrors();
    // SGL_Log("Loading default shaders...", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    // Load default shaders
    std::string spriteUVV = FOLDER_STRUCTURE::shadersDir + "spriteUVV.c";
    std::string spriteUVF = FOLDER_STRUCTURE::shadersDir + "spriteUVF.c";
    std::string spriteBatchUVV = FOLDER_STRUCTURE::shadersDir + "spriteBatchUVV.c";
    std::string spriteBatchUVF = FOLDER_STRUCTURE::shadersDir + "spriteBatchUVF.c";
    std::string textV = FOLDER_STRUCTURE::shadersDir + "textV.c";
    std::string textF = FOLDER_STRUCTURE::shadersDir + "textF.c";
    std::string particleV = FOLDER_STRUCTURE::shadersDir + "particleV.c";
    std::string particleF = FOLDER_STRUCTURE::shadersDir + "particleF.c";
    std::string lineV = FOLDER_STRUCTURE::shadersDir + "lineV.c";
    std::string lineF = FOLDER_STRUCTURE::shadersDir + "lineF.c";
    std::string pointV = FOLDER_STRUCTURE::shadersDir + "pointV.c";
    std::string pointF = FOLDER_STRUCTURE::shadersDir + "pointF.c";
    std::string FBOshaderV = FOLDER_STRUCTURE::shadersDir + "frameBufferV.c";
    std::string FBOshaderF = FOLDER_STRUCTURE::shadersDir + "frameBufferF.c";
    std::string pixelBatchV = FOLDER_STRUCTURE::shadersDir + "pixelBatchV.c";
    std::string pixelBatchF = FOLDER_STRUCTURE::shadersDir + "pixelBatchF.c";
    std::string lineBatchV = FOLDER_STRUCTURE::shadersDir + "lineBatchV.c";
    std::string lineBatchF = FOLDER_STRUCTURE::shadersDir + "lineBatchF.c";

    assetManager->loadShaders(spriteUVV.c_str(), spriteUVF.c_str(), nullptr, "spriteUV", SHADER_TYPE::SPRITE);
    assetManager->loadShaders(spriteBatchUVV.c_str(), spriteBatchUVF.c_str(), nullptr, "spriteBatchUV", SHADER_TYPE::SPRITE);
    assetManager->loadShaders(textV.c_str(), textF.c_str(), nullptr, "text", SHADER_TYPE::TEXT);
    assetManager->loadShaders(lineV.c_str(), lineF.c_str(), nullptr, "line", SHADER_TYPE::LINE);
    assetManager->loadShaders(pointV.c_str(), pointF.c_str(), nullptr, "point", SHADER_TYPE::PIXEL);
    assetManager->loadShaders(FBOshaderV.c_str(), FBOshaderF.c_str(), nullptr, "postProcessor", SHADER_TYPE::POST_PROCESSOR);
    assetManager->loadShaders(pixelBatchV.c_str(), pixelBatchF.c_str(), nullptr, "pixelBatch", SHADER_TYPE::PIXEL);
    assetManager->loadShaders(lineBatchV.c_str(), lineBatchF.c_str(), nullptr, "lineBatch", SHADER_TYPE::LINE);

    pDefaultPPShader = assetManager->getShader("postProcessor");
    SGL_Log("Default shaders compiled and linked.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    pOGLM->checkForGLErrors();
}

/**
 * @brief Toggle the window fullscreen feature
 * @param toggle Toggle fullscreen mode on or off
 * @return nothing
 * @section DESCRIPTION
 *
 * Enabling fullscreen requires the internal glViewPort to match the new size of the fullscreen window.
 * When applying the fullscreen transformation the resizeGL function must be called with the display's
 * max resolution to avoid blackbars
 */
void SGL_Window::toggleFullScreen(bool toggle)
{
    SGL_Log("TOGGLE FULLSCREEN.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    if (toggle)
    {
        SDL_SetWindowFullscreen(this->pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
        pWindowCreationSpecs.fullScreen = true;
        // If the internal resolution and the max display resolution aren't compatible, artifacts will form
        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);
        pWindowCreationSpecs.fullscreenW = current.w;
        pWindowCreationSpecs.fullscreenH = current.h;
        this->resizeGL(pWindowCreationSpecs.fullscreenW, pWindowCreationSpecs.fullscreenH);
        SGL_Log("Fullscreen enabled @ w: " + std::to_string(pWindowCreationSpecs.fullscreenW) + " h: " + std::to_string(pWindowCreationSpecs.fullscreenH), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);

    }
    else
    {
        SDL_SetWindowFullscreen(this->pWindow, 0);
        pWindowCreationSpecs.fullScreen = false;
        SGL_Log("Fullscreen disabled.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    }


    // if (!toggle && pWindowCreationSpecs.fullScreen)
    // {
    //     SDL_SetWindowFullscreen(this->pWindow, 0);
    //     pWindowCreationSpecs.fullScreen = false;
    //     SGL_Log("Fullscreen disabled.", LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    // }
    // if (toggle && !pWindowCreationSpecs.fullScreen)
    // {
    //     SDL_SetWindowFullscreen(this->pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
    //     pWindowCreationSpecs.fullScreen = true;
    //     // If the internal resolution and the max display resolution aren't compatible, artifacts will form
    //     SDL_DisplayMode current;
    //     SDL_GetCurrentDisplayMode(0, &current);
    //     pWindowCreationSpecs.fullscreenW = current.w;
    //     pWindowCreationSpecs.fullscreenH = current.h;
    //     this->resizeGL(pWindowCreationSpecs.fullscreenW, pWindowCreationSpecs.fullscreenH);
    //     SGL_Log("Fullscreen enabled @ w: " + std::to_string(pWindowCreationSpecs.fullscreenW) + " h: " + std::to_string(pWindowCreationSpecs.fullscreenH), LOG_LEVEL::SGL_DEBUG, LOG_COLOR::TERM_DEFAULT);
    // }
}

/**
 * @brief Update the post-processor shader
 * @param SGL_Shader New FBO shader
 * @return nothing
 */
void SGL_Window::setPostProcessorShader(SGL_Shader shader)
{
    if (shader.shaderType == SHADER_TYPE::POST_PROCESSOR)
        pDefaultPPShader = shader;
}


