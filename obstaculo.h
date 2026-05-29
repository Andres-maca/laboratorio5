#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QRectF>

class Obstacle
{
public:
    Obstacle();
    Obstacle(int id, const QRectF &rect, double restitution, double resistance = 100.0);

    int id() const;
    QRectF rect() const;
    double restitution() const;
    double resistance() const;

    void setResistance(double resistance);

private:
    int m_id;
    QRectF m_rect;
    double m_restitution;
    double m_resistance;
};

#endif
