#include "juego_turnos.h"

#include <QStringList>
#include <QtMath>

JuegoTurnos::JuegoTurnos(const JuegoConfig &config)
    : m_config(config),
      m_turnoIndex(0),
      m_ganadorId(0),
      m_turnoEnCurso(false),
      m_tiempo(0.0),
      m_tiempoDisparo(0.0)
{
    inicializarEscenario();
}

void JuegoTurnos::reiniciar()
{
    m_jugadores.clear();
    m_infraestructuras.clear();
    m_eventos.clear();
    m_proyectil = Proyectil();
    m_turnoIndex = 0;
    m_ganadorId = 0;
    m_turnoEnCurso = false;
    m_tiempo = 0.0;
    m_tiempoDisparo = 0.0;
    inicializarEscenario();
}

bool JuegoTurnos::disparar(double anguloGrados, double velocidadInicial)
{
    if (m_turnoEnCurso || m_ganadorId != 0 || m_jugadores.isEmpty()) {
        return false;
    }

    const Jugador jugador = jugadorActual();
    const double radians = qDegreesToRadians(anguloGrados);
    const double direction = jugador.id() == 1 ? 1.0 : -1.0;
    const Vector2 velocity(direction * velocidadInicial * qCos(radians),
                           -velocidadInicial * qSin(radians));
    const QPointF spawn = jugador.posicionDisparo();

    m_proyectil = Proyectil(Vector2(spawn.x(), spawn.y()),
                            velocity,
                            m_config.projectileMass,
                            m_config.projectileRadius,
                            jugador.id());
    m_turnoEnCurso = true;
    m_tiempoDisparo = 0.0;

    registrarEvento("turno", QString("%1 dispara con angulo %2 y velocidad %3")
                                 .arg(jugador.nombre())
                                 .arg(anguloGrados, 0, 'f', 1)
                                 .arg(velocidadInicial, 0, 'f', 1));
    return true;
}

void JuegoTurnos::avanzar()
{
    if (!m_turnoEnCurso || !m_proyectil.active() || m_ganadorId != 0) {
        return;
    }

    Vector2 velocity = m_proyectil.velocity();
    velocity.y += m_config.gravity * m_config.dt;
    m_proyectil.setVelocity(velocity);
    m_proyectil.move(m_config.dt);

    resolverLimites();
    resolverInfraestructura();
    resolverRepresentante();

    m_tiempo += m_config.dt;
    m_tiempoDisparo += m_config.dt;

    if (m_turnoEnCurso && m_tiempoDisparo >= m_config.maxShotTime) {
        registrarEvento("fin-turno", "El proyectil pierde oportunidad de impacto");
        terminarTurno();
    }
}

QList<Jugador> JuegoTurnos::jugadores() const
{
    return m_jugadores;
}

QList<Infraestructura> JuegoTurnos::infraestructuras() const
{
    return m_infraestructuras;
}

QList<CollisionEvent> JuegoTurnos::eventos() const
{
    return m_eventos;
}

Proyectil JuegoTurnos::proyectil() const
{
    return m_proyectil;
}

Jugador JuegoTurnos::jugadorActual() const
{
    return m_jugadores.at(m_turnoIndex);
}

double JuegoTurnos::tiempo() const
{
    return m_tiempo;
}

bool JuegoTurnos::turnoEnCurso() const
{
    return m_turnoEnCurso;
}

int JuegoTurnos::ganadorId() const
{
    return m_ganadorId;
}

JuegoConfig JuegoTurnos::config() const
{
    return m_config;
}

