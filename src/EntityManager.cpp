#include "EntityManager.h"



EntityManager::EntityManager()
    : m_totalEntities(0)
{
}

void EntityManager::update()
{
   for (auto  & e: m_entitiesToAdd) 
   {    

        m_entityMap[e->m_tag].push_back(e);
        m_entities.push_back(e);
   }
   

   removeDeadEntities(m_entities);
   
  
   for (auto & [tag, entityVec] : m_entityMap)
   {
        removeDeadEntities(entityVec);
   }

    m_entitiesToAdd.clear();

    
}


// sıkıntı olabilir!
void EntityManager::removeDeadEntities(EntityVec &vec)
{  
    
   vec.erase(std::remove_if(vec.begin(), vec.end(),
        [](const auto& entity) {
            return !entity->isActive();
        }), vec.end()); 
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag)
{   
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++,tag));
    m_entitiesToAdd.push_back(entity);
    return entity;
}

const EntityVec & EntityManager::getEntities()
{
    return m_entities;
}

const EntityVec & EntityManager::getEntities(const std::string &tag)
{
    return m_entityMap[tag];
}
