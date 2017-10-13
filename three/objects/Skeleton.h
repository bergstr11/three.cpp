//
// Created by byter on 13.10.17.
//

#ifndef THREE_QT_SKELETON_H
#define THREE_QT_SKELETON_H

#include <core/Object3D.h>
#include <textures/Texture.h>

namespace three {

class Bone : public Object3D
{
  Bone(Geometry::Ptr geometry) : Object3D(geometry) {}
public:
  using Ptr = std::shared_ptr<Bone>;
  static Ptr make(Geometry::Ptr geometry) {
    return Ptr(new Bone(geometry));
  }
};

class Skeleton
{
  std::vector<Bone::Ptr> _bones;
  std::vector<float> _boneMatrices;
  std::vector<math::Matrix4> _boneInverses;
  Texture::Ptr _boneTexture;

public:
  Skeleton() {}

  explicit Skeleton(const std::vector<Bone::Ptr> bones) : _bones(bones)
  {
    for(Bone::Ptr bone : _bones) {
      _boneInverses.push_back(bone->matrixWorld().inverse());
    }
  }

  const std::vector<Bone::Ptr> bones() const {return _bones;}

  void pose()
  {
    // recover the bind-time world matrices
    size_t index = 0;
    for(Bone::Ptr bone : _bones) {
      bone->matrixWorld() = _boneInverses[index++].inverse();
    }

    // compute the local matrices, positions, rotations and scales
    for(Bone::Ptr bone : _bones) {
      const Bone *parent = bone->parent() ? dynamic_cast<const Bone *>(bone->parent()) : nullptr;
      if (parent) {
        bone->matrix() = parent->matrixWorld().inverse();
        bone->matrix() *= bone->matrixWorld();
      }
      else {
        bone->matrix() = bone->matrixWorld();
      }
      bone->matrix().decompose( bone->position(), bone->quaternion(), bone->scale());
    }
  }

  void update()
  {
    // flatten bone matrices to array
    size_t index = 0;
    for(Bone::Ptr bone : _bones) {
      // compute the offset between the current and the original transform
      math::Matrix4 matrix = bone ? bone->matrixWorld() : math::Matrix4::identity();

      math::Matrix4 offsetMatrix = matrix * _boneInverses[index];
      offsetMatrix.writeTo(_boneMatrices.data(), index * 16);
    }

    if (_boneTexture) {
      _boneTexture->needsUpdate(true);
    }
  }
};

}
#endif //THREE_QT_SKELETON_H
