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
 * Do linear interpolation to get the vehicle position at a timestep which is not in the trajectory
 
 
 Cascaded Shadow Maps
 ================
 Possible options:
 * Redefine prototype of render method of all AbstractObject and children. Problem: need to redefine many functions, the cascade information is required only by the Object, not by Line for instance.
 * Instead of sending the cascade information by redefining the prototype of the render function create a public function that is used to set the uniform related to cascade distance. Maybe using something like <? extends class> like in java (https://stackoverflow.com/questions/30687305/c-equivalent-of-using-t-extends-class-for-a-java-parameter-return-type) to avoid defining this function to class that does not need it. Problem: this method is not really similar to the other: it would be the only public method of the class to set some OpenGL uniform. + most object does not require information about CSM to be rendered.
 * Other solution (not preferred option): when rendering, we only information from the scene. Maybe add a field to each object to be render to have access to the scene (add a const Scene *) and use INLINE methods to access whatever we need (light, view, projection, light transform matrices, cascade information...). Problem: does not show as easily what are the information we pass + some circle dependencies when defining headers. Is it a good idea to change the interface? Advantages: This is the only method which uses only parameters it needs (though it has access to all public parameters of the scene)
 
 Remark: need to use vector for cascade information or to give how many cascades are needed for the shadow program since we need to know how many transformations are needed and the number of transformation is not the same depending if we use the object or shadow shader. No we can still use table and access the number of element from constants.h
 
 Things to improve about CSM:
 * As of right now, the information about CSM are in different places: we need to make sure the FBO, the scene, the objects, and the shader use the same number of CSM.
    * Maybe use array instead of vector to keep the cascade information? If we use array we need to say how many cascades are used when rendering (best way is to use a macro with define in a header file and include this file anywhere we need.
    * Also need to define at one unique place the number of CSM. OK if defined at compile-time?
    * The first place we need to define how many cascade we need is when creating the scene then when creating the depthmap FBO.
 
 
 
 
 
 
 
 
