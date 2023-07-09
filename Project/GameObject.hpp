//
//  GameObject.hpp
//  FireFist
//
//  Created by Administrator on 09.07.23.
//

#ifndef GameObject_hpp
#define GameObject_hpp

#include <stdio.h>
#include <glm/glm.hpp>

#include "Texture.hpp"
#include "SpriteRenderer.hpp"


// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
    // object state
    glm::vec2   Position, Size, Velocity;
    glm::vec3   Color;
    float       Rotation;
    bool        IsActive;
    bool        IsSolid;
    bool        Destroyed;
    // render state
    Texture   Sprite;
    // constructor(s)
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture sprite, glm::vec2 velocity, glm::vec3 color = glm::vec3(1.0f));
    // draw sprite0
    virtual void Draw(SpriteRenderer &renderer);
    // move function
    glm::vec2 Move(float dt, unsigned int window_width, unsigned int window_height);
    void setPosition(glm::vec2 pos);
    void setSize(glm::vec2 size);
    void setVelocity(glm::vec2 v);
};
#endif /* GameObject_hpp */
