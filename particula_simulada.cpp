#include "particula_simulada.h"

Particle::Particle()
    : m_id(0),
      m_position(),
      m_velocity(),
      m_mass(1.0),
      m_radius(1.0),
      m_label("P0")
{
}

Particle::Particle(int id, const Vector2 &position, const Vector2 &velocity, double mass, double radius)
    : m_id(id),
      m_position(position),
      m_velocity(velocity),
      m_mass(mass),
      m_radius(radius),
      m_label(QString("P%1").arg(id))
{
}

int Particle::id() const
{
    return m_id;
}

Vector2 Particle::position() const
{
    return m_position;
}

Vector2 Particle::velocity() const
{
    return m_velocity;
}

double Particle::mass() const
{
    return m_mass;
}

double Particle::radius() const
{
    return m_radius;
}

QString Particle::label() const
{
    return m_label;
}

void Particle::setPosition(const Vector2 &position)
{
    m_position = position;
}

void Particle::setVelocity(const Vector2 &velocity)
{
    m_velocity = velocity;
}

void Particle::setMass(double mass)
{
    m_mass = mass;
}

void Particle::setRadius(double radius)
{
    m_radius = radius;
}

void Particle::appendMergedId(int id)
{
    m_label += QString("+P%1").arg(id);
}

void Particle::setLabel(const QString &label)
{
    m_label = label;
}

void Particle::move(double dt)
{
    m_position += m_velocity * dt;
}
