//
// Created by byter on 5/2/18.
//

#ifndef THREE_PP_QUICKHULL_H
#define THREE_PP_QUICKHULL_H

#include <threepp/util/PointsWalker.h>
#include <threepp/math/Line3.h>
#include <threepp/math/Plane.h>
#include <threepp/math/Triangle.h>
#include <iterator>
#include <limits>

namespace three {

class QuickHull
{
  class Face;

  struct VertexNode
  {
    Vertex point;
    VertexNode *prev = nullptr;
    VertexNode *next = nullptr;
    Face *face = nullptr; // the face that is able to see this vertex

    VertexNode(const Vertex &point) : point(point) {}
  };

  struct HalfEdge
  {
    VertexNode vertex;
    HalfEdge *prev = nullptr;
    HalfEdge *next = nullptr;
    HalfEdge *twin = nullptr;
    Face *face;

    HalfEdge(const VertexNode &vertex, Face &face) : vertex(vertex), face(&face) {}

    HalfEdge &setTwin(HalfEdge &edge )
    {
      twin = &edge;
      edge.twin = this;

      return *this;
    }

    VertexNode *head() {

      return &vertex;
    }

    VertexNode *tail() {

      return prev ? &prev->vertex : nullptr;
    }
  };

  enum class Mark {Visible, Deleted};

  struct Face
  {
    Vertex normal;
    Vertex midpoint;

    // signed distance from face to the origin
    float constant = 0;

    // reference to a vertex in a vertex list this face can see
    VertexNode *outside = nullptr;

    Mark mark = Mark::Visible;
    std::array<HalfEdge, 3> edges;

    Face(const VertexNode &a, const VertexNode &b, const VertexNode &c )
       : edges({HalfEdge(a, *this), HalfEdge(b, *this), HalfEdge(c, *this)})
    {
      // join edges
      edges[0].next = edges[2].prev = &edges[1];
      edges[1].next = edges[0].prev = &edges[2];
      edges[2].next = edges[1].prev = &edges[0];

      compute();
    }

    void compute()
    {
      VertexNode &a = edges[2].vertex;
      VertexNode &b = edges[0].vertex;
      VertexNode &c = edges[1].vertex;

      math::Triangle triangle( a.point, b.point, c.point );

      normal = triangle.getNormal();
      midpoint = triangle.getMidpoint();

      constant = math::dot(normal, midpoint);
    }

    HalfEdge &getEdge(int i) const
    {
      int index = i % 3;
      return i >= 0 ? (HalfEdge &)edges.at(index) : (HalfEdge &)edges.at(3 + index);
    }

    float distanceToPoint(const Vertex &point) const
    {
      return math::dot(normal, point) - constant;
    }
  };

  struct VertexList
  {
    VertexNode *head = nullptr, *tail = nullptr;

    bool empty() {
      return !head;
    }

    VertexList &clear()
    {
      head = tail = nullptr;

      return *this;
    }

    // Inserts a vertex before the target vertex
    VertexList &insertBefore(VertexNode &target, VertexNode &vertex)
    {
      vertex.prev = target.prev;
      vertex.next = &target;

      if (!vertex.prev) {

        head = &vertex;

      } else {

        vertex.prev->next = &vertex;
      }

      target.prev = &vertex;

      return *this;
    }

    // Inserts a vertex after the target vertex
    VertexList &insertAfter(VertexNode &target, VertexNode &vertex )
    {
      vertex.prev = &target;
      vertex.next = target.next;

      if (!vertex.next ) {

        tail = &vertex;
      }
      else {

        vertex.next->prev = &vertex;
      }

      target.next = &vertex;

      return *this;
    }

    // Appends a vertex to the end of the linked list
    VertexList &append( VertexNode &vertex )
    {
      if ( !head ) {

        head = &vertex;
      }
      else {

        tail->next = &vertex;
      }

      vertex.prev = tail;
      vertex.next = nullptr; // the tail has no subsequent vertex

      tail = &vertex;

      return *this;
    }

    // Removes a vertex from the linked list
    VertexList &remove( VertexNode *vertex )
    {
      if ( !vertex->prev ) {

        head = vertex->next;
      }
      else {

        vertex->prev->next = vertex->next;
      }

      if ( !vertex->next ) {

        tail = vertex->prev;
      }
      else {

        vertex->next->prev = vertex->prev;
      }

      return *this;
    }

    VertexList &removeSubList( VertexNode *a, VertexNode *b )
    {
      if ( !a->prev ) {

        head = b->next;
      }
      else {

        a->prev->next = b->next;
      }

      if ( !b->next ) {

        tail = a->prev;
      }
      else {

        b->next->prev = a->prev;
      }

      return *this;
    }

