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


using namespace cv;
using namespace std;

#define WIDTH 640
#define HEIGHT 480
typedef vector<Point> contour_t;
typedef vector<contour_t> contour_vector_t;
struct Shader
{
    std::string VertexSource;
    std::string FragmentSource;
};

static void on_trackbar(int pos, void* slider_value) {
    *((int*)slider_value) = pos;
}

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 position;
    layout (location = 1) in vec2 texCoord;
    out vec2 fragTexCoord;
    void main()
    {
        gl_Position = vec4(position, 1.0);
        fragTexCoord = texCoord;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 fragTexCoord;
    out vec4 fragColor;
    uniform sampler2D textureSampler;
    void main()
    {
        fragColor = texture(textureSampler, fragTexCoord) * 0.5;
    }
)";

int main()
{
    // Load the cascade classifier for hand detection
    cv::CascadeClassifier handCascade;
    handCascade.load("fist.xml"); // Provide the path to your hand cascade XML file

    // Open the default camera
    cv::VideoCapture cap(0);
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
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
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

        // Draw bounding boxes around detected hands
        for (const auto& rect : hands)
        {
            cv::rectangle(frame, rect, cv::Scalar(0, 255, 0), 2);
        }
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program
        glUseProgram(shaderProgram);

        // Render your OpenGL content here
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Create and bind vertex buffer object (VBO)
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // Define the vertices and texture coordinates for a quad
        GLfloat vertices[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
        };

        GLfloat texCoords[] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(texCoords), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(texCoords), texCoords);

        // Set up vertex attributes
        GLint positionAttrib = glGetAttribLocation(shaderProgram, "position");
        glEnableVertexAttribArray(positionAttrib);
        glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

        GLint texCoordAttrib = glGetAttribLocation(shaderProgram, "texCoord");
        glEnableVertexAttribArray(texCoordAttrib);
        glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(vertices));

        // Create and bind texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load the image data into the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.ptr());

        // Set uniform for texture sampler
        GLint textureSamplerUniform = glGetUniformLocation(shaderProgram, "textureSampler");
        glUniform1i(textureSamplerUniform, 0);

        // Render the quad
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Cleanup
        glDeleteTextures(1, &texture);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);

        glfwSwapBuffers(window);
        glfwPollEvents();
        // Display the frame
        //cv::imshow("Hand Detection", frame);

        // Break the loop if the 'q' key is pressed
        /*if (cv::waitKey(1) == 'q')
        {
            break;
        }*/
    }
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    // Release the video capture object and destroy the windows
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
