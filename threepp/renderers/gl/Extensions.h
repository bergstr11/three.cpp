//
// Created by byter on 02.10.17.
//

#ifndef THREEPP_EXTENSIONS_H
#define THREEPP_EXTENSIONS_H

#include <QOpenGLContext>
#include <unordered_map>
#include <threepp/Constants.h>
#include <threepp/util/Types.h>
#include <QOpenGLTexture>

namespace three {
namespace gl {

enum class Extension : uint16_t
{
  ARB_depth_texture               = 1,
  EXT_frag_depth                  = 1<<1,
  EXT_texture_filter_anisotropic  = 1<<2,
  EXT_blend_minmax                = 1<<3,
  EXT_texture_compression_s3tc    = 1<<4,
  EXT_shader_texture_lod          = 1<<5,
  OES_texture_float               = 1<<6,
  OES_texture_float_linear        = 1<<7,
  OES_texture_half_float          = 1<<8,
  OES_texture_half_float_linear   = 1<<9,
  OES_standard_derivatives        = 1<<10,
  ANGLE_instanced_arrays          = 1<<11,
  OES_element_index_uint          = 1<<12,
  GLEXT_draw_buffers              = 1<<13
};

class UseExtension
{
  uint16_t bits;

  template<typename Ret> static Ret decl() {return (Ret)0;}
  template<typename Ret, Extension e, Extension... Args> static Ret decl() {
    return (Ret)e | decl<Ret, Args...>();
  };

public:
  UseExtension(uint16_t bits=0) : bits(bits) {}

  template <Extension ... extensions>
  static UseExtension use()
  {
    return UseExtension(decl<uint16_t, extensions...>());
  }

  UseExtension &add(Extension ext) {
    bits |= (uint16_t)ext;
  }

  bool get(Extension ext) const {
    return (bits & (size_t)ext) != 0;
  }
};

class Extensions
{
  QOpenGLContext *const context;

  enum_map<Extension, bool> _extensions;

public:
  Extensions(QOpenGLContext *context) : context(context)
  {}

  void get(std::vector<Extension> extensions)
  {
    for(Extension extension : extensions) {
      get(extension);
    }
  }

  bool get(Extension extension)
  {
    if (_extensions.find(extension) != _extensions.end()) {
      return _extensions.at(extension);
    }
    switch (extension) {
      case Extension::EXT_texture_compression_s3tc:
        _extensions[extension] = context->hasExtension( "EXT_texture_compression_s3tc" );
        break;
      case Extension::ARB_depth_texture:
        _extensions[extension] = context->hasExtension("ARB_depth_texture");
        break;
      case Extension::EXT_texture_filter_anisotropic:
        _extensions[extension] = context->hasExtension("EXT_texture_filter_anisotropic");
        break;
      case Extension::EXT_blend_minmax:
        _extensions[extension] = context->hasExtension("EXT_blend_minmax");
      case Extension::OES_texture_float:
        _extensions[extension] = context->hasExtension("OES_texture_float");
        break;
      case Extension::OES_texture_float_linear:
        _extensions[extension] = context->hasExtension("OES_texture_float_linear");
        break;
      case Extension::OES_texture_half_float:
        _extensions[extension] = context->hasExtension("OES_texture_half_float");
        break;
      case Extension::OES_texture_half_float_linear:
        _extensions[extension] = context->hasExtension("OES_texture_half_float_linear");
        break;
      case Extension::OES_standard_derivatives:
        _extensions[extension] = context->hasExtension("OES_standard_derivatives");
        break;
      case Extension::ANGLE_instanced_arrays:
        _extensions[extension] = context->hasExtension("ANGLE_instanced_arrays");
        break;
      case Extension::OES_element_index_uint:
        _extensions[extension] = context->hasExtension("OES_element_index_uint");
        break;
      case Extension::EXT_shader_texture_lod:
        _extensions[extension] = context->hasExtension("EXT_shader_texture_lod");
        break;
      case Extension::EXT_frag_depth:
        _extensions[extension] = context->hasExtension("EXT_frag_depth");
        break;
    }
    return _extensions[extension];
  }

  TextureType extend(TextureType type)
  {
    switch(type) {
      case TextureType::HalfFloat:
        return get(Extension::OES_texture_half_float) ? TextureType::HalfFloatOES : type;
      default:
        return type;
    }
  }

  GLenum extend(Equation type)
  {
    switch(type) {
      case Equation::Min:
        return get(Extension::EXT_blend_minmax) ? GL_MIN_EXT : GL_MIN;
      case Equation::Max:
        return get(Extension::EXT_blend_minmax) ? GL_MAX_EXT : GL_MAX;
      default:
        return (GLenum)type;
    }
  }

  TextureFormat extend(TextureFormat format)
  {
    switch(format) {
      case TextureFormat::RGB_S3TC_DXT1:
      case TextureFormat::RGBA_S3TC_DXT1:
      case TextureFormat::RGBA_S3TC_DXT3:
      case TextureFormat::RGBA_S3TC_DXT5:
        return get(Extension::EXT_texture_compression_s3tc) ? format : TextureFormat::Undefined;
      default:
        return format;
    }
  }

  bool operator[](Extension extension)
  {
    return get(extension);
  }
};

}
}
#endif //THREEPP_EXTENSIONS_H