void JuegoTurnos::inicializarEscenario()
{
    const double playerY = m_config.height / 2.0;
    const double frameWidth = 135.0;
    const double frameHeight = 175.0;
    const double wallThickness = 24.0;
    const double frameTop = m_config.height - frameHeight - 25.0;
    const double leftFrameX = 135.0;
    const double rightFrameX = m_config.width - leftFrameX - frameWidth;

    m_jugadores.append(Jugador(1, "Jugador 1", QPointF(80, playerY),
                               QRectF(leftFrameX + 6.0, frameTop + frameHeight - 60.0, 120.0, 60.0)));
    m_jugadores.append(Jugador(2, "Jugador 2", QPointF(m_config.width - 80, playerY),
                               QRectF(rightFrameX + 6.0, frameTop + frameHeight - 60.0, 120.0, 60.0)));

    m_infraestructuras.append(Infraestructura(1, 1, QRectF(leftFrameX, frameTop, frameWidth, wallThickness), 150, m_config.infrastructureRestitution));
    m_infraestructuras.append(Infraestructura(2, 1, QRectF(leftFrameX, frameTop + wallThickness, wallThickness, frameHeight - wallThickness), 180, m_config.infrastructureRestitution));
    m_infraestructuras.append(Infraestructura(3, 1, QRectF(leftFrameX + frameWidth - wallThickness, frameTop + wallThickness, wallThickness, frameHeight - wallThickness), 180, m_config.infrastructureRestitution));

    m_infraestructuras.append(Infraestructura(4, 2, QRectF(rightFrameX, frameTop, frameWidth, wallThickness), 150, m_config.infrastructureRestitution));
    m_infraestructuras.append(Infraestructura(5, 2, QRectF(rightFrameX, frameTop + wallThickness, wallThickness, frameHeight - wallThickness), 180, m_config.infrastructureRestitution));
    m_infraestructuras.append(Infraestructura(6, 2, QRectF(rightFrameX + frameWidth - wallThickness, frameTop + wallThickness, wallThickness, frameHeight - wallThickness), 180, m_config.infrastructureRestitution));
}

void JuegoTurnos::resolverLimites()
{
    Vector2 position = m_proyectil.position();
    Vector2 velocity = m_proyectil.velocity();
    bool collided = false;
    QStringList limites;

    if (position.x - m_proyectil.radius() < 0.0) {
        position.x = m_proyectil.radius();
        velocity.x = qAbs(velocity.x) * m_config.limitRestitution;
        velocity.y *= m_config.limitRestitution;
        collided = true;
        limites.append("izquierdo");
    } else if (position.x + m_proyectil.radius() > m_config.width) {
        position.x = m_config.width - m_proyectil.radius();
        velocity.x = -qAbs(velocity.x) * m_config.limitRestitution;
        velocity.y *= m_config.limitRestitution;
        collided = true;
        limites.append("derecho");
    }

    if (position.y - m_proyectil.radius() < 0.0) {
        position.y = m_proyectil.radius();
        velocity.x *= m_config.limitRestitution;
        velocity.y = qAbs(velocity.y) * m_config.limitRestitution;
        collided = true;
        limites.append("superior");
    }

    const double floorLimit = m_config.height - m_config.floorHeight;
    if (position.y + m_proyectil.radius() > floorLimit) {
        position.y = floorLimit - m_proyectil.radius();
        velocity.x *= m_config.limitRestitution;
        velocity.y = -qAbs(velocity.y) * m_config.limitRestitution;
        collided = true;
        limites.append("piso");
    }

    if (collided) {
        m_proyectil.setPosition(position);
        m_proyectil.setVelocity(velocity);
        registrarEvento("limite-con-perdida", QString("Rebote contra limite %1 con perdida de energia")
                                             .arg(limites.join("+")));
    }
}

