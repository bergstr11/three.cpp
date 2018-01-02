//
// Created by byter on 29.08.17.
//

#ifndef THREE_QT_GLSTATE_H
#define THREE_QT_GLSTATE_H

#include <QOpenGLExtraFunctions>
#include <math/Vector4.h>
#include <material/Material.h>
#include <helper/Types.h>
#include <Constants.h>
#include <unordered_map>
#include "Helpers.h"

namespace three {
namespace gl {

class State
{
public:
  struct ColorBuffer
  {
    bool locked = false;
    math::Vector4 color = {0, 0, 0, 0};
    GLboolean currentColorMask = 0;
    math::Vector4 currentColorClear = {0, 0, 0, 0};

    QOpenGLExtraFunctions * const _f;

    ColorBuffer(QOpenGLExtraFunctions *fn) : _f(fn)
    {}

    ColorBuffer &setMask(bool _colorMask)
    {
      GLboolean colorMask = _colorMask ?  1 : 0;
      if (currentColorMask != colorMask && !locked) {
        _f->glColorMask(colorMask, colorMask, colorMask, colorMask);
        currentColorMask = colorMask;
      }
    }

    ColorBuffer &setLocked(bool locked)
    { this->locked = locked; }

    ColorBuffer &setClear(float r, float g, float b, float a, bool premultipliedAlpha = false)
    {
      if (premultipliedAlpha) {
        r *= a;
        g *= a;
        b *= a;
      }
      color.set(r, g, b, a);

      if (currentColorClear != color) {
        _f->glClearColor(r, g, b, a);
        currentColorClear = color;
      }
    }

    ColorBuffer &reset()
    {
      locked = false;

      currentColorMask = 0;
      currentColorClear.set(-1, 0, 0, 0); // set to invalid state
    }
  };

  struct DepthBuffer
  {
    State *const glState;

    bool locked = false;

    bool currentDepthMask = false;
    Func currentDepthFunc = Func::LessEqual;
    double currentDepthClear = 0;

    QOpenGLExtraFunctions * const _f;

    DepthBuffer &setTest(bool depthTest)
    {
      if (depthTest) {
        glState->enable(GL_DEPTH_TEST);
      }
      else {
        glState->disable(GL_DEPTH_TEST);
      }
      return *this;
    }

    DepthBuffer(State *state, QOpenGLExtraFunctions * f) : glState(state), _f(f)
    {}

    DepthBuffer &setMask(bool depthMask)
    {
      if (currentDepthMask != depthMask && !locked) {
        _f->glDepthMask(depthMask);
        currentDepthMask = depthMask;
      }
      return *this;
    }

    DepthBuffer &setFunc(Func depthFunc)
    {

      if (currentDepthFunc != depthFunc) {
        _f->glDepthFunc((GLenum) depthFunc);
        currentDepthFunc = depthFunc;
      }
      return *this;
    }

    DepthBuffer &setLocked(bool lock)
    {
      locked = lock;
      return *this;
    }

    DepthBuffer &setClear(double depth)
    {
      if (currentDepthClear != depth) {
        glClearDepth(depth);
        currentDepthClear = depth;
      }
      return *this;
    }

    void reset()
    {
      locked = false;

      currentDepthMask = false;
      currentDepthFunc = Func::Never;
      currentDepthClear = 0;

    }
  };

  struct StencilBuffer
  {
    State *const glState;

    bool locked = false;

    GLuint currentStencilMask = 0;
    Func currentStencilFunc = Func::Never;
    GLint currentStencilRef = 0;
    GLuint currentStencilFuncMask = 0;
    Op currentStencilFail = Op::Zero;
    Op currentStencilZFail = Op::Zero;
    Op currentStencilZPass = Op::Zero;
    GLint currentStencilClear = 0;

    QOpenGLExtraFunctions * const _f;

    StencilBuffer(QOpenGLExtraFunctions *fn, State *state) : glState(state), _f(fn)
    {}

