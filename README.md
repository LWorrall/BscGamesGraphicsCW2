# BscGamesGraphicsCW2
**Overview**

This repo holds code for a C++ program which renders a 3D scene when run. The program uses vertex and gramgnet shaders coded in GLSL to render a scene using OpenGL.

To use this program, download it and extract the contents of the ZIP file to an appropriate directory. The 'Project_Template.sln' file can be opened in Visual Studio.
From there, an executable can be built by pressing Ctrl + B. This will build an executable file in 'x64/Debug/'. This executable can be placed in a seperate directory, but must have the 'shader' and 'media' folders and their contents present in the same directory as the executable.

This program was built upon a previous version which was created for a submission of the first coursework task in the same module as this task.


The scene consists of a plane with five barrels positioned ontop. Both the barrels and plane are textured using .png image files.
The scene includes a moving light source that casts it's light around the floor and barrel's surfaces as it moves.
A multi-texturing effect is applied to the barrel to display two overlapping textures.
The scene also makes use of a fog effect which can be seen obscuring the barrels and view of the ground in the distance.


As well as these techniques, this program uses physically based rendering (PBR) as its main shading model.
Each object in the scene has different parameters that can be set to adjust how the lighting behaves with the object. 
These parameters are an objects colour, reflectance, and whether or not an object s metallic or dielectric.

PBR shading simulates physics theory and models in order to generate a method of graphics shading that can be regarded as more accurate than other shading methods.
This is different from other methods such as the Blinn-Phong method, where light is calculated with parameters and methods which are not based in the established physics of how light behaves in the real world.
The PBR shading in the program makes use of the microfacet model. This model is based on the theory that at a microscopic level, all surfaces have reflective properties equal to that of a mirror.
The rougher a surface is however, the more of the light gets reflected off at obscure angles, meaning the surface loses its perfectly reflective quality.
Within this shader, three different parameters can be set: A material’s roughness, colour, and whether it is metal or dielectric.
All three properties change how the final image of the shading looks. 

The main inspiration for the aesthetics of the scene is drawn from early videdo games making use of 3D graphics techniques in the '90s and 2000s.
The use of fog which obscures vision, low resolution textures (without antialiasing), and low-poly models contribute to this aesthetic.
