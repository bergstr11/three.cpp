#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <QDebug>
#include "ThreeDItem.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QQmlEngine qmlEngine;
  QQmlContext *mainQmlContext = new QQmlContext(&qmlEngine);

  qmlRegisterType<lo::ui::quick::ThreeDItem>("com.lightningobjects.ui", 1, 0, "ThreeDItem");

  QQmlComponent maincomponent(&qmlEngine);
  maincomponent.loadUrl(QUrl("qrc:///Main.qml"));
  if (maincomponent.isError()) {
    qCritical() << "error loading main screen: " << maincomponent.errorString();
    return -1;
  }

  maincomponent.create(mainQmlContext);
  int ret = app.exec();

  return ret;
}