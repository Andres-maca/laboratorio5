#ifndef PARTICULA_H
#define PARTICULA_H

#include <fstream>

class Particula
{
public:
    Particula(double xIn, double yIn, double velIn, double angIn, double gravedad);

    void actualizarPosicion(double dt, double ancho, double alto, std::ofstream &archivo);
    void actualizarValores(double xIn, double yIn);
    bool estaQuieto() const;

private:
    double x;
    double y;
    double velX;
    double velY;
    double g;
    double tiempoX;
    double tiempoY;
    double ang;
    int dir;
    bool colX;
};

#endif
