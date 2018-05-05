# Three::pp
another C++ port of Three.js, based on Three.js r86+ and a few of the Qt framework's OpenGL classes

May be used standalone (limited work required to remove the dependency on Qt's QOpenGLFunctions), or through the extensive QML (aka QtQuick) integration.

Currently in beta state. Some examples demonstrate the functionality. Buildable under Linux/gcc 7.3 and sometimes under Android. Windows available soon.

Contributors welcome

## Some Samples (more in the code base)
simple geometries with ambient light, spot light and shade:

![example 1](doc/example1.png "geometries, lights and shade") 

Environment Maps

![example 2](doc/sphere_envmap.png "environment mapping")

Model Loading (3DS model through Assimp)

![example 2](doc/MercedesGLE.png "3ds model")