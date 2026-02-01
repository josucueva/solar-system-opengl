# solar-system-opengl

A solar system model made purely with C++ with OpenGL.

This project demonstrates the creation of a simple solar system simulation using OpenGL for rendering. It features textured spheres representing planets orbiting around a central sun, with basic lighting and camera controls. This project followed the tutorials from [LearnOpenGL](https://learnopengl.com/) and adapted them to create a solar system model.

## Structure

- `main.cpp`: Main application file containing OpenGL setup, rendering loop, and solar system logic
- `Makefile`: Build configuration file for compiling the project
- `shader/`: Directory containing vertex and fragment shader source files
- `assets/`: Directory containing textures and other assets used in the application
- `includes/`: Directory containing header files for utility functions and classes
- `model/`: Directory containing model files

## Features

- Textured spheres representing planets and the sun
- Basic lighting to simulate sunlight
- Camera controls for navigating the scene
- Orbiting planets with different sizes, speeds and distances from the sun
- Background star field!!!

## Requirements

- C++11 or higher
- Make
- OpenGL 3.3 or higher
- GLFW
- GLEW
- GLM

## Building and Running

1. Clone the repository:
   ```bash
   git clone
   ```
2. Navigate to the project directory:
   ```bash
   cd solar-system-opengl
   ```
3. Build the project using the provided Makefile:
   ```bash
   make
   ```
4. Run the application:
   ```bash
   ./solar-system
   ```

## Controls

- `W`, `A`, `S`, `D`: Move the camera forward, left, backward, and right
- Mouse movement: Look around
- `ESC`: Exit the application

## Credits

- Textures sourced from [Solar System Scope](https://www.solarsystemscope.com/textures/) and other free texture resources
- Tutorials and guidance from [LearnOpenGL](https://learnopengl.com/)
- Developed by Joshua and Christian
