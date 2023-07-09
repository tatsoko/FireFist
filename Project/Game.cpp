//
//  Game.cpp
//  FireFist
//
//  Created by Administrator on 09.07.23.
//

#include "Game.hpp"

#include "ResourceManager.hpp"
#include "SpriteRenderer.hpp"
#include "CameraRenderer.hpp"

// Game-related State data
SpriteRenderer  *Renderer;
CameraRenderer  *cameraRenderer;
float           spawncd = 0.0f;
GameObject      *hand;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
    delete Renderer;
    delete cameraRenderer;
}

void Game::Init()
{
        spawnrate = 2.0f;
        // load shaders
        ResourceManager::LoadShader("sprite.vs", "sprite.frag", nullptr, "sprite");
        ResourceManager::LoadShader("camera.vs", "camera.frag", nullptr, "camera");
        // configure shaders
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
        ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
        ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
        // set render-specific controls
        Shader shader = ResourceManager::GetShader("sprite");
        Shader cameraShader = ResourceManager::GetShader("camera");
        Renderer = new SpriteRenderer(shader);
        cameraRenderer = new CameraRenderer(cameraShader);
        // load textures
        ResourceManager::LoadTexture("mosquito.png", true, "mosquito");
        ResourceManager::LoadTexture("fire.png", true, "fire");
    
        glm::vec2 mPos = glm::vec2(this->Width / 2.0f -  MOSQUITO_SIZE.x / 2.0f, this->Height / 2.0f -  MOSQUITO_SIZE.y / 2.0f);
        hand = new GameObject(mPos, MOSQUITO_SIZE, ResourceManager::GetTexture("fire"), glm::vec2(0.0f, 0.0f));
        SpawnMosquito();
}

void Game::Update(float dt, std::vector<cv::Rect> &hands)
{
    if(!hands.empty()){
        glm::vec2 hPos = glm::vec2(hands.at(0).x, hands.at(0).y);
        hand->setPosition(hPos);
        
        glm::vec2 hSize = glm::vec2(hands.at(0).width, hands.at(0).height);
        hand->setSize(hSize);
        hand->IsActive = true;
    } else {
        hand->IsActive = false;
    }
    
    for (GameObject &mos : this->mosquitos){
        if(!mos.Destroyed){
            mos.Move(dt, this->Width, this->Height);
            if(hand->IsActive)
                DoCollision(mos);
        }
    }
        
    
    spawncd += dt;
    if(spawncd >= spawnrate){
        SpawnMosquito();
        spawncd = 0.0f;
    }
}

void Game::SpawnMosquito(){
    // configure mosquito
    glm::vec2 mPos = glm::vec2(this->Width / 2.0f -  MOSQUITO_SIZE.x / 2.0f, this->Height / 2.0f -  MOSQUITO_SIZE.y / 2.0f);
    
    srand((unsigned)time(0));
    int veloX;
    veloX =  (rand()%2)+ 1 ;
    if(veloX == 0)
        veloX--;
    
    srand((unsigned)time(0));
    float veloY;
    veloY = (rand()%2)+ 1 ;
    if(veloY == 0)
        veloY--;
    
    glm::vec2 mVelocity = glm::vec2(-veloX,veloY);
    
    GameObject *mmm = new GameObject(mPos, MOSQUITO_SIZE, ResourceManager::GetTexture("mosquito"), MOSQUITO_VELOCITY * mVelocity);
    this->mosquitos.push_back(*mmm);
}

//bool CheckCollision(GameObject &one, GameObject &two);
bool CheckCollision(GameObject &one, GameObject &two);
Direction VectorDirection(glm::vec2 closest);

bool CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

void Game::DoCollision(GameObject &mosquito){
    bool collision = CheckCollision(*hand, mosquito);
    if(collision){
        mosquito.Destroyed = true;
    }
}

void Game::ProcessInput(float dt)
{
    
}

void Game::Render(cv::Mat frame)
{
    //exture mosquito = ResourceManager::GetTexture("mosquito");
    cameraRenderer->DrawCamera(frame);
    if(hand->IsActive)
        hand->Draw(*Renderer);
    for (GameObject &mos : this->mosquitos)
        if(!mos.Destroyed)
            mos.Draw(*Renderer);
}
