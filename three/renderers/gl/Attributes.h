//
// Created by byter on 21.09.17.
//

#ifndef THREE_QT_ATTRIBUTES_H
#define THREE_QT_ATTRIBUTES_H

#include <QOpenGLFunctions>
#include <core/BufferAttribute.h>
#include <Constants.h>


namespace three {
namespace gl {

struct Buffer {
  GLuint buf;
  GLenum type;
  unsigned bytesPerElement;
  unsigned version;
};

class Attributes : private QOpenGLFunctions
{
  std::unordered_map<sole::uuid, Buffer> _buffers;

  template <typename T>
  void createBuffer(Buffer &buffer, const BufferAttribute<T> &attribute, BufferType bufferType)
  {
    const std::vector<T> &array = attribute.array();
    GLenum usage = attribute.dynamic() ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

    glGenBuffers(1, &buffer.buf);

    glBindBuffer((GLenum)bufferType, buffer.buf);
    glBufferData((GLenum)bufferType, array.size(), array.data(), usage);

    const_cast<BufferAttribute<T> &>(attribute).onUpload.emitSignal(attribute);

    buffer.type = attribute.glType();

    buffer.bytesPerElement = attribute.bytesPerElement();
    buffer.version = attribute.version();
  }

public:
  template <typename T>
  void updateBuffer(const Buffer &buffer, BufferAttribute<T> &attribute, BufferType bufferType)
  {
    UpdateRange &updateRange = attribute.updateRange();

    glBindBuffer(bufferType, buffer.buf);

    if(!attribute.dynamic()) {
      glBufferData(bufferType, buffer.bytesPerElement * attribute.size(), attribute.data(), GL_STATIC_DRAW );
    }
    else if(updateRange.count == -1) {
      // Not using update ranges
      glBufferSubData(bufferType, 0, buffer.bytesPerElement * attribute.size(), attribute.data());
    }
    else if(updateRange.count == 0 ) {

      throw std::logic_error("updateBuffer: dynamic BufferAttribute marked as needsUpdate but updateRange.count is 0, ensure you are using set methods or updating manually");

    } else {
      glBufferSubData(bufferType,
                      updateRange.offset * buffer.bytesPerElement,
                      updateRange.count * buffer.bytesPerElement,
                      attribute.data(updateRange.offset));

      updateRange.count = -1; // reset range
    }
  }

  template <typename T>
  const Buffer &get(const BufferAttribute<T> &attribute ) {

    //if ( attribute.isInterleavedBufferAttribute ) attribute = attribute.data;

    return _buffers[ attribute.uuid ];
  }

  template <typename T>
  void remove(const BufferAttribute<T> &attribute)
  {
    //if ( attribute.isInterleavedBufferAttribute ) attribute = attribute.data;

    if (_buffers.find(attribute.uuid) != _buffers.end()) {

      const Buffer &data = _buffers[ attribute.uuid ];

      glDeleteBuffers(1, &data.buf);

      _buffers.erase(attribute.uuid);
    }
  }

  template <typename T>
  void update(BufferAttribute<T> &attribute, BufferType bufferType)
  {
    //if ( attribute.isInterleavedBufferAttribute ) attribute = attribute.data;

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
