#pragma once

#include <memory>
#include <string>
#include "Components.h" 

class Entity
{
    friend class EntityManager; //

    bool            m_active = true; // for iterator invalidation
    size_t          m_id     = 0; // has id.
    std::string     m_tag    = "default"; // each entity has tag.

    Entity(const size_t id , const std::string& tag); // private constructor!

public:
    
    // component pointers;
    std::shared_ptr<CTransform>     cTransform;
    std::shared_ptr<CShape>         cShape;
    std::shared_ptr<CColision>      cColision;
    std::shared_ptr<CInput>         cInput;
    std::shared_ptr<CScore>         cScore;
    std::shared_ptr<CLifespan>      cLifespan;

    // private member access functions
    bool isActive() const;
    const std::string & tag() const;
    size_t id() const;
    void destroy();
};