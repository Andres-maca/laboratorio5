#include <QApplication>
#include <QBrush>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <QLineF>
#include <QPointF>
#include <QVector>

#include <QtMath>

#include "juego_turnos.h"

class VentanaJuego : public QWidget
{
public:
    explicit VentanaJuego(QWidget *parent = nullptr)
        : QWidget(parent),
          m_config(),
          m_juego(m_config),
          m_scene(new QGraphicsScene(this)),
          m_view(new QGraphicsView(m_scene, this)),
          m_timer(new QTimer(this)),
          m_angleInput(new QDoubleSpinBox(this)),
          m_speedInput(new QDoubleSpinBox(this)),
          m_fireButton(new QPushButton("Disparar", this)),
          m_resetButton(new QPushButton("Reiniciar", this)),
          m_animationTimer(new QTimer(this)),
          m_lastEventCount(0),
          m_slimeFrame(0),
          m_victoryMessageShown(false)
    {
        setWindowTitle("Practica 5 - Juego por turnos");
        resize(1240, 760);

        m_scene->setSceneRect(0, 0, m_config.width, m_config.height);
        m_view->setRenderHint(QPainter::Antialiasing, true);
        m_view->setMinimumSize(1020, 680);
        m_view->setFrameShape(QFrame::NoFrame);
        m_view->setBackgroundBrush(QBrush(QColor("#eef2f6")));

        m_angleInput->setRange(10.0, 80.0);
        m_angleInput->setSingleStep(2.5);
        m_angleInput->setValue(45.0);
        m_angleInput->setSuffix(" deg");

        m_speedInput->setRange(80.0, 260.0);
        m_speedInput->setSingleStep(10.0);
        m_speedInput->setValue(170.0);

        auto *controlsBox = new QGroupBox("Lanzamiento", this);
        auto *controlsLayout = new QHBoxLayout(controlsBox);
        controlsLayout->addWidget(new QLabel("Angulo", this));
        controlsLayout->addWidget(m_angleInput);
        controlsLayout->addWidget(new QLabel("Velocidad", this));
        controlsLayout->addWidget(m_speedInput);
        controlsLayout->addWidget(m_fireButton);
        controlsLayout->addWidget(m_resetButton);

        auto *leftLayout = new QVBoxLayout();
        leftLayout->addWidget(m_view, 1);
        leftLayout->addWidget(controlsBox);

        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->addLayout(leftLayout, 1);

        connect(m_fireButton, &QPushButton::clicked, this, [this]() {
            disparar();
        });

        connect(m_resetButton, &QPushButton::clicked, this, [this]() {
            reiniciar();
        });

        connect(m_timer, &QTimer::timeout, this, [this]() {
            avanzar();
        });

        connect(m_animationTimer, &QTimer::timeout, this, [this]() {
            m_slimeFrame = (m_slimeFrame + 1) % 6;
            dibujarEscena();
        });

        cargarSprites();
        reiniciar();
        m_animationTimer->start(160);
    }

private:
    void reiniciar()
    {
        m_timer->stop();
        m_juego.reiniciar();
        m_trayectoria.clear();
        m_lastEventCount = 0;
        m_victoryMessageShown = false;
        actualizarBotones();
        dibujarEscena();
    }

    void disparar()
    {
        if (!m_juego.disparar(m_angleInput->value(), m_speedInput->value())) {
            return;
        }

        m_trayectoria.clear();
        registrarPuntoProyectil();
        actualizarEventos();
        actualizarBotones();
        dibujarEscena();
        m_timer->start(16);
    }

    void avanzar()
    {
        for (int i = 0; i < 3; ++i) {
            m_juego.avanzar();
            registrarPuntoProyectil();
        }

        actualizarEventos();
        dibujarEscena();
        actualizarBotones();
        mostrarVictoriaSiExiste();

        if (!m_juego.turnoEnCurso()) {
            m_timer->stop();
        }
    }

    void registrarPuntoProyectil()
    {
        const Proyectil proyectil = m_juego.proyectil();
        if (!proyectil.active()) {
            return;
        }

        const Vector2 position = proyectil.position();
        m_trayectoria.append(QPointF(position.x, position.y));
        if (m_trayectoria.size() > 450) {
            m_trayectoria.remove(0, m_trayectoria.size() - 450);
        }
    }

    void actualizarEventos()
    {
        m_lastEventCount = m_juego.eventos().size();
    }

