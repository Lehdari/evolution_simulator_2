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
#include <Genome.hpp>
#include <CreatureComponent.hpp>
#include <FoodComponent.hpp>
#include <WorldSingleton.hpp>
#include <ConfigSingleton.hpp>
#include <LineSingleton.hpp>
#include <EventHandlers.hpp>
#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>

#include <engine/LogicComponent.hpp>
#include <engine/EventComponent.hpp>
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
    _eventSystem        (_ecs),
    _creatureSystem     (_ecs),
    _foodSystem         (_ecs),
    _collisionSystem    (_ecs, _eventSystem),
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

    // Initialize OpenGL
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplSDL2_InitForOpenGL(_window, _glCtx);
    ImGui_ImplOpenGL3_Init("#version 420");

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
    _ecs.getSingleton<fug::SpriteSingleton>()->setWindowSize(
        (int)_settings.window.width, (int)_settings.window.height);
    _spriteSheetId = _ecs.getSingleton<fug::SpriteSingleton>()->addSpriteSheetFromFile(
        EVOLUTION_SIMULATOR_RES("sprites/sprites.png"), 128, 128);

    _ecs.getSingleton<LineSingleton>()->init();
    _ecs.getSingleton<LineSingleton>()->setWindowSize(
        (int)_settings.window.width, (int)_settings.window.height);

    fug::SpriteComponent creatureSpriteComponent(_spriteSheetId, 0);
    creatureSpriteComponent.setOrigin(Vec2f(ConfigSingleton::spriteRadius, ConfigSingleton::spriteRadius));
    fug::SpriteComponent foodSpriteComponent(_spriteSheetId, 1);
    foodSpriteComponent.setOrigin(Vec2f(ConfigSingleton::spriteRadius, ConfigSingleton::spriteRadius));
    foodSpriteComponent.setColor(Vec3f(0.2f, 0.6f, 0.0f));

    auto& config = *_ecs.getSingleton<ConfigSingleton>();

    // Create creatures
    constexpr int nCreatures = 500;
    for (int i=0; i<nCreatures; ++i) {
        fug::EntityId id = _ecs.getEmptyEntityId();

        // get position using rejection sampling
        Vec2f p(RNDS*1024.0f, RNDS*1024.0f);
        while (gauss2(p, 256.0f) < RND)
            p << RNDS*1024.0f, RNDS*1024.0f;

        double mass = ConfigSingleton::minCreatureMass + RND*(
            ConfigSingleton::maxCreatureMass-ConfigSingleton::minCreatureMass);

        _ecs.setComponent(id, fug::Orientation2DComponent(p, 0.0f,
            sqrtf(mass) / ConfigSingleton::spriteRadius));

        _ecs.setComponent(id, CreatureComponent(Genome(),
            0.1f*mass*config.massEnergyStorageConstant, mass, RND*M_PI*2.0f, RND));
        auto& g = _ecs.getComponent<CreatureComponent>(id)->getGenome();
        _ecs.setComponent(id, fug::SpriteComponent(creatureSpriteComponent));
        _ecs.getComponent<fug::SpriteComponent>(id)->setColor(Vec3f(
            g[Genome::COLOR_R], g[Genome::COLOR_G], g[Genome::COLOR_B]));
        _ecs.addComponent<fug::EventComponent>(id)->addHandler<EventHandler_Creature_CollisionEvent>();
    }

    // Create food
    constexpr int nFoods = 2500;
    for (int i=0; i<nFoods; ++i) {
        fug::EntityId id = _ecs.getEmptyEntityId();

        // get position using rejection sampling
        Vec2f p(RNDS*ConfigSingleton::worldSize, RNDS*ConfigSingleton::worldSize);
        while (gauss2(p, 256.0f) < RND)
            p << RNDS*ConfigSingleton::worldSize, RNDS*ConfigSingleton::worldSize;

        double mass = ConfigSingleton::minFoodMass + RND*(
            ConfigSingleton::maxFoodMass-ConfigSingleton::minFoodMass);
        _ecs.setComponent(id, FoodComponent(mass));
        _ecs.setComponent(id, fug::Orientation2DComponent(p, 0.0f,
            sqrt(mass) / ConfigSingleton::spriteRadius));
        _ecs.setComponent(id, fug::SpriteComponent(foodSpriteComponent));
    }
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

        updateWorld();
        updateGUI();

        // Render sprites
        _ecs.getSingleton<fug::SpriteSingleton>()->render(_viewport);
        _ecs.getSingleton<LineSingleton>()->render(_viewport);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
            _viewport.zoom(std::pow(1.414213562373f, (float)event.wheel.y), _cursorPosition);
            break;
        case SDL_MOUSEMOTION:
            _cursorPosition << (float)event.motion.x, (float)event.motion.y;
            break;
    }
}

