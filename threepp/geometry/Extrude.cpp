//
// Created by byter on 5/27/18.
//

#include "Extrude.h"
#include <threepp/math/Math.h>

namespace three {
namespace geometry {

using namespace std;
using namespace math;
using namespace extras;

Extrude::Extrude(const std::vector<extras::Shape::Ptr> &shapes, const ExtrudeOptions &options)
   : _shapes(shapes), _options(options)
{
  set(buffer::Extrude(_shapes, options));
  mergeVertices();
}

Extrude::Extrude(extras::Shape::Ptr shape, const ExtrudeOptions &options) : _options(options)
{
  _shapes.push_back(shape);

  set(buffer::Extrude(_shapes, options));
  mergeVertices();
}

namespace buffer {

struct Builder : private ExtrudeOptions
{
  friend class Extrude;

  bool extrudeByPath = false;

  vector<Vector2> contour;
  std::vector<Vector2> vertices;
  std::vector<Vector3> placeholder;
  std::vector<std::vector<Vector2>> holes;
  vector<shapeutils::Face> faces;

  BufferGeometry &geometry;

  unsigned shapeIndex = 0;
  ShapeGroup *currentShapeGroup = nullptr;

  attribute::growing_t<float, Vertex> positions;
  attribute::growing_t<float, UV> uvs;

  void f3( unsigned a, unsigned b, unsigned c );
  void f4( unsigned a, unsigned b, unsigned c, unsigned d );

  void buildLidFaces();
  void buildSideFaces();
  void sidewalls( const vector<Vector2> &contour, unsigned layeroffset );
  void setShapeGroup();

  void addShape( Shape::Ptr shape);

