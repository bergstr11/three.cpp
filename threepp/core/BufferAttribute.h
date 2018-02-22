//
// Created by byter on 23.08.17.
//

#ifndef THREEPP_BUFFERATTRIBUTE_H
#define THREEPP_BUFFERATTRIBUTE_H

#include <vector>
#include <cstring>
#include <memory>

#include <threepp/Constants.h>
#include <threepp/core/Color.h>
#include <threepp/util/sole.h>
#include <threepp/util/simplesignal.h>
#include <threepp/math/Vector2.h>
#include <threepp/math/Vector3.h>
#include <threepp/math/Vector4.h>
#include <threepp/math/Box3.h>
#include <threepp/util/Types.h>
#include <threepp/util/Resolver.h>

namespace three {

template<typename T>
struct Cpp2GL;

template<>
struct Cpp2GL<float> {
  static const GLenum glEnum = GL_FLOAT;
};
template<>
struct Cpp2GL<uint16_t> {
  static const GLenum glEnum = GL_UNSIGNED_SHORT;
};
template<>
struct Cpp2GL<int16_t> {
  static const GLenum glEnum = GL_SHORT;
};
template<>
struct Cpp2GL<uint32_t> {
  static const GLenum glEnum = GL_UNSIGNED_INT;
};
template<>
struct Cpp2GL<int32_t> {
  static const GLenum glEnum = GL_INT;
};
template<>
struct Cpp2GL<int8_t> {
  static const GLenum glEnum = GL_BYTE;
};
template<>
struct Cpp2GL<uint8_t> {
  static const GLenum glEnum = GL_UNSIGNED_BYTE;
};

struct Index
{
  union
  {
    struct
    {
      uint32_t a, b, c;
    };
    uint32_t elements[3];
  };

  Index() : a(0), b(0), c(0) {}
  Index(uint32_t a, uint32_t b, uint32_t c) : a(a), b(b), c(c) {}
};

/**
 * a buffer attribute associates a given buffer with a geometry attribute
 */
class BufferAttribute
{
protected:
  unsigned _version = 0;

  unsigned _itemSize;
  bool _normalized;

  UpdateRange _updateRange;

  explicit BufferAttribute(bufferattribute::Resolver::Ptr resolver, unsigned itemSize, bool normalized)
     : uuid(sole::uuid0()), resolver(resolver), _itemSize(itemSize), _normalized(normalized)
  {}

  BufferAttribute(unsigned itemSize, bool normalized)
     : uuid(sole::uuid0()), resolver(bufferattribute::Resolver::makeNull()), _itemSize(itemSize),
       _normalized(normalized)
  {}

public:
  virtual ~BufferAttribute() = default;

  bool dynamic = false;

  using Ptr = std::shared_ptr<BufferAttribute>;

  const bufferattribute::Resolver::Ptr resolver;

  Signal<void(const BufferAttribute &)> onUpload;

  const sole::uuid uuid;

  void needsUpdate() {_version++;}

  unsigned version() const {return _version;}

  bool normalized() const {return _normalized;}

  UpdateRange &updateRange() {return _updateRange;}

  unsigned itemSize() const {return _itemSize;}

  virtual const void *data(size_t offset) const = 0;

  virtual GLenum glType() const = 0;

  virtual size_t byteCount() const = 0;

  virtual unsigned bytesPerElement() const = 0;
};

/**
 * a typed buffer attribute
 *
 * @tparam Type
 */
template <typename Type>
class BufferAttributeT : public BufferAttribute
{
protected:
  Type *_data;
  size_t _size = 0;

  explicit BufferAttributeT(bufferattribute::Resolver::Ptr resolver, Type *data, size_t size,
                            unsigned itemSize, bool normalized)
  : BufferAttribute(resolver, itemSize, normalized), _data(data), _size(size)
  {}

