#include "game.h"
#include "ploscica.h"
#include <QKeyEvent>
#include <QDebug>
#include <QRandomGenerator>

Game::Game(QWidget *parent) : QGraphicsView(parent)
{
    scene = new QGraphicsScene(60, 0, 200, 400, this);
    trenutnaKocka = nullptr;


    // Nariši grid za preglednost
    for (int x = 0; x <= Game::stKockSirina; ++x) {
        scene->addLine(x * Game::stranicaKocke, 0, x * Game::stranicaKocke, 400, QPen(QColor(224,224,225, 28)));
    }
    for (int y = 0; y <= Game::stKockVisina; ++y) {
        scene->addLine(0, y * Game::stranicaKocke, 200, y * Game::stranicaKocke, QPen(QColor(224,224,225, 28)));
    }

    // Nastavitve za okno
    setWindowTitle("Tetris Clone");
    setScene(scene);
    setFixedSize(360, 410);
    setFocusPolicy(Qt::StrongFocus);

    // Izpis levela
    levelText = new QGraphicsTextItem("LEVEL: 1");
    levelText->setDefaultTextColor(Qt::white);
    levelText->setFont(QFont(PixeboyFont, 18));
    levelText->setPos(230, 30);
    scene->addItem(levelText);

    // Ustvarjanje timerja, ki poveča hitrost padanja kock na 500 točk
    dropTimer = new QTimer(this);
    connect(dropTimer, &QTimer::timeout, this, &Game::spustiKocko);
    dropTimer->start(1000);  // Začetna hitrost: 1000 ms na premik navzdol


    // Izpis točk
    tockeText = new QGraphicsTextItem("SCORE: 0");
    tockeText->setDefaultTextColor(Qt::white);
    tockeText->setFont(QFont(PixeboyFont, 18));
    tockeText->setPos(230, 0);  // Position it outside the game area
    scene->addItem(tockeText);

    // Game Over izpis
    gameOverText = new QGraphicsTextItem("GAME OVER");
    gameOverText->setDefaultTextColor(Qt::white);
    gameOverText->setFont(QFont(PixeboyFont, 34, QFont::Bold));
    gameOverText->setPos(65, 180);
    gameOverText->setZValue(1);    // Prekrije vse
    gameOverText->setVisible(false);
    scene->addItem(gameOverText);


    // Restart Gumb
    restartGumb = new QPushButton("Restart");
    restartGumb->setGeometry(250, 70, 80, 30);
    restartGumb->setParent(this->viewport());  // make it show on the view
    restartGumb->setFont(QFont(PixeboyFont));
    restartGumb->setVisible(false);


    connect(restartGumb, &QPushButton::clicked, this, &Game::restartajIgro);

    // Ustvari prazno plosco
    for (int i = 0; i < Game::stKockSirina; i++)
    {
        for (int j = 0; j < Game::stKockVisina; j++)
        {
            plosca[i][j] = nullptr;
        }
    }

    // Ustvari prvo kocko
    ustvariKocko();

}

Game::~Game()
{
    // Izbrisemo vse pointerje v arrayu
    for (int i = 0; i < Game::stKockSirina; i++)
    {
        for (int j = 0; j < Game::stKockVisina; j++)
        {
            delete plosca[i][j];
        }
    }

    // Izbrisemo vse ostale pointerje
    delete trenutnaKocka;
    delete dropTimer;
    delete levelText;
    delete tockeText;
    delete gameOverText;
    delete restartGumb;
    delete scene;
}

void Game::restartajIgro()
{
    if (trenutnaKocka != nullptr) {
        scene->removeItem(trenutnaKocka);
        delete trenutnaKocka;
        trenutnaKocka = nullptr;
    }

    // Izbrisemo vse kocke/tetromine in vse ploscice
    QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem* item : items) {
        if (dynamic_cast<Kocka*>(item) || dynamic_cast<Ploscica*>(item)) {
            scene->removeItem(item);
            delete item;
        }
    }

    // Izbrisemo plosco na prazno
    for (int x = 0; x < Game::stKockSirina; ++x) {
        for (int y = 0; y < Game::stKockVisina; ++y) {
            plosca[x][y] = nullptr;
        }
    }

    // Resetiramo tocke, level, gumb za restart skrijemo
    tocke = 0;
    tockeText->setPlainText("Score: 0");
    jeKonecIgre = false;
    gameOverText->setVisible(false);
    restartGumb->setVisible(false);

    // Od zacetka
    ustvariKocko();
}

