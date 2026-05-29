#include "simulador_colisiones.h"

#include <QtMath>

CollisionSimulator::CollisionSimulator(const SimulationConfig &config)
    : m_config(config),
      m_time(0.0)
{
}

void CollisionSimulator::addParticle(const Particle &particle)
{
    m_particles.append(particle);
}

void CollisionSimulator::addObstacle(const Obstacle &obstacle)
{
    m_obstacles.append(obstacle);
}

void CollisionSimulator::clear()
{
    m_particles.clear();
    m_obstacles.clear();
    m_events.clear();
    m_time = 0.0;
}

void CollisionSimulator::run()
{
    m_events.clear();
    m_time = 0.0;

    while (m_time < m_config.duration && !m_particles.isEmpty()) {
        step();
    }
}

void CollisionSimulator::step()
{
    for (Particle &particle : m_particles) {
        particle.move(m_config.dt);
        resolveWallCollisions(particle);
        resolveObstacleCollisions(particle);
    }

    bool merged = true;
    while (merged) {
        merged = resolveParticleCollisions();
    }

    m_time += m_config.dt;
}

QList<Particle> CollisionSimulator::particles() const
{
    return m_particles;
}

QList<Obstacle> CollisionSimulator::obstacles() const
{
    return m_obstacles;
}

QList<CollisionEvent> CollisionSimulator::events() const
{
    return m_events;
}

double CollisionSimulator::time() const
{
    return m_time;
}

SimulationConfig CollisionSimulator::config() const
{
    return m_config;
}

void CollisionSimulator::resolveWallCollisions(Particle &particle)
{
    Vector2 position = particle.position();
    Vector2 velocity = particle.velocity();
    bool collided = false;
    QStringList walls;

    if (position.x - particle.radius() < 0.0) {
        position.x = particle.radius();
        velocity.x = qAbs(velocity.x);
        collided = true;
        walls.append("izquierda");
    } else if (position.x + particle.radius() > m_config.width) {
        position.x = m_config.width - particle.radius();
        velocity.x = -qAbs(velocity.x);
        collided = true;
        walls.append("derecha");
    }

    if (position.y - particle.radius() < 0.0) {
        position.y = particle.radius();
        velocity.y = qAbs(velocity.y);
        collided = true;
        walls.append("superior");
    } else if (position.y + particle.radius() > m_config.height) {
        position.y = m_config.height - particle.radius();
        velocity.y = -qAbs(velocity.y);
        collided = true;
        walls.append("inferior");
    }

    if (collided) {
        particle.setPosition(position);
        particle.setVelocity(velocity);
        addEvent("pared-elastica",
                 QString("%1 rebota en pared %2 con velocidad (%3,%4)")
                     .arg(particle.label())
                     .arg(walls.join("+"))
                     .arg(velocity.x, 0, 'f', 2)
                     .arg(velocity.y, 0, 'f', 2));
    }
}

void CollisionSimulator::resolveObstacleCollisions(Particle &particle)
{
    for (const Obstacle &obstacle : m_obstacles) {
        const QRectF rect = obstacle.rect();
        Vector2 position = particle.position();
        Vector2 velocity = particle.velocity();

        const double closestX = clamp(position.x, rect.left(), rect.right());
        const double closestY = clamp(position.y, rect.top(), rect.bottom());
        Vector2 normal = position - Vector2(closestX, closestY);
        double distance = normal.length();

        if (distance > particle.radius()) {
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
                position.x = rect.left() - particle.radius();
            } else if (minDistance == rightDistance) {
                normal = Vector2(1.0, 0.0);
                position.x = rect.right() + particle.radius();
            } else if (minDistance == topDistance) {
                normal = Vector2(0.0, -1.0);
                position.y = rect.top() - particle.radius();
            } else {
                normal = Vector2(0.0, 1.0);
                position.y = rect.bottom() + particle.radius();
            }
        } else {
            normal = normal.normalized();
            position += normal * (particle.radius() - distance + 0.01);
        }

        const double normalVelocity = Vector2::dot(velocity, normal);
        const Vector2 perpendicular = normal * normalVelocity;
        const Vector2 parallel = velocity - perpendicular;
        velocity = parallel - perpendicular * obstacle.restitution();

        particle.setPosition(position);
        particle.setVelocity(velocity);

        addEvent("obstaculo-inelastico",
                 QString("%1 choca con obstaculo O%2, e=%3, velocidad final (%4,%5)")
                     .arg(particle.label())
                     .arg(obstacle.id())
                     .arg(obstacle.restitution(), 0, 'f', 2)
                     .arg(velocity.x, 0, 'f', 2)
                     .arg(velocity.y, 0, 'f', 2));
    }
}

bool CollisionSimulator::resolveParticleCollisions()
{
    for (int i = 0; i < m_particles.size(); ++i) {
        for (int j = i + 1; j < m_particles.size(); ++j) {
            Particle first = m_particles.at(i);
            Particle second = m_particles.at(j);
            const Vector2 delta = second.position() - first.position();
            const double minDistance = first.radius() + second.radius();

            if (delta.lengthSquared() > minDistance * minDistance) {
                continue;
            }

            const double totalMass = first.mass() + second.mass();
            const Vector2 newPosition = (first.position() * first.mass() + second.position() * second.mass()) / totalMass;
            const Vector2 newVelocity = (first.velocity() * first.mass() + second.velocity() * second.mass()) / totalMass;
            const double newRadius = std::sqrt(first.radius() * first.radius() + second.radius() * second.radius());

            Particle merged(first.id(), newPosition, newVelocity, totalMass, newRadius);
            merged.setLabel(first.label() + "+" + second.label());

            m_particles[i] = merged;
            m_particles.removeAt(j);

            addEvent("particulas-completamente-inelastico",
                     QString("%1 y %2 se unen: masa=%3, velocidad=(%4,%5)")
                         .arg(first.label())
                         .arg(second.label())
                         .arg(totalMass, 0, 'f', 2)
                         .arg(newVelocity.x, 0, 'f', 2)
                         .arg(newVelocity.y, 0, 'f', 2));
            return true;
        }
    }

    return false;
}

void CollisionSimulator::addEvent(const QString &type, const QString &description)
{
    m_events.append(CollisionEvent(m_time, type, description));
}

double CollisionSimulator::clamp(double value, double minValue, double maxValue)
{
    return qMax(minValue, qMin(value, maxValue));
}
