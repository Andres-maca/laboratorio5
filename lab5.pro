QT += core gui widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        evento_colision.cpp \
        infraestructura.cpp \
        juego_turnos.cpp \
        jugador.cpp \
        obstaculo.cpp \
        particula_simulada.cpp \
        proyectil.cpp \
        vector2d.cpp \
        main.cpp

HEADERS += \
        evento_colision.h \
        infraestructura.h \
        juego_turnos.h \
        jugador.h \
        obstaculo.h \
        particula_simulada.h \
        proyectil.h \
        vector2d.h

RESOURCES += \
        resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
