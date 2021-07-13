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

    auto& cc1 = *ecs.getComponent<CreatureComponent>(eId);
    auto& oc1 = *ecs.getComponent<fug::Orientation2DComponent>(eId);

    auto* cc2 = ecs.getComponent<CreatureComponent>(event.entityId);
    auto* fc2 = ecs.getComponent<FoodComponent>(event.entityId);

    if (ecs.getComponent<CreatureComponent>(event.entityId) != nullptr) {
        // collision object is another creature
        auto& oc2 = *ecs.getComponent<fug::Orientation2DComponent>(event.entityId);
        auto& m1 = cc1._mass;
        auto& m2 = cc2->_mass;
        double massSum = m1+m2;

        // direction reflection
        Vec2f d = oc1.getPosition()-oc2.getPosition();
        Vec2f pv = -d+d.normalized()*(oc1.getScale() + oc2.getScale())*ConfigSingleton::spriteRadius;

        oc1.translate(pv*(m2/massSum));
    }
    else if (fc2 != nullptr) {// collision object is food
        // dMass is the amount of food mass that is to be converted to creature mass
        double dMass = std::min(cc1._genome[Genome::CREATURE_SIZE]-cc1._mass,
            fc2->mass*config.creatureMassIncreaseFactor);
        cc1._mass += dMass;
        cc1._energy += (fc2->mass - dMass)*config.foodMassToEnergyConstant; // rest of the food mass becomes energy

        // cannot store more energy, energy is wasted
        if (cc1._energy > config.massEnergyStorageConstant*cc1._mass)
            cc1._energy = config.massEnergyStorageConstant*cc1._mass;

        // update the radius
        oc1.setScale(sqrtf(cc1._mass) / 64.0f);

        // remove the food
        ecs.removeEntity(event.entityId);
    }
}
