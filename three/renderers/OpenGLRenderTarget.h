//
// Created by byter on 05.08.17.
//

#ifndef THREE_QT_OPENGLRENDERTARGET_H
#define THREE_QT_OPENGLRENDERTARGET_H

#include <QVector4D>
#include <QOpenGLTexture>
#include <memory>

namespace three {

class OpenGLRenderTarget
{
  unsigned _width;
  unsigned _height;

  QVector4D _scissor;// = new Vector4( 0, 0, width, height );
  bool _scissorTest = false;

  QVector4D viewport;// = new Vector4( 0, 0, width, height );

  //if ( options.minFilter === undefined ) options.minFilter = LinearFilter;

  //QOpenGLTexture texture;// = new Texture( undefined, undefined, options.wrapS, options.wrapT, options.magFilter, options.minFilter, options.format, options.type, options.anisotropy, options.encoding );

  bool _depthBuffer;// = options.depthBuffer !== undefined ? options.depthBuffer : true;
  bool _stencilBuffer;// = options.stencilBuffer !== undefined ? options.stencilBuffer : true;
  //this.depthTexture = options.depthTexture !== undefined ? options.depthTexture : null;

public:
  using Ptr = std::shared_ptr<OpenGLRenderTarget>;

  struct Options {

  };

  OpenGLRenderTarget(unsigned width, unsigned height, Options options)

  {

  }
};
}

#endif //THREE_QT_OPENGLRENDERTARGET_H
