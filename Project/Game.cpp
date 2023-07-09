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
    ResourceManager::LoadShader("fire.vs", "fire.frag",nullptr, "fire");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
    static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Shader shader = ResourceManager::GetShader("sprite");
    Shader fireShader = ResourceManager::GetShader("fire");
    Shader cameraShader = ResourceManager::GetShader("camera");
    Renderer = new SpriteRenderer(shader);
    cameraRenderer = new CameraRenderer(fireShader);
    // load textures
    ResourceManager::LoadTexture("mosquito.png", true, "mosquito");
    ResourceManager::LoadTexture("fire.png", true, "fire");

    glm::vec2 mPos = glm::vec2(this->Width / 2.0f -  MOSQUITO_SIZE.x / 2.0f, this->Height / 2.0f -  MOSQUITO_SIZE.y / 2.0f);
    hand = new GameObject(mPos, MOSQUITO_SIZE, ResourceManager::GetTexture("fire"), glm::vec2(0.0f, 0.0f));
    SpawnMosquito();
}

void Game::Update(float dt, std::vector<cv::Point2d> cFist, std::vector<cv::Rect> rFist)
{
    if(!rFist.empty()){
        glm::vec2 hPos = glm::vec2(rFist.at(0).x, rFist.at(0).y);
        hand->setPosition(hPos);
        
        glm::vec2 hSize = glm::vec2(rFist.at(0).width, rFist.at(0).height);
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

float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void Game::SpawnMosquito(){
    // configure mosquito
    //glm::vec2 mPos = glm::vec2(this->Width / 2.0f -  MOSQUITO_SIZE.x / 2.0f, this->Height / 2.0f -  MOSQUITO_SIZE.y / 2.0f);
    
    //Spawn Velocity
    float veloX = RandomFloat(-1.0f, 1.0f);
    float veloY = RandomFloat(-1.0f, 1.0f);
    glm::vec2 mVelocity = glm::vec2(-veloX,veloY);
    
    //Spawn Position
    float posX = RandomFloat(0 + MOSQUITO_SIZE.x, this->Width - MOSQUITO_SIZE.x);
    float posY = RandomFloat(0 + MOSQUITO_SIZE.y, this->Height - MOSQUITO_SIZE.y);
    glm::vec2 mPos = glm::vec2(posX, posY);
    
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

void Game::Render(cv::Mat frame, std::vector<cv::Point2d> cFist, std::vector<cv::Rect> rFist)
{
    //exture mosquito = ResourceManager::GetTexture("mosquito");
    cameraRenderer->DrawCamera(frame, cFist, rFist);
    /*if(hand->IsActive)
        hand->Draw(*Renderer);*/
    for (GameObject &mos : this->mosquitos)
        if(!mos.Destroyed)
            mos.Draw(*Renderer);
}
