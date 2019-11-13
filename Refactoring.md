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


Changing files (still work to do)
=============
 * openglwindow.h and openglwindow.cpp. Finish changing constructor to remove scene selector and the 4 gl functions (initialize, update, cleanup and resize). Need to change all other functions and find a way to reduce number of function by avoiding creating function that just transit function/command. TODO: remove m_wasCameraOffset fro this class if possible.
 * scene.h and scene.cpp. Make changes to not inherit from QOpenGLFuntion but to use pointer instead. Need to check the rest of the code.
 * object.h and object.cpp defining the object and object builder.
 * vehicle.h and vehicle.cpp to replace abstractvehicle and vehicle_gl33
 * camera.cpp and camera.h


To-do list
============
 * Use static assimp library instead of shared library
 * Create class to encapsulate the frame buffer object instead of ussing directly opengl function in the scene class.
 * Use shared pointer (or unique_ptr?) and weak pointer to point from player to opengl window and openglwindow to player.
 * Delete old texture and material classes defined in files opengltexture.* and materialinfo.*, they will be replaced by classes defined in texture.* and material.*)
 * Remove name field from texture class, there is no need for that. Only required in the texture manager.
 * Redefine light class
 * No need to give light, view, and projection when computig the shadow map, change the function to avoid sending unnecessary data.
 * Update the timer of the scene in openglwindow not in scene as the timer is in the openglwindow
 
 
 Idea
 ==============
 * Do not use object in the vehicle class. Instead only use raw pointer to object. Maybe use decorator pattern for the vehicle model?
 * Move the frame object for shadow mapping n the openglwindow class and create two functions in the scene, one for rendering and one for generating the shadow map. So that the openglwindow is responsible for the openGL implementation and the scene only responsible for managing the object inside the scene.
 * Do linear interpolation to get the vehicle position at a timestep which is not in the trajectory
