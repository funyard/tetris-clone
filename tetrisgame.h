#ifndef TETRISGAME_H
#define TETRISGAME_H

#include "tetrispiece.h"

#include <QGraphicsView>
#include <QGraphicsScene>

class TetrisGame : public QGraphicsView {
    Q_OBJECT

public:
    TetrisGame(QWidget *parent = nullptr)
        : QGraphicsView(parent), scene(new QGraphicsScene(this)) {
        // Initialize game parameters
        boardWidth = 10;
        boardHeight = 20;
        blockSize = 30;
        score = 0;
        level = 1;
        linesCleared = 0;
        isGameOver = false;

        // Set up the scene
        scene->setSceneRect(0, 0, boardWidth * blockSize, boardHeight * blockSize);
        setScene(scene);
        setFixedSize(boardWidth * blockSize + 2, boardHeight * blockSize + 2);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        // Initialize the game board
        board.resize(boardHeight);
        for (int y = 0; y < boardHeight; ++y) {
            board[y].resize(boardWidth);
            for (int x = 0; x < boardWidth; ++x) {
                board[y][x] = nullptr;
            }
        }

        // Create UI elements
        createUI();

        // Start the game
        newGame();
    }

    void createUI() {
        // Create a container widget
        QWidget *container = new QWidget;
        QGridLayout *layout = new QGridLayout(container);

        // Create labels for score, level, and lines
        scoreLabel = new QLabel("Score: 0");
        levelLabel = new QLabel("Level: 1");
        linesLabel = new QLabel("Lines: 0");

        // Create buttons
        QPushButton *newGameBtn = new QPushButton("New Game");
        QPushButton *pauseBtn = new QPushButton("Pause");

        // Add widgets to layout
        layout->addWidget(this, 0, 0, 1, 2);
        layout->addWidget(scoreLabel, 1, 0);
        layout->addWidget(levelLabel, 1, 1);
        layout->addWidget(linesLabel, 2, 0);
        layout->addWidget(newGameBtn, 2, 1);
        layout->addWidget(pauseBtn, 3, 0, 1, 2);

        // Connect buttons
        connect(newGameBtn, &QPushButton::clicked, this, &TetrisGame::newGame);
        connect(pauseBtn, &QPushButton::clicked, this, &TetrisGame::togglePause);

        // Set the container as the central widget
        if (parentWidget()) {
            parentWidget()->setLayout(layout);
        }
    }

    void newGame() {
        // Clear the board
        clearBoard();

        // Reset game state
        score = 0;
        level = 1;
        linesCleared = 0;
        isGameOver = false;
        isPaused = false;

        // Update UI
        updateUI();

        // Create a new piece
        createNewPiece();

        // Start the game timer
        if (!gameTimer) {
            gameTimer = new QTimer(this);
            connect(gameTimer, &QTimer::timeout, this, &TetrisGame::gameTick);
        }
        gameTimer->start(gameSpeed());
    }

    void togglePause() {
        isPaused = !isPaused;
        if (isPaused) {
            gameTimer->stop();
        } else {
            gameTimer->start(gameSpeed());
        }
    }

    void clearBoard() {
        // Clear all blocks from the scene and board
        for (int y = 0; y < boardHeight; ++y) {
            for (int x = 0; x < boardWidth; ++x) {
                if (board[y][x]) {
                    scene->removeItem(board[y][x]);
                    delete board[y][x];
                    board[y][x] = nullptr;
                }
            }
        }
    }

    void createNewPiece() {
        if (currentPiece) {
            delete currentPiece;
        }

        // Create a new random piece
        currentPiece = new TetrisPiece(this);
        currentPiece->setPos(boardWidth / 2 - 2, 0);

        // Check if game over
        if (!isValidPosition(currentPiece->shape(), currentPiece->x(), currentPiece->y())) {
            gameOver();
        }
    }

    void gameTick() {
        if (isPaused || isGameOver) return;

        // Move the piece down
        if (isValidPosition(currentPiece->shape(), currentPiece->x(), currentPiece->y() + 1)) {
            currentPiece->moveBy(0, 1);
        } else {
            // Piece can't move down - lock it in place
            lockPiece();
            checkLines();
            createNewPiece();
        }
    }

