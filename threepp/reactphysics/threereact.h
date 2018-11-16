//
// Created by byter on 10/21/18.
//

#ifndef THREE_PP_MODEL_H
#define THREE_PP_MODEL_H

#include <threepp/core/Object3D.h>
#include <threepp/scene/Scene.h>
#include <threepp/objects/Mesh.h>
#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/engine/Timer.h>
#include <unordered_map>
#include <set>
#include <mutex>

namespace reactphysics3d {
  class RigidBody;
}

namespace three {
namespace react3d {

/**
 * object physics representation
 */
class PhysicsObject
{
  friend class PhysicsScene;

  math::Box3 _bodyBox;
  math::Vector3 _boxPosition;
  Object3D::Ptr _object;

  rp3d::RigidBody * _body;
  rp3d::Transform _previousTransform;

  math::Matrix4 _scalingMatrix = math::Matrix4::identity();

public:
  PhysicsObject(Object3D::Ptr object) : _object(object) {}

  void updateTransform(float interpolation);

  void updateFromObject();

  const Object3D::Ptr &object() {return _object;}

  const math::Box3 &getBoundingBox() {return _bodyBox;}

  rp3d::BoxShape *createBoundingBoxShape();

  rp3d::RigidBody * body() const {return _body;}
  const three::math::Vector3 boxPosition(Object3D::Ptr object=nullptr);
};

/**
 * physics scene representation
 */
class PhysicsScene
{
  rp3d::Timer _timer;

  rp3d::DynamicsWorld * const _dynamicsWorld;

  std::mutex _createBodyMutex;

  std::unordered_map<Object3D *, PhysicsObject> _objects;
  std::set<rp3d::Joint*> _joints;

protected:
  virtual void preUpdate() {}
  virtual void postUpdate() {}

public:
  PhysicsScene(Scene::Ptr object, const rp3d::Vector3 &gravity, const rp3d::WorldSettings &worldSettings);

  virtual ~PhysicsScene();

  using Ptr = std::shared_ptr<PhysicsScene>;
  static Ptr make2(Scene::Ptr scene, const rp3d::Vector3 &gravity, const rp3d::WorldSettings &worldSettings) {
    return Ptr(new PhysicsScene(scene, gravity, worldSettings));
  }

  rp3d::Timer &timer() {return _timer;}

  void reset();

  /**
   * update the physics world using the timer
   */
  void timedUpdate();

  /**
   * update the physics world using a fixed interpolation factor
   */
  void update(float factor);

  rp3d::DynamicsWorld *dynamicsWorld() {return _dynamicsWorld;}

  rp3d::HingeJoint* createHingeJoint(const rp3d::HingeJointInfo& jointInfo);

  PhysicsObject *createPhysics(Object3D::Ptr object, rp3d::BodyType);

  PhysicsObject *getPhysics(Object3D::Ptr object);

  void destroy(Object3D::Ptr object);
  void destroy(rp3d::Joint *joint);
};

enum class HingeType {DOOR, PROPELLER};
enum class HingeDir {LEFT, RIGHT, UP, DOWN};

/**
 * Hinge definition
 */
struct HingeData
{
  HingeType hingeType;
  std::string name;
  Object3D::Ptr anchor;
  Object3D::Ptr element;
  math::Vector3 hingePoint1;
  math::Vector3 hingePoint2;
  float minAngleLimit;
  float maxAngleLimit;

  HingeDir hingeDir;
  rp3d::HingeJoint *joint = nullptr;
  react3d::PhysicsObject *elementPhysics = nullptr;
  react3d::PhysicsObject *anchorPhysics = nullptr;

  bool needsUpdate = false;

  HingeData(HingeType hingeType) : hingeType(hingeType)
  {}

  void doUpdate();
  void requestUpdate(const math::Quaternion &q);

  using Ptr = std::shared_ptr<HingeData>;
  static Ptr make(HingeType hingeType) {
    return Ptr(new HingeData(hingeType));
  }
};

}
}

#endif //THREE_PP_MODEL_H
