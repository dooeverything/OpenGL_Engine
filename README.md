# 3D Renderer

### Features include
 1. 3D model loading (.fbx)
 2. 3D game engine like gui (by using ImGui)
 3. Phong Shading Model
 4. 3D Gizmos (only translation)
 5. Outline for picking objects (by using [jump flood algorithm](https://www.comp.nus.edu.sg/~tants/jfa.html))
 6. Shadow (Directional light)
 7. Load and apply texture to a game object
 8. Physically Based Rendering (PBR)
 9. 3D grid
 10. Marching Cube (Terrain, Metaball)
 11. Terrain Editor (Sculpting and removing)
 12. SPH Simulation with Screen Space Rendering on GPU ([Bilateral filter](https://developer.nvidia.com/content/fluid-simulation-alice-madness-returns) & [Curvature Flow](https://dl.acm.org/doi/10.1145/1507149.1507164))
 13. Cloth Simulation ([Small Steps in Physics Simulation](https://dl.acm.org/doi/10.1145/3309486.3340247), [A Survey on Position Based Dynamics, 2017](https://dl.acm.org/doi/10.2312/egt.20171034))

### Libraries
 1. C++17
 2. OpenGL 3.3
 3. [glm](https://github.com/g-truc/glm)
 4. [ImGui](https://github.com/ocornut/imgui)
 5. [stb library](https://github.com/nothings/stb)

### References
 1. [Learn OpenGL](learnopengl.com)
 2. [Game Programming in C++](https://www.amazon.com/Game-Programming-Creating-Games-Design/dp/0134597206)
 3. [Real Time Rendering, 4th Edition](https://www.amazon.com/Real-Time-Rendering-Fourth-Tomas-Akenine-M%C3%B6ller/dp/1138627003)
 4. [Computer Graphics Principles and Practice, 3/E](https://www.amazon.com/Computer-Graphics-Principles-Practice-3rd/dp/0321399528)
 5. [Physically Based Rendering, 3rd edition](https://www.amazon.com/Physically-Based-Rendering-Theory-Implementation/dp/0128006455)

### Mouse Inputs
 1. W/A/S/D to move
 2. Right mouse click to rotate the camera 
 3. Left mouse click for object picking
 
### Features to be added
- [ ] Shadow (Point light)
- [ ] Skeleton Animation
- [ ] Environment mapping
- [ ] enhanced fbx loader
- [ ] Bump mapping
- [X] ~~Marching Cube~~
- [X] ~~Terrain~~
- [ ] 3D gizmos for rotation and scale
- [x] ~~Optimize SPH Simulation (multi-thread)~~
- [ ] Cloth - Object Collision
