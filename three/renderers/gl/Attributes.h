//
// Created by byter on 21.09.17.
//

#ifndef THREE_QT_ATTRIBUTES_H
#define THREE_QT_ATTRIBUTES_H

#include <QOpenGLFunctions>
#include <core/BufferAttribute.h>
#include <Constants.h>
#include "Helpers.h"

namespace three {
namespace gl {

class Attributes
{
  QOpenGLFunctions * const _fn;
  std::unordered_map<sole::uuid, Buffer> _buffers;

  void createBuffer(Buffer &buffer, const BufferAttribute &attribute, BufferType bufferType)
  {
    GLenum usage = attribute.dynamic() ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

    _fn->glGenBuffers(1, &buffer.handle);

    _fn->glBindBuffer((GLenum)bufferType, buffer.handle);
    _fn->glBufferData((GLenum)bufferType, attribute.byteCount(), attribute.data(0), usage);

    const_cast<BufferAttribute &>(attribute).onUpload.emitSignal(attribute);

    buffer.type = attribute.glType();
    buffer.bytesPerElement = attribute.bytesPerElement();
    buffer.version = attribute.version();
  }

public:
  Attributes(QOpenGLFunctions *fn) : _fn(fn) {}

  void updateBuffer(const Buffer &buffer, BufferAttribute &attribute, BufferType bufferType)
  {
    UpdateRange &updateRange = attribute.updateRange();

    _fn->glBindBuffer((GLenum)bufferType, buffer.handle);

    if(!attribute.dynamic()) {
      _fn->glBufferData((GLenum)bufferType, attribute.byteCount(), attribute.data(0), GL_STATIC_DRAW );
    }
    else if(updateRange.count == -1) {
      // Not using update ranges
      _fn->glBufferSubData((GLenum)bufferType, 0, attribute.byteCount(), attribute.data(0));
    }
    else if(updateRange.count == 0 ) {

      throw std::logic_error("updateBuffer: dynamic BufferAttributeBase marked as needsUpdate but updateRange.count is 0, ensure you are using set methods or updating manually");

    } else {
      _fn->glBufferSubData((GLenum)bufferType,
                      updateRange.offset * buffer.bytesPerElement,
                      updateRange.count * buffer.bytesPerElement,
                      attribute.data(updateRange.offset));

      updateRange.count = -1; // reset range
    }
  }

  bool has(const BufferAttribute &attribute )
  {
    return _buffers.count(attribute.uuid) > 0;
  }

  const Buffer &get(const BufferAttribute &attribute ) const {

    //if ( attribute.isInterleavedBufferAttributeBase ) attribute = attribute.data;

    return _buffers.at(attribute.uuid);
  }

  void remove(const BufferAttribute &attribute)
  {
    //if ( attribute.isInterleavedBufferAttributeBase ) attribute = attribute.data;

    if (_buffers.find(attribute.uuid) != _buffers.end()) {

      const Buffer &data = _buffers[ attribute.uuid ];

      _fn->glDeleteBuffers(1, &data.handle);

      _buffers.erase(attribute.uuid);
    }
  }

  void update(BufferAttribute &attribute, BufferType bufferType)
  {
    //if ( attribute.isInterleavedBufferAttributeBase ) attribute = attribute.data;

    if (_buffers.find(attribute.uuid) == _buffers.end()) {
       createBuffer(_buffers[ attribute.uuid ], attribute, bufferType );
    }
    else {
      Buffer &buffer = _buffers[ attribute.uuid ];
      if ( buffer.version < attribute.version() ) {
        updateBuffer(buffer, attribute, bufferType);
        buffer.version = attribute.version();
      }
    }
  }
};

}
}
#endif //THREE_QT_ATTRIBUTES_H
