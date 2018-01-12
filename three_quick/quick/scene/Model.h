//
// Created by byter on 1/12/18.
//

#ifndef THREEPP_MODEL_H
#define THREEPP_MODEL_H

#include <QObject>
#include <QUrl>
#include <QVector3D>
#include <scene/Scene.h>
#include "quick/ThreeQObjectRoot.h"

namespace three {
namespace quick {

class Model : public ThreeQObjectRoot
{
Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QUrl file READ file WRITE setFile NOTIFY fileChanged)
  Q_PROPERTY(bool isScene READ isScene WRITE setIsScene NOTIFY isSceneChanged)
  Q_PROPERTY(QVector3D position READ position NOTIFY positionChanged)

  QUrl _file;
  QString _name;
  QVector3D _position;
  bool _isScene = true;

  three::Scene::Ptr _scene;

public:
  const QString name() {return _name;}

  void setName(const QString &name);

  const QUrl &file() const {return _file;}

  void setFile(const QUrl &file);

  const QVector3D position() const {return _position;}

  bool isScene() {return _isScene;}

  void setIsScene(bool isScene);

  void addTo(ObjectRootContainer *container) override;

  three::Scene::Ptr scene() {return _scene;}

signals:
  void fileChanged();
  void nameChanged();
  void isSceneChanged();
  void positionChanged();
  void modelLoaded();
};

}
}


#endif //THREEPP_MODEL_H
