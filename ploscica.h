#ifndef PLOSCICA_H
#define PLOSCICA_H

#include <QGraphicsRectItem>
#include <QColor>

class Ploscica : public QGraphicsRectItem {
public:
    Ploscica(QColor color);
    static constexpr double stranicaPloscice = 20.0; // const ne dela ker double(?)
};

#endif // PLOSCICA_H
