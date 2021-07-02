//
// Project: evolution_simulator_2
// File: EventHandlers.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include <EventHandlers.hpp>
#include <CreatureComponent.hpp>
#include <FoodComponent.hpp>
#include <ecs/Ecs.hpp>
#include <graphics/Orientation2DComponent.hpp>


void EventHandler_Creature_CollisionEvent::handleEvent(
    fug::Ecs& ecs, const fug::EntityId& eId, const CollisionEvent& event)
{
    // TODO move these constants to simulation config
    constexpr float spriteInvScale = 64.0f; // radius in sprite pixels
    constexpr float maxCreatureMass = 16.0f;
    constexpr float creatureMassIncreaseFactor = 0.25f; // factor by which mass is sustained when eating food
    constexpr float maxEnergyMassConstant = 100.0f; // how much energy each creature can hold w.r.t. their mass
    constexpr float foodMassToEnergyConstant = 10.0f; // ratio by which food mass in converted to creature energy

    auto& cc = *ecs.getComponent<CreatureComponent>(eId);
    auto& oc1 = *ecs.getComponent<fug::Orientation2DComponent>(eId);

    if (ecs.getComponent<CreatureComponent>(event.entityId) != nullptr) {
        // collision object is another creature
        auto& oc2 = *ecs.getComponent<fug::Orientation2DComponent>(event.entityId);

        // direction reflection
        Vec2f dp = oc1.getPosition()-oc2.getPosition();
        float dpNorm = dp.norm();
        Vec2f n = dp / dpNorm;
        Vec2f d(cosf(cc._direction), sinf(cc._direction));
        Vec2f r = d - 2.0f * (d.dot(n)) * n;
        cc._direction = atan2f(r(1), r(0));

        // unstuck entities inside one another
        float rSum = (oc1.getScale() + oc2.getScale())*spriteInvScale;
        if (dpNorm < rSum)
            oc1.translate(n * (rSum-dpNorm) * 0.5f);
    }
    else if (ecs.getComponent<FoodComponent>(event.entityId) != nullptr) {
        // collision object is food
        float foodMass = 1.0f; // TODO make food mass a property of FoodComponent

        // dMass is the amount of food mass that is to be converted to creature mass
        float dMass = std::min(cc._genome[3]-cc._mass, foodMass*creatureMassIncreaseFactor);
        cc._mass += dMass;
        cc._energy += (foodMass - dMass)*foodMassToEnergyConstant; // rest of the food mass becomes energy

        // cannot grow larger, the food is wasted
        if (cc._mass >= maxCreatureMass)
            cc._mass = maxCreatureMass;

        // cannot store more energy, energy is wasted
        if (cc._energy > maxEnergyMassConstant*cc._mass)
            cc._energy = maxEnergyMassConstant*cc._mass;

        // update the radius
        oc1.setScale(sqrtf(cc._mass) / 64.0f);

        // remove the food
        ecs.removeEntity(event.entityId);
    }
}
