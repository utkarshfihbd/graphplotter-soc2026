**I tried to play around with the tutorials provided and made -**


-> a hexagon with 6 different colored triangles 

-> a cube with 6 different colors (a predecessor to rubiks cube) 

and finally

-> a rubiks cube with face rotations


the shader_util.cpp, shader_util.hpp, gl_framework.cpp, gl_framework.hpp are mostly the same as present in the tutorials. shader glsl files are rewritten and so is the main .cpp file.
there is a makefile in each of these files and they also contain executables.
either just run "./hexgaon" , "./solidcube" , "./rubiks-cube" or type "make" to create the executables. 



**some personal experience lol -** 

when i first read the tut1 of how to create the triangle it went all over my head. there were so many functions and things and i didnt understand what was their purpose but eventually i understood the essence of it (barely lol)

i tried to make the colorcube on my own from tut2 and hardcoded some stuff. i just used a single array with all the vertex positions and colors as opposed to two different arrays as used in the tutorial. similar stuff for rubiks cube

theres a lot of stuff that stayed the same throughout like most of the main functions, the renderGL function and the initBuffer function except for some logical changes. i had to look up for some help to understand how to implement rotation and stuff lol.

anyways it felt pretty good when the codes worked successfully.  
