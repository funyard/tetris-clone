#include "kocka.h"
#include "ploscica.h"

Kocka::Kocka(TetrominoTipi t) {
    tip = t;
    izgradi();

    for (const QPoint &pt : oblika) {
        Ploscica *tile = new Ploscica(barvaKocke);
        tile->setPos(pt.x() * 20, pt.y() * 20);
        addToGroup(tile);
    }
    setTransformOriginPoint(boundingRect().center());
}

int Kocka::stKock = 7;

void Kocka::izgradi() {
    switch (tip) {
    case I: oblika = {{-1, 0}, {0, 0}, {1, 0}, {2, 0}}; barvaKocke = Qt::cyan; break;
    case O: oblika = {{0, 0}, {1, 0}, {0, 1}, {1, 1}}; barvaKocke = Qt::yellow; break;
    case T: oblika = {{-1, 0}, {0, 0}, {1, 0}, {0, 1}}; barvaKocke = Qt::magenta; break;
    case S: oblika = {{0, 0}, {1, 0}, {-1, 1}, {0, 1}}; barvaKocke = Qt::green; break;
    case Z: oblika = {{-1, 0}, {0, 0}, {0, 1}, {1, 1}}; barvaKocke = Qt::red; break;
    case J: oblika = {{-1, 0}, {0, 0}, {1, 0}, {1, 1}}; barvaKocke = Qt::blue; break;
    case L: oblika = {{-1, 0}, {0, 0}, {1, 0}, {-1, 1}}; barvaKocke = QColor(255,165,0); break; // oranžna
    // case PK: shape = {{0, 0}, {1, 0}, {0, 1}, {1, 1}, {0,2}, {1,2}}; color = Qt::yellow; break;
    // Če želimo lahko še tu dodamo kakšne dodatne oblike (npr. pravokotnik)
    }
}

void Kocka::zarotiraj() {
    if (tip == O) return;  // 2x2 se ne rabi rotirati

    for (QGraphicsItem *item : childItems()) {
        QPointF pos = item->pos();
        qreal x = pos.x() / 20;
        qreal y = pos.y() / 20;
        item->setPos(-y * 20, x * 20);  // rotatacija 90 stopinj
    }
}
