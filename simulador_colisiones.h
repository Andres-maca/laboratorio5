#ifndef COLLISIONSIMULATOR_H
#define COLLISIONSIMULATOR_H

#include <QList>
#include <QString>

#include "evento_colision.h"
#include "obstaculo.h"
#include "particula_simulada.h"

struct SimulationConfig
{
    double width = 900.0;
    double height = 650.0;
    double dt = 0.02;
    double duration = 20.0;
    double obstacleRestitution = 0.65;
};

class CollisionSimulator
{
public:
    explicit CollisionSimulator(const SimulationConfig &config);

    void addParticle(const Particle &particle);
    void addObstacle(const Obstacle &obstacle);
    void clear();
    void run();
    void step();

    QList<Particle> particles() const;
    QList<Obstacle> obstacles() const;
    QList<CollisionEvent> events() const;
    double time() const;
    SimulationConfig config() const;

    bool writePositionCsv(const QString &filePath) const;
    bool writeCollisionLog(const QString &filePath) const;
    bool writeSvgGraph(const QString &filePath) const;

private:
    void recordPositions();
    void resolveWallCollisions(Particle &particle);
    void resolveObstacleCollisions(Particle &particle);
    bool resolveParticleCollisions();
    void addEvent(const QString &type, const QString &description);
    static double clamp(double value, double minValue, double maxValue);

    SimulationConfig m_config;
    QList<Particle> m_particles;
    QList<Obstacle> m_obstacles;
    QList<CollisionEvent> m_events;
    QList<QString> m_positionRows;
    double m_time;
};

#endif
