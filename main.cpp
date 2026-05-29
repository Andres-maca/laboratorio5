#include <QApplication>
#include <QBrush>
#include <QFrame>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPen>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <QMap>
#include <QPointF>
#include <QVector>

#include "simulador_colisiones.h"

static void configurarEscenarioPractica(CollisionSimulator &simulator)
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

class VentanaSimulacion : public QWidget
{
public:
    explicit VentanaSimulacion(QWidget *parent = nullptr)
        : QWidget(parent),
          m_config(),
          m_simulator(m_config),
          m_scene(new QGraphicsScene(this)),
          m_view(new QGraphicsView(m_scene, this)),
          m_timer(new QTimer(this)),
          m_startButton(new QPushButton("Iniciar", this)),
          m_stepButton(new QPushButton("Paso", this)),
          m_resetButton(new QPushButton("Reiniciar", this)),
          m_speedSlider(new QSlider(Qt::Horizontal, this)),
          m_statsLabel(new QLabel(this)),
          m_eventList(new QListWidget(this)),
          m_lastEventCount(0),
          m_running(false)
    {
        m_config.width = 900.0;
        m_config.height = 650.0;
        m_config.dt = 0.02;
        m_config.duration = 18.0;
        m_config.obstacleRestitution = 0.62;

        m_simulator = CollisionSimulator(m_config);

        setWindowTitle("Practica 5 - Simulacion de colisiones");
        resize(1180, 780);

        m_scene->setSceneRect(0, 0, m_config.width, m_config.height);
        m_view->setRenderHint(QPainter::Antialiasing, true);
        m_view->setMinimumSize(930, 680);
        m_view->setFrameShape(QFrame::NoFrame);
        m_view->setBackgroundBrush(QBrush(QColor("#f4f6f8")));

        m_speedSlider->setRange(1, 10);
        m_speedSlider->setValue(3);

        m_eventList->setMinimumWidth(280);
        m_eventList->setAlternatingRowColors(true);

        auto *controlsLayout = new QHBoxLayout();
        controlsLayout->addWidget(m_startButton);
        controlsLayout->addWidget(m_stepButton);
        controlsLayout->addWidget(m_resetButton);
        controlsLayout->addSpacing(10);
        controlsLayout->addWidget(new QLabel("Velocidad", this));
        controlsLayout->addWidget(m_speedSlider);

        auto *leftLayout = new QVBoxLayout();
        leftLayout->addWidget(m_view);
        leftLayout->addLayout(controlsLayout);

        auto *rightLayout = new QVBoxLayout();
        rightLayout->addWidget(new QLabel("Estado", this));
        rightLayout->addWidget(m_statsLabel);
        rightLayout->addSpacing(12);
        rightLayout->addWidget(new QLabel("Colisiones", this));
        rightLayout->addWidget(m_eventList, 1);

        auto *mainLayout = new QHBoxLayout(this);
        mainLayout->addLayout(leftLayout, 1);
        mainLayout->addLayout(rightLayout);

        connect(m_startButton, &QPushButton::clicked, this, [this]() {
            m_running = !m_running;
            m_startButton->setText(m_running ? "Pausar" : "Iniciar");
            if (m_running) {
                m_timer->start(30);
            } else {
                m_timer->stop();
            }
        });

        connect(m_stepButton, &QPushButton::clicked, this, [this]() {
            avanzarSimulacion(1);
        });

        connect(m_resetButton, &QPushButton::clicked, this, [this]() {
            reiniciarSimulacion();
        });

        connect(m_timer, &QTimer::timeout, this, [this]() {
            avanzarSimulacion(m_speedSlider->value());
        });

        reiniciarSimulacion();
    }

private:
    void reiniciarSimulacion()
    {
        m_timer->stop();
        m_running = false;
        m_startButton->setText("Iniciar");
        m_lastEventCount = 0;
        m_trails.clear();
        m_eventList->clear();

        m_simulator = CollisionSimulator(m_config);
        configurarEscenarioPractica(m_simulator);
        registrarPuntosActuales();
        dibujarEscena();
    }

    void avanzarSimulacion(int steps)
    {
        for (int i = 0; i < steps; ++i) {
            m_simulator.step();
            registrarPuntosActuales();
        }

        actualizarEventos();
        dibujarEscena();
    }

    void registrarPuntosActuales()
    {
        for (const Particle &particle : m_simulator.particles()) {
            const Vector2 position = particle.position();
            QVector<QPointF> &points = m_trails[particle.label()];
            points.append(QPointF(position.x, position.y));
            if (points.size() > 500) {
                points.remove(0, points.size() - 500);
            }
        }
    }

