#ifndef JUEGO_TURNOS_H
#define JUEGO_TURNOS_H

#include <QList>
#include <QString>

#include "evento_colision.h"
#include "infraestructura.h"
#include "jugador.h"
#include "proyectil.h"

struct JuegoConfig
{
    double width = 1000.0;
    double height = 650.0;
    double dt = 0.016;
    double gravity = 120.0;
    double limitRestitution = 0.82;
    double infrastructureRestitution = 0.45;
    double infrastructureFriction = 0.92;
    double damageFactor = 0.08;
    double projectileMass = 3.0;
    double projectileRadius = 9.0;
    double maxShotTime = 8.0;
    double floorHeight = 25.0;
};

class JuegoTurnos
{
public:
    explicit JuegoTurnos(const JuegoConfig &config = JuegoConfig());

    void reiniciar();
    bool disparar(double anguloGrados, double velocidadInicial);
    void avanzar();

    QList<Jugador> jugadores() const;
    QList<Infraestructura> infraestructuras() const;
    QList<CollisionEvent> eventos() const;
    Proyectil proyectil() const;
    Jugador jugadorActual() const;
    double tiempo() const;
    bool turnoEnCurso() const;
    int ganadorId() const;
    JuegoConfig config() const;

private:
    void inicializarEscenario();
    void resolverLimites();
    void resolverInfraestructura();
    void resolverRepresentante();
    void terminarTurno();
    void registrarEvento(const QString &type, const QString &description);
    bool infraestructuraRivalDestruida(int atacanteId) const;
    static double clamp(double value, double minValue, double maxValue);
    static bool circuloTocaRect(const Vector2 &center, double radius, const QRectF &rect);

    JuegoConfig m_config;
    QList<Jugador> m_jugadores;
    QList<Infraestructura> m_infraestructuras;
    QList<CollisionEvent> m_eventos;
    Proyectil m_proyectil;
    int m_turnoIndex;
    int m_ganadorId;
    bool m_turnoEnCurso;
    double m_tiempo;
    double m_tiempoDisparo;
};

#endif
