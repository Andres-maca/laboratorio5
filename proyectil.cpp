#include "proyectil.h"

Proyectil::Proyectil()
    : m_particle(0, Vector2(), Vector2(), 1.0, 8.0),
      m_duenoId(0),
      m_active(false)
{
}

Proyectil::Proyectil(const Vector2 &position, const Vector2 &velocity, double mass, double radius, int duenoId)
    : m_particle(0, position, velocity, mass, radius),
      m_duenoId(duenoId),
      m_active(true)
{
}

Vector2 Proyectil::position() const
{
    return m_particle.position();
}

Vector2 Proyectil::velocity() const
{
    return m_particle.velocity();
}

double Proyectil::mass() const
{
    return m_particle.mass();
}

double Proyectil::radius() const
{
    return m_particle.radius();
}

int Proyectil::duenoId() const
{
    return m_duenoId;
}

bool Proyectil::active() const
{
    return m_active;
}

double Proyectil::speed() const
{
    return m_particle.velocity().length();
}

void Proyectil::setPosition(const Vector2 &position)
{
    m_particle.setPosition(position);
}

void Proyectil::setVelocity(const Vector2 &velocity)
{
    m_particle.setVelocity(velocity);
}

void Proyectil::setActive(bool active)
{
    m_active = active;
}

void Proyectil::move(double dt)
{
    m_particle.move(dt);
}