    /*
     * Appends a chain of vertices where 'vertex' is the head.
     */
    VertexList &appendChain( VertexNode *vertex )
    {
      if ( !head ) {

        head = vertex;
      }
      else {

        tail->next = vertex;
      }

      vertex->prev = tail;

      // ensure that the 'tail' reference points to the last vertex of the chain
      while ( vertex->next ) {

        vertex = vertex->next;
      }

      tail = vertex;

      return *this;
    }
  };

  float tolerance = - 1;

  std::vector<Face *> newFaces; // this array holds the faces that are generated within a single iteration

  // the vertex lists work as follows:
  //
  // let 'a' and 'b' be 'Face' instances
  // let 'v' be points wrapped as instance of 'Vertex'
  //
  //     [v, v, ..., v, v, v, ...]
  //      ^             ^
  //      |             |
  //  a.outside     b.outside
  //
  VertexList assigned;
  VertexList unassigned;

  QuickHull &clear()
  {
    faces.clear();
    vertices.clear();

    return *this;
  }

public:
  std::vector<Face> faces; // the generated faces of the convex hull
  std::vector<VertexNode> vertices; // vertices of the hull (internal representation of given geometry data)

private:
  /*
   * Adds a vertex to the 'assigned' list of vertices and assigns it to the given face
   */
  void addVertexToFace(VertexNode &vertex, Face &face )
  {
    vertex.face = &face;

    if ( !face.outside ) {

      assigned.append( vertex );
    }
    else {

      assigned.insertBefore( *face.outside, vertex );
    }

    face.outside = &vertex;
  }

  /**
   * Computes the extremes of a simplex which will be the initial hull
   *
   * @param minVertices (out) indices into _vertices
   * @param maxVertices (out) indices into _vertices
   */
  void computeExtremes(std::array<unsigned, 3> &minVertices, std::array<unsigned, 3> &maxVertices)
  {
    Vertex min(vertices[0].point);
    Vertex max(vertices[0].point);

    // compute the min/max vertex on all six directions
    for (unsigned i = 0, l = vertices.size(); i < l; i ++ ) {

      VertexNode &vertex = vertices[ i ];
      const Vertex &point = vertex.point;

      // update the min coordinates
      for (unsigned j = 0; j < 3; j ++ ) {

        if ( point[j] < min[j] ) {

          min[j] = point[j];
          minVertices[ j ] = i;
        }
      }

      // update the max coordinates
      for (unsigned j = 0; j < 3; j ++ ) {

        if ( point[j] > max[j] ) {

          max[j] = point[j];
          maxVertices[ j ] = i;
        }
      }
    }

    // use min/max vectors to compute an optimal epsilon
    tolerance = 3 * std::numeric_limits<float>::epsilon() * (
       std::max( std::abs( min.x() ), std::abs( max.x() ) ) +
       std::max( std::abs( min.y() ), std::abs( max.y() ) ) +
       std::max( std::abs( min.z() ), std::abs( max.z() ) )
    );
  }

