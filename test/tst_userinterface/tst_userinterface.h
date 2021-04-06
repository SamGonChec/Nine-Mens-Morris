#ifndef TST_USERINTERFACE_H
#define TST_USERINTERFACE_H
#include <QtTest>
#include <QTest>
#include <QCoreApplication>

#include "include/menu.h"
#include "include/singleplayergame.h"

class UserInterface : public QObject
{
    Q_OBJECT

public:
    UserInterface() {}
    ~UserInterface() {}

private:
    QGraphicsScene testScene;

private slots:
    void testOnePlayerScreen();
    void testTwoPlayerScreen();
    void testTutorialScreen();
    void testQuitButton();
    void testOnePlayerMainMenuButton();
    void testTwoPlayerMainMenuButton();

};
#endif // TST_USERINTERFACE_H
