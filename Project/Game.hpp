//
//  Game.hpp
//  FireFist
//
//  Created by Administrator on 09.07.23.
//

#ifndef Game_hpp
#define Game_hpp
#pragma once
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/background_segm.hpp>
#include "GameObject.hpp"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// Represents the four possible (collision) directions
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};
typedef std::tuple<bool, Direction, glm::vec2> Collision; // <collision?, what direction?, difference vector center - closest point>
const glm::vec2 MOSQUITO_SIZE(40.0f, 40.0f);
// Initial velocity of the player paddle
const float MOSQUITO_VELOCITY(50.0f);

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState                   State;
    bool                        Keys[1024];
    unsigned int                Width, Height;
    std::vector<GameObject>     mosquitos;
    float                       spawnrate;
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt, std::vector<cv::Rect> &hand);
    void Render(cv::Mat frame);
    void DoCollision(GameObject &mosquito);
    void SpawnMosquito();
};
#endif /* Game_hpp */