  /**
   * Computes the initial simplex assigning to its faces all the points
   * that are candidates to form part of the hull
   */
  void computeInitialHull()
  {
    std::array<unsigned, 3> min {0, 0, 0};
    std::array<unsigned, 3> max {0, 0, 0};
    computeExtremes(min, max);

    // 1. Find the two vertices 'v0' and 'v1' with the greatest 1d separation
    // (max.x - min.x)
    // (max.y - min.y)
    // (max.z - min.z)

    float maxDistance = 0;
    unsigned index = 0;

    for (unsigned i = 0; i < 3; i ++ ) {

      float distance = vertices[max[i]].point[i] - vertices[min[i]].point[i];
      if ( distance > maxDistance ) {

        maxDistance = distance;
        index = i;
      }
    }

    VertexNode &v0 = vertices[min[ index ]];
    VertexNode &v1 = vertices[max[ index ]];

    // 2. The next vertex 'v2' is the one farthest to the line formed by 'v0' and 'v1'
    maxDistance = 0;
    unsigned v2Index;
    math::Line3 line3( v0.point, v1.point );

    for (unsigned i = 0, l = vertices.size(); i < l; i ++ ) {

      if (i != min[ index ] && i != max[ index ]) {

        VertexNode &vertex = vertices[ i ];

        Vertex closestPoint = line3.closestPointToPoint( vertex.point, true );

        float distance = closestPoint.distanceToSquared( vertex.point );

        if ( distance > maxDistance ) {

          maxDistance = distance;
          v2Index = i;
        }
      }
    }

    VertexNode &v2 = vertices[v2Index];

    // 3. The next vertex 'v3' is the one farthest to the plane 'v0', 'v1', 'v2'
    maxDistance = - 1;
    unsigned v3Index;
    math::Plane plane = math::Plane::fromCoplanarPoints( v0.point, v1.point, v2.point );

    for (unsigned i = 0, l = vertices.size(); i < l; i ++ ) {

      if ( i != min[ index ] && i != max[ index ] && i != v2Index ) {

        VertexNode &vertex = vertices[ i ];

        float distance = std::abs( plane.distanceToPoint( vertex.point ) );

        if ( distance > maxDistance ) {

          maxDistance = distance;
          v3Index = i;
        }
      }
    }

    VertexNode & v3 = vertices[v3Index];
    std::vector<Face> faces;

    if ( plane.distanceToPoint( v3.point ) < 0 ) {
      // the face is not able to see the point so 'plane.normal' is pointing outside the tetrahedron

      faces.emplace_back(v0, v1, v2);
      faces.emplace_back(v3, v1, v0);
      faces.emplace_back(v3, v2, v1);
      faces.emplace_back(v3, v0, v2);

      // set the twin edge
      for (unsigned i = 0; i < 3; i ++ ) {

        unsigned j = ( i + 1 ) % 3;

        // join face[ i ] i > 0, with the first face
        faces[ i + 1 ].getEdge( 2 ).setTwin( faces[ 0 ].getEdge( j ) );

        // join face[ i ] with face[ i + 1 ], 1 <= i <= 3
        faces[ i + 1 ].getEdge( 1 ).setTwin( faces[ j + 1 ].getEdge( 0 ) );
      }
    }
    else {
      // the face is able to see the point so 'plane.normal' is pointing inside the tetrahedron

      faces.emplace_back( v0, v2, v1 );
      faces.emplace_back( v3, v0, v1 );
      faces.emplace_back( v3, v1, v2 );
      faces.emplace_back( v3, v2, v0 );

      // set the twin edge

      for (unsigned i = 0; i < 3; i ++ ) {

        unsigned j = ( i + 1 ) % 3;

        // join face[ i ] i > 0, with the first face
        faces[ i + 1 ].getEdge( 2 ).setTwin( faces[ 0 ].getEdge( ( 3 - i ) % 3 ) );

        // join face[ i ] with face[ i + 1 ]
        faces[ i + 1 ].getEdge( 0 ).setTwin( faces[ j + 1 ].getEdge( 1 ) );
      }
    }

    // the initial hull is the tetrahedron
    for (unsigned i = 0; i < 4; i ++ ) {
      faces.push_back( faces[ i ] );
    }

    // initial assignment of vertices to the faces of the tetrahedron
    for (unsigned i = 0, l = vertices.size(); i < l; i ++ ) {

      if ( i != min[ index ] && i != max[ index ] && i != v2Index && i != v3Index) {

        maxDistance = tolerance;
        int maxFaceIndex = -1;

        VertexNode &vertex = vertices[ i ];

        for (unsigned j = 0; j < 4; j ++ ) {

          float distance = faces[ j ].distanceToPoint( vertex.point );

          if ( distance > maxDistance ) {

            maxDistance = distance;
            maxFaceIndex = j;
          }
        }

        if (maxFaceIndex >= 0) {

          addVertexToFace( vertex, faces[maxFaceIndex] );
        }
      }
    }
  }

  // Removes all the visible vertices that a given face is able to see which are stored in the 'assigned' vertext list

  VertexNode *removeAllVerticesFromFace( Face &face ) {

    if ( face.outside ) {

      // reference to the first and last vertex of this face
      VertexNode *start = face.outside;
      VertexNode *end = face.outside;

      while ( end->next && end->next->face == &face ) {

        end = end->next;
      }

      assigned.removeSubList( start, end );

      // fix references
      start->prev = end->next = nullptr;
      face.outside = nullptr;

      return start;
    }
    return nullptr;
  }

