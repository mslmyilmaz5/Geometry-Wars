#include "Entity.h"

Entity::Entity(const size_t id, const std::string & e_tag)
    : m_id(id)
    , m_tag(e_tag)
{  
}


bool Entity::isActive() const
{
    return m_active;
}

const std::string &Entity::tag() const
{
    return m_tag;
}

size_t Entity::id() const
{
    return m_id;
}

void Entity::destroy()
{
    m_active = false;
}
