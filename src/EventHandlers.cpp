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
#include <ConfigSingleton.hpp>
#include <ecs/Ecs.hpp>
#include <graphics/Orientation2DComponent.hpp>


void EventHandler_Creature_CollisionEvent::handleEvent(
    fug::Ecs& ecs, const fug::EntityId& eId, const CollisionEvent& event)
{
    static auto& config = *ecs.getSingleton<ConfigSingleton>();

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
        float rSum = (oc1.getScale() + oc2.getScale())*ConfigSingleton::spriteRadius;
        if (dpNorm < rSum)
            oc1.translate(n * (rSum-dpNorm) * 0.5f);
    }
    else if (ecs.getComponent<FoodComponent>(event.entityId) != nullptr) {
        // collision object is food
        float foodMass = 1.0f; // TODO make food mass a property of FoodComponent

        // dMass is the amount of food mass that is to be converted to creature mass
        float dMass = std::min(cc._genome[3]-cc._mass, foodMass*config.creatureMassIncreaseFactor);
        cc._mass += dMass;
        cc._energy += (foodMass - dMass)*config.foodMassToEnergyConstant; // rest of the food mass becomes energy

        // cannot store more energy, energy is wasted
        if (cc._energy > config.massEnergyStorageConstant*cc._mass)
            cc._energy = config.massEnergyStorageConstant*cc._mass;

        // update the radius
        oc1.setScale(sqrtf(cc._mass) / 64.0f);

        // remove the food
        ecs.removeEntity(event.entityId);
    }
}