  /*
   * Removes all the visible vertices that 'face' is able to see
   */
  QuickHull & deleteFaceVertices( Face &face, Face *absorbingFace )
  {
    VertexNode *faceVertices = removeAllVerticesFromFace( face );

    if ( faceVertices ) {

      if ( !absorbingFace ) {

        // mark the vertices to be reassigned to some other face
        unassigned.appendChain( faceVertices );
      }
      else {
        // if there's an absorbing face try to assign as many vertices as possible to it
        VertexNode *vertex = faceVertices;

        do {

          // we need to buffer the subsequent vertex at this point because the 'vertex.next' reference
          // will be changed by upcoming method calls
          VertexNode *nextVertex = vertex->next;

          float distance = absorbingFace->distanceToPoint( vertex->point );

          // check if 'vertex' is able to see 'absorbingFace'
          if ( distance > tolerance ) {

            addVertexToFace( *vertex, *absorbingFace );
          }
          else {

            unassigned.append( *vertex );
          }

          // now assign next vertex
          vertex = nextVertex;

        } while ( vertex );
      }
    }

    return *this;
  }

  /**
   * Computes a chain of half edges in CCW order called the 'horizon'.
   * For an edge to be part of the horizon it must join a face that can see
   * 'eyePoint' and a face that cannot see 'eyePoint'.
   */
  QuickHull &computeHorizon( const Vertex &eyePoint, HalfEdge *crossEdge, Face &face,
                             std::vector<HalfEdge *> &horizon )
  {
    // moves face's vertices to the 'unassigned' vertex list
    deleteFaceVertices( face, nullptr );

    face.mark = Mark::Deleted;

    HalfEdge *edge;
    if ( !crossEdge ) {

      edge = crossEdge = &face.getEdge( 0 );

    } else {

      // start from the next edge since 'crossEdge' was already analyzed
      // (actually 'crossEdge.twin' was the edge who called this method recursively)
      edge = crossEdge->next;
    }

    do {

      HalfEdge *twinEdge = edge->twin;
      Face *oppositeFace = twinEdge->face;

      if ( oppositeFace->mark == Mark::Visible ) {

        if ( oppositeFace->distanceToPoint( eyePoint ) > tolerance ) {

          // the opposite face can see the vertex, so proceed with next edge
          computeHorizon( eyePoint, twinEdge, *oppositeFace, horizon );
        }
        else {
          // the opposite face can't see the vertex, so this edge is part of the horizon
          horizon.push_back( edge );
        }
      }

      edge = edge->next;

    } while ( edge != crossEdge );

    return *this;
  }

  /*
   * Finds the next vertex to create faces with the current hull
   */
  VertexNode *nextVertexToAdd()
  {
    // if the 'assigned' list of vertices is empty, no vertices are left. return with 'undefined'
    if ( !assigned.empty() ) {

      VertexNode *eyeVertex = nullptr;
      float maxDistance = 0;

      // grab the first available face and start with the first visible vertex of that face
      Face *eyeFace = assigned.head->face;
      VertexNode *vertex = eyeFace->outside;

      // now calculate the farthest vertex that face can see
      do {

        float distance = eyeFace->distanceToPoint( vertex->point );

        if ( distance > maxDistance ) {

          maxDistance = distance;
          eyeVertex = vertex;
        }

        vertex = vertex->next;

      } while ( vertex && vertex->face == eyeFace );

      return eyeVertex;
    }
    return nullptr;
  }

  /*
   * Removes a vertex from the 'assigned' list of vertices and from the given face
   */
  void removeVertexFromFace( VertexNode *vertex, Face *face )
  {
    if ( vertex == face->outside ) {

      // fix face.outside link
      if ( vertex->next && vertex->next->face == face) {

        // face has at least 2 outside vertices, move the 'outside' reference
        face->outside = vertex->next;
      }
      else {

        // vertex was the only outside vertex that face had
        face->outside = nullptr;
      }
    }

    assigned.remove( vertex );
  }

  /*
   * Creates a face with the vertices 'eyeVertex.point', 'horizonEdge.tail' and 'horizonEdge.head' in CCW order
   */
  HalfEdge &addAdjoiningFace( VertexNode *eyeVertex, HalfEdge *horizonEdge )
  {
    // all the half edges are created in ccw order thus the face is always pointing outside the hull
    faces.emplace_back( *eyeVertex, *horizonEdge->tail(), *horizonEdge->head() );
    Face &face = faces.back();

    // join face.getEdge( - 1 ) with the horizon's opposite edge face.getEdge( - 1 ) = face.getEdge( 2 )
    face.getEdge( -1 ).setTwin( *horizonEdge->twin );

    return face.getEdge( 0 ); // the half edge whose vertex is the eyeVertex
  }

