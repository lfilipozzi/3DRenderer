Keep track of file changed for refactoring and modification to do.

File changed
=============
 * main.cpp. Note: Keep the name AnimationWindow instead of MainWindow
 * animationwindow.h and animationwindow.cpp. Note: Use unique_ptr instead of ptr for m_openglwindow.
 * shaderprogram.h and shaderprogram.cpp. New files added to define the ShaderProgram class.
 * texture.h and texture.cpp. New files added to define the Class Texture and TextureManager
 * material.h and material.cpp. New files added to define the material class.
 
Other changes done
===============
 * Use pointer to OpenGL ES 2,0 in skybox instead of pointer to OpenGL 3.0 core


Changing files (still work to do)
=============
 * openglwindow.h and openglwindow.cpp. Finish changing constructor to remove scene selector and the 4 gl functions (initialize, update, cleanup and resize). Need to change all other functions and find a way to reduce number of function by avoiding creating function that just transit function/command. TODO: remove m_wasCameraOffset fro this class if possible.
 * scene.h and scene.cpp. Make changes to not inherit from QOpenGLFuntion but to use pointer instead. Need to check the rest of the code.


To-do list
============
 * Use static assimp library instead of shared library
 * Create class to encapsulate the frame buffer object instead of ussing directly opengl function in the scene class.
 * Use shared pointer (or unique_ptr?) and weak pointer to point from player to opengl window and openglwindow to player.
 * Delete old texture and material classes defined in files opengltexture.* and materialinfo.*, they will be replaced by classes defined in texture.* and material.*)
