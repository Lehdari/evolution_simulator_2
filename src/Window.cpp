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
#include <ResourceSingleton.hpp>
#include <MapSingleton.hpp>
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
    _settings               (settings),
    _window                 (nullptr),
    _quit                   (false),
    _paused                 (false),
    _viewport               (_settings.window.width, _settings.window.height,
                             Vec2f(_settings.window.width*0.5f, _settings.window.height*0.5f), 32.0f),
    _cursorPosition         (0.0f, 0.0f),
    _activeCreature         (-1),
    _activeCreatureFollow   (false),
    _lastTicks              (0),
    _frameTicks             (0),
    _windowContext          (*this),
    _eventSystem            (_ecs),
    _creatureSystem         (_ecs),
    _foodSystem             (_ecs),
    _collisionSystem        (_ecs, _eventSystem),
    _spriteSystem           (_ecs),
    _spriteSheetId          (-1)
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

    _ecs.getSingleton<ResourceSingleton>()->init(_spriteSheetId);
    _ecs.getSingleton<MapSingleton>();

    fug::SpriteComponent creatureSpriteComponent(_spriteSheetId, 0);
    creatureSpriteComponent.setOrigin(Vec2f(ConfigSingleton::spriteRadius, ConfigSingleton::spriteRadius));
    fug::SpriteComponent foodSpriteComponent(_spriteSheetId, 1);
    foodSpriteComponent.setOrigin(Vec2f(ConfigSingleton::spriteRadius, ConfigSingleton::spriteRadius));
    foodSpriteComponent.setColor(Vec3f(0.2f, 0.6f, 0.0f));

    auto& config = *_ecs.getSingleton<ConfigSingleton>();
    auto& map = *_ecs.getSingleton<MapSingleton>();

    // Create creatures
    constexpr int nCreatures = 2000;
    for (int i=0; i<nCreatures; ++i) {
        // get position using rejection sampling
        Vec2f p(RNDS*1024.0f, RNDS*1024.0f);
        while (gauss2(p, 256.0f) < RND)
            p << RNDS*1024.0f, RNDS*1024.0f;

        double mass = ConfigSingleton::minCreatureMass + RND*(
            ConfigSingleton::maxCreatureMass-ConfigSingleton::minCreatureMass);

        createCreature(_ecs, Genome(), mass, 1.0, p, RND*M_PI*2.0f, RND);
    }

    // Create food
    constexpr int nFoods = 5000;
    auto foodPositions = map.sampleFertility(nFoods);
    for (auto& p : foodPositions) {
        double mass = RNDRANGE(ConfigSingleton::minFoodMass, ConfigSingleton::maxFoodMass);
        createFood(_ecs, FoodComponent::Type::PLANT, mass, p);
    }
}

void Window::loop(void)
{
    auto& map = *_ecs.getSingleton<MapSingleton>();

    // Application main loop
    while (!_quit) {
        // Event handling
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            handleEvent(event);
        }

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!_paused)
            updateWorld();

        _creatureSystem.setStage(CreatureSystem::Stage::PROCESS_INPUTS);
        _ecs.runSystem(_creatureSystem);

        updateGUI();

        // Render world
        map.render(_viewport);
        _ecs.runSystem(_spriteSystem);
        _ecs.getSingleton<fug::SpriteSingleton>()->render(_viewport);
        _ecs.getSingleton<LineSingleton>()->render(_viewport);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap draw and display buffers
        SDL_GL_SwapWindow(_window);

        if (!_paused) {
            // Map update (GPGPU pass)
            map.diffuseFertility();
        }

        uint32_t curTicks = SDL_GetTicks();
        _frameTicks = curTicks - _lastTicks;
        _lastTicks = curTicks;
    }
}

