cmake_minimum_required(VERSION 3.26)
project(FireFist)

find_package(OpenCV REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
if (GLEW_FOUND)

include_directories( ${GLEW_INCLUDE_DIRS}  "/usr/local/Cellar/glfw/3.3.8/include"
${OpenCV_INCLUDE_DIRS})

set(FireFist_SOURCES main.cpp)

file(GLOB FireFist_SOURCES ./*.cpp
                 ./*.hpp
                 ./*.frag
                 ./*.vs
                 ./*.h)
        
add_executable(FireFist ${FireFist_SOURCES})
target_link_libraries (FireFist GLEW::GLEW glm::glm "/usr/local/Cellar/glfw/3.3.8/lib/libglfw.3.dylib" ${OPENGL_LIBRARIES} ${OpenCV_LIBS})
endif()