  BufferAttributeT(size_t size, unsigned itemSize, bool normalized)
     : BufferAttribute(itemSize, normalized), _size(size), _data(nullptr)
  {}

public:
  using Ptr = std::shared_ptr<BufferAttributeT<Type>>;

  const Type &tdata(uint32_t offset) {return _data[offset];}

  const Type *tdata() {return _data;}

  void clear() {memset(_data, 0, byteCount());}

  GLenum glType() const override {return Cpp2GL<Type>::glEnum;}

  unsigned bytesPerElement() const override {return sizeof(Type);}

  size_t size() const {return _size;}

  size_t itemCount() const {return _size / _itemSize;}

  const Type operator [] (size_t index) {return _data[index];}

  BufferAttributeT &set(Type value, size_t offset=0)
  {
    _data[offset] = value;

    return *this;
  }

  Type get_x(size_t index) const
  {
    return _data[ index * _itemSize ];
  }

  BufferAttributeT &set_x(size_t index, Type x)
  {
    _data[ index * _itemSize ] = x;

    return *this;
  }

  Type get_y(size_t index) const
  {
    return _data[ index * _itemSize + 1 ];
  }

  BufferAttributeT &set_y(size_t index, Type y)
  {
    _data[ index * _itemSize + 1 ] = y;

    return *this;
  }

  Type &at(size_t index)
  {
    return _data[ index];
  }

  template <typename ItemType>
  ItemType &item_at(size_t index)
  {
    if(sizeof(ItemType) / sizeof(Type) != _itemSize) throw std::invalid_argument("invalid item type");
    return reinterpret_cast<ItemType &>(_data[index * _itemSize]);
  }

  Type get_z(size_t index) const
  {
    return _data[ index * _itemSize + 2 ];
  }

  BufferAttributeT &set_z(size_t index, Type z)
  {
    _data[ index * _itemSize + 2 ] = z;

    return *this;
  }

  Type get_w(size_t index) const
  {
    return _data[ index * _itemSize + 3 ];
  }

  BufferAttributeT &set_w(size_t index, Type w)
  {
    _data[ index * _itemSize + 3 ] = w;

    return *this;
  }

  void setXY(size_t index, Type x, Type y)
  {
    index *= _itemSize;

    _data[ index + 0 ] = x;
    _data[ index + 1 ] = y;
  }

  void setXYZ(size_t index, Type x, Type y, Type z )
  {
    index *= _itemSize;

    _data[ index + 0 ] = x;
    _data[ index + 1 ] = y;
    _data[ index + 2 ] = z;
  }

  void setXYZW(size_t index, Type x, Type y, Type z, Type w)
  {
    index *= _itemSize;

    _data[ index + 0 ] = x;
    _data[ index + 1 ] = y;
    _data[ index + 2 ] = z;
    _data[ index + 3 ] = w;
  }

  math::Box3 box3()
  {
    Type minX = std::numeric_limits<Type>::infinity();
    Type minY = std::numeric_limits<Type>::infinity();
    Type minZ = std::numeric_limits<Type>::infinity();

    Type maxX = - std::numeric_limits<Type>::infinity();
    Type maxY = - std::numeric_limits<Type>::infinity();
    Type maxZ = - std::numeric_limits<Type>::infinity();

    for(size_t i = 0, l=itemCount(); i < l; i++) {
      Type x = get_x(i);
      Type y = get_y(i);
      Type z = get_z(i);

      if ( x < minX ) minX = x;
      if ( y < minY ) minY = y;
      if ( z < minZ ) minZ = z;

      if ( x > maxX ) maxX = x;
      if ( y > maxY ) maxY = y;
      if ( z > maxZ ) maxZ = z;
    }

    return math::Box3(math::Vector3(minX, minY, minZ), math::Vector3(maxX, maxY, maxZ));
  }

  void apply(const math::Matrix4 &matrix)
  {
    for(size_t i = 0, l = itemCount(); i < l; i ++ ) {
      math::Vector3 v1(get_x(i),  get_y(i), get_z(i));

      v1.apply(matrix);

      setXYZ( i, v1.x(), v1.y(), v1.z() );
    }
  }

