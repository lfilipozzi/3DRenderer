QT       += core widgets

CONFIG += c++14
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    modelloader.cpp \
    sceneselector.cpp \
    scene_gl33.cpp \
    openglwindow.cpp \
    model3d_gl33.cpp \
    light.cpp \
    abstractobject3d_gl33.cpp \
    camera.cpp \
    inputmanager.cpp \
    abstractscene.cpp \
    abstractvehicle.cpp \
    vehicle_gl33.cpp \
    flatsurface_gl33.cpp \
    animationplayer.cpp \
    playerbutton.cpp \
    line_gl33.cpp \
    frame_gl33.cpp \
    skybox_gl33.cpp \
    abstractcomplex_gl33.cpp \
    animationwindow.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

HEADERS += \
    modelloader.h \
    sceneselector.h \
    scene_gl33.h \
    abstractscene.h \
    openglwindow.h \
    abstractobject3d_gl33.h \
    model3d_gl33.h \
    light.h \
    camera.h \
    inputmanager.h \
    position.h \
    abstractvehicle.h \
    vehicle_gl33.h \
    flatsurface_gl33.h \
    materialinfo.h \
    animationplayer.h \
    playerbutton.h \
    texture.h \
    line_gl33.h \
    frame_gl33.h \
    skybox_gl33.h \
    abstractcomplex_gl33.h \
    animationwindow.h

unix: !macx {
    INCLUDEPATH += assimp/include
    LIBS += /usr/lib/libassimp.so
}

macx {
    INCLUDEPATH +=  /usr/local/include
    LIBS += /usr/local/lib/libassimp.dylib
}

win32 {
    INCLUDEPATH += "C:/ProgramFiles/Assimp/include"
    LIBS += -L"C:/ProgramFiles/Assimp/lib/Release" -lassimp
}

DISTFILES += \
    README.md