void Game::ustvariKocko()
{
    // Če je konec igre ne bomo ustvarili kocke
    if (jeKonecIgre) return;

    // Izberemo nakljucno obliko iz enuma oblik tetromin
    Kocka::TetrominoTipi tip = (Kocka::TetrominoTipi)(QRandomGenerator::global()->bounded(Kocka::stKock)); // Če dodamo kocko moramo tu spremeniti meje
    trenutnaKocka = new Kocka(tip);
    // Postavimo na vrh
    trenutnaKocka->setPos(100, 0);

    // Logika za preverjanje konca igre. Če je pozicija, kjer ustvarimo kocko že zapolnjena, je igre konec
    if (!jeValidnaPozicija(trenutnaKocka)) {
        gameOverText->setVisible(true);
        restartGumb->setVisible(true);
        jeKonecIgre = true;
        return;
    }
    scene->addItem(trenutnaKocka);
}

bool Game::jeValidnaPozicija(Kocka *block, QPointF offset)
{
    // Loopamo čez vse kocke
    for (QGraphicsItem *item : block->childItems()) {
        QPointF pos = block->pos() + item->pos() + offset;
        int x = qRound(pos.x() / Game::stranicaKocke);
        int y = qRound(pos.y() / Game::stranicaKocke);

        // Preverimo NE VALIDNE pozicije
        // če x < 0 ali x >= Game::stKockSirina smo na robovih
        // če y >= Game::stKockVisina smo pod najnižjo linijo
        // če na tej poziciji že obstaja ploscica
        if (x < 0 || x >= Game::stKockSirina || y >= Game::stKockVisina || (plosca[x][y])) {
            return false;
        }
    }
    return true;
}

bool Game::jeValidnaRotacija(Kocka *kocka, QPointF offset)
{
    // Najprej zarotiramo in pogledamo ce pozicija sploh validna
    kocka->zarotiraj();

    // Podobno kot pri preverjanju ce je pozicija validna
    for (auto item : kocka->childItems())
    {
        QPointF pos = kocka->pos() + item->pos() + offset;
        int x = qRound(pos.x() / Game::stranicaKocke);
        int y = qRound(pos.y() / Game::stranicaKocke);

        if (x <= 0 || x >= Game::stKockSirina ||  (plosca[x][y]))
        {
            kocka->zarotiraj();
            kocka->zarotiraj();
            kocka->zarotiraj();
            return false;
        }

    }

    // Nato vrnemo kocko v originalno pozicijo
    kocka->zarotiraj();
    kocka->zarotiraj();
    kocka->zarotiraj();
    return true;
}

void Game::postaviKocko()
{
    for (QGraphicsItem *item : trenutnaKocka->childItems()) {
        Ploscica *tile = dynamic_cast<Ploscica*>(item);
        if (tile) {
            QPointF pos = trenutnaKocka->pos() + tile->pos();
            int x = qRound(pos.x() / Game::stranicaKocke);
            int y = qRound(pos.y() / Game::stranicaKocke);
            plosca[x][y] = tile;
            QPointF scenePos = tile->mapToScene(QPointF(0, 0));
            tile->setParentItem(nullptr);
            tile->setPos(scenePos); // remove from block group
        }
    }

    // Preidemo na novo kocko
    scene->removeItem(trenutnaKocka);
    delete trenutnaKocka;
    trenutnaKocka = nullptr;

    izbrisiLinije();
    ustvariKocko();
}

