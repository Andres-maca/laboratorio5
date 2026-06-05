#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H

#include <QString>

class CollisionEvent
{
public:
    CollisionEvent();
    CollisionEvent(double time, const QString &type, const QString &description);

    double time() const;
    QString type() const;
    QString description() const;

private:
    double m_time;
    QString m_type;
    QString m_description;
};

#endif