void Window::handleEvent(SDL_Event& event)
{
    static auto& world = *_ecs.getSingleton<WorldSingleton>();

    switch (event.type) {
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    _quit = true;
                    break;
            }
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    _quit = true;
                    break;
                case SDLK_PAUSE:
                    _paused = !_paused;
                    break;
                case SDLK_F12:
                    SDL_SetWindowFullscreen(_window, SDL_GetWindowFlags(_window) ^ SDL_WINDOW_FULLSCREEN);
                    break;
            }
        case SDL_MOUSEWHEEL:
            _viewport.zoom(std::pow(1.414213562373f, (float)event.wheel.y), _cursorPosition, 0.125f);
            break;
        case SDL_MOUSEMOTION:
            _cursorPosition << (float)event.motion.x, (float)event.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button) {
                case SDL_BUTTON_LEFT: {
                    Vec2f clickWorldPos = _viewport.toWorld(_cursorPosition);
                    static Vec2f maxRadiusVec(
                        ConfigSingleton::maxObjectRadius, ConfigSingleton::maxObjectRadius);
                    // Find the clicked creature (if any)
                    Vector<fug::EntityId> entities;
                    world.getEntities(entities, clickWorldPos-maxRadiusVec, clickWorldPos+maxRadiusVec);
                    for (auto& eId : entities) {
                        if (_ecs.getComponent<FoodComponent>(eId) != nullptr)
                            continue;
                        auto* oc = _ecs.getComponent<fug::Orientation2DComponent>(eId);
                        if ((oc->getPosition()-clickWorldPos).norm() <
                            oc->getScale()*ConfigSingleton::spriteRadius) {
                            _activeCreature = eId;
                            break;
                        }
                    }
                } break;
            }
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

    {   // Main simulation controls
        ImGui::Begin("Simulation Controls");

        auto nCreatures = world.getNumberOf(WorldSingleton::EntityType::CREATURE);
        auto nFood = world.getNumberOf(WorldSingleton::EntityType::FOOD);

        ImGui::Text("N. Creatures: %lu\n", nCreatures);
        ImGui::Text("N. Food: %lu\n", nFood);

        ImGui::Checkbox("Paused", &_paused);

        if (ImGui::CollapsingHeader("Food Controls")) {
            static double foodPerTickMin = 0.001;
            static double foodPerTickMax = 100.0;
            ImGui::SliderScalar("foodPerTick", ImGuiDataType_Double, &config.foodPerTick,
                                &foodPerTickMin, &foodPerTickMax, "%.5f", ImGuiSliderFlags_Logarithmic);

            static double foodGrowthRateMin = 0.0001;
            static double foodGrowthRateMax = 0.1;
            ImGui::SliderScalar("foodGrowthRate", ImGuiDataType_Double, &config.foodGrowthRate,
                                &foodGrowthRateMin, &foodGrowthRateMax, "%.5f", ImGuiSliderFlags_Logarithmic);
        }

        // mutation menu
        if (ImGui::CollapsingHeader("Mutation")) {
            // slider bounds and drop menu item names
            static const float probabilityBounds[2] = { 0.0001f, 1.0f };
            static const float amplitudeBounds[2] = { 0.0001f, 1.0f };
            static const char* modeTitles[] = { "Additive", "Multiplicative" };

            ImGui::Indent();
            int stageId = 0;

            // list all stages
            for (auto stageIt = config.mutationStages.begin(); stageIt < config.mutationStages.end(); ++stageIt) {
                auto& stage = *stageIt;
                std::stringstream stageName;
                stageName << "Stage " << ++stageId;
                bool stageEnabled = true; // the following CollapsingHeader will set this to false to signify stage deletion

                if (ImGui::CollapsingHeader(stageName.str().c_str(), &stageEnabled)) {

                    // element names
                    std::stringstream probabilityName, amplitudeName, modeName;
                    probabilityName << "Probability##evolution" << stageId;
                    amplitudeName << "Amplitude##evolution" << stageId;
                    modeName << "Mode##evolution" << stageId;

                    // sliders for probability and amplitude
                    ImGui::SliderScalar(probabilityName.str().c_str(), ImGuiDataType_Float, &stage.probability,
                                        probabilityBounds, probabilityBounds+1, "%.5f", ImGuiSliderFlags_Logarithmic);

                    ImGui::SliderScalar(amplitudeName.str().c_str(), ImGuiDataType_Float, &stage.amplitude,
                                        amplitudeBounds, amplitudeBounds+1, "%.5f", ImGuiSliderFlags_Logarithmic);

                    // drop menu for the mutation mode
                    const char* currentModeTitle = modeTitles[stage.mode];
                    if (ImGui::BeginCombo(modeName.str().c_str(), currentModeTitle)) {
                        for (int n = 0; n < IM_ARRAYSIZE(modeTitles); n++)
                        {
                            bool isSelected = (currentModeTitle == modeTitles[n]);
                            if (ImGui::Selectable(modeTitles[n], isSelected))
                                stage.mode = static_cast<Genome::MutationMode>(n);
                            if (isSelected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                }

                // stage is deleted
                if (!stageEnabled)
                    stageIt = config.mutationStages.erase(stageIt);
            }

            // button for adding new stages
            if (ImGui::Button("Add Stage")) {
                config.mutationStages.emplace_back(0.1f, 0.1f, Genome::MutationMode::ADDITIVE);
            }

            ImGui::Unindent();
        }

        ImGui::End();
    }

    if (_activeCreature >= 0) {
        // Selected creature controls
        auto* cc = _ecs.getComponent<CreatureComponent>(_activeCreature);
        auto* oc = _ecs.getComponent<fug::Orientation2DComponent>(_activeCreature);
        auto* sc = _ecs.getComponent<fug::SpriteComponent>(_activeCreature);

        if (cc != nullptr && sc != nullptr) {
            ImGui::Begin("Creature");

            ImGui::Text("Creature %lu", _activeCreature);
            ImGui::Text("Energy: %0.5f", cc->energy);

            ImGui::Checkbox("Follow", &_activeCreatureFollow);
            if (_activeCreatureFollow)
                _viewport.centerTo(oc->getPosition());

            Vec3f color = sc->getColor();
            ImGui::ColorPicker3("Creature color", color.data());
            sc->setColor(color);

            ImGui::End();
        }
        else // creature has been removed
            _activeCreature = -1;
    }

}

void Window::updateWorld(void)
{
    static auto& world = *_ecs.getSingleton<WorldSingleton>();
    static auto& config = *_ecs.getSingleton<ConfigSingleton>();
    static auto& map = *_ecs.getSingleton<MapSingleton>();

    // initiate pixel data transfer from GPU
    map.prefetch();

    _creatureSystem.setStage(CreatureSystem::Stage::COGNITION);
    _ecs.runSystem(_creatureSystem);
    _creatureSystem.setStage(CreatureSystem::Stage::DYNAMICS);
    _ecs.runSystem(_creatureSystem);

    // map the pixel data memory
    map.map();

    if (_activeCreature >= 0 && _ecs.getComponent<CreatureComponent>(_activeCreature) == nullptr)
        _activeCreature = -1;
    _creatureSystem.setStage(CreatureSystem::Stage::REPRODUCTION);
    _ecs.runSystem(_creatureSystem);

    {   // Create new food
        static double nNewFood = 0.0;
        nNewFood += config.foodPerTick;
        auto foodPositions = map.sampleFertility((int)nNewFood);
        for (auto& p : foodPositions) {
            createFood(_ecs, FoodComponent::Type::PLANT, ConfigSingleton::minFoodMass, p);
        }
        nNewFood -= (int)nNewFood;
    }

    if (world.getNumberOf(WorldSingleton::EntityType::CREATURE) < 1000) {
        for (int i = 0l; i < 1000; ++i) {   // create a new creatures
            if (RND > 0.0001) continue;

            Vec2f p(RNDS * 1024.0f, RNDS * 1024.0f);
            while (gauss2(p, 256.0f) < RND)
                p << RNDS * 1024.0f, RNDS * 1024.0f;

            double mass = ConfigSingleton::minCreatureMass + RND * (
                ConfigSingleton::maxCreatureMass - ConfigSingleton::minCreatureMass);

            createCreature(_ecs, Genome(1.0f, RNDRANGE(0.001f, 0.005f)),
                mass, 1.0, p, RND * M_PI * 2.0f, RND);
        }
    }

    _foodSystem.setStage(FoodSystem::Stage::GROW);
    _ecs.runSystem(_foodSystem);

    // unmap the pixel data memory
    map.unmap();

    addEntitiesToWorld();

    _ecs.runSystem(_collisionSystem);

    while (_eventSystem.swap())
        _ecs.runSystem(_eventSystem);

    addEntitiesToWorld();
}

void Window::addEntitiesToWorld(void)
{
    _ecs.getSingleton<WorldSingleton>()->reset();

    _creatureSystem.setStage(CreatureSystem::Stage::ADD_TO_WORLD);
    _ecs.runSystem(_creatureSystem);

    _foodSystem.setStage(FoodSystem::Stage::ADD_TO_WORLD);
    _ecs.runSystem(_foodSystem);
}
