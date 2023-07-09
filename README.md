# FireFist

FireFist is an augmented reality (AR) project that combines computer vision techniques and game development to create an interactive experience. This project detects a user's fist in the camera frame using OpenCV cascade classifiers and renders a flame over it. Additionally, it incorporates a game structure where mosquitos fly across the screen. When the fist is detected, the mosquito dies upon collision with it. The project is implemented in C++ and utilizes OpenCV and OpenGL libraries.

## Features

- Real-time fist detection: FireFist uses OpenCV cascade classifiers to detect the presence of a fist in the camera frame. The algorithm identifies the fist based on a set of predefined patterns, allowing for accurate and efficient detection.

- Flame rendering: Once the fist is detected, FireFist overlays a flame effect on top of it using OpenGL. The flame effect enhances the AR experience, providing visual feedback to the user.

- Mini-game: FireFist incorporates a game structure where virtual mosquito objects fly across the screen. These mosquitoes serve as targets for the user to eliminate by colliding with their fist. The game provides an engaging and interactive element to the AR experience.

## Requirements

- C++ compiler compatible with C++11 standard
- OpenCV library (version 4.6.X or higher)
- OpenGL library (version 4.0 or higher)

## Installation

Clone the repository:

   ```shell
   git clone https://github.com/tatsoko/FireFist.git
   ```
## Acknowledgements

FireFist was developed by Tatev Tsokolakyan and Leonie Wargitsch as a final project for Erweiterte Realität (IN2018) course at TUM.
