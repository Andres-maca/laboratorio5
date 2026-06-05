#include "evento_colision.h"

CollisionEvent::CollisionEvent()
    : m_time(0.0),
      m_type(),
      m_description()
{
}

CollisionEvent::CollisionEvent(double time, const QString &type, const QString &description)
    : m_time(time),
      m_type(type),
      m_description(description)
{
}

double CollisionEvent::time() const
{
    return m_time;
}

QString CollisionEvent::type() const
{
    return m_type;
}

QString CollisionEvent::description() const
{
    return m_description;
}
