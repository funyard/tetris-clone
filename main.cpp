#include <QApplication>
#include "game.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);


    Game igra;
    igra.show();

    return app.exec();
}