    void lockPiece() {
        // Add the piece's blocks to the board
        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                if (currentPiece->shape()[y][x]) {
                    int boardX = currentPiece->x() + x;
                    int boardY = currentPiece->y() + y;
                    if (boardY >= 0 && boardX >= 0 && boardX < boardWidth) {
                        QGraphicsRectItem *block = new QGraphicsRectItem(0, 0, blockSize, blockSize);
                        block->setPos(boardX * blockSize, boardY * blockSize);
                        block->setBrush(currentPiece->color());
                        scene->addItem(block);
                        board[boardY][boardX] = block;
                    }
                }
            }
        }
    }

    void checkLines() {
        int linesRemoved = 0;

        for (int y = boardHeight - 1; y >= 0; --y) {
            bool lineComplete = true;
            for (int x = 0; x < boardWidth; ++x) {
                if (!board[y][x]) {
                    lineComplete = false;
                    break;
                }
            }

            if (lineComplete) {
                // Remove the line
                for (int x = 0; x < boardWidth; ++x) {
                    scene->removeItem(board[y][x]);
                    delete board[y][x];
                    board[y][x] = nullptr;
                }

                // Move all lines above down
                for (int yy = y - 1; yy >= 0; --yy) {
                    for (int x = 0; x < boardWidth; ++x) {
                        if (board[yy][x]) {
                            board[yy + 1][x] = board[yy][x];
                            board[yy][x] = nullptr;
                            board[yy + 1][x]->setPos(x * blockSize, (yy + 1) * blockSize);
                        }
                    }
                }

                // Check the same line again (since we moved everything down)
                y++;
                linesRemoved++;
            }
        }

        // Update score
        if (linesRemoved > 0) {
            linesCleared += linesRemoved;
            score += calculateScore(linesRemoved);
            level = linesCleared / 10 + 1;
            updateUI();

            // Increase speed
            if (gameTimer && !isPaused) {
                gameTimer->setInterval(gameSpeed());
            }
        }
    }

    int calculateScore(int lines) {
        switch (lines) {
        case 1: return 100 * level;
        case 2: return 300 * level;
        case 3: return 500 * level;
        case 4: return 800 * level;
        default: return 0;
        }
    }

    int gameSpeed() const {
        // Speed decreases with level (max speed at level 10)
        return qMax(100, 1000 - (level * 100));
    }

    void updateUI() {
        scoreLabel->setText(QString("Score: %1").arg(score));
        levelLabel->setText(QString("Level: %1").arg(level));
        linesLabel->setText(QString("Lines: %1").arg(linesCleared));
    }

    void gameOver() {
        isGameOver = true;
        gameTimer->stop();
        QMessageBox::information(this, "Game Over",
                                 QString("Game Over!\nScore: %1\nLevel: %2\nLines: %3")
                                     .arg(score).arg(level).arg(linesCleared));
    }

    bool isValidPosition(const QVector<QVector<bool>> &shape, int x, int y) {
        for (int py = 0; py < 4; ++py) {
            for (int px = 0; px < 4; ++px) {
                if (shape[py][px]) {
                    int boardX = x + px;
                    int boardY = y + py;

                    // Check if out of bounds
                    if (boardX < 0 || boardX >= boardWidth || boardY >= boardHeight) {
                        return false;
                    }

                    // Check if collides with existing blocks
                    if (boardY >= 0 && board[boardY][boardX]) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void keyPressEvent(QKeyEvent *event) override {
        if (isPaused || isGameOver) return;

        switch (event->key()) {
        case Qt::Key_Left:
            if (isValidPosition(currentPiece->shape(), currentPiece->x() - 1, currentPiece->y())) {
                currentPiece->moveBy(-1, 0);
            }
            break;
        case Qt::Key_Right:
            if (isValidPosition(currentPiece->shape(), currentPiece->x() + 1, currentPiece->y())) {
                currentPiece->moveBy(1, 0);
            }
            break;
        case Qt::Key_Down:
            if (isValidPosition(currentPiece->shape(), currentPiece->x(), currentPiece->y() + 1)) {
                currentPiece->moveBy(0, 1);
            }
            break;
        case Qt::Key_Up:
            rotatePiece();
            break;
        case Qt::Key_Space:
            hardDrop();
            break;
        default:
            QGraphicsView::keyPressEvent(event);
        }
    }

    void rotatePiece() {
        QVector<QVector<bool>> rotatedShape(4, QVector<bool>(4, false));
        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                rotatedShape[x][3 - y] = currentPiece->shape()[y][x];
            }
        }

        if (isValidPosition(rotatedShape, currentPiece->x(), currentPiece->y())) {
            currentPiece->setShape(rotatedShape);
        }
    }

    void hardDrop() {
        while (isValidPosition(currentPiece->shape(), currentPiece->x(), currentPiece->y() + 1)) {
            currentPiece->moveBy(0, 1);
        }
        lockPiece();
        checkLines();
        createNewPiece();
    }

private:
    QGraphicsScene *scene;
    TetrisPiece *currentPiece = nullptr;
    QVector<QVector<QGraphicsRectItem*>> board;
    int boardWidth;
    int boardHeight;
    int blockSize;
    int score;
    int level;
    int linesCleared;
    bool isGameOver;
    bool isPaused = false;
    QTimer *gameTimer = nullptr;
    QLabel *scoreLabel;
    QLabel *levelLabel;
    QLabel *linesLabel;
};

#endif // TETRISGAME_H
