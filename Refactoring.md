Keep track of file changed for refactoring and modification to do.

File changed
=============
 * Rename the shader files.
 * main.cpp. Note: Keep the name AnimationWindow instead of MainWindow
 * animationwindow.h and animationwindow.cpp. Note: Use unique_ptr instead of ptr for m_openglwindow.
 * shaderprogram.h and shaderprogram.cpp. New files added to define the ShaderProgram class.
 * texture.h and texture.cpp. New files added to define the Class Texture and TextureManager
 * material.h and material.cpp. New files added to define the material class.
 

Deleted files or renamed
============
 * src/modelloader.cpp
 * src/model3d_gl33.cpp
 * src/abstractvehicle.cpp
 * src/vehicle_gl33.cpp
 * src/skybox_gl33.cpp => src/skybox.cpp
 * src/flatsurface_gl33.cpp
 * src/abstractcomplex_gl33.cpp
 * src/line_gl33.cpp => src/line.cpp
 * src/abstractobject3d_gl33.cpp
 * src/frame_gl33.cpp => src/frame.cpp
    
 * include/modelloader.h
 * include/model3d_gl33.h
 * include/abstractvehicle.h
 * include/vehicle_gl33.h
 * include/skybox_gl33.h => include/skybox.h
 * include/flatsurface_gl33.h
 * include/abstractcomplex_gl33.h
 * include/materialinfo.h
 * include/opengltexture.h
 * include/line_gl33.h => include/line.h
 * include/abstractobject3d_gl33.h
 * include/frame_gl33.h => include/frame.h

 * shader source files

Other changes done
===============
 * Use pointer to OpenGL ES 2,0 in skybox instead of pointer to OpenGL 3.0 core
 * Improve code for parsing XML file, this is a bit messy. FOr instance the code to transform string to qvector3D or 4D is copied several times. The XML schema is not well done too. Two type of transform are used for node or groups... Need to make sure that one of each buffer is provided (six buffer does not mean we provide one of each)
 * Check if possible to share unique_ptr less often when building objects. Instead pass the unqieu_ptr to function using operator* and reference instead, check what is the consequence of doing that.


Changing files (still work to do)
=============
 * openglwindow.h and openglwindow.cpp. Finish changing constructor to remove scene selector and the 4 gl functions (initialize, update, cleanup and resize). Need to change all other functions and find a way to reduce number of function by avoiding creating function that just transit function/command. TODO: remove m_wasCameraOffset fro this class if possible.
 * scene.h and scene.cpp. Make changes to not inherit from QOpenGLFuntion but to use pointer instead. Need to check the rest of the code.
 * object.h and object.cpp defining the object and object builder.
 * camera.cpp and camera.h


To-do list
============
 * Use static assimp library instead of shared library
 * Use shared pointer (or unique_ptr?) and weak pointer to point from player to opengl window and openglwindow to player.
 * Update the timer of the scene in openglwindow not in scene as the timer is in the openglwindow
 
 
 
 
 
 
 
 
