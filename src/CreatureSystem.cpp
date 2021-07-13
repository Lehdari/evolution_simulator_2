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
    auto& g = creatureComponent._genome;

    if (_cognitionOutputs.size() < eId+1)
        _cognitionOutputs.resize(eId+1);

    _cognitionOutputs[eId] <<
        (float)(RNDS*g[Genome::ACCELERATION_RANDOMNESS])+g[Genome::ACCELERATION_BIAS],
        RNDS*g[Genome::DIRECTION_RANDOMNESS];
}

void CreatureSystem::dynamics(
    const fug::EntityId& eId,
    CreatureComponent& creatureComponent,
    fug::Orientation2DComponent& orientationComponent)
{
    static auto& config = *_ecs.getSingleton<ConfigSingleton>();

    // some shorthands for the creature variables
    auto& g = creatureComponent._genome;
    auto& e = creatureComponent._energy;
    auto& s = creatureComponent._speed;
    auto& d = creatureComponent._direction;
    auto& m = creatureComponent._mass;

    // creature dynamics
    float drag = std::clamp(s*s*config.creatureDragCoefficient, 0.0f, s);
    s -= std::copysignf(drag, s); // drag

    float a = _cognitionOutputs[eId](0); // acceleration
    s += a;
    e -= abs(a)*m*config.creatureMovementEnergyUseConstant; // acceleration energy usage

    float da = _cognitionOutputs[eId](1); // direction change
    d += da*(float)M_PI_2;
    e -= abs(da)*m*config.creatureMovementEnergyUseConstant; // direction change energy usage

    // constant energy usage, relative to sqrt of mass
    e -= config.creatureEnergyUseConstant*sqrt(m);

    // if energy reaches 0, the creature dies
    if (e <= 0.0) {
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
    auto& g = creatureComponent._genome;
    auto& e = creatureComponent._energy;
    auto& s = creatureComponent._speed;
    auto& d = creatureComponent._direction;
    auto& m = creatureComponent._mass;

    // reproduction
    double minChildEnergy = ConfigSingleton::minCreatureMass*config.massEnergyStorageConstant;
    if (e > minChildEnergy && RND < g[Genome::REPRODUCTION_PROBABILITY]) {
        double childSize = g[Genome::CHILD_SIZE_MIN]+std::max(0.0,
            RND*(g[Genome::CHILD_SIZE_MAX]-g[Genome::CHILD_SIZE_MIN]));
        double childEnergy = minChildEnergy + childSize*(e-minChildEnergy);
        double childMass = childEnergy/config.massEnergyStorageConstant;

        Genome childGenome = g;
        childGenome.mutate(g[Genome::MUTATION_PROBABILITY_1], g[Genome::MUTATION_AMPLITUDE_1]);
        childGenome.mutate(g[Genome::MUTATION_PROBABILITY_2], g[Genome::MUTATION_AMPLITUDE_2]);

        fug::EntityId childId = _ecs.getEmptyEntityId();

        // Child components
        CreatureComponent childCreatureComponent = CreatureComponent(
            childGenome, childMass*config.massEnergyStorageConstant, childMass,
            creatureComponent._direction, creatureComponent._speed);

        fug::Orientation2DComponent childOrientationComponent = orientationComponent;
        childOrientationComponent.setScale(sqrtf(childMass) / ConfigSingleton::spriteRadius);
        float cpr = ConfigSingleton::spriteRadius *
            (orientationComponent.getScale() + childOrientationComponent.getScale());
        float cpd = RND*M_PI*2.0f;
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
    auto& g = creatureComponent._genome;
    auto& e = creatureComponent._energy;
    auto& s = creatureComponent._speed;
    auto& d = creatureComponent._direction;
    auto& m = creatureComponent._mass;

    // whisker
    Vec2f dVec = Vec2f(cosf(d), sinf(d)); // direction vector
    auto& p = orientationComponent.getPosition(); // creature position
    float r = orientationComponent.getScale()*ConfigSingleton::spriteRadius; // creature radius
    Vec2f wBegin = p+r*dVec;
    Vec2f wEnd = p+(r+10.0f)*dVec; // TODO variable whisker length
    Vec2f wd = wEnd-wBegin;

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

    // search for (closest) contact
    float t = 1.0f; // t stores ray length to the nearest contact
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
        float wa = wd(0)*wd(0) + wd(1)*wd(1);
        float wb = 2.0f*(wd(0)*wbp(0) + wd(1)*wbp(1));
        float wc = wbp(0)*wbp(0) + wbp(1)*wbp(1) - wr*wr;

        // no intersection when determinant is < 0
        float wDet = wb*wb - 4.0f*wa*wc;
        if (wDet < 0.0f)
            continue;

        // solve quadratic (only negative used since it's always the closer one)
        float tCand = (-wb-sqrtf(wDet))/(2.0f*wa);
        if (tCand < 0.0f || tCand >= t) // skip if contact is behind (<0) or further away than the current
            continue;

        // closer contact found
        t = tCand;
        cColor = _ecs.getComponent<fug::SpriteComponent>(wEId)->getColor();
    }

    lineSingleton.drawLine(wBegin, wBegin + wd*t, color, cColor);
}
