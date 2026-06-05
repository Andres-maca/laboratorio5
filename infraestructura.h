#ifndef INFRAESTRUCTURA_H
#define INFRAESTRUCTURA_H

#include "obstaculo.h"

class Infraestructura
{
public:
    Infraestructura();
    Infraestructura(int id, int duenoId, const QRectF &rect, double resistencia, double restitucion);

    int id() const;
    int duenoId() const;
    QRectF rect() const;
    double resistencia() const;
    double resistenciaInicial() const;
    double restitucion() const;
    bool destruida() const;

    void aplicarDano(double dano);

private:
    int m_duenoId;
    Obstacle m_obstaculo;
    double m_resistenciaInicial;
};

#endif
