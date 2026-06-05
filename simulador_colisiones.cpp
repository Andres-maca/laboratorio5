#include "simulador_colisiones.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QStringList>
#include <QTextStream>
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
    m_positionRows.clear();
    m_time = 0.0;
}

void CollisionSimulator::run()
{
    m_positionRows.clear();
    m_events.clear();
    m_time = 0.0;
    recordPositions();

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
    recordPositions();
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

bool CollisionSimulator::writePositionCsv(const QString &filePath) const
{
    QDir().mkpath(QFileInfo(filePath).absolutePath());

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out << "time,particle,x,y,vx,vy,mass,radius\n";
    for (const QString &row : m_positionRows) {
        out << row << '\n';
    }

    return true;
}

bool CollisionSimulator::writeCollisionLog(const QString &filePath) const
{
    QDir().mkpath(QFileInfo(filePath).absolutePath());

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out << "time,type,description\n";
    for (const CollisionEvent &event : m_events) {
        out << QString::number(event.time(), 'f', 3) << ','
            << event.type() << ','
            << '"' << event.description() << '"' << '\n';
    }

    return true;
}

bool CollisionSimulator::writeSvgGraph(const QString &filePath) const
{
    QDir().mkpath(QFileInfo(filePath).absolutePath());

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    const double margin = 40.0;
    const double svgWidth = m_config.width + margin * 2.0;
    const double svgHeight = m_config.height + margin * 2.0;
    const QStringList colors = {"#d7263d", "#1b998b", "#2e86ab", "#f46036", "#6a4c93", "#2d3047"};

    QTextStream out(&file);
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << svgWidth
        << "\" height=\"" << svgHeight << "\" viewBox=\"0 0 " << svgWidth << ' ' << svgHeight << "\">\n";
    out << "<rect width=\"100%\" height=\"100%\" fill=\"#f7f7f2\"/>\n";
    out << "<rect x=\"" << margin << "\" y=\"" << margin << "\" width=\"" << m_config.width
        << "\" height=\"" << m_config.height << "\" fill=\"white\" stroke=\"#1f2933\" stroke-width=\"2\"/>\n";

    for (const Obstacle &obstacle : m_obstacles) {
        const QRectF rect = obstacle.rect();
        out << "<rect x=\"" << margin + rect.x() << "\" y=\"" << margin + rect.y()
            << "\" width=\"" << rect.width() << "\" height=\"" << rect.height()
            << "\" fill=\"#9ca3af\" stroke=\"#374151\" stroke-width=\"2\"/>\n";
    }

    QMap<QString, QString> paths;
    for (const QString &row : m_positionRows) {
        const QStringList values = row.split(',');
        if (values.size() < 4) {
            continue;
        }

        const QString label = values.at(1);
        const double x = values.at(2).toDouble();
        const double y = values.at(3).toDouble();
        paths[label] += QString("%1,%2 ").arg(margin + x, 0, 'f', 2).arg(margin + y, 0, 'f', 2);
    }

    int colorIndex = 0;
    for (auto it = paths.constBegin(); it != paths.constEnd(); ++it) {
        const QString color = colors.at(colorIndex % colors.size());
        out << "<polyline points=\"" << it.value()
            << "\" fill=\"none\" stroke=\"" << color
            << "\" stroke-width=\"2\" stroke-opacity=\"0.85\"/>\n";
        out << "<text x=\"12\" y=\"" << 24 + colorIndex * 18
            << "\" fill=\"" << color << "\" font-family=\"Arial\" font-size=\"14\">"
            << it.key() << "</text>\n";
        ++colorIndex;
    }

    for (const Particle &particle : m_particles) {
        out << "<circle cx=\"" << margin + particle.position().x
            << "\" cy=\"" << margin + particle.position().y
            << "\" r=\"" << particle.radius()
            << "\" fill=\"#111827\" fill-opacity=\"0.25\" stroke=\"#111827\"/>\n";
    }

    out << "</svg>\n";
    return true;
}

void CollisionSimulator::recordPositions()
{
    for (const Particle &particle : m_particles) {
        const Vector2 position = particle.position();
        const Vector2 velocity = particle.velocity();
        m_positionRows.append(QString("%1,%2,%3,%4,%5,%6,%7,%8")
                                  .arg(m_time, 0, 'f', 3)
                                  .arg(particle.label())
                                  .arg(position.x, 0, 'f', 3)
                                  .arg(position.y, 0, 'f', 3)
                                  .arg(velocity.x, 0, 'f', 3)
                                  .arg(velocity.y, 0, 'f', 3)
                                  .arg(particle.mass(), 0, 'f', 3)
                                  .arg(particle.radius(), 0, 'f', 3));
    }
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
