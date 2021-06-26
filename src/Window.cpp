//
// Project: evolution_simulator_2
// File: Window.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include <Window.hpp>
#include <Utils.hpp>
#include <CreatureComponent.hpp>

#include <engine/LogicComponent.hpp>
#include <graphics/SpriteSingleton.hpp>


Window::Window(
    const Window::Settings &settings
) :
    _settings           (settings),
    _window             (nullptr),
    _quit               (false),
    _viewport           (Vec2f(_settings.window.width*0.5f, _settings.window.height*0.5f), 32.0f),
    _cursorPosition     (0.0f, 0.0f),
    _lastTicks          (0),
    _frameTicks         (0),
    _windowContext      (*this),
    _creatureSystem     (_ecs),
    _spriteSystem       (_ecs),
    _spriteSheetId      (-1)
{
    int err;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error: Could not initialize SDL!\n");
        return;
    }

    _window = SDL_CreateWindow(
            _settings.window.name.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            (int)_settings.window.width,
            (int)_settings.window.height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (_window == nullptr) {
        printf("Error: SDL Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, _settings.gl.contextMajor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, _settings.gl.contextMinor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, _settings.gl.contextFlags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, _settings.gl.profileMask);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, _settings.gl.doubleBuffer);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    _glCtx = SDL_GL_CreateContext(_window);

    if (_glCtx == nullptr) {
        printf("Error: SDL OpenGL context could not be created! SDL_Error: %s\n",
               SDL_GetError());
        return;
    }

    // Load OpenGL extensions
    if (!gladLoadGL()) {
        printf("Error: gladLoadGL failed\n");
        return;
    }

    // Initialize OpenGL
    glViewport(0, 0, _settings.window.width, _settings.window.height);
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize world
    init();
}

Window::~Window()
{
    // Destroy window and quit SDL subsystems
    SDL_GL_DeleteContext(_glCtx);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void Window::init(void)
{
    _ecs.getSingleton<fug::SpriteSingleton>()->init();
    _spriteSheetId = _ecs.getSingleton<fug::SpriteSingleton>()->addSpriteSheetFromFile(
        EVOLUTION_SIMULATOR_RES("sprites/creature.png"), 128, 128);

    // TODO TEMP begin
    // Test entity
    fug::EntityId testEntityId = 0;
    _ecs.addComponent<fug::Orientation2DComponent>(testEntityId);
    _ecs.getComponent<fug::Orientation2DComponent>(testEntityId)->setScale(1.0f/64.0f);
    _ecs.addComponent<CreatureComponent>(testEntityId);
    _ecs.setComponent(testEntityId, fug::SpriteComponent(_spriteSheetId, 0));
    _ecs.getComponent<fug::SpriteComponent>(testEntityId)->setOrigin(Vec2f(64.0f, 64.0f));
    // TODO TEMP end
}

void Window::loop(void)
{
    static int frame = 0;

    // Application main loop
    while (!_quit) {
        // Event handling
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            handleEvent(event);
        }

        if (++frame >= 4)
            frame = 0;

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Run systems
        runSystems();

        // Render sprites
        _ecs.getSingleton<fug::SpriteSingleton>()->render(_viewport);

        // Swap draw and display buffers
        SDL_GL_SwapWindow(_window);
        SDL_Delay(1000/_settings.window.framerateLimit);

        uint32_t curTicks = SDL_GetTicks();
        _frameTicks = curTicks - _lastTicks;
        _lastTicks = curTicks;
    }
}

void Window::handleEvent(SDL_Event& event)
{
    switch (event.type) {
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    _quit = true;
                    break;
            }
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                _quit = true;
            break;
        case SDL_MOUSEWHEEL:
            _viewport.zoom(std::pow(1.414213562373f, event.wheel.y), _cursorPosition);
            break;
        case SDL_MOUSEMOTION:
            _cursorPosition << (float)event.motion.x, (float)event.motion.y;
            break;
    }
}

void Window::runSystems(void)
{
    _ecs.runSystem(_creatureSystem);
    _ecs.runSystem(_spriteSystem);
}
