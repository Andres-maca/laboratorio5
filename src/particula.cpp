#include "particula.h"

#include <cmath>

Particula::Particula(double xIn, double yIn, double velIn, double angIn, double gravedad)
    : x(xIn),
      y(yIn),
      velX(0.0),
      velY(0.0),
      g(gravedad),
      tiempoX(0.0),
      tiempoY(0.0),
      ang(angIn),
      dir(1),
      colX(true)
{
    constexpr double pi = 3.14159265358979323846;
    velX = velIn * std::cos(ang * (pi / 180.0));
    velY = velIn * std::sin(ang * (pi / 180.0));
}

void Particula::actualizarPosicion(double dt, double ancho, double alto, std::ofstream &archivo)
{
    constexpr double pi = 3.14159265358979323846;

    double xNuevo = x + velX * tiempoX * dir;
    double yNuevo = y - (velY * tiempoY - 0.5 * g * tiempoY * tiempoY);

    archivo << xNuevo << ' ' << yNuevo << ' ' << velX << ' ' << velY << '\n';

    if (xNuevo > ancho || xNuevo < 0.0) {
        colX = true;
        actualizarValores(xNuevo, yNuevo);
    }

    if (yNuevo > alto) {
        yNuevo = alto;
        colX = false;
        actualizarValores(xNuevo, yNuevo);

        velY *= 0.7;
        if (std::fabs(velY) < 1.0) {
            velY = 0.0;
        }

        ang = -std::atan2(velY, velX) * 180.0 / pi;
    }

    tiempoX += dt;
    tiempoY += dt;
}

void Particula::actualizarValores(double xIn, double yIn)
{
    if (colX) {
        dir *= -1;
        x = xIn;
        y = yIn;
        tiempoX = 0.0;
    } else {
        x = xIn;
        y = yIn;
        tiempoY = 0.0;
        tiempoX = 0.0;
    }
}

bool Particula::estaQuieto() const
{
    return velY == 0.0;
}
