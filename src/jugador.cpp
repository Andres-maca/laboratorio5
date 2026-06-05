#include "jugador.h"

Jugador::Jugador()
    : m_id(0),
      m_nombre(),
      m_posicionDisparo(),
      m_representante()
{
}

Jugador::Jugador(int id, const QString &nombre, const QPointF &posicionDisparo, const QRectF &representante)
    : m_id(id),
      m_nombre(nombre),
      m_posicionDisparo(posicionDisparo),
      m_representante(representante)
{
}

int Jugador::id() const
{
    return m_id;
}

QString Jugador::nombre() const
{
    return m_nombre;
}

QPointF Jugador::posicionDisparo() const
{
    return m_posicionDisparo;
}

QRectF Jugador::representante() const
{
    return m_representante;
}
