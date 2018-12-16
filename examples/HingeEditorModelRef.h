//
// Created by byter on 10/23/18.
//

#ifndef THREE_PP_TESTMODELREF_H
#define THREE_PP_TESTMODELREF_H

//#define IS_LOGGING_ACTIVE
#include <threepp/quick/objects/ModelRef.h>
#include <threepp/quick/scene/Dynamics.h>
#include <QStringListModel>

namespace three {
namespace quick {

class Dynamics;

/**
 * subclasses ModelRef to provide support functions for hinge editing based on the
 * physics integration with reactphysics3d
 */
class HingeEditorModelRef : public quick::ModelRef
{
Q_OBJECT
  Q_PROPERTY(QObject *dynamics READ dynamics WRITE setDynamics NOTIFY dynamicsChanged)

  std::vector<DynamicMesh::Ptr> _markers;

  Dynamics *_dynamics = nullptr;

public:
  HingeEditorModelRef(QObject *parent = nullptr)  : ModelRef(parent) {}

  QObject *dynamics() const {return _dynamics;}

  void setDynamics(QObject *physics);

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
  void dynamicsChanged();
};

}
}


#endif //THREE_PP_TESTMODELREF_H
