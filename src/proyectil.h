#ifndef PROYECTIL_H
#define PROYECTIL_H

#include "particula_simulada.h"

class Proyectil
{
public:
    Proyectil();
    Proyectil(const Vector2 &position, const Vector2 &velocity, double mass, double radius, int duenoId);

    Vector2 position() const;
    Vector2 velocity() const;
    double mass() const;
    double radius() const;
    int duenoId() const;
    bool active() const;
    double speed() const;

    void setPosition(const Vector2 &position);
    void setVelocity(const Vector2 &velocity);
    void setActive(bool active);
    void move(double dt);

private:
    Particle m_particle;
    int m_duenoId;
    bool m_active;
};

#endif