    void actualizarEventos()
    {
        const QList<CollisionEvent> events = m_simulator.events();
        for (int i = m_lastEventCount; i < events.size(); ++i) {
            const CollisionEvent event = events.at(i);
            m_eventList->insertItem(0, QString("%1 s | %2\n%3")
                                           .arg(event.time(), 0, 'f', 2)
                                           .arg(event.type())
                                           .arg(event.description()));
        }

        m_lastEventCount = events.size();

        while (m_eventList->count() > 80) {
            delete m_eventList->takeItem(m_eventList->count() - 1);
        }
    }

    void dibujarEscena()
    {
        m_scene->clear();
        dibujarCaja();
        dibujarObstaculos();
        dibujarTrayectorias();
        dibujarParticulas();
        actualizarEstado();
    }

    void dibujarCaja()
    {
        m_scene->addRect(0, 0, m_config.width, m_config.height,
                         QPen(QColor("#17202a"), 3),
                         QBrush(QColor("#ffffff")));
    }

    void dibujarObstaculos()
    {
        for (const Obstacle &obstacle : m_simulator.obstacles()) {
            QGraphicsRectItem *item = m_scene->addRect(obstacle.rect(),
                                                       QPen(QColor("#4b5563"), 2),
                                                       QBrush(QColor("#9ca3af")));
            item->setToolTip(QString("Obstaculo O%1, e=%2")
                                 .arg(obstacle.id())
                                 .arg(obstacle.restitution(), 0, 'f', 2));

            QGraphicsTextItem *label = m_scene->addText(QString("O%1").arg(obstacle.id()));
            label->setDefaultTextColor(QColor("#111827"));
            label->setPos(obstacle.rect().center().x() - 10, obstacle.rect().center().y() - 14);
        }
    }

    void dibujarTrayectorias()
    {
        for (auto it = m_trails.constBegin(); it != m_trails.constEnd(); ++it) {
            const QVector<QPointF> points = it.value();
            if (points.size() < 2) {
                continue;
            }

            QPen pen(colorParaEtiqueta(it.key()), 1.5);
            pen.setCosmetic(true);
            for (int i = 1; i < points.size(); ++i) {
                m_scene->addLine(QLineF(points.at(i - 1), points.at(i)), pen);
            }
        }
    }

    void dibujarParticulas()
    {
        for (const Particle &particle : m_simulator.particles()) {
            const Vector2 position = particle.position();
            const double radius = particle.radius();
            const QRectF rect(position.x - radius, position.y - radius, radius * 2.0, radius * 2.0);
            const QColor color = colorParaEtiqueta(particle.label());

            QGraphicsEllipseItem *item = m_scene->addEllipse(rect, QPen(QColor("#111827"), 2), QBrush(color));
            item->setToolTip(QString("%1 | masa=%2 | v=(%3,%4)")
                                 .arg(particle.label())
                                 .arg(particle.mass(), 0, 'f', 2)
                                 .arg(particle.velocity().x, 0, 'f', 1)
                                 .arg(particle.velocity().y, 0, 'f', 1));

            QGraphicsTextItem *label = m_scene->addText(particle.label());
            label->setDefaultTextColor(QColor("#111827"));
            label->setPos(position.x + radius + 3, position.y - radius - 4);
        }
    }

    void actualizarEstado()
    {
        int wallEvents = 0;
        int obstacleEvents = 0;
        int particleEvents = 0;

        for (const CollisionEvent &event : m_simulator.events()) {
            if (event.type() == "pared-elastica") {
                ++wallEvents;
            } else if (event.type() == "obstaculo-inelastico") {
                ++obstacleEvents;
            } else if (event.type() == "particulas-completamente-inelastico") {
                ++particleEvents;
            }
        }

        m_statsLabel->setText(QString("Tiempo: %1 s\nParticulas activas: %2\nPared elastica: %3\nObstaculo inelastico: %4\nUnion de particulas: %5")
                                  .arg(m_simulator.time(), 0, 'f', 2)
                                  .arg(m_simulator.particles().size())
                                  .arg(wallEvents)
                                  .arg(obstacleEvents)
                                  .arg(particleEvents));
    }

    QColor colorParaEtiqueta(const QString &label) const
    {
        if (label.contains("P1")) {
            return QColor("#e63946");
        }
        if (label.contains("P2")) {
            return QColor("#2a9d8f");
        }
        if (label.contains("P3")) {
            return QColor("#457b9d");
        }
        if (label.contains("P4")) {
            return QColor("#f4a261");
        }

        return QColor("#6d597a");
    }

    SimulationConfig m_config;
    CollisionSimulator m_simulator;
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QTimer *m_timer;
    QPushButton *m_startButton;
    QPushButton *m_stepButton;
    QPushButton *m_resetButton;
    QSlider *m_speedSlider;
    QLabel *m_statsLabel;
    QListWidget *m_eventList;
    QMap<QString, QVector<QPointF>> m_trails;
    int m_lastEventCount;
    bool m_running;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    VentanaSimulacion window;
    window.show();

    return app.exec();
}