void Game::izbrisiLinije()
{
    int stPociscenihLinij = 0;

    // Zacnemo na spodnji liniji
    for (int y = Game::stKockVisina - 1; y >= 0; --y) {

        // Če obstaja v liniji nullptr linija ni polna
        bool full = true;
        for (int x = 0; x < Game::stKockSirina; ++x) {
            if (!plosca[x][y]) {
                full = false;
                break;
            }
        }

        if (full) {
            stPociscenihLinij++;

            // Počistimo polno linijo
            for (int x = 0; x < Game::stKockSirina; ++x) {
                scene->removeItem(plosca[x][y]);
                delete plosca[x][y];
                plosca[x][y] = nullptr;
            }

            // Vse, kar je nad linijo, prestavimo navzdol
            for (int row = y - 1; row >= 0; --row) {
                for (int x = 0; x < Game::stKockSirina; ++x) {
                    plosca[x][row + 1] = plosca[x][row];
                    if (plosca[x][row + 1]) {
                        plosca[x][row + 1]->moveBy(0, Game::stranicaKocke);
                    }
                }
            }

            y++; // če se slučajno na isto mesto prestavi polna vrsta, da to vrsto ponovno preverimo
        }
    }


    // Logika za dodajanje tock in visanje levela
    if (stPociscenihLinij > 0) {
        tocke += stPociscenihLinij * 100;
        tockeText->setPlainText(QString("Score: %1").arg(tocke));

        // Povisamo level vsakih 500 točk
        int newLevel = (tocke / 500) + 1;
        if (newLevel > level) {
            level = newLevel;
            levelText->setPlainText(QString("Level: %1").arg(level));

            // Povišamo hitrost padanja kock (vsak level se bo kocka za eno ploscico navzdol prestavila 100ms hitreje
            int newInterval = qMax(100, 1000 - (level - 1) * 100);
            dropTimer->start(newInterval);
        }
    }
}

void Game::spustiKocko()
{
    if (!trenutnaKocka) return;

    // Preverimo, če je to da kocko prestavimo za eno enoto navzdol validna poteza
    if (jeValidnaPozicija(trenutnaKocka, {0, Game::stranicaKocke})) {
        trenutnaKocka->moveBy(0, Game::stranicaKocke);
    } else {
        // Drugače kocko postavimo
        postaviKocko();
    }
}

void Game::keyPressEvent(QKeyEvent *event)
{
    if (!trenutnaKocka) return;

    switch (event->key()) {
    case Qt::Key_Left:
        if (jeValidnaPozicija(trenutnaKocka, {-Game::stranicaKocke, 0}))
            trenutnaKocka->moveBy(-Game::stranicaKocke, 0);
        break;
    case Qt::Key_Right:
        if (jeValidnaPozicija(trenutnaKocka, {Game::stranicaKocke, 0}))
            trenutnaKocka->moveBy(Game::stranicaKocke, 0);
        break;
    case Qt::Key_Down:
        if (jeValidnaPozicija(trenutnaKocka, {0, Game::stranicaKocke}))
            trenutnaKocka->moveBy(0, Game::stranicaKocke);
        break;
    case Qt::Key_Up:
        // Preverili bomo tudi, če smo ob steni/ob kocki, da se bomo vseeno lahko zarotirali
        if (trenutnaKocka->tip != Kocka::I)
        {
            if (jeValidnaRotacija(trenutnaKocka, {0,0}))
                trenutnaKocka->zarotiraj();
            // Objekt na desni strani kocke
            else if (jeValidnaRotacija(trenutnaKocka, {-Game::stranicaKocke, 0}))
            {
                trenutnaKocka->moveBy(-Game::stranicaKocke, 0);
                trenutnaKocka->zarotiraj();
            }
            // Objekt na levi strani kocke
            else if (jeValidnaRotacija(trenutnaKocka, {2*Game::stranicaKocke, 0}))
            {
                trenutnaKocka->moveBy(Game::stranicaKocke, 0);
                trenutnaKocka->zarotiraj();
            }
            break;
        }
        else // Saj je moznost pri rotaciji kocke I, da se sirina poveca za 2
        {
            if (jeValidnaRotacija(trenutnaKocka, {0,0}))
                trenutnaKocka->zarotiraj();
            // Ob desni
            else if (jeValidnaRotacija(trenutnaKocka, {-2*Game::stranicaKocke, 0}))
            {
                trenutnaKocka->moveBy(-2*Game::stranicaKocke, 0);
                trenutnaKocka->zarotiraj();
            }
            // Ob levi
            else if (jeValidnaRotacija(trenutnaKocka, {2*Game::stranicaKocke, 0}))
            {
                trenutnaKocka->moveBy(2*Game::stranicaKocke, 0);
                trenutnaKocka->zarotiraj();
            }
            break;
        }
    }

}
