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
#include <graphics/SpriteComponent.hpp>


void EventHandler_Creature_CollisionEvent::handleEvent(
    fug::Ecs& ecs, const fug::EntityId& eId, const CollisionEvent& event)
{
    static auto& config = *ecs.getSingleton<ConfigSingleton>();

    auto& cc1 = *ecs.getComponent<CreatureComponent>(eId);
    auto& oc1 = *ecs.getComponent<fug::Orientation2DComponent>(eId);
    auto& oc2 = *ecs.getComponent<fug::Orientation2DComponent>(event.entityId);

    auto* cc2 = ecs.getComponent<CreatureComponent>(event.entityId);
    auto* fc2 = ecs.getComponent<FoodComponent>(event.entityId);

    // push vector from the other entity
    Vec2f pd = oc1.getPosition()-oc2.getPosition();
    Vec2f pv = -pd+pd.normalized()*(oc1.getScale() + oc2.getScale())*ConfigSingleton::spriteRadius;

    if (ecs.getComponent<CreatureComponent>(event.entityId) != nullptr) {
        // collision object is another creature
        auto& m1 = cc1._mass;
        auto& m2 = cc2->_mass;
        double massSum = m1+m2;

        // direction reflection
        oc1.translate(pv*(m2/massSum));

        // attack
        CreatureCognition::AttackInput attackInput = CreatureCognition::AttackInput::Zero();
        auto* sc2 = ecs.getComponent<fug::SpriteComponent>(event.entityId);
        attackInput.block<3,1>(0,0) = sc2->getColor();
        attackInput(3) = (float)cc2->_mass;
        attackInput(4) = (float)cc1._mass;
        attackInput(5) = (float)(cc1._energy / config.massEnergyStorageConstant);

        auto attackOutput = cc1._cognition.attack(attackInput);
        double damage = (attackOutput(0)+1.0f)*0.5*cc1._energy;
        double damageMassFactor = cc1._mass / cc2->_mass;
        cc2->_energy -= damage*damageMassFactor;
        cc1._energy -= damage;
    }
    else if (fc2 != nullptr) {// collision object is food
        double feedMass = cc1._mass*config.creatureFeedRate;
        if (feedMass >= fc2->mass) { // food gets completely eaten
            feedMass = fc2->mass;
            ecs.removeEntity(event.entityId);
        }
        else { // food gets partially eaten
            fc2->mass -= feedMass;
            oc2.setScale(sqrtf((float)fc2->mass) / ConfigSingleton::spriteRadius);
            oc1.translate(pv);
        }

        // dMass is the amount of food mass that is to be converted to creature mass, rest becomes energy
        double dMass = std::min(cc1._genome[Genome::CREATURE_SIZE]-cc1._mass,
            feedMass*config.creatureMassIncreaseFactor);
        cc1._mass += dMass;

        double energyConstant = 0.0;
        switch (fc2->type) {
            case FoodComponent::Type::PLANT:
                energyConstant = config.foodPlantMassToEnergyConstant;
                break;
            case FoodComponent::Type::MEAT:
                energyConstant = config.foodMeatMassToEnergyConstant;
                break;
        }
        cc1._energy += (feedMass - dMass)*energyConstant; // rest of the food mass becomes energy

        // cannot store more energy, energy is wasted
        if (cc1._energy > config.massEnergyStorageConstant*cc1._mass)
            cc1._energy = config.massEnergyStorageConstant*cc1._mass;

        // update the radius
        oc1.setScale(sqrtf((float)cc1._mass) / ConfigSingleton::spriteRadius);
    }
}
