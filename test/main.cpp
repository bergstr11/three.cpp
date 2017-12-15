#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <QDebug>
#include <quick/ThreeDScene.h>
#include "ThreeDItem.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QQmlEngine qmlEngine;
  QQmlContext *mainQmlContext = new QQmlContext(&qmlEngine);

  //qmlRegisterType<lo::ui::quick::ThreeDItem>("three.quick", 1, 0, "ThreeDItem");
  three::quick::ThreeDScene::init();

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