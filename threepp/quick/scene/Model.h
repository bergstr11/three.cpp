//
// Created by byter on 1/12/18.
//

#ifndef THREEPPQ_MODEL_H
#define THREEPPQ_MODEL_H

#include <QObject>
#include <QUrl>
#include <QVector3D>
#include <QVariantMap>
#include <threepp/scene/Scene.h>
#include <threepp/quick/ThreeQObjectRoot.h>

namespace three {

namespace loader {
class Assimp;
}

namespace quick {

class ThreeQObject;
class Intersect;

class Options : public QObject
{
  friend class Model;
  Q_OBJECT
  Q_PROPERTY(bool preferPhong READ preferPhong WRITE setPreferPhong NOTIFY preferPhongChanged);

  bool _preferPhong = true;
  std::shared_ptr<loader::Assimp> _assimp;

  void setAssimp(std::shared_ptr<loader::Assimp> assimp);

public:
  bool preferPhong() const {return _preferPhong;}
  void setPreferPhong(bool prefer);

signals:
  void preferPhongChanged();
};

/**
 * represents a complete 3D scene loaded from a file
 *
 * @property name arbitrarily assigned name
 * @property file a file URI, either pointing to a filesystem resource ('file:' prefix) or a Qt resource
 * (':' prefix)
 * @property replacements resource name replacements, e.g. if a resource referenced inside the model file
 * is actually stored under a different name
 */
class Model : public ThreeQObjectRoot
{
Q_OBJECT
public:
  enum UP {UP_X, UP_Y, UP_Z};
  Q_ENUM(UP)

private:
  Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
  Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
  Q_PROPERTY(QVariantMap replacements READ replacements WRITE setReplacements NOTIFY replacementsChanged)
  Q_PROPERTY(QObject *options READ options CONSTANT)

  QString _file;
  QString _name;
  UP _up = UP_Y;

  QVariantMap _replacements;

  Options _options;

  ThreeDItem *_item = nullptr;

  std::shared_ptr<loader::Assimp> _assimp;

  void loadFile(const QString &file);

public:
  Model(QObject *parent=nullptr) : ThreeQObjectRoot(parent)
  {}

  const QString name() {return _name;}

  void setName(const QString &name);

  const QString &file() const {return _file;}

  void setFile(const QString &file);

  const QVariantMap &replacements() const {return _replacements;}

  void setReplacements(const QVariantMap &replacements);

  void setItem(ThreeDItem *item) override;

  QObject *options() {return &_options;}

  three::Scene::Ptr importedScene();

signals:
  void fileChanged();
  void nameChanged();
  void modelLoaded();
  void replacementsChanged();
};

}
}


#endif //THREEPPQ_MODEL_H
