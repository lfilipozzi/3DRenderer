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
    src/main.cpp \
    src/modelloader.cpp \
    src/scene_gl33.cpp \
    src/openglwindow.cpp \
    src/model3d_gl33.cpp \
    src/light.cpp \
    src/abstractobject3d_gl33.cpp \
    src/camera.cpp \
    src/inputmanager.cpp \
    src/abstractscene.cpp \
    src/abstractvehicle.cpp \
    src/vehicle_gl33.cpp \
    src/flatsurface_gl33.cpp \
    src/animationplayer.cpp \
    src/playerbutton.cpp \
    src/line_gl33.cpp \
    src/frame_gl33.cpp \
    src/skybox_gl33.cpp \
    src/abstractcomplex_gl33.cpp \
    src/animationwindow.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/resources.qrc

HEADERS += \
    include/modelloader.h \
    include/scene_gl33.h \
    include/abstractscene.h \
    include/openglwindow.h \
    include/abstractobject3d_gl33.h \
    include/model3d_gl33.h \
    include/light.h \
    include/camera.h \
    include/inputmanager.h \
    include/position.h \
    include/abstractvehicle.h \
    include/vehicle_gl33.h \
    include/flatsurface_gl33.h \
    include/materialinfo.h \
    include/animationplayer.h \
    include/playerbutton.h \
    include/texture.h \
    include/line_gl33.h \
    include/frame_gl33.h \
    include/skybox_gl33.h \
    include/abstractcomplex_gl33.h \
    include/animationwindow.h

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