    StencilBuffer &setTest(bool stencilTest)
    {
      if (stencilTest) {
        glState->enable(GL_STENCIL_TEST);
      }
      else {
        glState->disable(GL_STENCIL_TEST);
      }
      return *this;
    }

    StencilBuffer &setMask(GLuint stencilMask)
    {
      if (currentStencilMask != stencilMask && !locked) {
        _f->glStencilMask(stencilMask);
        currentStencilMask = stencilMask;
      }
      return *this;
    }

    StencilBuffer &setFunc(Func stencilFunc, GLint stencilRef, GLuint stencilMask)
    {
      if (currentStencilFunc != stencilFunc || currentStencilRef != stencilRef || currentStencilFuncMask != stencilMask) {
        _f->glStencilFunc((GLenum) stencilFunc, stencilRef, stencilMask);

        currentStencilFunc = stencilFunc;
        currentStencilRef = stencilRef;
        currentStencilFuncMask = stencilMask;
      }
      return *this;
    }

    StencilBuffer &setOp(Op stencilFail, Op stencilZFail, Op stencilZPass)
    {

      if (currentStencilFail != stencilFail || currentStencilZFail != stencilZFail || currentStencilZPass != stencilZPass) {
        _f->glStencilOp((GLenum) stencilFail, (GLenum) stencilZFail, (GLenum) stencilZPass);

        currentStencilFail = stencilFail;
        currentStencilZFail = stencilZFail;
        currentStencilZPass = stencilZPass;
      }
      return *this;
    }

    StencilBuffer &setLocked(bool lock)
    {
      locked = lock;
      return *this;
    }

    StencilBuffer &setClear(GLint stencil)
    {
      if (currentStencilClear != stencil) {
        _f->glClearStencil(stencil);
        currentStencilClear = stencil;
      }
      return *this;
    }

    void reset()
    {
      bool locked = false;

      GLuint currentStencilMask = 0;
      Func currentStencilFunc = Func::Never;
      GLint currentStencilRef = 0;
      GLuint currentStencilFuncMask = 0;
      Op currentStencilFail = Op::Zero;
      Op currentStencilZFail = Op::Zero;
      Op currentStencilZPass = Op::Zero;
      Op currentStencilClear = Op::Zero;
    }
  };

  ColorBuffer colorBuffer;
  DepthBuffer depthBuffer;
  StencilBuffer stencilBuffer;

  GLint maxVertexAttributes;
  std::vector<GLuint> newAttributes;
  std::vector<GLuint> enabledAttributes;
  std::vector<GLuint> attributeDivisors;

  std::unordered_map<GLenum, bool> capabilities;

  std::vector<GLint> compressedTextureFormats;

  GLuint currentProgram = 0;

  Blending currentBlending = Blending::None;
  BlendEq currentBlendEquation = BlendEq::None;
  BlendFunc currentBlendSrc = BlendFunc::None;
  BlendFunc currentBlendDst = BlendFunc::None;
  BlendEq currentBlendEquationAlpha = BlendEq::None;
  BlendFunc currentBlendSrcAlpha = BlendFunc::None;
  BlendFunc currentBlendDstAlpha = BlendFunc::None;
  bool currentPremultipledAlpha = false;

  FrontFaceDirection currentFaceDirection = FrontFaceDirection::CCW;
  CullFace currentCullFace = CullFace::Back;

  GLfloat currentLineWidth = 0;

  GLfloat currentPolygonOffsetFactor = 0;
  GLfloat currentPolygonOffsetUnits = 0;

  bool currentScissorTest = false;

  GLuint maxTextures;

  int initialTextureSlot, currentTextureSlot;

  struct BoundTexture {
    const TextureTarget target;
    const GLuint texture;

    BoundTexture(TextureTarget target, GLuint texture) : target(target), texture(texture) {}
    BoundTexture() : target(TextureTarget::twoD), texture(0) {}
  };
  std::unordered_map<GLuint, BoundTexture> currentBoundTextures;

  math::Vector4 currentScissor = {0, 0, 0, 0};
  math::Vector4 currentViewport = {0, 0, 0, 0};

