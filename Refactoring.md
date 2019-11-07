Keep track of file changed for refactoring and modification to do.

File changed
=============
 * main.cpp. Note: Keep the name AnimationWindow instead of MainWindow
 * animationwindow.h and animationwindow.cpp. Note: Use unique_ptr instead of ptr for m_openglwindow.


Changing files (still work to do)
=============
 * openglwindow.h and openglwindow.cpp. Finish changing constructor to remove scene selector and the 4 gl functions (initialize, update, cleanup and resize). Need to change all other functions and find a way to reduce number of function by avoiding creating function that just transit function/command.


To-do list
============
 * Need to change the texture class and constructor to use a QImage instead of a QString and check thath the QImage is not null
 * Create class to encapsulate the frame buffer object instead of ussing directly opengl function in the scene class.
