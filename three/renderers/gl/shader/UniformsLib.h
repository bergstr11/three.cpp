//
// Created by byter on 30.09.17.
//

#ifndef THREE_QT_UNIFORMSLIB_H
#define THREE_QT_UNIFORMSLIB_H

#include <textures/DataTexture.h>

namespace three {
namespace gl {
namespace uniforms {

extern DataTexture::Ptr LTC_MAT_TEXTURE;
extern DataTexture::Ptr LTC_MAG_TEXTURE;

/**
 * Uniforms library for RectAreaLight shared webgl shaders
 * @author abelnation
 *
 * NOTE: This is a temporary location for the BRDF approximation texture data
 *       based off of Eric Heitz's work (see citation below).  BRDF data for
 *       RectAreaLight is currently approximated using a precomputed texture
 *       of roughly 80kb in size.  The hope is to find a better way to include
 *       the large texture data before including the full RectAreaLight implementation
 *       in the main build files.
 *
 * TODO: figure out a way to compress the LTC BRDF data
 * TODO: validate the RectAreaLight BRDF and shaders
 */
void initRectAreaLight();

}
}
}
#endif //THREE_QT_UNIFORMSLIB_H
