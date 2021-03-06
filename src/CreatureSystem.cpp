//
// Project: evolution_simulator_2
// File: CreatureSystem.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include <CreatureSystem.hpp>
#include <WorldSingleton.hpp>
#include <ConfigSingleton.hpp>
#include <LineSingleton.hpp>
#include <EventHandlers.hpp>
#include <Utils.hpp>
#include <FoodComponent.hpp>

#include <engine/EventComponent.hpp>
#include <graphics/SpriteComponent.hpp>


CreatureSystem::CreatureSystem(fug::Ecs& ecs) :
    _ecs    (ecs),
    _stage  (Stage::DYNAMICS)
{
}

void CreatureSystem::setStage(CreatureSystem::Stage stage)
{
    _stage = stage;
}

void CreatureSystem::operator()(
    const fug::EntityId& eId,
    CreatureComponent& creatureComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    switch (_stage) {
        case Stage::COGNITION:
            cognition(eId, creatureComponent, orientationComponent);
            break;
        case Stage::DYNAMICS:
            dynamics(eId, creatureComponent, orientationComponent);
            break;
        case Stage::REPRODUCTION:
            reproduction(eId, creatureComponent, orientationComponent);
            break;
        case Stage::ADD_TO_WORLD:
            addToworld(eId, creatureComponent, orientationComponent);
            break;
        case Stage::PROCESS_INPUTS:
            processInputs(eId, creatureComponent, orientationComponent);
            break;
    }
}

void CreatureSystem::cognition(
    const fug::EntityId& eId,
    CreatureComponent& creatureComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    // input bias term
    creatureComponent.cognition.forward();
}

void CreatureSystem::dynamics(
    const fug::EntityId& eId,
    CreatureComponent& creatureComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    static auto& config = *_ecs.getSingleton<ConfigSingleton>();

    // some shorthands for the creature variables
    auto& g = creatureComponent.genome;
    auto& e = creatureComponent.energy;
    auto& s = creatureComponent.speed;
    auto& d = creatureComponent.direction;
    auto& m = creatureComponent.mass;
    float r = orientationComponent.getScale()*ConfigSingleton::spriteRadius;

    auto& cognitionOutput = creatureComponent.cognition._output;

    creatureComponent.age += 1.0;
    // aging simulated as increased energy use over time. inversely proportional to mass
    creatureComponent.agingFactor = pow(1.000077, creatureComponent.age / m);

    float a = cognitionOutput(0); // acceleration
    s += a*r;
    e -= abs(a)*m*config.creatureAccelerationEnergyUseConstant*creatureComponent.agingFactor; // acceleration energy usage

    float drag = std::clamp(s*s, 0.0f, s);
    s -= std::copysignf(drag, s); // drag
    s = std::clamp(s, -r, r);

    float da = cognitionOutput(1); // direction change
    d += da*(float)M_PI_4;
    e -= da*da*m*config.creatureTurnEnergyUseConstant*creatureComponent.agingFactor; // direction change energy usage

    // constant energy usage, relative to sqrt of mass
    e -= config.creatureEnergyUseConstant*sqrt(m);

    // if energy reaches 0, the creature dies
    if (e <= 0.0) {
        createFood(_ecs, FoodComponent::Type::MEAT, m, orientationComponent.getPosition());
        _ecs.removeEntity(eId);
        return;
    }

    // update orientation component
    Vec2f dVec = Vec2f(cosf(d), sinf(d));
    Vec2f p = orientationComponent.getPosition() + s*dVec;
    if (p(0) < -ConfigSingleton::worldSize) p(0) = -ConfigSingleton::worldSize;
    if (p(0) >= ConfigSingleton::worldSize) p(0) = ConfigSingleton::worldSize;
    if (p(1) < -ConfigSingleton::worldSize) p(1) = -ConfigSingleton::worldSize;
    if (p(1) >= ConfigSingleton::worldSize) p(1) = ConfigSingleton::worldSize;
    orientationComponent.setPosition(p);
    orientationComponent.setRotation(d);
}

