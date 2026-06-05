QT += core gui widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/evento_colision.cpp \
        src/infraestructura.cpp \
        src/juego_turnos.cpp \
        src/jugador.cpp \
        src/obstaculo.cpp \
        src/particula_simulada.cpp \
        src/proyectil.cpp \
        src/vector2d.cpp \
        src/main.cpp

HEADERS += \
        src/evento_colision.h \
        src/infraestructura.h \
        src/juego_turnos.h \
        src/jugador.h \
        src/obstaculo.h \
        src/particula_simulada.h \
        src/proyectil.h \
        src/vector2d.h

RESOURCES += \
        resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
