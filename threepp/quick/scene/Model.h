//
// Created by byter on 1/12/18.
//

#ifndef THREEPPQ_MODEL_H
#define THREEPPQ_MODEL_H

#include <QObject>
#include <QUrl>
#include <QVector3D>
#include <QVariantMap>
#include <QString>
#include <QQmlListProperty>
#include <threepp/scene/Scene.h>
#include <threepp/loader/Assimp.h>
#include <threepp/quick/ThreeQObjectRoot.h>
#include <threepp/quick/materials/MeshPhongMaterial.h>
#include <threepp/quick/materials/MeshBasicMaterial.h>
#include <threepp/quick/materials/MeshLambertMaterial.h>
#include <threepp/quick/materials/MeshStandardMaterial.h>

namespace three {

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
 * handles a named material by determining what material type should be instantiated and
 * configuring it after loading
 */
class MaterialHandler : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString materialNames READ materialNames WRITE setMaterialNames NOTIFY materialNamesChanged)
  Q_PROPERTY(Material *createAs READ createAs WRITE setCreateAs NOTIFY createAsChanged)

  QString _materialNames;
  Material *_createAs = nullptr;

  loader::AssimpMaterialHandler *_assimpHandler = nullptr;

protected:

  void setMaterialNames(const QString &names) {
    if(_materialNames != names) {
      _materialNames = names;
      emit materialNamesChanged();
    }
  }

  Material *createAs() const {return _createAs;}

  void setAssimpHandlerCreateAs();

  void setCreateAs(Material *material) {
    if(_createAs != material) {
      _createAs = material;

      if(_assimpHandler) {
        setAssimpHandlerCreateAs();
      }
      emit createAsChanged();
    }
  }

public:
  MaterialHandler(QObject *parent=nullptr) : QObject(parent)
  {}

  void setAssimpHandler(loader::AssimpMaterialHandler *assimpHandler) {
    _assimpHandler = assimpHandler;
    setAssimpHandlerCreateAs();
  }

  void setLoadedMaterial(three::Material::Ptr material);
  const QString &materialNames() const {return _materialNames;}

  bool hasName(const std::string &name);

signals:
  void createAsChanged();
  void materialNamesChanged();
  void loaded(QVariant material);
};

class ModelMaterialHandler : public loader::AssimpMaterialHandler
{
  QList<quick::MaterialHandler *> _handlers;

protected:
  void handle(const std::string &name, three::MeshPhongMaterial &material, three::Material::Ptr mp) const override;
  void handle(const std::string &name, three::MeshToonMaterial &material, three::Material::Ptr mp) const override;
  void handle(const std::string &name, three::MeshLambertMaterial &material, three::Material::Ptr mp) const override;
  void handle(const std::string &name, three::MeshStandardMaterial &material, three::Material::Ptr mp) const override;
  void handle(const std::string &name, three::MeshBasicMaterial &material, three::Material::Ptr mp) const override;

public:
  ModelMaterialHandler() {}

  QList<quick::MaterialHandler *> &handlers() {return _handlers;}
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
  Q_PROPERTY(QString resourcePrefix READ resourcePrefix WRITE setResourcePrefix NOTIFY resourcePrefixChanged)
  Q_PROPERTY(QVariantMap replacements READ replacements WRITE setReplacements NOTIFY replacementsChanged)
  Q_PROPERTY(QObject *options READ options CONSTANT)
  Q_PROPERTY(QQmlListProperty<three::quick::MaterialHandler> handlers READ handlers)
  Q_CLASSINFO("DefaultProperty", "handlers")

  QString _file;
  QString _name;
  UP _up = UP_Y;

  QVariantMap _replacements;

  QString _resourcePrefix;

  Options _options;

  ThreeDItem *_item = nullptr;

  loader::AssimpOptions assimpOptions;

  ModelMaterialHandler _materialHandler;

  std::shared_ptr<loader::Assimp> _assimp;

  void loadFile(const QString &file);

protected:
  static void append_handler(QQmlListProperty<quick::MaterialHandler> *list, quick::MaterialHandler *obj);
  static int count_handlers(QQmlListProperty<quick::MaterialHandler> *);
  static quick::MaterialHandler *handler_at(QQmlListProperty<quick::MaterialHandler> *, int);
  static void clear_handlers(QQmlListProperty<quick::MaterialHandler> *);

  QQmlListProperty<quick::MaterialHandler> handlers();

public:
  Model(QObject *parent=nullptr) : ThreeQObjectRoot(parent)
  {}

  const QString name() {return _name;}

  void setName(const QString &name);

  const QString resourcePrefix() {return _resourcePrefix;}

  void setResourcePrefix(const QString &prefix);

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
  void resourcePrefixChanged();
};

}
}


#endif //THREEPPQ_MODEL_H
