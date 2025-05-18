#ifndef KOCKA_H
#define KOCKA_H

#include <QGraphicsItemGroup>
#include <QColor>
#include <QVector>
#include <QPoint>

class Kocka : public QGraphicsItemGroup {
public:
    static int stKock;
    enum TetrominoTipi { I, O, T, S, Z, J, L};
    Kocka(TetrominoTipi tipi);
    void zarotiraj();
    TetrominoTipi tip;

private:
    QVector<QPoint> oblika;
    QColor barvaKocke;
    void izgradi();
};

#endif // KOCKA_H
