//
// Created by byter on 2/28/18.
//

#include "Camera.h"

namespace three {
namespace helper {

using namespace math;

enum PointID {n1, n2, n3, n4, f1, f2, f3, f4, p, u1, u2, u3, c, t, cn1, cn2, cn3, cn4, cf1, cf2, cf3, cf4};

template <typename ID>
void setPoint(ID point, float x, float y, float z,
              const Matrix4 &world, const Matrix4 &projection,
              std::unordered_map<ID, std::vector<float>> &pointMap,
              BufferAttributeT<float>::Ptr position)
{
  Vector3 vector( x, y, z );
  vector.unproject(world, projection);

  const auto &points = pointMap.find(point);

  if(points != pointMap.end()) {
    for ( unsigned i = 0, l = points->second.size(); i < l; i ++ ) {
      position->setXYZ( points->second[ i ], vector.x(), vector.y(), vector.z() );
    }
  }
}

template <typename ID>
void addPoint(ID id, const Color &color,
              attribute::prealloc_t<float, Vector3> vertices, attribute::prealloc_t<float, Color> colors,
              std::unordered_map<ID, std::vector<float>> &pointMap)
{
  vertices->next() = {0, 0, 0};
  colors->next() = {color.r, color.g, color.b};

  pointMap[ id ].push_back(vertices->itemCount() - 1);
}

template <typename ID>
void addLine(ID a, ID b, const Color &color,
             attribute::prealloc_t<float, Vector3> vertices, attribute::prealloc_t<float, Color> colors,
             std::unordered_map<ID, std::vector<float>> &pointMap)
{
  addPoint( a, color, vertices, colors, pointMap);
  addPoint( b, color, vertices, colors, pointMap);
}

Maker::Maker(three::Camera::Ptr camera) : _helperGeometry(BufferGeometry::make())
{
  auto vertices = attribute::prealloc<float, Vertex>(50, true);
  auto colors = attribute::prealloc<float, Color>(50, true);

  // colors
  Color colorFrustum( 0xffaa00 );
  Color colorCone( 0xff0000 );
  Color colorUp( 0x00aaff );
  Color colorTarget( 0xffffff );
  Color colorCross( 0x333333 );

  // near

  addLine( n1, n2, colorFrustum, vertices, colors, pointMap );
  addLine( n2, n4, colorFrustum, vertices, colors, pointMap );
  addLine( n4, n3, colorFrustum, vertices, colors, pointMap );
  addLine( n3, n1, colorFrustum, vertices, colors, pointMap );

  // far

  addLine( f1, f2, colorFrustum, vertices, colors, pointMap );
  addLine( f2, f4, colorFrustum, vertices, colors, pointMap );
  addLine( f4, f3, colorFrustum, vertices, colors, pointMap );
  addLine( f3, f1, colorFrustum, vertices, colors, pointMap );

  // sides

  addLine( n1, f1, colorFrustum, vertices, colors, pointMap );
  addLine( n2, f2, colorFrustum, vertices, colors, pointMap );
  addLine( n3, f3, colorFrustum, vertices, colors, pointMap );
  addLine( n4, f4, colorFrustum, vertices, colors, pointMap );

  // cone

  addLine( p, n1, colorCone, vertices, colors, pointMap );
  addLine( p, n2, colorCone, vertices, colors, pointMap );
  addLine( p, n3, colorCone, vertices, colors, pointMap );
  addLine( p, n4, colorCone, vertices, colors, pointMap );

  // up

  addLine( u1, u2, colorUp, vertices, colors, pointMap );
  addLine( u2, u3, colorUp, vertices, colors, pointMap );
  addLine( u3, u1, colorUp, vertices, colors, pointMap );

  // target

  addLine( c, t, colorTarget, vertices, colors, pointMap );
  addLine( p, c, colorCross, vertices, colors, pointMap );

  // cross

  addLine( cn1, cn2, colorCross, vertices, colors, pointMap );
  addLine( cn3, cn4, colorCross, vertices, colors, pointMap );

  addLine( cf1, cf2, colorCross, vertices, colors, pointMap );
  addLine( cf3, cf4, colorCross, vertices, colors, pointMap );

  _helperGeometry->setPosition(vertices);
  _helperGeometry->setColor(colors);
}

void Maker::update(const three::Camera::Ptr &camera)
{
  float w = 1, h = 1;

  // we need just camera projection matrix
  // world matrix must be identity

  const auto &pm = camera->projectionMatrix();
  Matrix4 mw = Matrix4::identity();

  BufferAttributeT<float>::Ptr pos = _helperGeometry->position();

  // center / target
  setPoint( c, 0, 0, - 1, pm, mw, pointMap, pos);
  setPoint( t, 0, 0, 1, pm, mw, pointMap, pos);

  // near
  setPoint( n1, - w, - h, - 1, pm, mw, pointMap, pos);
  setPoint( n2, w, - h, - 1, pm, mw, pointMap, pos);
  setPoint( n3, - w, h, - 1, pm, mw, pointMap, pos);
  setPoint( n4, w, h, - 1, pm, mw, pointMap, pos);

  // far
  setPoint( f1, - w, - h, 1, pm, mw, pointMap, pos);
  setPoint( f2, w, - h, 1, pm, mw, pointMap, pos);
  setPoint( f3, - w, h, 1, pm, mw, pointMap, pos);
  setPoint( f4, w, h, 1, pm, mw, pointMap, pos);

  // up
  setPoint( u1, w * 0.7, h * 1.1, - 1, pm, mw, pointMap, pos);
  setPoint( u2, - w * 0.7, h * 1.1, - 1, pm, mw, pointMap, pos);
  setPoint( u3, 0, h * 2, - 1, pm, mw, pointMap, pos);

  // cross
  setPoint( cf1, - w, 0, 1, pm, mw, pointMap, pos);
  setPoint( cf2, w, 0, 1, pm, mw, pointMap, pos);
  setPoint( cf3, 0, - h, 1, pm, mw, pointMap, pos);
  setPoint( cf4, 0, h, 1, pm, mw, pointMap, pos);

  setPoint( cn1, - w, 0, - 1, pm, mw, pointMap, pos);
  setPoint( cn2, w, 0, - 1, pm, mw, pointMap, pos);
  setPoint( cn3, 0, - h, - 1, pm, mw, pointMap, pos);
  setPoint( cn4, 0, h, - 1, pm, mw, pointMap, pos);

  pos->needsUpdate();
}

}
}
