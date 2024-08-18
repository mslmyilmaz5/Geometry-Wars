#include "Game.h"
#include <iostream>
#include <fstream>
#include <math.h>

// publics

Game::Game(const std::string & config)
{ 
    
    init(config);
}

void Game::init(const std::string & path)
{
    
    srand(time(0));

    try 
    {   
        std::string type,fontfile;
        int w,h,fl,f,font_size,r,g,b;
        std::ifstream fin(path);

        while (fin >> type)
        {
            if (type == "Window")
            {
                fin >> w >> h >> fl >> f;
            }
            else if (type == "Font")
            {
                fin >> m_fontConfig.FS >> m_fontConfig.FW >> m_fontConfig.R >> m_fontConfig.G >> m_fontConfig.B;
            }
            else if (type == "Player")
            {
                fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S
                    >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB
                    >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB
                    >> m_playerConfig.OT >> m_playerConfig.V;
            }
            else if (type == "Enemy")
            {
                fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN
                    >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG
                    >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN
                    >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
            }
            else
            {
                fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S
                    >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB
                    >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB
                    >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
            }
        }

        if      (f == 1) m_window.create(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height), "Game2", sf::Style::Fullscreen);
        else    m_window.create(sf::VideoMode(w,h),"Game2");
        m_window.setFramerateLimit(fl);
        spawnPlayer();
        setBoard();
    }
    catch (const std::exception & e)
    {
        std::cout << "Exception " << e.what() << std::endl;
    }
    
}

void Game::run()
{   
   
    while (m_running)
    {
        m_entities.update();
        if (m_paused)
        {
            sUserInput();
            sRender();
        } 
        else 
        {
            sLifeSpan();
            sEnemySpawner();
            sMovement();
            sCollision();
            sUserInput();
            sRender();
          
        }
        m_currentFrame++;
     
    }
}



void Game::spawnPlayer()
{   
    auto entity         = m_entities.addEntity("player");
    entity->cTransform  = std::make_shared<CTransform>(Vec2(m_window.getSize().x / 2, m_window.getSize().y / 2),Vec2(0.0f,0.0f), 0.0f);
    entity->cShape      = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V
                        , sf::Color(m_playerConfig.FR,m_playerConfig.FG,m_playerConfig.FB)
                        , sf::Color(m_playerConfig.OR,m_playerConfig.OG,m_playerConfig.OB), m_playerConfig.OT);
    entity->cInput      = std::make_shared<CInput>();
    entity->cColision   = std::make_shared<CColision>(m_playerConfig.CR);

    m_player = entity;
    
}

void Game::spawnEnemy()
{
    

    float     random_x            = std::rand() % ( 1 + m_window.getSize().x - m_enemyConfig.SR);
    float     random_y            = std::rand() % ( 1 + m_window.getSize().y - m_enemyConfig.SR);
    float     random_speed        = m_enemyConfig.SMIN + ( std::rand() % int((1 + m_enemyConfig.SMAX - m_enemyConfig.SMIN)));
    float     random_angle        = 0 + (std::rand() % 361);
    float     angle_rad           = random_angle  * (3.14159265358979323846f / 180.0f);
    
    float     random_x_direction  = random_speed * std::cos(angle_rad);
    float     random_y_direction  = random_speed * std::sin(angle_rad);

    int       random_vertices     = m_enemyConfig.VMIN + ( std::rand() % (1 + m_enemyConfig.VMAX - m_enemyConfig.VMIN));
    int       random_r            = 0 + ( std::rand() % 256);
    int       random_g            = 0 + ( std::rand() % 256);
    int       random_b            = 0 + ( std::rand() % 256);

    auto entity                   = m_entities.addEntity("enemy");
    entity->cTransform            = std::make_shared<CTransform>(Vec2(random_x,random_y), Vec2(random_x_direction,random_y_direction), angle_rad);
    entity->cShape                = std::make_shared<CShape>(m_enemyConfig.SR, random_vertices
                                  , sf::Color(random_r,random_g,random_b)
                                  , sf::Color(255,255,255), m_enemyConfig.OT);
    entity->cColision             = std::make_shared<CColision>(m_enemyConfig.CR);
    entity->cScore                = std::make_shared<CScore>(random_vertices * 100);

    m_lastEnemySpawnTime = m_currentFrame;


}

