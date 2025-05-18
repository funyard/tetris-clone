#include "ploscica.h"

#include <QColor>
#include <QPen>

Ploscica::Ploscica(QColor color) {
    setRect(0, 0, 20, 20);
    setBrush(color);
    setPen(QPen(Qt::black));
}
