#ifndef GAME_H
#define GAME_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsTextItem>
#include <QPushButton>
#include <QFontDatabase>
#include <QFont>

#include "kocka.h"
#include "ploscica.h"


class Game : public QGraphicsView {
    Q_OBJECT
public:
    Game(QWidget *parent = nullptr);
    ~Game();
    void keyPressEvent(QKeyEvent *event) override;


private:
    QGraphicsScene *scene;
    Kocka *trenutnaKocka;

    static const int stKockSirina = 10;
    static const int stKockVisina = 20;
    static constexpr double stranicaKocke = 20.0; // const ne dela ker double(?)

    // Za shranjevanje že postavljenih kock
    Ploscica *plosca[stKockSirina][stKockVisina]; // Izhodišče plošče je v zgornjem levem kotu

    bool jeValidnaPozicija(Kocka *block, QPointF offset = {0, 0});
    void postaviKocko();
    void izbrisiLinije();
    void ustvariKocko();
    void restartajIgro();
    void spustiKocko();


    int level = 1;
    bool jeKonecIgre = false;
    int tocke = 0;

    // Za pospeševanje padanja kock vsakih 500 pik
    QTimer* dropTimer;

    // Taglavni napisi
    QGraphicsTextItem* levelText;
    QGraphicsTextItem* tockeText;
    QGraphicsTextItem* gameOverText;

    // Restart gumb
    QPushButton* restartGumb;

    // Custom font
    int fontId = QFontDatabase::addApplicationFont(":/assets/Pixeboy-Font");
    QStringList listFontov = QFontDatabase::applicationFontFamilies(fontId);
    QString PixeboyFont = listFontov[0];
    bool jeValidnaRotacija(Kocka *kocka, QPointF offset);
};

#endif // GAME_H
