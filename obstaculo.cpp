#include "obstaculo.h"

Obstacle::Obstacle()
    : m_id(0),
      m_rect(),
      m_restitution(0.7),
      m_resistance(100.0)
{
}

Obstacle::Obstacle(int id, const QRectF &rect, double restitution, double resistance)
    : m_id(id),
      m_rect(rect),
      m_restitution(restitution),
      m_resistance(resistance)
{
}

int Obstacle::id() const
{
    return m_id;
}

QRectF Obstacle::rect() const
{
    return m_rect;
}

double Obstacle::restitution() const
{
    return m_restitution;
}

double Obstacle::resistance() const
{
    return m_resistance;
}

void Obstacle::setResistance(double resistance)
{
    m_resistance = resistance;
}