    void actualizarBotones()
    {
        const bool puedeDisparar = !m_juego.turnoEnCurso() && m_juego.ganadorId() == 0;
        m_fireButton->setEnabled(puedeDisparar);
        m_angleInput->setEnabled(puedeDisparar);
        m_speedInput->setEnabled(puedeDisparar);
    }

    void dibujarTurno()
    {
        QGraphicsTextItem *turno = m_scene->addText(
            QString("Turno: Jugador %1").arg(m_juego.jugadorActual().id())
            );

        QFont font;
        font.setPointSize(18);
        font.setBold(true);

        turno->setFont(font);
        turno->setDefaultTextColor(QColor("#ffffff"));
        turno->setPos(50, 50);
    }

    void dibujarEscena()
    {
        m_scene->clear();
        dibujarEscenario();
        dibujarInfraestructura();
        dibujarRepresentantes();
        dibujarProyectil();
        dibujarGuiaDisparo();
        dibujarTurno();
    }

    void dibujarEscenario()
    {
        if (!m_escenarioPixmap.isNull()) {
            dibujarImagenEnRect(m_escenarioPixmap, QRectF(0, 0, m_config.width, m_config.height), Qt::IgnoreAspectRatio);
        }

        if (!m_pisoPixmap.isNull()) {
            dibujarImagenEnRect(m_pisoPixmap, QRectF(0, m_config.height - m_config.floorHeight,
                                                      m_config.width, m_config.floorHeight),
                                Qt::IgnoreAspectRatio);
        }

        m_scene->addRect(0, 0, m_config.width, m_config.height,
                         QPen(QColor("#111827"), 3),
                         QBrush(Qt::NoBrush));
    }

    void dibujarRepresentantes()
    {
        for (const Jugador &jugador : m_juego.jugadores()) {
            const QColor color = colorJugador(jugador.id());
            const QRectF rect = jugador.representante();
            const QPointF spawn = jugador.posicionDisparo();
            const QPixmap playerPixmap = jugador.id() == 1 ? m_totoroPixmap : m_totoroDerechoPixmap;

            m_scene->addEllipse(QRectF(spawn.x() - 8, spawn.y() - 8, 16, 16),
                                QPen(QColor("#111827"), 2),
                                QBrush(color));
            m_scene->addLine(QLineF(spawn.x(), spawn.y(),
                                    spawn.x() + (jugador.id() == 1 ? 42.0 : -42.0), spawn.y() - 42.0),
                             QPen(color.darker(130), 5));

            const QRectF playerRect(jugador.id() == 1 ? spawn.x() - 105.0 : m_config.width - 155.0,
                                    spawn.y() - 120.0, 180.0, 240.0);
            dibujarImagenEnRect(playerPixmap, playerRect, Qt::KeepAspectRatio);

            dibujarImagenEnRect(frameSlimeActual(), rect, Qt::KeepAspectRatio);
        }
    }

    void dibujarInfraestructura()
    {
        for (const Infraestructura &infra : m_juego.infraestructuras()) {
            const QColor baseColor = colorJugador(infra.duenoId());
            QColor fill = infra.destruida() ? QColor("#e5e7eb") : baseColor.lighter(145);
            QPen pen(infra.destruida() ? QColor("#9ca3af") : QColor("#374151"), 2);
            if (infra.destruida()) {
                pen.setStyle(Qt::DashLine);
            }

            if (!m_bloquePixmap.isNull()) {
                dibujarImagenEnRect(m_bloquePixmap, infra.rect(), Qt::IgnoreAspectRatio);
                m_scene->addRect(infra.rect(), pen, QBrush(Qt::NoBrush));
                if (infra.destruida()) {
                    m_scene->addRect(infra.rect(), QPen(Qt::NoPen), QBrush(QColor(229, 231, 235, 170)));
                }
            } else {
                m_scene->addRect(infra.rect(), pen, QBrush(fill));
            }

            const double ratio = infra.resistenciaInicial() == 0.0
                                     ? 0.0
                                     : infra.resistencia() / infra.resistenciaInicial();
            const QRectF bar(infra.rect().x(), infra.rect().y() - 10,
                             infra.rect().width() * ratio, 6);
            m_scene->addRect(QRectF(infra.rect().x(), infra.rect().y() - 10,
                                    infra.rect().width(), 6),
                             QPen(Qt::NoPen),
                             QBrush(QColor("#e5e7eb")));
            m_scene->addRect(bar, QPen(Qt::NoPen), QBrush(QColor("#22c55e")));

        }
    }

