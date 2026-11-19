#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "qt/GameController.h"

// Vereiste 11: clean main (i.e. nothing in the main that should be in a class)
// Startup only; world and UI logic live in their own classes.
int main(int argc,char* argv[]) {
    QGuiApplication app(argc,argv);
    QCoreApplication::setOrganizationName("ForgeFlow");
    QCoreApplication::setApplicationName("ForgeFlow");
    QQuickStyle::setStyle("Fusion");
    forgeflow::GameController game;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("game",&game);
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    if(engine.rootObjects().isEmpty())return 1;
    return app.exec();
}
