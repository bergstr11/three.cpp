//
// Created by byter on 1/12/18.
//

#ifndef THREEPP_MODEL_H
#define THREEPP_MODEL_H

#include <QObject>
#include <QUrl>
#include <QVector3D>
#include <QVariantMap>
#include <scene/Scene.h>
#include "quick/ThreeQObjectRoot.h"

namespace three {

namespace loader {
class Assimp;
}

namespace quick {

class Model : public ThreeQObjectRoot
{
Q_OBJECT
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
  Q_PROPERTY(bool isScene READ isScene WRITE setIsScene NOTIFY isSceneChanged)
  Q_PROPERTY(QVector3D position READ position NOTIFY positionChanged)
  Q_PROPERTY(QVariantMap replacements READ replacements WRITE setReplacements NOTIFY replacementsChanged)

  QString _file;
  QString _name;
  QVector3D _position;
  bool _isScene = true;
  ObjectRootContainer *_container = nullptr;

  QVariantMap _replacements;

  std::shared_ptr<loader::Assimp> _assimp;

  void loadFile(const QString &file);

public:
  Model(QObject *parent=nullptr) : ThreeQObjectRoot(parent) {}

  const QString name() {return _name;}

  void setName(const QString &name);

  const QString &file() const {return _file;}

  const QVariantMap &replacements() const {return _replacements;}

  void setReplacements(const QVariantMap &replacements);

  void setFile(const QString &file);

  const QVector3D position() const {return _position;}

  bool isScene() {return _isScene;}

  void setIsScene(bool isScene);

  void addTo(ObjectRootContainer *container) override;

  three::Scene::Ptr scene();

signals:
  void fileChanged();
  void nameChanged();
  void isSceneChanged();
  void positionChanged();
  void modelLoaded();
  void replacementsChanged();
};

}
}


#endif //THREEPP_MODEL_H
