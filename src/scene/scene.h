#ifndef SCENE_H
#define SCENE_H

#include <entt/entity/registry.hpp>

#include "components.h"

class Scene 
{
public:
    Scene() = default;
    
    entt::registry& getRegistry()
    {
        return m_registry;
    }

    entt::entity findEntityByName(const std::string& name)
    {
        auto view = m_registry.view<NameComponent>();
        for(auto entity : view) 
        {
            if(view.get<NameComponent>(entity).name == name) 
            {
                return entity;
            }
        }
        return entt::null; // Return null entity if not found
    }

    entt::entity getActiveCamera() const {
        auto view = m_registry.view<CameraComponent>();
        for(auto entity : view) 
        {
            if(view.get<CameraComponent>(entity).active) 
            {
                return entity;
            }
        }
        return entt::null;
    }

private:
    entt::registry m_registry;
};

#endif // SCENE_H