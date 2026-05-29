#ifndef PARTICLE_H
#define PARTICLE_H

#include <QString>

#include "vector2d.h"

class Particle
{
public:
    Particle();
    Particle(int id, const Vector2 &position, const Vector2 &velocity, double mass, double radius);

    int id() const;
    Vector2 position() const;
    Vector2 velocity() const;
    double mass() const;
    double radius() const;
    QString label() const;

    void setPosition(const Vector2 &position);
    void setVelocity(const Vector2 &velocity);
    void setMass(double mass);
    void setRadius(double radius);
    void appendMergedId(int id);
    void setLabel(const QString &label);

    void move(double dt);

private:
    int m_id;
    Vector2 m_position;
    Vector2 m_velocity;
    double m_mass;
    double m_radius;
    QString m_label;
};

#endif