  /*
   * Adds 'horizon.length' faces to the hull, each face will be linked with the
   * horizon opposite face and the face on the left/right
   */
  void addNewFaces( VertexNode *eyeVertex, std::vector<HalfEdge *> &horizon )
  {
    newFaces.clear();

    HalfEdge *firstSideEdge = nullptr;
    HalfEdge *previousSideEdge = nullptr;

    for ( HalfEdge *horizonEdge : horizon ) {
      // returns the right side edge
      HalfEdge &sideEdge = addAdjoiningFace( eyeVertex, horizonEdge );

      if ( !firstSideEdge ) {

        firstSideEdge = &sideEdge;

      } else {

        // joins face.getEdge( 1 ) with previousFace.getEdge( 0 )
        sideEdge.next->setTwin( *previousSideEdge );
      }

      newFaces.push_back( sideEdge.face );
      previousSideEdge = &sideEdge;

    }

    // perform final join of new faces
    firstSideEdge->next->setTwin( *previousSideEdge );
  }

  /*
   * Reassigns as many vertices as possible from the unassigned list to the new faces
   */
  void resolveUnassignedPoints( std::vector<Face *> &newFaces )
  {
    if ( !unassigned.empty() ) {

      VertexNode *vertex = unassigned.head;
      do {

        // buffer 'next' reference, see .deleteFaceVertices()
        VertexNode *nextVertex = vertex->next;

        float maxDistance = tolerance;

        Face *maxFace = nullptr;

        for ( Face *face : newFaces ) {

          if ( face->mark == Mark::Visible ) {

            float distance = face->distanceToPoint( vertex->point );

            if ( distance > maxDistance ) {

              maxDistance = distance;
              maxFace = face;
            }

            if ( maxDistance > 1000 * tolerance ) break;
          }
        }

        // 'maxFace' can be null e.g. if there are identical vertices
        if ( maxFace ) {

          addVertexToFace( *vertex, *maxFace );
        }

        vertex = nextVertex;

      } while ( vertex );
    }
  }

  /*
   * Removes inactive faces
   */
  void reindexFaces()
  {
    std::vector<Face> activeFaces;

    for ( const Face &face : faces ) {

      if ( face.mark == Mark::Visible ) {
        activeFaces.push_back( face );
      }
    }

    faces.clear();
    faces.insert(faces.begin(), activeFaces.begin(), activeFaces.end());
  }

  void addVertexToHull( VertexNode *eyeVertex )
  {
    std::vector<HalfEdge *> horizon;

    unassigned.clear();

    // remove 'eyeVertex' from 'eyeVertex.face' so that it can't be added to the 'unassigned' vertex list
    removeVertexFromFace( eyeVertex, eyeVertex->face );

    computeHorizon( eyeVertex->point, nullptr, *eyeVertex->face, horizon );

    addNewFaces( eyeVertex, horizon );

    // reassign 'unassigned' vertices to the new faces
    resolveUnassignedPoints( newFaces );
  }

  void cleanup()
  {
    assigned.clear();
    unassigned.clear();
    newFaces.clear();
  }

  void compute()
  {
    computeInitialHull();

    // add all available vertices gradually to the hull
    while (VertexNode *vertex = nextVertexToAdd() ) {

      addVertexToHull( vertex );
    }

    reindexFaces();

    cleanup();
  }

public:
  /**
   * initialize this object from the given vertices
   *
   * @param iter the vertex iterator
   * @param end the iteration end marker
   * @return this object
   */
  template <typename points_iterator>
  QuickHull &setFromPoints(points_iterator &iter, const points_iterator &iter_end)
  {
    clear();

    while(iter != iter_end) {
      vertices.emplace_back( *iter );
      ++iter;
    }

    compute();

    return *this;
  }

  /**
   * initialize this object from the vertices recursively collected from the given object tree
   *
   * @param object the tree root
   * @return this object
   */
  QuickHull &setFromObject(const Object3D::Ptr object)
  {
    clear();

    PointsWalker::Ptr begin = PointsWalker::make(object);
    PointsWalker end(object->children().end());
    return setFromPoints(*begin, end);
  }

  /**
   * create an object from the vertices in the given object tree
   *
   * @param object the tree root
   */
  explicit QuickHull (const Object3D::Ptr object)
  {
    PointsWalker::Ptr begin = PointsWalker::make(object);
    PointsWalker end(object->children().end());
    setFromPoints(*begin, end);
  }

  /**
   * create an object from the given vertices
   *
   * @param iter the vertex iterator
   * @param end the iteration end marker
   * @return this object
   */
  template <typename points_iterator>
  QuickHull(points_iterator &iter, const points_iterator &iter_end)
  {
    setFromPoints(iter, iter_end);
  }

  QuickHull() = default;
};

}
#endif //THREE_PP_QUICKHULL_H