void Window::updateGUI()
{
    static auto& world = *_ecs.getSingleton<WorldSingleton>();
    static auto& config = *_ecs.getSingleton<ConfigSingleton>();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(_window);
    ImGui::NewFrame();

    ImGui::Begin("Simulation Controls");

    auto nCreatures = world.getNumberOf(WorldSingleton::EntityType::CREATURE);
    auto nFood = world.getNumberOf(WorldSingleton::EntityType::FOOD);

    double foodMassToEnergyConstantMin = 1.0;
    double foodMassToEnergyConstantMax = 1000.0;
    double targetNCreatures = 500;

    ImGui::Text("N. Creatures: %lu\n", nCreatures);
    ImGui::Text("N. Food: %lu\n", nFood);

    // P-controlled foodMassToEnergyConstant (raise or lower how much energy creatures get from food
    // depending on how much of them are alive)
    config.foodMassToEnergyConstant += (targetNCreatures-(double)nCreatures)*0.00001;
    config.foodMassToEnergyConstant = std::clamp(config.foodMassToEnergyConstant,
        foodMassToEnergyConstantMin, foodMassToEnergyConstantMax);
    ImGui::SliderScalar("foodMassToEnergyConstant", ImGuiDataType_Double, &config.foodMassToEnergyConstant,
        &foodMassToEnergyConstantMin, &foodMassToEnergyConstantMax, "%.3f", ImGuiSliderFlags_Logarithmic);

    ImGui::End();
}

void Window::updateWorld(void)
{
    static auto& world = *_ecs.getSingleton<WorldSingleton>();
    static auto& config = *_ecs.getSingleton<ConfigSingleton>();

    _creatureSystem.setStage(CreatureSystem::Stage::COGNITION);
    _ecs.runSystem(_creatureSystem);
    _creatureSystem.setStage(CreatureSystem::Stage::DYNAMICS);
    _ecs.runSystem(_creatureSystem);
    _creatureSystem.setStage(CreatureSystem::Stage::REPRODUCTION);
    _ecs.runSystem(_creatureSystem);

    int nFoodsCurrent = world.getNumberOf(WorldSingleton::EntityType::FOOD);
    {   // Create new food
        fug::SpriteComponent foodSpriteComponent(_spriteSheetId, 1);
        foodSpriteComponent.setOrigin(Vec2f(ConfigSingleton::spriteRadius, ConfigSingleton::spriteRadius));
        foodSpriteComponent.setColor(Vec3f(0.2f, 0.6f, 0.0f));

        int nNewFood = std::max(100 / std::max((nFoodsCurrent - 10000) / 1000, 1), 0);
        for (int i = 0; i < nNewFood; ++i) {
            fug::EntityId id = _ecs.getEmptyEntityId();

            // get position using rejection sampling
            Vec2f p(RNDS * ConfigSingleton::worldSize, RNDS * ConfigSingleton::worldSize);
            while (gauss2(p, 256.0f) < RND)
                p << RNDS * ConfigSingleton::worldSize, RNDS * ConfigSingleton::worldSize;

            _ecs.setComponent(id, fug::Orientation2DComponent(p, 0.0f,
                sqrtf(config.minFoodMass) / ConfigSingleton::spriteRadius));
            _ecs.setComponent(id, FoodComponent(config.minFoodMass));
            _ecs.setComponent(id, fug::SpriteComponent(foodSpriteComponent));
        }
    }

    _foodSystem.setStage(FoodSystem::Stage::GROW);
    _ecs.runSystem(_foodSystem);

    addEntitiesToWorld();

    _ecs.runSystem(_collisionSystem);

    while (_eventSystem.swap())
        _ecs.runSystem(_eventSystem);

    addEntitiesToWorld();

    _creatureSystem.setStage(CreatureSystem::Stage::PROCESS_INPUTS);
    _ecs.runSystem(_creatureSystem);

    _ecs.runSystem(_spriteSystem);
}

void Window::addEntitiesToWorld(void)
{
    _ecs.getSingleton<WorldSingleton>()->reset();

    _creatureSystem.setStage(CreatureSystem::Stage::ADD_TO_WORLD);
    _ecs.runSystem(_creatureSystem);

    _foodSystem.setStage(FoodSystem::Stage::ADD_TO_WORLD);
    _ecs.runSystem(_foodSystem);
}
