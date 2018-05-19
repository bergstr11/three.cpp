//
// Created by byter on 5/6/18.
//

#ifndef THREE_PP_POINTSWALKER_H
#define THREE_PP_POINTSWALKER_H

#include <threepp/core/Object3D.h>
#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>

namespace three {

namespace points_walker {

struct always_true
{
  bool operator()(const Vertex &v)
  {
    return value;
  }

  static always_true empty;

private:
  bool value = true;
};

}

/**
 * an iterator that will walk object vertices recursively in an object tree, transforming
 * to world coordinates on the fly
 */
template <typename VertexPredicate=points_walker::always_true>
struct PointsWalker
{
  using Ptr = std::shared_ptr<PointsWalker>;

private:
  using child_iterator = std::vector<Object3D::Ptr>::const_iterator;

  Object3D * const _object;
  const Vertex *_data;
  const uint32_t *_index;
  size_t _dataCount = 0;

  VertexPredicate &_predicate;

  child_iterator _iterChildren;
  child_iterator _endChildren;
  Ptr _childWalker;

  PointsWalker(Object3D *object, const Vertex *data, const uint32_t *index,
               size_t dataCount, VertexPredicate &pred)
     : _object(object),
       _iterChildren(object->children().begin()),
       _endChildren(object->children().end()),
       _data(data),
       _index(index),
       _dataCount(dataCount),
       _childWalker(nullptr),
       _predicate(pred)
  {
    object->updateMatrixWorld( true );
  }

  PointsWalker(Object3D *object, const child_iterator &beginChildren, Ptr walker, VertexPredicate &pred)
     : _object(object),
       _iterChildren(beginChildren),
       _endChildren(object->children().end()),
       _data(nullptr),
       _index(nullptr),
       _dataCount(0),
       _childWalker(walker),
       _predicate(pred)
  {
    //qDebug() << "PointsWalker#childWalker" << object->name().c_str();
    object->updateMatrixWorld( true );
  }

  PointsWalker(const child_iterator &end)
     : _object(nullptr),
       _iterChildren(end),
       _endChildren(end),
       _data(nullptr),
       _index(nullptr),
       _dataCount(0),
       _childWalker(nullptr),
       _predicate(VertexPredicate::empty)
  {}

public:

  static Ptr make(Object3D *object, VertexPredicate &pred=VertexPredicate::empty)
  {
    Geometry::Ptr geometry = object->geometry();
    const Vertex *data = nullptr;
    const uint32_t *index = nullptr;
    size_t dataCount = 0;

    if(geometry) {
      if(LinearGeometry *lin = geometry->typer) {
        data = lin->vertices().data();
        dataCount = lin->vertices().size();
      }
      else if(BufferGeometry *buf = geometry->typer) {
        data = buf->position()->data<Vertex>();
        BufferAttributeT<uint32_t>::Ptr ix = buf->index();
        if(ix) {
          dataCount = ix->itemCount();
          index = ix->data_t();
        }
        else dataCount = buf->position()->itemCount();
      }
      else {
        throw std::invalid_argument("unsupported geometry type");
      }
    }
    while(dataCount) {
      Vertex vertex = index ? data[*index] : *data;
      if(pred(vertex)) break;
      dataCount--;
      if(index) index++; else data++;
    }
    if(dataCount) {
      //qDebug() << "PointsWalker" << object->name().c_str() << dataCount;
      return Ptr(new PointsWalker(object, data, index, dataCount, pred));
    }
    else {
      //qDebug() << "PointsWalker##seekChildren" << object->name().c_str();
      auto beginChildren = object->children().begin();
      auto endChildren = object->children().end();
      while(beginChildren != endChildren) {
        Ptr ptr = make(beginChildren->get(), pred);
        ++beginChildren;
        if(ptr) return Ptr(new PointsWalker(object, beginChildren, ptr, pred));
      }
    }
    return nullptr;
  }

  const Vertex operator*() const
  {
    if(_dataCount) {
      Vertex value = _index ? _data[*_index] : *_data;
      return value.apply(_object->matrixWorld());
    }
    else if(_childWalker)
      return *(*_childWalker);
    else
      throw std::out_of_range("PointsWalker");
  }

  PointsWalker &operator++()
  {
    while(_dataCount > 0) {
      _dataCount--;
      const Vertex &vertex = _index ? _data[*(++_index)] : *(++_data);
      if(_dataCount && _predicate(vertex)) return *this;
    }
    if(_childWalker && !_childWalker->atEnd()) {
      ++(*_childWalker);
      if(!_childWalker->atEnd())
        return *this;
    }
    while(_iterChildren != _endChildren) {
      _childWalker = make(_iterChildren->get(), _predicate);
      ++_iterChildren;
      if(_childWalker) break;
    }
    return *this;
  }

  bool operator==(const PointsWalker& rhs) const
  {
    return _dataCount == rhs._dataCount && _iterChildren == rhs._iterChildren &&
        (((!_childWalker || _childWalker->atEnd()) && !rhs._childWalker) || _childWalker == rhs._childWalker);
  }

  bool operator!=(const PointsWalker& rhs) const
  {
    return !(*this == rhs);
  }

  bool atEnd() const {
    return _dataCount == 0 && _iterChildren == _endChildren && (!_childWalker || _childWalker->atEnd());
  }

  PointsWalker end() {
    return PointsWalker(_object->children().end());
  }

  void walkThrough() {
    auto _end = end();
    while(*this != _end) ++(*this);
  }
};

}

#endif //THREE_PP_POINTSWALKER_H