void CreatureSystem::reproduction(
    const fug::EntityId& eId,
    CreatureComponent& creatureComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    static auto& config = *_ecs.getSingleton<ConfigSingleton>();

    // some shorthands for the creature variables
    auto& g = creatureComponent.genome;
    auto& e = creatureComponent.energy;
    auto& s = creatureComponent.speed;
    auto& d = creatureComponent.direction;
    auto& m = creatureComponent.mass;

    auto& cognitionOutput = creatureComponent.cognition._output;

    // energy required for production of an unit of mass
    double reproductionEnergyConstant = config.massEnergyStorageConstant*g[Genome::CHILD_ENERGY]+
        config.foodMeatMassToEnergyConstant;
    double minChildEnergy = ConfigSingleton::minCreatureMass*reproductionEnergyConstant;

    if (e > minChildEnergy && RND*10.0f < (cognitionOutput(2)+1.0f)*0.5f) {
        double childSize = g[Genome::CHILD_SIZE_MIN]+std::max(0.0,
            RND*(g[Genome::CHILD_SIZE_MAX]-g[Genome::CHILD_SIZE_MIN]));
        double childEnergy = minChildEnergy + childSize*(e-minChildEnergy);
        double childMass = childEnergy/reproductionEnergyConstant;

        Genome childGenome = g;
        for (auto& stage : config.mutationStages)
            childGenome.mutate(stage.probability, stage.amplitude, stage.mode);

        fug::EntityId childId = _ecs.getEmptyEntityId();

        // Child components
        CreatureComponent childCreatureComponent = CreatureComponent(
            childGenome, childMass*config.massEnergyStorageConstant*g[Genome::CHILD_ENERGY], childMass,
            creatureComponent.direction, creatureComponent.speed);

        fug::Orientation2DComponent childOrientationComponent = orientationComponent;
        childOrientationComponent.setScale(sqrtf(childMass) / ConfigSingleton::spriteRadius);
        // Birth child either on the left or on the right side
        float cpr = ConfigSingleton::spriteRadius * 1.05f *
            (orientationComponent.getScale() + childOrientationComponent.getScale());
        float cpd = RND < 0.5 ? d + M_PI_2 : d - M_PI_2;
        childOrientationComponent.translate(Vec2f(cpr*cosf(cpd), cpr*sinf(cpd)));

        fug::SpriteComponent childSpriteComponent = *_ecs.getComponent<fug::SpriteComponent>(eId);
        childSpriteComponent.setColor(Vec3f(
            childGenome[Genome::COLOR_R], childGenome[Genome::COLOR_G], childGenome[Genome::COLOR_B]));

        fug::EventComponent childEventComponent;
        childEventComponent.addHandler<EventHandler_Creature_CollisionEvent>();

        _ecs.setComponent(childId, std::move(childCreatureComponent));
        _ecs.setComponent(childId, std::move(childOrientationComponent));
        _ecs.setComponent(childId, std::move(childSpriteComponent));
        _ecs.setComponent(childId, std::move(childEventComponent));

        // reduce parent's energy by the amount given to child
        e -= childEnergy;
    }
}

void CreatureSystem::addToworld(
    const fug::EntityId& eId,
    CreatureComponent& creatureComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    static auto& config = *_ecs.getSingleton<ConfigSingleton>();
    static auto& world = *_ecs.getSingleton<WorldSingleton>();

    // add entity to the world singleton
    world.addEntity(eId, orientationComponent.getPosition(), WorldSingleton::EntityType::CREATURE);
}

