# Relief-Mapping
Realization of Normal Mapping, Parallax Mapping, Relief Mapping with OpenGL  
  
The part of Relief Mapping : Achieved Self-occusion But no Self-shadowing  
  
Run test platform: Windows 10, Visual Studio 12 2013  
OpenGL version: **Version 3.3 Core**. Encapsulation: **GLAD**  
Library used: **GLFW**, **GLM**  
  
****
### How to use example program:  
Under the same folder, you can find an .exe file named: Various_Mapping_Test  
Open it, use your keyboard:  
press "1" : the toy box will be rended with no mapping. (default)  
press "2" : the toy box will be rended with normal mapping.  
press "3" : the toy box will be rended with parallax mapping.  
press "4" : the toy box will be rended with relief mapping.  
press "w" : Control the camera to move up.  
press "s" : Control the camera to move down.  
press "a" : Control the camera to move left.  
press "d" : Control the camera to move right.  
press "q" : Control the camera to move forward.  
press "e" : Control the camera to move backward.  
And also you can use your mouse:  
The cursor movement can control the rotation of the camera lens.  
The scroll wheel can control the focal length of the camera.  
  
****
### Explanation:  
**Camera.h**  
**Shader.h**  
**Shader.cpp**  
These are the two encapsulated classes I used directly(For using camera and reading shaders).  
**stb_image.h**  
This is a header file I use directly to read different image formats.  
  
All vertex shaders are in the folder called vertex_shader.  
All fragment shaders are in the folder called fragment_shader.  
The folder called img contains all images I used: wood_texture, toyBox_depth, toyBox_normal.  

****
### Result:  
![image](https://github.com/Oitron/Relief-Mapping/blob/master/presentation/result_01.png)
![image](https://github.com/Oitron/Relief-Mapping/blob/master/presentation/result_02.png)
  
****
### Source:  
[1]	F. Policarpo, M. M. Oliveira, and J. L. D. Comba, “Real-Time Relief Mapping on Arbitrary Polygonal Surfaces,” p. 8.  

