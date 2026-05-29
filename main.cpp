#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include <fstream>

#include "particula.h"
#include "simulador_colisiones.h"

static void configurePracticeScenario(CollisionSimulator &simulator)
{
    const SimulationConfig config = simulator.config();

    simulator.addObstacle(Obstacle(1, QRectF(230, 120, 70, 70), config.obstacleRestitution));
    simulator.addObstacle(Obstacle(2, QRectF(580, 110, 80, 80), config.obstacleRestitution));
    simulator.addObstacle(Obstacle(3, QRectF(340, 410, 90, 90), config.obstacleRestitution));
    simulator.addObstacle(Obstacle(4, QRectF(680, 360, 65, 65), config.obstacleRestitution));

    simulator.addParticle(Particle(1, Vector2(100, 120), Vector2(95, 54), 2.0, 16.0));
    simulator.addParticle(Particle(2, Vector2(780, 175), Vector2(-110, 35), 3.0, 18.0));
    simulator.addParticle(Particle(3, Vector2(180, 545), Vector2(130, -82), 1.6, 15.0));
    simulator.addParticle(Particle(4, Vector2(735, 535), Vector2(-88, -72), 2.4, 17.0));
}

static bool runProvidedParticleExample(const QString &filePath)
{
    QDir().mkpath(QFileInfo(filePath).absolutePath());

    std::ofstream archivo(filePath.toStdString());
    if (!archivo.is_open()) {
        return false;
    }

    Particula particle(50.0, 250.0, 100.0, 65.0, 9.81);

    const double dt = 0.1;
    const double ancho = 900.0;
    const double alto = 700.0;
    int steps = 0;
    const int maxSteps = 2000;

    while (!particle.estaQuieto() && steps < maxSteps) {
        particle.actualizarPosicion(dt, ancho, alto, archivo);
        ++steps;
    }

    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    SimulationConfig config;
    config.width = 900.0;
    config.height = 650.0;
    config.dt = 0.02;
    config.duration = 18.0;
    config.obstacleRestitution = 0.62;

    CollisionSimulator simulator(config);
    configurePracticeScenario(simulator);
    simulator.run();

    const QString outputDir = QDir::currentPath() + "/output";
    const QString positionsFile = outputDir + "/posiciones.csv";
    const QString collisionsFile = outputDir + "/colisiones.csv";
    const QString graphFile = outputDir + "/trayectoria.svg";
    const QString providedParticleFile = outputDir + "/trayectoria_particula.txt";

    const bool positionsOk = simulator.writePositionCsv(positionsFile);
    const bool collisionsOk = simulator.writeCollisionLog(collisionsFile);
    const bool graphOk = simulator.writeSvgGraph(graphFile);
    const bool providedParticleOk = runProvidedParticleExample(providedParticleFile);

    qInfo() << "Simulacion finalizada.";
    qInfo() << "Particulas restantes:" << simulator.particles().size();
    qInfo() << "Colisiones registradas:" << simulator.events().size();
    qInfo() << "Archivo de posiciones:" << positionsFile << positionsOk;
    qInfo() << "Archivo de colisiones:" << collisionsFile << collisionsOk;
    qInfo() << "Grafica SVG:" << graphFile << graphOk;
    qInfo() << "Trayectoria con clase Particula entregada:" << providedParticleFile << providedParticleOk;

    return (positionsOk && collisionsOk && graphOk && providedParticleOk) ? 0 : 1;
}