  void apply(const math::Matrix3 &matrix)
  {
    for(size_t i = 0, l = itemCount(); i < l; i ++ ) {
      math::Vector3 v1(get_x(i),  get_y(i), get_z(i));

      v1.apply(matrix);

      setXYZ( i, v1.x(), v1.y(), v1.z() );
    }
  }

  BufferAttributeT &copyAt(unsigned dstIndex, const BufferAttributeT &srcAttribute, unsigned srcIndex)
  {
    std::memcpy(_data + dstIndex * _itemSize, srcAttribute._data + srcIndex * srcAttribute._itemSize, _itemSize);
    return *this;
  }

  size_t byteCount() const override {return _size * sizeof(Type);}

  const void *data(size_t offset) const override {return _data+offset;}
};

/**
 * a buffer attribute that manages a preallocated buffer
 *
 * @tparam Type
 */
template <typename ComponentType, typename ItemType=ComponentType>
class PreallocBufferAttribute : public BufferAttributeT<ComponentType>
{
  constexpr static size_t itemSize = sizeof(ItemType) / sizeof(ComponentType);

  friend struct attribute;

  template <typename CT, typename IT>
  friend class GrowingBufferAttribute;

  using Super = BufferAttributeT<ComponentType>;
  using Base = BufferAttribute;

  ItemType *it;
  size_t _offset = 0;

protected:
  PreallocBufferAttribute(size_t itemCount, bool normalized)
     : Super(itemCount * itemSize, itemSize, normalized)
  {
    it = new ItemType[itemCount];
    Super::_data = reinterpret_cast<ComponentType *>(it);
  }

  PreallocBufferAttribute(const std::vector<ItemType> &items, bool normalized)
     : Super(items.size() * itemSize, itemSize, normalized)
  {
    it = new ItemType[items.size()];
    memcpy(it, items.data(), Super::byteCount());
    Super::_data = reinterpret_cast<ComponentType *>(it);
  }

  PreallocBufferAttribute(const std::initializer_list<ItemType> &items, bool normalized)
     : Super(items.size() * itemSize, itemSize, normalized)
  {
    it = new ItemType[items.size()];
    for(auto i=items.begin(); i != items.end(); i++) next() = *i;
    Super::_data = reinterpret_cast<ComponentType *>(it);
  }

public:
  ItemType &next()
  {
    return it[_offset++];
  }

  ItemType &back()
  {
    return it[_offset-1];
  }

  using Ptr = std::shared_ptr<PreallocBufferAttribute>;

  ~PreallocBufferAttribute() {
    delete[] it;
  }
};

/**
 * a buffer attribute that manages a growing buffer
 *
 * @tparam Type
 */
template <typename ComponentType, typename ItemType=ComponentType>
class GrowingBufferAttribute : public BufferAttributeT<ComponentType>
{
  constexpr static size_t itemSize = sizeof(ItemType) / sizeof(ComponentType);

  friend struct attribute;

  using Super = BufferAttributeT<ComponentType>;
  using Base = BufferAttribute;

  std::vector<ItemType> _array;

protected:
  explicit GrowingBufferAttribute(bool normalized)
     : Super(0, itemSize, normalized)
  {}

  GrowingBufferAttribute(const std::initializer_list<ItemType> &value, bool normalized)
     : Super(0, itemSize, normalized), _array(value)
  {}

public:
  template<typename... _Args>
  void emplace_back(_Args&&... __args)
  {
    _array.emplace_back(std::forward<_Args>(__args)...);
    Super::_size = _array.size() * itemSize;
    Super::_data = reinterpret_cast<ComponentType *>(_array.data());
  }

  ItemType &next()
  {
    _array.emplace_back();
    Super::_size = _array.size() * itemSize;
    Super::_data = reinterpret_cast<ComponentType *>(_array.data());

    return _array.back();
  }

