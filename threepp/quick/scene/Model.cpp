//
// Created by byter on 1/12/18.
//

#include "Model.h"

#include <QQuaternion>
#include <threepp/loader/Assimp.h>
#include <threepp/quick/loader/FileSystemLoader.h>
#include <threepp/quick/loader/QtResourceLoader.h>
#include <threepp/quick/objects/ThreeQObject.h>
#include <threepp/quick/elements/RayCaster.h>
#include <threepp/quick/objects/Mesh.h>
#include <threepp/quick/objects/Node.h>

namespace three {
namespace quick {

using namespace std;

void Model::setReplacements(const QVariantMap &replacements)
{
  if(_replacements != replacements) {
    _replacements = replacements;
    emit replacementsChanged();
  }
}

void Model::loadFile(const QString &file)
{
  unordered_map<string, string> replacements;
  for(const auto &repl : _replacements.keys()) {
    replacements[repl.toStdString()] = _replacements[repl].toString().toStdString();
  }

  _assimp = make_shared<loader::Assimp>();

  QFileInfo info(file);
  if(info.isNativePath()) {
    FileSystemLoader *loader = new FileSystemLoader(*_assimp, file, replacements);
    QObject::connect(loader, &FileSystemLoader::loaded, this, &Model::modelLoaded);
    QObject::connect(loader, &QThread::finished, loader, &QObject::deleteLater);
    loader->start();
  }
  else {
    QtResourceLoader *loader = new QtResourceLoader(*_assimp, file, replacements);
    QObject::connect(loader, &QtResourceLoader::loaded, this, &Model::modelLoaded);
    QObject::connect(loader, &QThread::finished, loader, &QObject::deleteLater);
    loader->start();
  }
}

void Model::setFile(const QString &file)
{
  if (_file != file) {
    _file = file;
    emit fileChanged();

    if(_item) loadFile(file);
  }
}

void Model::setName(const QString &name) {
  if (_name != name) {
    _name = name;
    emit nameChanged();
  }
}

three::Scene::Ptr Model::importedScene() {
  return _assimp ? _assimp->scene() : nullptr;
}

void Model::setItem(ThreeDItem *item)
{
  _item = item;
  if(_item && (!(_file.isNull() || _file.isEmpty()))) loadFile(_file);
}

int Model::createObject(const QByteArray &modelName,
                        const QByteArray &objectName,
                        const QVector3D &modelUp,
                        float modelHeight,
                        float modelScale,
                        three::quick::Intersect *intersect,
                        const QVariantMap &arguments)
{
  Object3D::Ptr object = importedScene()->getChildByName(modelName.toStdString());
  if(object) {
    Object3D::Ptr marker(object->cloned());

    marker->setName(objectName.toStdString());
    marker->scale() = modelScale;

    const math::Vector3 up(modelUp.x(), modelUp.y(), modelUp.z());
    const Intersection &is = intersect->intersection;
    const math::Vector3 &fnorm = is.face.normal;

    //calculate the rotation for aligning the marker to the mesh surface
    math::Vector3 axis = fnorm.y() == 1 || fnorm.y() == -1 ?
                         math::Vector3( 1, 0, 0 ) : math::cross( up, fnorm );

    float radians = acos( math::dot(fnorm, up) );
    float rad2 = up.angleTo(fnorm);
    float rad3 = fnorm.angleTo(up);

    qDebug() << "setRotationFromAxisAngle" << QVector3D(axis.x(), axis.y(), axis.z()) << radians << rad2 << rad3;

    marker->setRotationFromAxisAngle( axis, M_PI_2 );

    //position into the middle of the face
    marker->position() = is.object->geometry()->centroid(is.face) + is.face.normal * modelHeight;

    // add the marker as a child of the intersected object so it will rotate with it
    is.object->add(marker);
  }
  return 0;
}

}
}
