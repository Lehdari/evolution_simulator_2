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
    _ecs    (ecs)
{
}

void CreatureSystem::operator()(
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

    float a = (float)(RNDS*g[Genome::ACCELERATION_RANDOMNESS])+g[Genome::ACCELERATION_BIAS]; // acceleration
    s += a;
    e -= abs(a)*m*config.creatureMovementEnergyUseConstant; // acceleration energy usage

    float da = RNDS*g[Genome::DIRECTION_RANDOMNESS]; // direction change
    d += da*(float)M_PI_2;
    e -= abs(da)*m*config.creatureMovementEnergyUseConstant; // direction change energy usage

    // constant energy usage, relative to sqrt of mass
    e -= config.creatureEnergyUseConstant*sqrt(m);

    // reproduction
    double minChildEnergy = ConfigSingleton::minCreatureMass*config.massEnergyStorageConstant;
    if (e > minChildEnergy && RND > g[Genome::REPRODUCTION_PROBABILITY]) {
        double childSize = g[Genome::CHILD_SIZE_MIN]+std::max(0.0,
            RND*(g[Genome::CHILD_SIZE_MAX]-g[Genome::CHILD_SIZE_MIN]));
        double childEnergy = minChildEnergy + childSize*(e-minChildEnergy);
        double childMass = childEnergy/config.massEnergyStorageConstant;

        Genome childGenome = g;
        childGenome.mutate(0.5f, 0.001f);
        childGenome.mutate(0.1f, 0.01f);
        childGenome.mutate(0.01f, 0.1f);

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

    // add entity to the world singleton
    auto* world = _ecs.getSingleton<WorldSingleton>();
    world->addEntity(eId, orientationComponent.getPosition(), WorldSingleton::EntityType::CREATURE);

    auto* lineSingleton = _ecs.getSingleton<LineSingleton>();
    float r = orientationComponent.getScale()*ConfigSingleton::spriteRadius;
    lineSingleton->drawLine(p+r*dVec, p+(r+3.0f)*dVec);
}
