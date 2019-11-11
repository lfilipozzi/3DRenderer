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
# You can also select to disable deprecated APIs only up to a certain version of
# Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000   # disables all the APIs
                                                    # deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/scene.cpp \
    src/openglwindow.cpp \
    src/light.cpp \
    src/abstractobject3d_gl33.cpp \
    src/camera.cpp \
    src/inputmanager.cpp \
    src/animationplayer.cpp \
    src/playerbutton.cpp \
    src/line_gl33.cpp \
    src/frame_gl33.cpp \
    src/skybox.cpp \
    src/animationwindow.cpp \
    src/shaderprogram.cpp \ # New
    src/material.cpp \
    src/texture.cpp \
    src/object.cpp \
    src/vehicle.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/resources.qrc

HEADERS += \
    include/scene.h \
    include/openglwindow.h \
    include/abstractobject3d_gl33.h \
    include/light.h \
    include/camera.h \
    include/inputmanager.h \
    include/position.h \
    include/animationplayer.h \
    include/playerbutton.h \
    include/line_gl33.h \
    include/frame_gl33.h \
    include/skybox.h \
    include/animationwindow.h \
    include/shaderprogram.h \ # new
    include/material.h \
    include/texture.h \
    include/object.h \
    include/vehicle.h

unix: !macx {
    INCLUDEPATH += lib/assimp/include
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
