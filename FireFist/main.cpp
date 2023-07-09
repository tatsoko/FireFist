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
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace cv;
using namespace std;

#define WIDTH 1280
#define HEIGHT 720
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
    uniform sampler2D tex1;
	uniform vec2 origin;
	uniform float iTime;
	uniform float scale;

	vec3 random3(vec3 c) {
		float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
		vec3 r;
		r.z = fract(512.0*j);
		j *= .125;
		r.x = fract(512.0*j);
		j *= .125;
		r.y = fract(512.0*j);
		return r-0.5;
	}
	const float F3 =  0.333333;
	const float G3 =  0.1666667;


	float simplex3d(vec3 p) {

		 /* calculate s and x */
		 vec3 s = floor(p + dot(p, vec3(F3)));
		 vec3 x = p - s + dot(s, vec3(G3));
	 
		 /* calculate i1 and i2 */
		 vec3 e = step(vec3(0.0), x - x.yzx);
		 vec3 i1 = e*(1.0 - e.zxy);
		 vec3 i2 = 1.0 - e.zxy*(1.0 - e);
	 	
		 /* x1, x2, x3 */
		 vec3 x1 = x - i1 + G3;
		 vec3 x2 = x - i2 + 2.0*G3;
		 vec3 x3 = x - 1.0 + 3.0*G3;
	 
		 /* 2. find four surflets and store them in d */
		 vec4 w, d;
	 
		 /* calculate surflet weights */
		 w.x = dot(x, x);
		 w.y = dot(x1, x1);
		 w.z = dot(x2, x2);
		 w.w = dot(x3, x3);
	 
		 /* w fades from 0.6 at the center of the surflet to 0.0 at the margin */
		 w = max(0.6 - w, 0.0);
	 
		 /* calculate surflet components */
		 d.x = dot(random3(s), x);
		 d.y = dot(random3(s + i1), x1);
		 d.z = dot(random3(s + i2), x2);
		 d.w = dot(random3(s + 1.0), x3);
	 
		 /* multiply d by w^4 */
		 w *= w;
		 w *= w;
		 d *= w;
	 
		 /* 3. return the sum of the four surflets */
		 return dot(d, vec4(52.0));
	}

	/* const matrices for 3d rotation */
	const mat3 rot1 = mat3(-0.37, 0.36, 0.85,-0.14,-0.93, 0.34,0.92, 0.01,0.4);
	const mat3 rot2 = mat3(-0.55,-0.39, 0.74, 0.33,-0.91,-0.24,0.77, 0.12,0.63);
	const mat3 rot3 = mat3(-0.71, 0.52,-0.47,-0.08,-0.72,-0.68,-0.7,-0.45,0.56);
	#define FLAMERADIUS 0.46
	#define FLAMEBOOST 0.35		// Adds the flame shape mask over the top of the multiplied noise to maintain more of original shape.
	#define EDGEMIN 0.015		// Edge cutoff at base of flame. Values around 0.0 to 0.1 look best.
	#define EDGEMAX .8 			// Edge cutoff at full height of flame. Values around 0.5 to 1.0 look best.
	#define FALLOFFPOW 1.0
	#define NOISEBIGNESS 1.2 
	#define WIDEN .8 		
	#define WAVE 0.25 		
	#define NIGHTSPEEDBONUS 1.25 		
	#define SHAPE 0 			// tear (0), egg (1), shitty tear (2)	
	#define BREATHWILDNESS 1	// not very wild (0), pretty wild (1), zakk wylde (2)
	#define PI 3.14159265359
	/* directional artifacts can be reduced by rotating each octave */
	float simplex3d_fractal(vec3 m) {
		return   0.5333333*simplex3d(m*rot1)
				+0.2666667*simplex3d(2.0*m*rot2)
				+0.1333333*simplex3d(4.0*m*rot3)
				+0.0666667*simplex3d(8.0*m);
	}

    void main()
    {
		float time = 28.22+NIGHTSPEEDBONUS*iTime;
		float bignessScale = 1.0/NOISEBIGNESS;	
		vec2 uv  = fragTexCoord;
		vec2 p = vec2(fragTexCoord.x, 1 - fragTexCoord.y);
		float aspect = 1.0;
		vec2 positionFromCenter = p-vec2(origin.x * aspect, 1 - origin.y);
		positionFromCenter/=FLAMERADIUS * 1;
		positionFromCenter.x /= WIDEN * 1;
		float positionFromBottom = 0.5*(positionFromCenter.y+1.0);
		vec2 waveOffset = vec2(0.);
		waveOffset.x += positionFromBottom*(sin(4.0*positionFromCenter.y-4.0*time));
		waveOffset.x += 0.1*positionFromBottom*(sin(4.0*positionFromCenter.x-1.561*time));
    
		positionFromCenter += WAVE*waveOffset;
    
		float outerMask = length(positionFromCenter);
		positionFromCenter.x += positionFromCenter.x / (1.0-(positionFromCenter.y));
		float flameMask = clamp(1.0-length(positionFromCenter), 0.0, 1.0);
		flameMask = 1.0-pow(1.0-flameMask, FALLOFFPOW);
		vec3 p3 = bignessScale*0.25*vec3(p.x, p.y, 0.0) + vec3(0.0, -time*0.1, time*0.025);
		float noise = simplex3d(p3*32.0);// simplex3d_fractal(p3*8.0+8.0);
		noise = 0.5 + 0.5*noise;
		vec3 finalColor;
		float value = flameMask*noise;    
		value += FLAMEBOOST*flameMask;
		float edge = mix(EDGEMIN, EDGEMAX, pow(0.5*(positionFromCenter.y+1.0), 1.2) );  
        float edgedValue = clamp(value-edge, 0.0 , 1.0);
        float steppedValue = smoothstep(edge,edge+0.1, value);
        float highlight = 1.0-edgedValue;
        float repeatedValue = highlight;
        highlight = highlight;
        
        p3 = bignessScale*0.1*vec3(p.x, p.y, 0.0) + vec3(0.0, -time*0.01, time*0.025);
        noise = simplex3d(p3*32.0);// simplex3d_fractal(p3*8.0+8.0);
        noise = 0.5 + 0.5*noise;
        repeatedValue = mix(repeatedValue, noise, 0.65);
        
        repeatedValue = 0.5*sin(6.0*PI*(1.0-pow(1.0-repeatedValue,1.8)) - 0.5*PI)+0.5;
        float steppedLines = smoothstep(0.95, 1.0, pow(repeatedValue, 8.0));
        steppedLines = mix(steppedLines, 0.0, 0.8-noise);
        highlight = max(steppedLines, highlight);
        
        highlight = pow(highlight, 2.0);
        
        vec3 flameHighlightColor = mix(vec3(0.0,1.0,1.0), vec3(0.0,1.0,2.0), p.y);
        
        float whiteFlash =  sin(time*3.0);
        whiteFlash = pow(whiteFlash, 4.0);        
        flameHighlightColor += vec3(0.3,0.2,0.2) * whiteFlash;
        
        vec3 flameBodyColor = mix(vec3(0.2,0.2,0.2), vec3(0.0,0.05,0.25), p.y);
        
        finalColor = flameHighlightColor*(steppedValue*highlight); // + vec3(0.1,0.4,0.8)*edgedValue;
        //finalColor = vec3(1.0)*steppedValue*highlight;
        finalColor += flameBodyColor*steppedValue;
    	
        finalColor = vec3(1.0,0.2,0.0)*steppedValue*highlight;

        fragColor = texture(tex1, fragTexCoord);
		fragColor += vec4(finalColor,1.0);
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
	// Set the desired resolution
	int width = WIDTH;
	int height = HEIGHT;
	cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

	cv::Mat frame;

	cv::namedWindow("Hand Detection", cv::WINDOW_NORMAL);
	if (!glfwInit()) {
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Window", nullptr, nullptr);
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
		cv::flip(frame, frame, 1);
		// Convert frame to grayscale
		cv::Mat gray;
		cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

		// Detect hands in the frame
		std::vector<cv::Rect> hands;
		handCascade.detectMultiScale(gray, hands, 1.0485258, 6, 0, cv::Size(30, 30));
		//cv::Mat fistRegion = cv::Mat::zeros(frame.size(), frame.type());
		// Draw bounding boxes around detected hands
		std::vector<cv::Point2d> cFist;
		std::vector<cv::Rect> rFist;
		for (const auto& rect : hands)
		{
			cv::rectangle(frame, rect, cv::Scalar(0, 255, 0), 2);
			std::cout << rect.tl();
			rFist.push_back(rect);
			cFist.push_back(cv::Point2d(double(rect.tl().x + rect.width / 2) / WIDTH, double(rect.tl().y - rect.height / 2) / HEIGHT));
			//cv::Mat roi(fistRegion, rect);
			//frame(rect).copyTo(roi);
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

		GLuint texture;
		glGenTextures(1, &texture);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.ptr());

		glUniform1i(glGetUniformLocation(shaderProgram, "tex1"), 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		cv::Point2d r = cFist.empty() ? cv::Point2d(-0.5, -0.5) : cFist[0];
		GLint posUniformLocation = glGetUniformLocation(shaderProgram, "origin");
		glUniform2f(posUniformLocation, r.x, r.y);

		auto currentTime = std::chrono::steady_clock::now();
		auto duration = currentTime.time_since_epoch();
		float time = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 3000.0f;

		GLint timeUniformLocation = glGetUniformLocation(shaderProgram, "iTime");
		glUniform1f(timeUniformLocation, time);

		float scale = cFist.empty() ? 1.0 : rFist[0].width / 800.0;
		GLint scaleUniformLocation = glGetUniformLocation(shaderProgram, "scale");
		glUniform1f(scaleUniformLocation, scale);

	
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