void Game::sMovement()
{   
    
    auto shape_position = m_player->cShape->circle.getPosition();
    m_player->cTransform->velocity = {0,0};
    
    if (m_player->cInput->up)
    {   
        if (shape_position.y  - m_player->cColision->radius<= 0) m_player->cTransform->velocity.y = 0; 
        else m_player->cTransform->velocity.y = -m_playerConfig.S;
    }
    
    if (m_player->cInput->down)
    {   
        if (shape_position.y  + m_player->cColision->radius >= m_window.getSize().y) m_player->cTransform->velocity.y = 0; 
        else m_player->cTransform->velocity.y = m_playerConfig.S;
    }

    if (m_player->cInput->right)
    {   
        if (shape_position.x  + m_player->cColision->radius >= m_window.getSize().x) m_player->cTransform->velocity.x = 0; 
        else m_player->cTransform->velocity.x = m_playerConfig.S;
    }

    if (m_player->cInput->left)
    {   
        if (shape_position.x  - m_player->cColision->radius <= 0) m_player->cTransform->velocity.x = 0;
        else m_player->cTransform->velocity.x = -m_playerConfig.S;
    }


    m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
    m_player->cTransform->pos.y += m_player->cTransform->velocity.y;

    for (auto & e : m_entities.getEntities())
    {   
        if (e->tag() == "enemy" || e->tag() == "s_enemy")
        {
            auto shape_position = e->cShape->circle.getPosition();
            if (shape_position.x - e->cColision->radius <= 0 || shape_position.x + e->cColision->radius >= m_window.getSize().x)
            {
                e->cTransform->velocity.x *= -1;  
            }
        
            if (shape_position.y - e->cColision->radius <= 0 || shape_position.y + e->cColision->radius >= m_window.getSize().y)
            {
                e->cTransform->velocity.y*= -1; 
            }
            e->cTransform->pos.x += e->cTransform->velocity.x;
            e->cTransform->pos.y += e->cTransform->velocity.y;
        }

        if (e->tag() == "bullet")
        {
            e->cTransform->pos.x += e->cTransform->velocity.x;
            e->cTransform->pos.y += e->cTransform->velocity.y;
        }

        if (e->tag() == "sWeapon")
        {

            e->cTransform->pos.x += m_player->cTransform->velocity.x;
            e->cTransform->pos.y += m_player->cTransform->velocity.y;
        }

    }

  
    
}

void Game::setBoard()
{   
    
    if (!m_font.loadFromFile(m_fontConfig.FS)) exit(-1);
   
    m_text.setFont(m_font);
    m_text.setFillColor(sf::Color(m_fontConfig.R, m_fontConfig.G, m_fontConfig.B));
    m_text.setCharacterSize(m_fontConfig.FW);
    m_text.setPosition(0, 0);
}

void Game::sLifeSpan()
{
    for (auto & e : m_entities.getEntities())
    {
        if (e->tag() == "s_enemy" || e->tag() == "bullet" || e->tag() == "sWeapon")
        {
            if (e->cLifespan->remaining > 0)
            {
                e->cLifespan->remaining--;
                if (e->isActive() && e->cLifespan->remaining > 0)
                {
                    float multiplier = static_cast<float>(e->cLifespan->remaining) / e->cLifespan->total;
                    auto fillColor = e->cShape->circle.getFillColor();
                    sf::Color new_color {fillColor.r, fillColor.g, fillColor.b, static_cast<sf::Uint8>(255 * multiplier)};
                    e->cShape->circle.setFillColor(new_color);
                    auto outlineColor = e->cShape->circle.getOutlineColor();
                    sf::Color new_color_o {outlineColor.r, outlineColor.g, outlineColor.b, static_cast<sf::Uint8>(255 * multiplier)};
                    e->cShape->circle.setOutlineColor(new_color_o);
                }
            }
            if (e->cLifespan->remaining <= 0) e->destroy();
        }
    }
}

void Game::sRender()
{  
    m_window.clear(); 
    m_text.setString("P-> PAUSE\nESC-> EXIT\nSCORE: " + std::to_string(m_score) + "\nRIGHT CLICK S.W (30 second cd)" );
    for (auto & e: m_entities.getEntities())
    {   
        if (e->tag() == "enemy" || e->tag() == "player" || e->tag() == "bullet" || e->tag() == "s_enemy" || e->tag() == "sWeapon")
        {
            e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
            e->cTransform->angle += 1.0f;
            e->cShape->circle.setRotation(e->cTransform->angle);
            m_window.draw(e->cShape->circle);
        }
       
    } 
    m_window.draw(m_text);
    m_window.display();
}

void Game::sEnemySpawner()
{   
    if (m_currentFrame % m_enemyConfig.SI == 0) spawnEnemy();
}

