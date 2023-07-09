//
//  CameraRenderer.hpp
//  FireFist
//
//  Created by Administrator on 09.07.23.
//

#ifndef CameraRenderer_hpp
#define CameraRenderer_hpp

#include <stdio.h>
#pragma once
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/background_segm.hpp>
#include "Texture.hpp"
#include "Shader.hpp"


class CameraRenderer
{
public:
    
    // Constructor (inits shaders/shapes)
    CameraRenderer(Shader &shader);
    // Destructor
    ~CameraRenderer();
    // Renders a defined quad textured with given sprite
    void DrawCamera(cv::Mat frame, std::vector<cv::Point2d> cFist, std::vector<cv::Rect> rFist);
    
private:
    // Render state
    Shader       shader;
    unsigned int quadVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
    

};

#endif /* CameraRenderer_hpp */
