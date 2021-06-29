//
// Project: evolution_simulator_2
// File: CollisionEvent.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_COLLISIONEVENT_HPP
#define EVOLUTION_SIMULATOR_2_COLLISIONEVENT_HPP


#include <utils/Types.hpp>
#include <engine/EventTypeId.hpp>


struct CollisionEvent {
    fug::EntityId   entityId; // ID of entity the collision occurred with

    explicit CollisionEvent(fug::EntityId entityId);
};


#endif //EVOLUTION_SIMULATOR_2_COLLISIONEVENT_HPP
