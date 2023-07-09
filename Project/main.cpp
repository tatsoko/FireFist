//
//  old.cpp
//  FireFist
//
//  Created by Administrator on 08.07.23.
//

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/background_segm.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Game.hpp"
#include "ResourceManager.hpp"

using namespace cv;
using namespace std;
//using namespace aw;

#define WIDTH 1280
#define HEIGHT 720
// Camera settings
#define CAM 0
VideoCapture cap;


typedef vector<Point> contour_t;
typedef vector<contour_t> contour_vector_t;
/*struct Shader
{
    std::string VertexSource;
    std::string FragmentSource;
};*/


// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
static void on_trackbar(int pos, void* slidervalue);
void init();
void mainLoop();
Game FireFist(WIDTH, HEIGHT);

int main()
{
    mainLoop();
    return 0;
}

void mainLoop(){
    init();
    // Load the cascade classifier for hand detection
    cv::CascadeClassifier handCascade;
    handCascade.load("fist.xml"); // Provide the path to your hand cascade XML file

    // Open the default camera
    cv::VideoCapture cap(0);
    //ocv_initVideoStream(cap);
    
    if (!cap.isOpened())
    {
        std::cout << "Failed to open the camera." << std::endl;
        return -1;
    }
    
    cv::Mat frame;
    cv::namedWindow("Hand Detection", cv::WINDOW_NORMAL);
    if (!glfwInit()) {
        return -1;
    }

    
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    glfwMakeContextCurrent(window); //Draw in this context
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        return -1;
    }

   // initialize game
   // ---------------
    FireFist.Init();
    
   // deltaTime variables
   // -------------------
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    while (!glfwWindowShouldClose(window))
    {
        // Capture frame from camera
        cap >> frame;

        // Convert frame to grayscale
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Detect hands in the frame
        std::vector<cv::Rect> hands;
        handCascade.detectMultiScale(gray, hands, 1.0485258, 6, 0, cv::Size(30, 30));

        std::vector<cv::Point2d> cFist;
        std::vector<cv::Rect> rFist;
        // Draw bounding boxes around detected hands
        for (const auto& rect : hands)
        {
            cv::rectangle(frame, rect, cv::Scalar(0, 255, 0), 2);
            std::cout << rect.tl();
            rFist.push_back(rect);
            cFist.push_back(cv::Point2d(double(rect.tl().x + rect.width / 2) / WIDTH, double(rect.tl().y - rect.height / 2) / HEIGHT));
        }
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        // manage user input
        // -----------------
        FireFist.ProcessInput(deltaTime);

        // update game state
        // -----------------
        FireFist.Update(deltaTime, cFist, rFist);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        FireFist.Render(frame, cFist, rFist);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // delete all resources as loaded using the resource manager
    // ---------------------------------------------------------
    ResourceManager::Clear();
    glfwTerminate(); //Ending glfv; Destroys any remaining windows and other allocated resources
    
    // Release the video capture object and destroy the windows
    cap.release();
    cv::destroyAllWindows();
}

void init(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    glfwWindowHint(GLFW_RESIZABLE, false);
}

static void on_trackbar(int pos, void* slider_value) {
    *((int*)slider_value) = pos;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

