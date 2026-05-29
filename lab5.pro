QT = core

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        evento_colision.cpp \
        simulador_colisiones.cpp \
        obstaculo.cpp \
        particula_simulada.cpp \
        particula.cpp \
        vector2d.cpp \
        main.cpp

HEADERS += \
        evento_colision.h \
        simulador_colisiones.h \
        obstaculo.h \
        particula_simulada.h \
        particula.h \
        vector2d.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