  Builder(const ExtrudeOptions &options, BufferGeometry &geometry)
     : ExtrudeOptions(options), geometry(geometry),
       positions(attribute::growing<float, Vertex>()), uvs(attribute::growing<float, UV>())
  {
    if(!uvGenerator) uvGenerator = WorldUVGenerator::make();
  }
};

Vector2 scalePt2( Vector2 pt, Vector2 vec, float size ) {

  return vec * size + pt;
}

Vector2 getBevelVec( const Vector2 &inPt, const Vector2 &inPrev, const Vector2 &inNext )
{
  // computes for inPt the corresponding point inPt' on a new contour
  //   shifted by 1 unit (length of normalized vector) to the left
  // if we walk along contour clockwise, this new contour is outside the old one
  //
  // inPt' is the intersection of the two lines parallel to the two
  //  adjacent edges of inPt at a distance of 1 unit on the left side.

  float v_trans_x, v_trans_y, shrink_by; // resulting translation vector for inPt

  // good reading for geometry algorithms (here: line-line intersection)
  // http://geomalgorithms.com/a05-_intersect-1.html

  float v_prev_x = inPt.x() - inPrev.x(),
     v_prev_y = inPt.y() - inPrev.y();
  float v_next_x = inNext.x() - inPt.x(),
     v_next_y = inNext.y() - inPt.y();

  float v_prev_lensq = ( v_prev_x * v_prev_x + v_prev_y * v_prev_y );

  // check for collinear edges
  float collinear0 = ( v_prev_x * v_next_y - v_prev_y * v_next_x );

  if ( std::abs( collinear0 ) > numeric_limits<float>::epsilon()) {

    // not collinear

    // length of vectors for normalizing

    float v_prev_len = sqrt( v_prev_lensq );
    float v_next_len = sqrt( v_next_x * v_next_x + v_next_y * v_next_y );

    // shift adjacent points by unit vectors to the left

    float ptPrevShift_x = ( inPrev.x() - v_prev_y / v_prev_len );
    float ptPrevShift_y = ( inPrev.y() + v_prev_x / v_prev_len );

    float ptNextShift_x = ( inNext.x() - v_next_y / v_next_len );
    float ptNextShift_y = ( inNext.y() + v_next_x / v_next_len );

    // scaling factor for v_prev to intersection point

    float sf = ( ( ptNextShift_x - ptPrevShift_x ) * v_next_y -
               ( ptNextShift_y - ptPrevShift_y ) * v_next_x ) /
             ( v_prev_x * v_next_y - v_prev_y * v_next_x );

    // vector from inPt to intersection point

    v_trans_x = ( ptPrevShift_x + v_prev_x * sf - inPt.x() );
    v_trans_y = ( ptPrevShift_y + v_prev_y * sf - inPt.y() );

    // Don't normalize!, otherwise sharp corners become ugly
    //  but prevent crazy spikes
    float v_trans_lensq = ( v_trans_x * v_trans_x + v_trans_y * v_trans_y );
    if ( v_trans_lensq <= 2.0f ) {

      return Vector2( v_trans_x, v_trans_y );

    } else {

      shrink_by = sqrt( v_trans_lensq / 2 );
    }
  }
  else {

    // handle special case of collinear edges

    bool direction_eq = false; // assumes: opposite
    if ( v_prev_x > numeric_limits<float>::epsilon() ) {

      if ( v_next_x > numeric_limits<float>::epsilon() ) {

        direction_eq = true;
      }
    }
    else {

      if ( v_prev_x < -numeric_limits<float>::epsilon() ) {

        if ( v_next_x < -numeric_limits<float>::epsilon() ) {

          direction_eq = true;
        }
      }
      else {

        if ( sgn( v_prev_y ) == sgn( v_next_y ) ) {

          direction_eq = true;
        }
      }
    }

    if ( direction_eq ) {

      // console.log("Warning: lines are a straight sequence");
      v_trans_x = - v_prev_y;
      v_trans_y = v_prev_x;
      shrink_by = sqrt( v_prev_lensq );

    } else {

      // console.log("Warning: lines are a straight spike");
      v_trans_x = v_prev_x;
      v_trans_y = v_prev_y;
      shrink_by = sqrt( v_prev_lensq / 2 );
    }
  }

  return Vector2( v_trans_x / shrink_by, v_trans_y / shrink_by );
}

void Builder::sidewalls( const vector<Vector2> &contour, unsigned layeroffset )
{
  for (int i = contour.size(); -- i >= 0; ) {

    int j = i;
    int k = i - 1;
    if ( k < 0 ) k = contour.size() - 1;

    unsigned s = 0, sl = steps + bevelSegments * 2;

    for ( s = 0; s < sl; s ++ ) {

      unsigned slen1 = vertices.size() * s;
      unsigned slen2 = vertices.size() * ( s + 1 );

      f4( layeroffset + j + slen1,
          layeroffset + k + slen1,
          layeroffset + k + slen2,
          layeroffset + j + slen2 );
    }
  }
}

void Builder::buildLidFaces()
{
  unsigned start = positions->itemCount();

  if ( bevelEnabled ) {

    unsigned layer = 0; // steps + 1
    unsigned offset = vertices.size() * layer;

    // Bottom faces
    for ( const auto &face : faces ) {

      f3( face[ 2 ] + offset, face[ 1 ] + offset, face[ 0 ] + offset );
    }

    layer = steps + bevelSegments * 2;
    offset = vertices.size() * layer;

    // Top faces
    for ( const auto &face : faces ) {

      f3( face[ 0 ] + offset, face[ 1 ] + offset, face[ 2 ] + offset );
    }
  }
  else {

    // Bottom faces
    for ( const auto &face : faces ) {

      f3( face[ 2 ], face[ 1 ], face[ 0 ] );
    }

    // Top faces
    for ( const auto &face : faces ) {

      f3( face[ 0 ] + vertices.size() * steps,
          face[ 1 ] + vertices.size() * steps,
          face[ 2 ] + vertices.size()* steps );
    }
  }

  if(currentShapeGroup) {
    if(currentShapeGroup->posStart < 0) currentShapeGroup->posStart = start;
    currentShapeGroup->posCount += positions->itemCount() - start;
  }
  else
    geometry.addGroup( start, positions->itemCount() - start, 0 );
}

// Create faces for the z-sides of the shape

void Builder::buildSideFaces()
{
  unsigned start = positions->itemCount();
  unsigned layeroffset = 0;

  sidewalls( contour, layeroffset );

  layeroffset += contour.size();

  for ( const auto &hole : holes) {

    sidewalls( hole, layeroffset );

    //, true
    layeroffset += hole.size();
  }

  if(currentShapeGroup) {
    if(currentShapeGroup->posStart < 0) currentShapeGroup->posStart = start;
    currentShapeGroup->posCount += positions->itemCount() - start;
  }
  else
    geometry.addGroup( start, positions->itemCount() - start, 1 );
}

void Builder::f3( unsigned a, unsigned b, unsigned c )
{
  positions->next() = placeholder[a];
  positions->next() = placeholder[b];
  positions->next() = placeholder[c];

  unsigned nextIndex = positions->itemCount();
  uvGenerator->generateTopUV( positions, uvs, nextIndex - 3, nextIndex - 2, nextIndex - 1 );
}

void Builder::f4( unsigned a, unsigned b, unsigned c, unsigned d ) {

  positions->next() = placeholder[a];
  positions->next() = placeholder[b];
  positions->next() = placeholder[d];

  positions->next() = placeholder[b];
  positions->next() = placeholder[c];
  positions->next() = placeholder[d];

  unsigned nextIndex = positions->itemCount();
  uvGenerator->generateSideWallUV( positions, uvs, nextIndex - 6, nextIndex - 3, nextIndex - 2, nextIndex - 1 );
}

void Builder::setShapeGroup()
{
  if(!shapeGroups.empty()) {
    if(!currentShapeGroup ||
       shapeIndex < currentShapeGroup->shapeStart ||
       shapeIndex >= currentShapeGroup->shapeStart + currentShapeGroup->shapeCount) {
      //not current sg, find a new one
      currentShapeGroup = nullptr;
      for(auto &sg : shapeGroups) {
        if(shapeIndex >= sg.shapeStart && shapeIndex < sg.shapeStart + sg.shapeCount) {
          currentShapeGroup = &sg;
          break;
        }
      }
    }
  }
  shapeIndex++;
}

void Builder::addShape( Shape::Ptr shape )
{
  setShapeGroup();

  contour.clear();
  vertices.clear();
  placeholder.clear();
  holes.clear();
  faces.clear();

  std::vector<Vector3> splineTube_tangents;
  std::vector<Vector3> splineTube_normals;
  std::vector<Vector3> splineTube_binormals;
  std::vector<Vector2> extrudePts;

  if ( extrudePath ) {

    extrudePath->getSpacedPoints( steps, extrudePts );

    extrudeByPath = true;
    bevelEnabled = false; // bevels not supported for path extrusion

    // SETUP TNB variables

    // TODO1 - have a .isClosed in spline?

    extrudePath->computeFrenetFrames( steps, false, splineTube_tangents, splineTube_normals, splineTube_binormals );
  }

  // Safeguards if bevels are not enabled

  if ( ! bevelEnabled ) {

    bevelSegments = 0;
    bevelThickness = 0;
    bevelSize = 0;
  }

  // Variables initialization

  shape->extractPoints( curveSegments, vertices, holes );

  bool reverse = !shapeutils::isClockWise( vertices );
  if ( reverse ) {

    std::reverse(vertices.begin(), vertices.end());

    // Maybe we should also check if holes are in the opposite direction, just to be safe ...
    for ( auto &hole : holes ) {

      if ( shapeutils::isClockWise( hole ) ) {

        std::reverse(hole.begin(), hole.end());
      }
    }
  }


  faces = shapeutils::triangulateShape( vertices, holes );

  /* Vertices */

  contour = vertices; // vertices has all points but contour has only points of circumference
  for (const auto &hole : holes) {

    vertices.insert(vertices.end(), hole.begin(), hole.end());
  }

  // Find directions for point movement

  vector<Vector2> contourMovements;

  for ( unsigned i = 0, il = contour.size(), j = il - 1, k = i + 1; i < il; i ++, j ++, k ++ ) {

    if ( j == il ) j = 0;
    if ( k == il ) k = 0;

    //  (j)---(i)---(k)
    // console.log('i,j,k', i, j , k)

    contourMovements.push_back(getBevelVec( contour[ i ], contour[ j ], contour[ k ] ));
  }

  vector<Vector2> verticesMovements = contourMovements;
  vector<vector<Vector2>> holesMovements;

  for ( unsigned h = 0, hl = holes.size(); h < hl; h ++ ) {

    const auto &ahole = holes[ h ];

    vector<Vector2> oneHoleMovements;

    for ( unsigned i = 0, il = ahole.size(), j = il - 1, k = i + 1; i < il; i ++, j ++, k ++ ) {

      if ( j == il ) j = 0;
      if ( k == il ) k = 0;

      //  (j)---(i)---(k)
      oneHoleMovements.push_back(getBevelVec( ahole[ i ], ahole[ j ], ahole[ k ] ));
    }

    holesMovements.push_back( oneHoleMovements );
    verticesMovements.insert(verticesMovements.end(), oneHoleMovements.begin(), oneHoleMovements.end() );
  }


  // Loop bevelSegments, 1 for the front, 1 for the back

  for ( unsigned b = 0; b < bevelSegments; b ++ ) {

    //for ( b = bevelSegments; b > 0; b -- ) {

    float t = (float)b / bevelSegments;
    float z = bevelThickness * cos( t * (float)M_PI_2 );
    float bs = bevelSize * sin( t * (float)M_PI_2 );

    // contract shape

    for ( unsigned i = 0, il = contour.size(); i < il; i ++ ) {

      Vector2 vert = scalePt2( contour[ i ], contourMovements[ i ], bs );

      placeholder.emplace_back( vert.x(), vert.y(), - z );
    }

    // expand holes

    for ( unsigned h = 0, hl = holes.size(); h < hl; h ++ ) {

      const auto &ahole = holes[ h ];
      const auto &oneHoleMovements = holesMovements[ h ];

      for ( unsigned i = 0, il = ahole.size(); i < il; i ++ ) {

        Vector2 vert = scalePt2( ahole[ i ], oneHoleMovements[ i ], bs );

        placeholder.emplace_back( vert.x(), vert.y(), - z );
      }
    }
  }

  float bs = bevelSize;

  // Back facing vertices

  for ( unsigned i = 0; i < vertices.size(); i ++ ) {

    Vector2 vert = bevelEnabled ? scalePt2( vertices[ i ], verticesMovements[ i ], bs ) : vertices[ i ];

    if ( ! extrudeByPath ) {

      placeholder.emplace_back( vert.x(), vert.y(), 0 );
    }
    else {

      // v( vert.x, vert.y + extrudePts[ 0 ].y, extrudePts[ 0 ].x );

      Vector3 normal = splineTube_normals[ 0 ] * vert.x();
      Vector3 binormal = splineTube_binormals[ 0 ] * vert.y();

      Vector3 position2 = extrudePts[ 0 ] + normal + binormal;

      placeholder.emplace_back( position2.x(), position2.y(), position2.z() );
    }
  }

  // Add stepped vertices...
  // Including front facing vertices

  for ( unsigned s = 1; s <= steps; s ++ ) {

    for ( unsigned i = 0; i < vertices.size(); i ++ ) {

      Vector2 vert = bevelEnabled ? scalePt2( vertices[ i ], verticesMovements[ i ], bs ) : vertices[ i ];

      if ( ! extrudeByPath ) {

        placeholder.emplace_back( vert.x(), vert.y(), depth / steps * s );
      }
      else {

        // v( vert.x, vert.y + extrudePts[ s - 1 ].y, extrudePts[ s - 1 ].x );

        Vector3 normal = splineTube_normals[ s ] * vert.x();
        Vector3 binormal = splineTube_binormals[ s ] * vert.y();

        Vector3 position2 = extrudePts[ s ] + normal + binormal;

        placeholder.emplace_back( position2.x(), position2.y(), position2.z() );
      }
    }
  }


  // Add bevel segments planes

  //for ( b = 1; b <= bevelSegments; b ++ ) {
  for ( int b = bevelSegments - 1; b >= 0; b -- ) {

    float t = (float)b / bevelSegments;
    float z = bevelThickness * cos( t * (float)M_PI_2 );
    bs = bevelSize * sin( t * (float)M_PI_2 );

    // contract shape
    for ( unsigned i = 0, il = contour.size(); i < il; i ++ ) {

      Vector2 vert = scalePt2( contour[ i ], contourMovements[ i ], bs );
      placeholder.emplace_back( vert.x(), vert.y(), depth + z );
    }

    // expand holes
    for ( unsigned h = 0, hl = holes.size(); h < hl; h ++ ) {

      const auto &ahole = holes[ h ];
      const auto &oneHoleMovements = holesMovements[ h ];

      for ( unsigned i = 0, il = ahole.size(); i < il; i ++ ) {

        Vector2 vert = scalePt2( ahole[ i ], oneHoleMovements[ i ], bs );

        if ( ! extrudeByPath ) {

          placeholder.emplace_back( vert.x(), vert.y(), depth + z );
        }
        else {

          placeholder.emplace_back( vert.x(),
                                    vert.y() + extrudePts[ steps - 1 ].y(),
                                    extrudePts[ steps - 1 ].x() + z );
        }
      }
    }
  }

  /* Faces */

  // Top and bottom faces
  buildLidFaces();

  // Sides faces
  buildSideFaces();
}

Extrude::Extrude(const std::vector<Shape::Ptr> &shapes, const ExtrudeOptions &options)
{
  Builder builder(options, *this);

  for(Shape::Ptr shape : shapes) {
    builder.addShape(shape);
  }
  for(const auto &sg : builder.shapeGroups) {
    addGroup(sg.posStart, sg.posCount, sg.group);
  }
  setPosition(builder.positions);
  setUV(builder.uvs);

  computeVertexNormals();
}

}

void WorldUVGenerator::generateTopUV(const attribute::growing_t<float, Vertex> &positions,
                                     attribute::growing_t<float, UV> &uvs,
                                     unsigned indexA, unsigned indexB, unsigned indexC )
{
  const auto &itemA = positions->item(indexA);
  const auto &itemB = positions->item(indexB);
  const auto &itemC = positions->item(indexC);

  uvs->next() = {itemA.x(), itemA.y()};
  uvs->next() = {itemB.x(), itemB.y()};
  uvs->next() = {itemC.x(), itemC.y()};
}

void WorldUVGenerator::generateSideWallUV(const attribute::growing_t<float, Vertex> &positions,
                                          attribute::growing_t<float, UV> &uvs,
                                          unsigned indexA, unsigned indexB, unsigned indexC, unsigned indexD )
{
  const auto &itemA = positions->item(indexA);
  const auto &itemB = positions->item(indexB);
  const auto &itemC = positions->item(indexC);
  const auto &itemD = positions->item(indexD);

  if ( abs( itemA.y() - itemB.y() ) < 0.01 ) {

    uvs->next() = {itemA.x(), 1 - itemA.z()}; //0
    uvs->next() = {itemB.x(), 1 - itemB.z()}; //1
    uvs->next() = {itemD.x(), 1 - itemD.z()}; //3

    uvs->next() = {itemB.x(), 1 - itemB.z()}; //1
    uvs->next() = {itemC.x(), 1 - itemC.z()}; //2
    uvs->next() = {itemD.x(), 1 - itemD.z()}; //3
  }
  else {

    uvs->next() = {itemA.y(), 1 - itemA.z()}; //0
    uvs->next() = {itemB.y(), 1 - itemB.z()}; //1
    uvs->next() = {itemD.y(), 1 - itemD.z()}; //3

    uvs->next() = {itemB.y(), 1 - itemB.z()}; //1
    uvs->next() = {itemC.y(), 1 - itemC.z()}; //2
    uvs->next() = {itemD.y(), 1 - itemD.z()}; //3
  }
}

}
}
