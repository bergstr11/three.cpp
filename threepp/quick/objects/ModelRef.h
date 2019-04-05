//
// Created by byter on 1/12/18.
//

#ifndef THREEPPQ_MESH_H
#define THREEPPQ_MESH_H

#include <QMetaObject>
#include <threepp/objects/Node.h>
#include <threepp/quick/scene/Model.h>
#include <threepp/quick/scene/Scene.h>
#include <threepp/quick/objects/ThreeQObject.h>

namespace three {
namespace quick {

class ObjectPicker;

class DLQX ModelRef : public ThreeQObject
{
Q_OBJECT
  Q_PROPERTY(Model *model READ model WRITE setModel NOTIFY modelChanged)
  Q_PROPERTY(QString selector READ selector WRITE setSelector)
  Q_PROPERTY(Type type READ type WRITE setType)
  Q_PROPERTY(Operation operation READ operation WRITE setOperation NOTIFY operationChanged)
  Q_PROPERTY(three::quick::ThreeQObject *object READ threeQObject NOTIFY objectChanged)

public:
  enum Type {Node, Mesh, Light, Camera};
  Q_ENUM(Type)

  enum Operation {Replace, Append};
  Q_ENUM(Operation)

private:
  Model *_model = nullptr;
  QString _selector;
  Type _type = Mesh;
  Operation _operation = Replace;

  QMetaObject::Connection _loadedConnection;
  QMetaObject::Connection _fileConnection;

  ThreeQObject *_threeQObject = nullptr;

protected:
  Object3D::Ptr _create() override
  {
    return nullptr;
  }

  void matchType(Object3D::Ptr parent, Object3D::Ptr obj);

  enum class Eval {name, skipLevel, skipLevels, collectLevel, collectLevels};
  bool evaluateSelector(QStringList::iterator &iter,
                        QStringList::iterator &end,
                        Object3D::Ptr parent,
                        const std::vector<Object3D::Ptr> &children,
                        Eval wildcard=Eval::name);

  ThreeQObject *getThreeQObject();

public:
  ModelRef(QObject *parent=nullptr) : ThreeQObject(parent) {}

  ~ModelRef() override {
    if(_threeQObject) _threeQObject->deleteLater();
  }

  Model *model() const {return _model;}

  void setModel(Model *model);

  ThreeQObject *threeQObject() {return getThreeQObject();}

  const QString &selector() const {return _selector;}

  void setSelector(const QString &selector) {_selector = selector;}

  Operation operation() const {return _operation;}

  void setOperation(Operation operation);

  Type type() const {return _type;}

  void setType(Type type) {_type = type;}

signals:
  void modelChanged();
  void objectChanged();
  void operationChanged();

private slots:
  void cleanup();
  void update();
};

}
}

#endif //THREEPPQ_MESH_H
