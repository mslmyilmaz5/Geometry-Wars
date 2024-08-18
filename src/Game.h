#pragma once

#include "Entity.h"
#include "EntityManager.h"
#include <SFML/Graphics.hpp>


struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S;};
struct EnemyConfig  { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX;};
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S;};
struct FontConfig   { std::string FS; int FW, R, G, B;};

class Game
{   
    sf::Clock               m_sw_cooldown;
    sf::RenderWindow        m_window;
    EntityManager           m_entities;
    sf::Font                m_font;
    sf::Text                m_text;
    PlayerConfig            m_playerConfig;
    EnemyConfig             m_enemyConfig;
    BulletConfig            m_bulletConfig;
    FontConfig              m_fontConfig;
    int                     m_score               = 0;
    int                     m_currentFrame        = 0;
    int                     m_lastEnemySpawnTime  = 0;
    bool                    m_paused  = false; 
    bool                    m_running = true;  
    bool                    m_weaponCooldownClock = false;
    std::shared_ptr<Entity> m_player;
    void init(const std::string & config); // initialize the gamestate with a config file path
    void setPaused(bool paused);
    
    // sytems
    void sMovement();
    void sUserInput();
    void sLifeSpan();
    void sRender();
    void sEnemySpawner();
    void sCollision();
    
    void setBoard();
    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> entity);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2 & mousePos);
    void spawnSpecialWeapon();


public:

    Game(const std::string & config); 
    void run();

};

