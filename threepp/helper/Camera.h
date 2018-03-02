//
// Created by byter on 2/28/18.
//

#ifndef THREE_PPH_CAMERA_H
#define THREE_PPH_CAMERA_H

#include <memory>

#include <threepp/objects/Line.h>
#include <threepp/core/BufferAttribute.h>
#include <threepp/camera/Camera.h>

namespace three {
namespace helper {

struct Maker {
  enum PointID {n1, n2, n3, n4, f1, f2, f3, f4, p, u1, u2, u3, c, t, cn1, cn2, cn3, cn4, cf1, cf2, cf3, cf4};
  std::unordered_map<PointID, std::vector<float>> pointMap;

  BufferGeometry::Ptr _helperGeometry;

  Maker(three::Camera::Ptr camera);
  void update(const three::Camera::Ptr &camera);
};

class Camera : private Maker, public LineSegments
{
  three::Camera::Ptr _camera;

  Camera(three::Camera::Ptr camera)
     : Object3D(object::ResolverT<LineSegments>::make(*this)),
       Maker(camera), LineSegments(_helperGeometry, LineBasicMaterial::make(Colors::Face))
  {
    camera->updateProjectionMatrix();
    _matrix = camera->matrixWorld();
    matrixAutoUpdate = false;
    update(camera);
  }

public:
  using Ptr = std::shared_ptr<Camera>;
  static Ptr make(std::string name, three::Camera::Ptr camera) {
    Ptr p(new Camera(camera));
    p->_name = name;
    return p;
  }
};

}
}

#endif //THREE_PPH_CAMERA_H
