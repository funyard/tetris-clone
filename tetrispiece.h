#ifndef TETRISPIECE_H
#define TETRISPIECE_H

#include <QColor>
#include <QTime>
#include "tetrisgame.h"

class TetrisPiece {
public:
    TetrisPiece(TetrisGame *game) : game(game) {
        // Initialize with random shape and color
        srand(QTime::currentTime().msec());
        int shapeIndex = rand() % 7;
        setShape(shapes[shapeIndex]);
        color = colors[shapeIndex];
    }

    const QVector<QVector<bool>>& shape() const { return currentShape; }
    int x() const { return posX; }
    int y() const { return posY; }
    QColor getColor() const { return color; }

    void setShape(const QVector<QVector<bool>> &newShape) {
        currentShape = newShape;
    }

    void setPos(int x, int y) {
        posX = x;
        posY = y;
    }

    void moveBy(int dx, int dy) {
        posX += dx;
        posY += dy;
    }

private:
    TetrisGame *game;
    QVector<QVector<bool>> currentShape;
    int posX = 0;
    int posY = 0;
    QColor color;

    // All possible tetromino shapes
    static const QVector<QVector<bool>> shapes[7];
    static const QColor colors[7];
};

// Define the shapes of all tetrominoes
const QVector<QVector<bool>> TetrisPiece::shapes[7] = {
    // I
    {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // J
    {
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // L
    {
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // O
    {
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // S
    {
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // T
    {
        {0, 1, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // Z
    {
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }
};

// Define colors for each tetromino
const QColor TetrisPiece::colors[7] = {
    QColor(Qt::cyan),     // I
    QColor(Qt::blue),     // J
    QColor(255, 165, 0),  // L (orange)
    QColor(Qt::yellow),   // O
    QColor(Qt::green),    // S
    QColor(128, 0, 128),  // T (purple)
    QColor(Qt::red)       // Z
};

#endif // TETRISPIECE_H