    void dibujarTrayectoria()
    {
        if (m_trayectoria.size() < 2) {
            return;
        }

        QPen pen(QColor("#2563eb"), 2);
        pen.setCosmetic(true);
        for (int i = 1; i < m_trayectoria.size(); ++i) {
            m_scene->addLine(QLineF(m_trayectoria.at(i - 1), m_trayectoria.at(i)), pen);
        }
    }

    void dibujarProyectil()
    {
        const Proyectil proyectil = m_juego.proyectil();
        if (!proyectil.active()) {
            return;
        }

        const Vector2 position = proyectil.position();
        const double radius = proyectil.radius();
        m_scene->addEllipse(QRectF(position.x - radius, position.y - radius, radius * 2, radius * 2),
                            QPen(QColor("#111827"), 2),
                            QBrush(QColor("#facc15")));
    }

    void dibujarGuiaDisparo()
    {
        if (m_juego.turnoEnCurso() || m_juego.ganadorId() != 0) {
            return;
        }

        const Jugador jugador = m_juego.jugadorActual();
        const double direction = jugador.id() == 1 ? 1.0 : -1.0;
        const double radians = qDegreesToRadians(m_angleInput->value());
        const QPointF start = jugador.posicionDisparo();
        const QPointF end(start.x() + direction * qCos(radians) * 70.0,
                          start.y() - qSin(radians) * 70.0);

        QPen pen(colorJugador(jugador.id()).darker(125), 4);
        pen.setCapStyle(Qt::RoundCap);
        m_scene->addLine(QLineF(start, end), pen);
    }

    QColor colorJugador(int id) const
    {
        return id == 1 ? QColor("#dc2626") : QColor("#2563eb");
    }

    void mostrarVictoriaSiExiste()
    {
        if (m_victoryMessageShown || m_juego.ganadorId() == 0) {
            return;
        }

        m_victoryMessageShown = true;
        QMessageBox::information(this,
                                 "Victoria",
                                 QString("Ganaste Jugador %1").arg(m_juego.ganadorId()));
    }

    void cargarSprites()
    {
        m_totoroPixmap.load(":/sprites/totoro.png");
        m_totoroDerechoPixmap.load(":/sprites/totoroDerecho.png");
        m_slimePixmap.load(":/sprites/slimeV2.png");
        m_bloquePixmap.load(":/sprites/madera.png");
        m_escenarioPixmap.load(":/sprites/escenarioV2.png");
        m_pisoPixmap.load(":/sprites/piso.png");
    }

    QPixmap frameSlimeActual() const
    {
        if (m_slimePixmap.isNull()) {
            return QPixmap();
        }

        const int frameCount = 6;
        const int x0 = m_slimeFrame * m_slimePixmap.width() / frameCount;
        const int x1 = (m_slimeFrame + 1) * m_slimePixmap.width() / frameCount;
        return m_slimePixmap.copy(x0, 0, x1 - x0, m_slimePixmap.height());
    }

    void dibujarImagenEnRect(const QPixmap &pixmap, const QRectF &target, Qt::AspectRatioMode mode)
    {
        if (pixmap.isNull()) {
            m_scene->addRect(target, QPen(QColor("#111827"), 1), QBrush(QColor("#e5e7eb")));
            return;
        }

        const QPixmap scaled = pixmap.scaled(target.size().toSize(), mode, Qt::SmoothTransformation);
        QPointF position = target.topLeft();
        if (mode == Qt::KeepAspectRatio) {
            position.setX(target.x() + (target.width() - scaled.width()) / 2.0);
            position.setY(target.y() + (target.height() - scaled.height()) / 2.0);
        }

        QGraphicsPixmapItem *item = m_scene->addPixmap(scaled);
        item->setPos(position);
    }

    JuegoConfig m_config;
    JuegoTurnos m_juego;
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    QTimer *m_timer;
    QDoubleSpinBox *m_angleInput;
    QDoubleSpinBox *m_speedInput;
    QPushButton *m_fireButton;
    QPushButton *m_resetButton;
    QTimer *m_animationTimer;
    QVector<QPointF> m_trayectoria;
    QPixmap m_totoroPixmap;
    QPixmap m_totoroDerechoPixmap;
    QPixmap m_slimePixmap;
    QPixmap m_bloquePixmap;
    QPixmap m_escenarioPixmap;
    QPixmap m_pisoPixmap;
    int m_lastEventCount;
    int m_slimeFrame;
    bool m_victoryMessageShown;
};



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    VentanaJuego window;
    window.show();

    return app.exec();
}
