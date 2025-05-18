QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = icon.ico #https://www.flaticon.com/free-icons/tetris" Tetris icons created by cah nggunung - Flaticon

SOURCES += \
    game.cpp \
    kocka.cpp \
    main.cpp \
    ploscica.cpp

HEADERS += \
    game.h \
    kocka.h \
    ploscica.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    files.qrc
