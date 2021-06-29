//
// Project: evolution_simulator_2
// File: EventHandlers.hpp
//
// Copyright (c) 2021 Miika 'Lehdari' Lehtimäki
// You may use, distribute and modify this code under the terms
// of the licence specified in file LICENSE which is distributed
// with this source code package.
//

#ifndef EVOLUTION_SIMULATOR_2_EVENTHANDLERS_HPP
#define EVOLUTION_SIMULATOR_2_EVENTHANDLERS_HPP


#include <engine/EventHandler.hpp>
#include <CollisionEvent.hpp>


FUG_DEFINE_EVENT_TEMPLATES(CollisionEvent);

FUG_EVENT_HANDLER(EventHandler_Creature_CollisionEvent, CollisionEvent);


#endif //EVOLUTION_SIMULATOR_2_EVENTHANDLERS_HPP