void CreatureSystem::processInputs(
    const fug::EntityId& eId,
    CreatureComponent& creatureComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    static auto& config = *_ecs.getSingleton<ConfigSingleton>();
    static auto& world = *_ecs.getSingleton<WorldSingleton>();
    static auto& lineSingleton = *_ecs.getSingleton<LineSingleton>();

    // some shorthands for the creature variables
    auto& g = creatureComponent.genome;
    auto& e = creatureComponent.energy;
    auto& s = creatureComponent.speed;
    auto& d = creatureComponent.direction;
    auto& m = creatureComponent.mass;

    // whisker
    float r = orientationComponent.getScale()*ConfigSingleton::spriteRadius; // creature radius
    float t = 5.0f+5.0f*r; // whisker length
    float wd = d; // whisker direction
    Vec2f wv = Vec2f(cosf(wd), sinf(wd)); // direction vector
    auto& p = orientationComponent.getPosition(); // creature position
    Vec2f wBegin = p+r*wv;
    Vec2f wEnd = p+(r+t)*wv; // TODO variable whisker length

    // fetch potential contacts
    static Vector<fug::EntityId> wEntities;
    wEntities.clear();
    world.getEntities(wEntities,
        Vec2f(std::min(wBegin(0), wEnd(0))-ConfigSingleton::maxObjectRadius,
        std::min(wBegin(1), wEnd(1))-ConfigSingleton::maxObjectRadius),
        Vec2f(std::max(wBegin(0), wEnd(0))+ConfigSingleton::maxObjectRadius,
        std::max(wBegin(1), wEnd(1))+ConfigSingleton::maxObjectRadius));

    auto& color = _ecs.getComponent<fug::SpriteComponent>(eId)->getColor();
    auto cColor = color; // in case of contact, color end of whisker with the contact entity color

    // search for (closest) contact, t stores ray length to the nearest contact (so far)
    fug::EntityId cEId = -1; // contact entity ID
    for (auto& wEId : wEntities) {
        if (eId == wEId) // do not self-collide
            continue;

        // position and radius of potential contact
        auto* woc = _ecs.getComponent<fug::Orientation2DComponent>(wEId);
        Vec2f wp = woc->getPosition();
        float wr = woc->getScale()*ConfigSingleton::spriteRadius;

        // helper vector required in lot of subsequent quadratic term calculations
        Vec2f wbp = wBegin-wp;

        // quadratic terms of 2D ray-circle intersection
        float wa = wv(0)*wv(0) + wv(1)*wv(1);
        float wb = 2.0f*(wv(0)*wbp(0) + wv(1)*wbp(1));
        float wc = wbp(0)*wbp(0) + wbp(1)*wbp(1) - wr*wr;

        // no intersection when determinant is < 0
        float wDet = wb*wb - 4.0f*wa*wc;
        if (wDet < 0.0f)
            continue;

        // solve quadratic (only negative used since it's always the closer one)
        float tCand = (-wb-sqrtf(wDet))/(2.0f*wa);
        if (tCand < -r || tCand >= t) // skip if contact is behind or further away than the current closest
            continue;

        // closer contact found
        t = tCand;
        cColor = _ecs.getComponent<fug::SpriteComponent>(wEId)->getColor();
        cEId = wEId;
    }

    if (t < 0.0f)
        t = 0.0f;

    auto& cognitionInput = creatureComponent.cognition._input;
    cognitionInput = CreatureCognition::Input::Zero();
    cognitionInput(0) = (float)m;
    cognitionInput(1) = (float)(e / config.massEnergyStorageConstant);
    cognitionInput(2) = s;
    cognitionInput(3) = (float)creatureComponent.agingFactor;
    cognitionInput(4) = t;
    if (cEId >= 0) {
        auto* ccc = _ecs.getComponent<CreatureComponent>(cEId);
        auto* cfc = _ecs.getComponent<FoodComponent>(cEId);

        if (ccc == nullptr) {
            // contact is food
            cognitionInput(5) = 1.0f;
            cognitionInput(7) = (float)cfc->mass;
        }
        else {
            // contact is creature
            cognitionInput(6) = 1.0f;
            cognitionInput(7) = (float)ccc->mass;
        }

        cognitionInput.block<3,1>(8,0) = cColor;
    }

    lineSingleton.drawLine(wBegin, wBegin + wv*t, color, cColor);
}
