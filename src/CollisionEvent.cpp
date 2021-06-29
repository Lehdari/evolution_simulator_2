//
// Project: evolution_simulator_2
// File: CollisionEvent.cpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#include <CollisionEvent.hpp>
#include <utility>


FUG_DEFINE_EVENT_TEMPLATES(CollisionEvent);


CollisionEvent::CollisionEvent(fug::EntityId entityId) :
    entityId    (std::move(entityId))
{
}
