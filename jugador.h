#ifndef JUGADOR_H
#define JUGADOR_H

#include <QPointF>
#include <QRectF>
#include <QString>

class Jugador
{
public:
    Jugador();
    Jugador(int id, const QString &nombre, const QPointF &posicionDisparo, const QRectF &representante);

    int id() const;
    QString nombre() const;
    QPointF posicionDisparo() const;
    QRectF representante() const;

private:
    int m_id;
    QString m_nombre;
    QPointF m_posicionDisparo;
    QRectF m_representante;
};

#endif