void JuegoTurnos::resolverInfraestructura()
{
    for (Infraestructura &infra : m_infraestructuras) {
        if (infra.destruida() || infra.duenoId() == m_proyectil.duenoId()) {
            continue;
        }

        Vector2 position = m_proyectil.position();
        Vector2 velocity = m_proyectil.velocity();
        const QRectF rect = infra.rect();
        const double closestX = clamp(position.x, rect.left(), rect.right());
        const double closestY = clamp(position.y, rect.top(), rect.bottom());
        Vector2 normal = position - Vector2(closestX, closestY);
        double distance = normal.length();

        if (distance > m_proyectil.radius()) {
            continue;
        }

        if (distance == 0.0) {
            const double leftDistance = qAbs(position.x - rect.left());
            const double rightDistance = qAbs(rect.right() - position.x);
            const double topDistance = qAbs(position.y - rect.top());
            const double bottomDistance = qAbs(rect.bottom() - position.y);
            const double minDistance = qMin(qMin(leftDistance, rightDistance), qMin(topDistance, bottomDistance));

            if (minDistance == leftDistance) {
                normal = Vector2(-1.0, 0.0);
                position.x = rect.left() - m_proyectil.radius();
            } else if (minDistance == rightDistance) {
                normal = Vector2(1.0, 0.0);
                position.x = rect.right() + m_proyectil.radius();
            } else if (minDistance == topDistance) {
                normal = Vector2(0.0, -1.0);
                position.y = rect.top() - m_proyectil.radius();
            } else {
                normal = Vector2(0.0, 1.0);
                position.y = rect.bottom() + m_proyectil.radius();
            }
        } else {
            normal = normal.normalized();
            position += normal * (m_proyectil.radius() - distance + 0.01);
        }

        const double impactSpeed = velocity.length();
        const double dano = m_config.damageFactor * m_proyectil.mass() * impactSpeed;
        infra.aplicarDano(dano);

        const double normalVelocity = Vector2::dot(velocity, normal);
        const Vector2 perpendicular = normal * normalVelocity;
        const Vector2 parallel = velocity - perpendicular;
        velocity = parallel * m_config.infrastructureFriction - perpendicular * infra.restitucion();

        m_proyectil.setPosition(position);
        m_proyectil.setVelocity(velocity);

        registrarEvento("infraestructura-inelastica",
                        QString("Impacto en I%1: dano=%2, resistencia=%3")
                            .arg(infra.id())
                            .arg(dano, 0, 'f', 1)
                            .arg(infra.resistencia(), 0, 'f', 1));

        if (infra.destruida()) {
            registrarEvento("infraestructura-destruida", QString("Infraestructura I%1 destruida").arg(infra.id()));
        }
        return;
    }
}

void JuegoTurnos::resolverRepresentante()
{
    const int atacanteId = m_proyectil.duenoId();
    for (const Jugador &jugador : m_jugadores) {
        if (jugador.id() == atacanteId) {
            continue;
        }

        if (!circuloTocaRect(m_proyectil.position(), m_proyectil.radius(), jugador.representante())) {
            continue;
        }

        m_ganadorId = atacanteId;
        registrarEvento("victoria", QString("%1 golpea al slime rival").arg(jugadorActual().nombre()));
        terminarTurno();
        return;
    }
}

void JuegoTurnos::terminarTurno()
{
    m_proyectil.setActive(false);
    m_turnoEnCurso = false;

    if (m_ganadorId == 0 && !m_jugadores.isEmpty()) {
        m_turnoIndex = (m_turnoIndex + 1) % m_jugadores.size();
    }
}

void JuegoTurnos::registrarEvento(const QString &type, const QString &description)
{
    m_eventos.append(CollisionEvent(m_tiempo, type, description));
}

bool JuegoTurnos::infraestructuraRivalDestruida(int atacanteId) const
{
    for (const Infraestructura &infra : m_infraestructuras) {
        if (infra.duenoId() != atacanteId && !infra.destruida()) {
            return false;
        }
    }

    return true;
}

double JuegoTurnos::clamp(double value, double minValue, double maxValue)
{
    return qMax(minValue, qMin(value, maxValue));
}

bool JuegoTurnos::circuloTocaRect(const Vector2 &center, double radius, const QRectF &rect)
{
    const double closestX = clamp(center.x, rect.left(), rect.right());
    const double closestY = clamp(center.y, rect.top(), rect.bottom());
    const Vector2 difference = center - Vector2(closestX, closestY);
    return difference.lengthSquared() <= radius * radius;
}