  GLuint createTexture(TextureTarget target)
  {
    uint8_t data[4]; // 4 is required to match default unpack alignment of 4.
    GLuint texture;
    _f->glGenTextures(1, &texture);

    _f->glBindTexture((GLenum)target, texture);
    _f->glTexParameteri((GLenum)target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    _f->glTexParameteri((GLenum)target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if(target == TextureTarget::cubeMap) {
      for(GLenum t=(GLenum)TextureTarget::cubeMapPositiveX; t <=(GLenum)TextureTarget::cubeMapNegativeZ; t++)
        _f->glTexImage2D(t, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else
      _f->glTexImage2D((GLenum)target, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    return texture;
  }

  QOpenGLExtraFunctions * const _f;
  std::unordered_map<TextureTarget, GLuint> emptyTextures = {};

public:
  State(QOpenGLExtraFunctions *fn, int initialTextureSlot=-1) :
     colorBuffer(fn), stencilBuffer(fn, this), depthBuffer(this, fn), _f(fn),
     initialTextureSlot(initialTextureSlot), currentTextureSlot(initialTextureSlot)
  {}

  void setInitialTextureSlot(GLenum slot) {
    initialTextureSlot = currentTextureSlot = slot;
  }

  void init()
  {
    if(!emptyTextures.count(TextureTarget::twoD))
      emptyTextures[TextureTarget::twoD] = createTexture(TextureTarget::twoD);
    if(!emptyTextures.count(TextureTarget::cubeMap))
      emptyTextures[TextureTarget::cubeMap] = createTexture(TextureTarget::cubeMap);

    colorBuffer.setClear(0, 0, 0, 1);
    depthBuffer.setClear(1);
    stencilBuffer.setClear(0);

    _f->glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, (GLint *)&maxTextures);

    _f->glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttributes);
    newAttributes.resize(maxVertexAttributes);
    enabledAttributes.resize(maxVertexAttributes);
    attributeDivisors.resize(maxVertexAttributes);

    enable(GL_DEPTH_TEST);
    depthBuffer.setFunc(Func::LessEqual);

    setFaceDirection(FrontFaceDirection::CW);
    setCullFace(CullFace::Back);

    enable(GL_BLEND);
    setBlending(Blending::Normal);
    check_glerror(_f);
  }

  State &initAttributes()
  {
    newAttributes.assign(newAttributes.size(), 0);
    return *this;
  }

  State &enableAttribute(GLuint attribute)
  {
    newAttributes[attribute] = 1;

    if (enabledAttributes[attribute] == 0) {
      _f->glEnableVertexAttribArray(attribute);
      enabledAttributes[attribute] = 1;
    }

    if (attributeDivisors[attribute] != 0) {
      _f->glVertexAttribDivisor(attribute, 0);
      attributeDivisors[attribute] = 0;
    }
    check_glerror(_f);
    return *this;
  }

  State &enableAttributeAndDivisor(uint8_t attribute, GLuint meshPerAttribute)
  {
    newAttributes[attribute] = 1;

    if (enabledAttributes[attribute] == 0) {
      _f->glEnableVertexAttribArray(attribute);
      enabledAttributes[attribute] = 1;
    }

    if (attributeDivisors[attribute] != meshPerAttribute) {
      _f->glVertexAttribDivisor(attribute, meshPerAttribute);
      attributeDivisors[attribute] = meshPerAttribute;
    }
    check_glerror(_f);
    return *this;
  }

  State &disableUnusedAttributes()
  {
    for (size_t i = 0, l = enabledAttributes.size(); i != l; ++i) {
      if (enabledAttributes[i] != newAttributes[i]) {
        _f->glDisableVertexAttribArray(i);
        enabledAttributes[i] = 0;
      }
    }
  }

  void enable(GLenum id)
  {
    if (capabilities.count(id) == 0 || !capabilities[id]) {
      _f->glEnable(id);
      capabilities[id] = true;
    }
  }

  void disable(GLenum id)
  {
    if (capabilities.count(id) == 0 || capabilities[id]) {
      _f->glDisable(id);
      capabilities[id] = false;
    }
  }

  const std::vector<GLint> &getCompressedTextureFormats()
  {
    if (compressedTextureFormats.empty()) {
      GLint numFormats;
      _f->glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numFormats);
      compressedTextureFormats.resize(numFormats);
      _f->glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, compressedTextureFormats.data());
    }

    return compressedTextureFormats;
  }

  bool hasCompressedTextureFormat(TextureFormat format)
  {
    getCompressedTextureFormats();

    return std::find(compressedTextureFormats.begin(), compressedTextureFormats.end(), (GLint)format) != compressedTextureFormats.end();
  }

  bool useProgram(GLuint program)
  {
    if (currentProgram != program) {

      _f->glUseProgram(program);
      currentProgram = program;
      return true;
    }
    return false;
  }

  State &
  setBlending(Blending blending, BlendEq blendEquation = BlendEq::None, BlendFunc blendSrc = BlendFunc::None,
              BlendFunc blendDst = BlendFunc::None, BlendEq blendEquationAlpha = BlendEq::None,
              BlendFunc blendSrcAlpha = BlendFunc::None, BlendFunc blendDstAlpha = BlendFunc::None,
              bool premultipliedAlpha = false)
  {
    if (blending != Blending::None) {
      enable(GL_BLEND);
    }
    else {
      disable(GL_BLEND);
    }

    if (blending != Blending::Custom) {
      if (blending != currentBlending || premultipliedAlpha != currentPremultipledAlpha) {

        switch (blending) {
          case Blending::Additive:
            if (premultipliedAlpha) {
              _f->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
              _f->glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
            }
            else {
              _f->glBlendEquation(GL_FUNC_ADD);
              _f->glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            }
            break;

          case Blending::Subtractive:
            if (premultipliedAlpha) {
              _f->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
              _f->glBlendFuncSeparate(GL_ZERO, GL_ZERO, GL_ONE_MINUS_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
            }
            else {
              _f->glBlendEquation(GL_FUNC_ADD);
              _f->glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
            }
            break;

          case Blending::Multiply:
            if (premultipliedAlpha) {
              _f->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
              _f->glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_SRC_ALPHA);
            }
            else {
              _f->glBlendEquation(GL_FUNC_ADD);
              _f->glBlendFunc(GL_ZERO, GL_SRC_COLOR);
            }
            break;

          default:
            if (premultipliedAlpha) {
              _f->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
              _f->glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            }
            else {
              _f->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
              _f->glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            }
        }
      }

      currentBlendEquation = BlendEq::None;
      currentBlendSrc = BlendFunc::None;
      currentBlendDst = BlendFunc::None;
      currentBlendEquationAlpha = BlendEq::None;
      currentBlendSrcAlpha = BlendFunc::None;
      currentBlendDstAlpha = BlendFunc::None;
    }
    else {
      blendEquationAlpha = blendEquationAlpha != BlendEq::None ? blendEquationAlpha : blendEquation;
      blendSrcAlpha = blendSrcAlpha != BlendFunc::None ? blendSrcAlpha : blendSrc;
      blendDstAlpha = blendDstAlpha != BlendFunc::None ? blendDstAlpha : blendDst;

      if (blendEquation != currentBlendEquation || blendEquationAlpha != currentBlendEquationAlpha) {

        _f->glBlendEquationSeparate((GLenum) blendEquation, (GLenum) blendEquationAlpha);

        currentBlendEquation = blendEquation;
        currentBlendEquationAlpha = blendEquationAlpha;
      }

      if (blendSrc != currentBlendSrc || blendDst != currentBlendDst || blendSrcAlpha != currentBlendSrcAlpha ||
          blendDstAlpha != currentBlendDstAlpha) {

        _f->glBlendFuncSeparate((GLenum) blendSrc, (GLenum) blendDst, (GLenum) blendSrcAlpha, (GLenum) blendDstAlpha);

        currentBlendSrc = blendSrc;
        currentBlendDst = blendDst;
        currentBlendSrcAlpha = blendSrcAlpha;
        currentBlendDstAlpha = blendDstAlpha;
      }
    }

    currentBlending = blending;
    currentPremultipledAlpha = premultipliedAlpha;
  }

  State &setMaterial(const Material::Ptr material, bool frontFaceCW)
  {
    material->side == Side::Double ? disable(GL_CULL_FACE) : enable(GL_CULL_FACE);

    FrontFaceDirection dir = material->side == Side::Back ?
                             (frontFaceCW ? FrontFaceDirection::CCW : FrontFaceDirection::CW) :
                             (frontFaceCW ? FrontFaceDirection::CW : FrontFaceDirection::CCW);

    setFaceDirection(dir);

    if (material->transparent)
      setBlending(material->blending, material->blendEquation, material->blendSrc, material->blendDst,
                  material->blendEquationAlpha, material->blendSrcAlpha, material->blendDstAlpha, material->premultipliedAlpha);
    else
      setBlending(Blending::None);

    depthBuffer.setFunc(material->depthFunc);
    depthBuffer.setTest(material->depthTest);
    depthBuffer.setMask(material->depthWrite);
    colorBuffer.setMask(material->colorWrite);

    setPolygonOffset(material->polygonOffset, material->polygonOffsetFactor, material->polygonOffsetUnits);
  }

  //

  //setFlipSided
  State &setFaceDirection(FrontFaceDirection faceDirection)
  {
    if (currentFaceDirection != faceDirection) {
      if(faceDirection != FrontFaceDirection::Undefined) {
        _f->glFrontFace((GLenum) faceDirection);
        check_glerror(_f);
      }
      currentFaceDirection = faceDirection;
    }
  }

  State &setCullFace(CullFace cullFace)
  {
    if (cullFace != CullFace::None) {
      enable(GL_CULL_FACE);

      if (cullFace != currentCullFace) {
        _f->glCullFace((GLenum) cullFace);
        check_glerror(_f);
      }
    }
    else {
      disable(GL_CULL_FACE);
      check_glerror(_f);
    }

    currentCullFace = cullFace;
  }

  State &setLineWidth(GLfloat width)
  {
    if (width != currentLineWidth) {
      _f->glLineWidth(width);
      currentLineWidth = width;
    }
  }

  State &setPolygonOffset(bool polygonOffset, GLfloat factor, GLfloat units)
  {
    if (polygonOffset) {
      enable(GL_POLYGON_OFFSET_FILL);

      if (currentPolygonOffsetFactor != factor || currentPolygonOffsetUnits != units) {

        _f->glPolygonOffset(factor, units);

        currentPolygonOffsetFactor = factor;
        currentPolygonOffsetUnits = units;
      }
    }
    else {
      disable(GL_POLYGON_OFFSET_FILL);
    }
  }

  bool getScissorTest()
  {
    return currentScissorTest;
  }

  State &setScissorTest(bool scissorTest)
  {
    currentScissorTest = scissorTest;

    if (scissorTest)
      enable(GL_SCISSOR_TEST);
    else
      disable(GL_SCISSOR_TEST);
  }

  // texture
  State &activeTexture(GLenum glSlot=0)
  {
    if(glSlot == 0) glSlot = GL_TEXTURE0 + maxTextures - 1;

    if ( currentTextureSlot != glSlot ) {
      _f->glActiveTexture( glSlot );
      currentTextureSlot = glSlot;
    }
  }

  void bindTexture(TextureTarget target, GLint webglTexture=-1)
  {
    if(currentTextureSlot < 0)
      activeTexture();

    BoundTexture *boundTexture = currentBoundTextures.count(currentTextureSlot) ?
                                 &currentBoundTextures[currentTextureSlot] : nullptr;

    if(!boundTexture || boundTexture->target != target || boundTexture->texture != webglTexture ) {

      _f->glBindTexture((GLenum)target, webglTexture >= 0 ? webglTexture : emptyTextures[target]);
      check_glerror(_f);

      currentBoundTextures.emplace(currentTextureSlot, BoundTexture(target, webglTexture));
    }
  }

  void compressedTexImage2D(TextureTarget target, GLint level, TextureFormat internalFormat,
                            GLsizei width, GLsizei height, const std::vector<unsigned char> &data)
  {
    _f->glCompressedTexImage2D((GLenum)target, level, (GLenum)internalFormat, width, height, 0, data.size(), data.data());
    check_glerror(_f);
  }

  void texImage2D(TextureTarget target,
                  GLint level,
                  TextureFormat internalFormat,
                  GLsizei width,
                  GLsizei height,
                  TextureFormat format,
                  TextureType type,
                  const QImage &image)
  {
    _f->glTexImage2D((GLenum)target, level, (GLint)internalFormat, width, height, 0, (GLenum)format, (GLenum)type, image.bits());
    check_glerror(_f);
  }

  void texImage2D(TextureTarget target,
                  GLint level,
                  TextureFormat internalFormat,
                  TextureFormat format,
                  TextureType type,
                  const QImage &image)
  {
    _f->glTexImage2D((GLenum)target, level, (GLint)internalFormat, image.width(), image.height(), 0, (GLenum)format,
                 (GLenum)type, image.bits());
    check_glerror(_f);
  }

  void texImage2D(TextureTarget target,
                  GLint level,
                  TextureFormat internalFormat,
                  GLsizei width,
                  GLsizei height,
                  TextureFormat format,
                  TextureType type,
                  const unsigned char *pixels)
  {
    glTexImage2D((GLenum)target, level, (GLint)internalFormat, width, height, 0, (GLenum)format, (GLenum)type, pixels);
    check_glerror(_f);
  }

  void texImage2D(TextureTarget target,
                  GLint level,
                  TextureFormat internalFormat,
                  GLsizei width,
                  GLsizei height,
                  TextureFormat format,
                  TextureType type)
  {
    glTexImage2D((GLenum)target, level, (GLint)internalFormat, width, height, 0, (GLenum)format, (GLenum)type, nullptr);
    check_glerror(_f);
  }

  void texImage2D(TextureTarget target,
                  GLint level,
                  TextureFormat internalFormat,
                  TextureFormat format,
                  TextureType type,
                  const Mipmap &mipmap)
  {
    _f->glTexImage2D((GLenum)target, level, (GLint)internalFormat,
                 mipmap.width, mipmap.height, 0, (GLenum)format, (GLenum)type, mipmap.data.data());
    check_glerror(_f);
  }

  void scissor(const math::Vector4 &scissor)
  {
    if(currentScissor != scissor) {
      _f->glScissor( scissor.x(), scissor.y(), scissor.z(), scissor.w() );
      currentScissor = scissor;
    }
  }

  void viewport(const math::Vector4 &viewport)
  {
    if(currentViewport != viewport) {
      _f->glViewport( viewport.x(), viewport.y(), viewport.z(), viewport.w());
      currentViewport = viewport;
    }
  }

  void reset()
  {
    for(size_t i=0; i < enabledAttributes.size(); i ++ ) {
      if (enabledAttributes[ i ] == 1) {
        _f->glDisableVertexAttribArray( i );
        enabledAttributes[ i ] = 0;
      }
    }

    /*for(auto &cap : capabilities) {
      _f->glDisable(cap.first);
    }
    capabilities.clear();*/

    compressedTextureFormats.clear();

    currentTextureSlot = initialTextureSlot;
    currentBoundTextures.clear();

    currentViewport.set(0, 0, 0, 0);

    currentProgram = 0;

    currentBlending = Blending::None;

    currentFaceDirection = FrontFaceDirection::CW;
    currentCullFace = CullFace::None;

    colorBuffer.reset();
    depthBuffer.reset();
    stencilBuffer.reset();
  }
};

}
}

#endif //THREE_QT_GLSTATE_H