void Game::sCollision()
{
    auto checkCollision = [](const Vec2& posA, float radiusA, const Vec2& posB, float radiusB) {
        float dx = posA.x - posB.x;
        float dy = posA.y - posB.y;
        float distanceSquared = dx * dx + dy * dy;
        float radiusSum = radiusA + radiusB;
        return distanceSquared < radiusSum * radiusSum;
    };

    for (auto & bullet : m_entities.getEntities("bullet"))
    {
        for (auto & enemy : m_entities.getEntities("enemy"))
        {
            if (checkCollision(enemy->cTransform->pos, enemy->cColision->radius, bullet->cTransform->pos, bullet->cColision->radius))
            {   
                enemy->destroy(); 
                bullet->destroy();
                m_score += enemy->cScore->score;
                spawnSmallEnemies(enemy);
                break;  
            }
        }

        for (auto & s_enemy : m_entities.getEntities("s_enemy"))
        {
            if (checkCollision(s_enemy->cTransform->pos, s_enemy->cColision->radius, bullet->cTransform->pos, bullet->cColision->radius))
            {   
                s_enemy->destroy(); 
                bullet->destroy();
                m_score += s_enemy->cScore->score;
                break;  
            }
        }
    }

    

    for (auto & entitiy : m_entities.getEntities())
    {   
        if (entitiy->tag() == "enemy" || entitiy->tag() == "s_enemy")
        {
            if (checkCollision(entitiy->cTransform->pos, entitiy->cColision->radius, m_player->cTransform->pos, m_player->cColision->radius))
            {
                m_player->cTransform->pos.x = m_window.getSize().x / 2; 
                m_player->cTransform->pos.y = m_window.getSize().y / 2;
                break;  
            }
            
            for (auto & sWeapon: m_entities.getEntities("sWeapon"))
            {
                if (checkCollision(entitiy->cTransform->pos, entitiy->cColision->radius, sWeapon->cTransform->pos, sWeapon->cColision->radius  ))
                {
                    entitiy->destroy();
                    break;  
                }
            } 
            
           
        }
        

        
    }
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
    int entity_vertices = entity->cShape->circle.getPointCount();

    for (size_t i = 1; i <= entity_vertices; i++)
    {  
        float angle_deg = 360.0f / entity_vertices * i;
        float angle_rad = angle_deg * (3.14159265358979323846f / 180.0f); 
        
        float xd  = 3.0f * std::cos(angle_rad);
        float yd  = 3.0f * std::sin(angle_rad); 
        
        auto small_enemy        = m_entities.addEntity("s_enemy");
        small_enemy->cTransform = std::make_shared<CTransform>(Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y), Vec2(xd, yd), angle_deg);
        small_enemy->cShape     = std::make_shared<CShape>(m_enemyConfig.SR / 2, entity_vertices
                                , sf::Color(entity->cShape->circle.getFillColor())
                                , sf::Color(entity->cShape->circle.getOutlineColor()), m_enemyConfig.OT);
        small_enemy->cColision  = std::make_shared<CColision>(m_enemyConfig.CR / 2);
        small_enemy->cScore     = std::make_shared<CScore>(entity_vertices * 200);
        small_enemy->cLifespan  = std::make_shared<CLifespan>(m_enemyConfig.L);
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePos)
{
    
   
    float       angle             = atan2(mousePos.y - entity->cTransform->pos.y, mousePos.x - entity->cTransform->pos.x);
    float x_direction             = m_bulletConfig.S * std::cos(angle);
    float y_direction             = m_bulletConfig.S * std::sin(angle);
    float          sx             = m_player->cTransform->pos.x;
    float          sy             = m_player->cTransform->pos.y;

    auto bullet                   = m_entities.addEntity("bullet");
    bullet->cTransform            = std::make_shared<CTransform>(Vec2(sx,sy), Vec2(x_direction,y_direction), angle);
    bullet->cShape                = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V
                                  , sf::Color(m_bulletConfig.FR,m_bulletConfig.FG,m_bulletConfig.FB)
                                  , sf::Color(m_bulletConfig.OR,m_bulletConfig.OG,m_bulletConfig.OB), m_bulletConfig.OT);
    bullet->cColision             = std::make_shared<CColision>(m_bulletConfig.CR);
    bullet->cLifespan             = std::make_shared<CLifespan>(m_bulletConfig.L);
    

}

void Game::spawnSpecialWeapon()
{   
    if (m_weaponCooldownClock)
    {
        if (m_sw_cooldown.getElapsedTime().asSeconds() >= 30.0f) m_weaponCooldownClock = false;
        else return; 
    }

    // Proceed to spawn the weapon
    if (m_entities.getEntities("sWeapon").size() == 0)
    {
        auto sWeapon                  = m_entities.addEntity("sWeapon");
        sWeapon->cTransform           = std::make_shared<CTransform>(Vec2(m_player->cTransform->pos.x, m_player->cTransform->pos.y),Vec2(0,0),0);
        sWeapon->cShape               = std::make_shared<CShape>(80,4
                                      , sf::Color(211,211,211,0)
                                      , sf::Color(105,105,105), 2);
        sWeapon->cLifespan            = std::make_shared<CLifespan>(600);
        sWeapon->cColision            = std::make_shared<CColision>(80);
        
        m_sw_cooldown.restart();
        m_weaponCooldownClock = true;
    }
}

void Game::sUserInput()
{
    sf::Event event;
    while(m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_running = false;
        }

        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    m_player->cInput->up = true;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = true;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = true;
                    break;
                case sf::Keyboard::D:
                    m_player->cInput->right = true;
                    break;
                case sf::Keyboard::P:
                    m_paused = !m_paused;
                    break;
                case sf::Keyboard::Escape:
                    m_running = false;
                default: break;
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            switch( event.key.code)
            {
                case sf::Keyboard::W:
                    m_player->cInput->up = false;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = false;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = false;
                    break;
                case sf::Keyboard::D:
                    m_player->cInput->right = false;
                
                    break;
                default: break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {     
                spawnBullet(m_player,Vec2 (event.mouseButton.x,event.mouseButton.y));
            }

            if (event.mouseButton.button == sf::Mouse::Right)
            {   
                spawnSpecialWeapon();
            }
        }
    }
}


