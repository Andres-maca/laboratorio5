#include "infraestructura.h"

#include <QtMath>

Infraestructura::Infraestructura()
    : m_duenoId(0),
      m_obstaculo(0, QRectF(), 0.55, 0.0),
      m_resistenciaInicial(0.0)
{
}

Infraestructura::Infraestructura(int id, int duenoId, const QRectF &rect, double resistencia, double restitucion)
    : m_duenoId(duenoId),
      m_obstaculo(id, rect, restitucion, resistencia),
      m_resistenciaInicial(resistencia)
{
}

int Infraestructura::id() const
{
    return m_obstaculo.id();
}

int Infraestructura::duenoId() const
{
    return m_duenoId;
}

QRectF Infraestructura::rect() const
{
    return m_obstaculo.rect();
}

double Infraestructura::resistencia() const
{
    return m_obstaculo.resistance();
}

double Infraestructura::resistenciaInicial() const
{
    return m_resistenciaInicial;
}

double Infraestructura::restitucion() const
{
    return m_obstaculo.restitution();
}

bool Infraestructura::destruida() const
{
    return m_obstaculo.resistance() <= 0.0;
}

void Infraestructura::aplicarDano(double dano)
{
    m_obstaculo.setResistance(qMax(0.0, m_obstaculo.resistance() - dano));
}
