//
// Created by byter on 10/23/18.
//

#ifndef THREE_PP_TESTMODELREF_H
#define THREE_PP_TESTMODELREF_H

//#define IS_LOGGING_ACTIVE
#include <threepp/quick/objects/ModelRef.h>
#include <threepp/quick/scene/R3dPhysics.hxx>
#include <QStringListModel>

namespace three {
namespace quick {

namespace r3d {
class Physics;
}

/**
 * subclasses ModelRef to provide support functions for hinge editing based on the
 * physics integration with reactphysics3d
 */
class HingeEditorModelRef : public quick::ModelRef
{
Q_OBJECT

  std::vector<DynamicMesh::Ptr> _markers;

  r3d::Physics *_physics = nullptr;

protected:
  Object3D::Ptr _create() override;

public:
  HingeEditorModelRef(QObject *parent = nullptr)  : ModelRef(parent) {}

  /**
   * save the current hinge definition to a file
   *
   * @param file
   */
  Q_INVOKABLE bool saveHingeFile(const QString &file);

  /**
   * set a marker on the model at the last picked position
   */
  Q_INVOKABLE void setMarker(three::quick::ObjectPicker *picker);

  /**
   * reset everything, removing all physics data
   */
  Q_INVOKABLE void resetAll();

  /**
   * remove all current hinge markers
   */
  Q_INVOKABLE void removeMarkers();

signals:
  void hingeNamesChanged();
};

}
}


#endif //THREE_PP_TESTMODELREF_H
