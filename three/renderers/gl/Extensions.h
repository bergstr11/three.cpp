//
// Created by byter on 02.10.17.
//

#ifndef THREE_QT_EXTENSIONS_H
#define THREE_QT_EXTENSIONS_H

#include <QOpenGLContext>
#include <unordered_map>

namespace three {
namespace gl {

enum class Extension
{
  WEBGL_depth_texture,
  EXT_frag_depth,
  EXT_texture_filter_anisotropic,
  WEBGL_compressed_texture_s3tc,
  WEBGL_compressed_texture_pvrtc,
  WEBGL_compressed_texture_etc1,
  OES_texture_float,
  OES_texture_float_linear,
  OES_texture_half_float,
  OES_texture_half_float_linear,
  OES_standard_derivatives,
  ANGLE_instanced_arrays,
  OES_element_index_uint
};

class Extensions
{
  QOpenGLContext *const context;

  std::unordered_map<Extension, bool> _extensions;

public:
  Extensions(QOpenGLContext *context) : context(context)
  {}

  void get(std::vector<Extension> extensions) const
  {
    for(Extension extension : extensions) {
      get(extension);
    }
  }

  bool get(Extension extension) const
  {
    if (_extensions.find(extension) != _extensions.end()) {
      return _extensions[extension];
    }
    switch (extension) {
      case Extension::WEBGL_compressed_texture_etc1:
        _extensions[extension] = context->hasExtension( "WEBGL_compressed_texture_etc1" );
        break;
      case Extension::WEBGL_compressed_texture_pvrtc:
        _extensions[extension] = context->hasExtension( "WEBGL_compressed_texture_pvrtc" )
                                 || context->hasExtension( "WEBKIT_WEBGL_compressed_texture_pvrtc" );
        break;
      case Extension::WEBGL_compressed_texture_s3tc:
        _extensions[extension] = context->hasExtension( "WEBGL_compressed_texture_s3tc" )
                                 || context->hasExtension( "MOZ_WEBGL_compressed_texture_s3tc" )
                                 || context->hasExtension( "WEBKIT_WEBGL_compressed_texture_s3tc" );
        break;
      case Extension::WEBGL_depth_texture:
        _extensions[extension] = context->hasExtension("WEBGL_depth_texture")
                                 || context->hasExtension("MOZ_WEBGL_depth_texture")
                                 || context->hasExtension("WEBKIT_WEBGL_depth_texture");
        break;
      case Extension::EXT_texture_filter_anisotropic:
        _extensions[extension] = context->hasExtension("EXT_texture_filter_anisotropic")
                                 || context->hasExtension("MOZ_EXT_texture_filter_anisotropic")
                                 || context->hasExtension("WEBKIT_EXT_texture_filter_anisotropic");
        break;
      case Extension::OES_texture_float
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
      case Extension::EXT_frag_depth:
        _extensions[extension] = context->hasExtension("EXT_frag_depth");
        break;
    }
    return _extensions[extension];
  }

  bool operator[](Extension extension) const {
    return get(extension);
  }
};

}
}
#endif //THREE_QT_EXTENSIONS_H