  ItemType &back()
  {
    return _array.back();
  }

  ItemType &at(size_t index)
  {
    return _array.at(index);
  }

  typename PreallocBufferAttribute<ComponentType, ItemType>::Ptr clone() {
    auto cloned = typename PreallocBufferAttribute<ComponentType, ItemType>::Ptr(
       new PreallocBufferAttribute<ComponentType, ItemType>(this->itemCount(), this->normalized()));

    memcpy(cloned->_data, this->_data, this->byteCount());
    return cloned;
  };

  using Ptr = std::shared_ptr<GrowingBufferAttribute>;
};

/**
 * a buffer attribute that wraps an externally managed buffer
 *
 * @tparam Type
 */
template <typename Type>
class ExternalBufferAttribute : public three::BufferAttributeT<Type>
{
  friend struct attribute;

  using Super = three::BufferAttributeT<Type>;

protected:
  ExternalBufferAttribute(Type *data, size_t itemSize, size_t itemCount)
     : three::BufferAttributeT<Type>(itemCount * itemSize, itemSize, true)
  {
    Super::_data = data;
  }

public:
  using Ptr = std::shared_ptr<ExternalBufferAttribute>;
};

struct attribute {

  //growing buffer, based on vector
  template <typename ComponentType, typename ItemType=ComponentType>
  using growing_t = std::shared_ptr<GrowingBufferAttribute<ComponentType, ItemType>>;

  template <typename ComponentType, typename ItemType=ComponentType>
  static growing_t<ComponentType, ItemType> growing(bool normalized=false)
  {
    return typename GrowingBufferAttribute<ComponentType, ItemType>::Ptr(
       new GrowingBufferAttribute<ComponentType, ItemType>(normalized));
  }

  template <typename ComponentType, typename ItemType=ComponentType>
  static growing_t<ComponentType, ItemType>
  growing(const std::initializer_list<ItemType> &value, bool normalized=false)
  {
    return typename GrowingBufferAttribute<ComponentType, ItemType>::Ptr(
       new GrowingBufferAttribute<ComponentType, ItemType>(value, normalized));
  }

  //internally preallocated buffer, fixed size
  template <typename ComponentType, typename ItemType=ComponentType>
  using prealloc_t = std::shared_ptr<PreallocBufferAttribute<ComponentType, ItemType>>;

  template <typename ComponentType, typename ItemType=ComponentType>
  static prealloc_t<ComponentType, ItemType> prealloc(size_t itemCount, bool normalized = false)
  {
    return typename PreallocBufferAttribute<ComponentType, ItemType>::Ptr(
       new PreallocBufferAttribute<ComponentType, ItemType>(itemCount, normalized));
  }

  template <typename ComponentType, typename ItemType=ComponentType>
  static prealloc_t<ComponentType, ItemType>
  prealloc(const std::initializer_list<ItemType> &value, bool normalized=false)
  {
    return typename PreallocBufferAttribute<ComponentType, ItemType>::Ptr(
       new PreallocBufferAttribute<ComponentType, ItemType>(value, normalized));
  }

  template <typename ComponentType, typename ItemType=ComponentType>
  static prealloc_t<ComponentType, ItemType> copied(const std::vector<ItemType> &items, bool normalized=false)
  {
    return typename PreallocBufferAttribute<ComponentType, ItemType>::Ptr(
       new PreallocBufferAttribute<ComponentType, ItemType>(items, normalized));
  }

  //externally allocated buffer, fixed size
  template <typename ComponentType, typename ItemType>
  static typename ExternalBufferAttribute<ComponentType>::Ptr external(void *data, size_t size) {
    return typename ExternalBufferAttribute<ComponentType>::Ptr(new ExternalBufferAttribute<ComponentType> (
       reinterpret_cast<ComponentType *>(data), sizeof(ItemType) / sizeof(ComponentType), size));
  }
};

}
#endif //THREEPP_BUFFERATTRIBUTE_H
