//
//  GameObject.cpp
//  FireFist
//
//  Created by Administrator on 09.07.23.
//

#include "GameObject.hpp"
#include <iostream>

GameObject::GameObject()
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), IsSolid(false), Destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture sprite, glm::vec2 velocity,glm::vec3 color)
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(sprite), IsSolid(false), Destroyed(false) { }

void GameObject::Draw(SpriteRenderer &renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

glm::vec2 GameObject::Move(float dt, unsigned int window_width, unsigned int window_height)
{
    // move the object
    this->Position += this->Velocity * dt;
    // then check if outside window bounds and if so, reverse velocity and restore at correct position
    if (this->Position.x <= 0.0f)
    {
        this->Velocity.x = -this->Velocity.x;
        this->Position.x = 0.0f;
    }
    else if (this->Position.x + this->Size.x >= window_width)
    {
        this->Velocity.x = -this->Velocity.x;
        this->Position.x = window_width - this->Size.x;
    }
    if (this->Position.y <= 0.0f)
    {
        this->Velocity.y = -this->Velocity.y;
        this->Position.y = 0.0f;
    }
    
    else if (this->Position.x + this->Size.x >= window_height)
    {
        this->Velocity.y = -this->Velocity.y;
        this->Position.y = window_height - this->Size.y;
    }
    return this->Position;
}

void GameObject::setPosition(glm::vec2 pos){
    this->Position = pos;
}

void  GameObject::setSize(glm::vec2 size){
    this->Size = size;
}

void  GameObject::setVelocity(glm::vec2 v){
    this->Velocity = v;
}

