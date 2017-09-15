//
// Created by byter on 29.08.17.
//

#ifndef THREE_QT_GLSTATE_H
#define THREE_QT_GLSTATE_H

#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <math/Vector4.h>
#include <material/Material.h>
#include <Constants.h>
#include <unordered_map>
#include <unordered_set>

namespace three {
namespace gl {

using glfunctions = QOpenGLExtraFunctions;

class State : private glfunctions
{
public:
  struct ColorBuffer : private glfunctions
  {
    bool locked = false;
    math::Vector4 color = {0, 0, 0, 0};
    GLboolean currentColorMask = 0;
    math::Vector4 currentColorClear = {0, 0, 0, 0};

    ColorBuffer(QOpenGLContext *context) : glfunctions(context)
    {}

    ColorBuffer &setMask(GLboolean colorMask)
    {
      if (currentColorMask != colorMask && !locked) {
        glColorMask(colorMask, colorMask, colorMask, colorMask);
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
        glClearColor(r, g, b, a);
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

  struct DepthBuffer : public glfunctions
  {
    State *const glState;

    bool locked = false;

    bool currentDepthMask = false;
    Func currentDepthFunc = Func::LessEqual;
    double currentDepthClear = 0;

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

    DepthBuffer(State *state) : glState(state)
    {}

    DepthBuffer &setMask(bool depthMask)
    {
      if (currentDepthMask != depthMask && !locked) {
        glDepthMask(depthMask);
        currentDepthMask = depthMask;
      }
      return *this;
    }

    DepthBuffer &setFunc(Func depthFunc)
    {

      if (currentDepthFunc != depthFunc) {
        glDepthFunc((GLenum) depthFunc);
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

  struct StencilBuffer : private glfunctions
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

    StencilBuffer(QOpenGLContext *context, State *state) : glfunctions(context), glState(state)
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
        glStencilMask(stencilMask);
        currentStencilMask = stencilMask;
      }
      return *this;
    }

    StencilBuffer &setFunc(Func stencilFunc, GLint stencilRef, GLuint stencilMask)
    {
      if (currentStencilFunc != stencilFunc || currentStencilRef != stencilRef || currentStencilFuncMask != stencilMask) {
        glStencilFunc((GLenum) stencilFunc, stencilRef, stencilMask);

        currentStencilFunc = stencilFunc;
        currentStencilRef = stencilRef;
        currentStencilFuncMask = stencilMask;
      }
      return *this;
    }

    StencilBuffer &setOp(Op stencilFail, Op stencilZFail, Op stencilZPass)
    {

      if (currentStencilFail != stencilFail || currentStencilZFail != stencilZFail || currentStencilZPass != stencilZPass) {
        glStencilOp((GLenum) stencilFail, (GLenum) stencilZFail, (GLenum) stencilZPass);

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
        glClearStencil(stencil);
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
  std::vector<uint8_t> newAttributes;
  std::vector<uint8_t> enabledAttributes;
  std::vector<uint8_t> attributeDivisors;

  std::unordered_set<GLenum> capabilities;

  std::vector<GLint> compressedTextureFormats;

  GLuint currentProgram;

  Blending currentBlending = Blending::None;
  BlendEq currentBlendEquation = BlendEq::None;
  BlendFunc currentBlendSrc = BlendFunc::None;
  BlendFunc currentBlendDst = BlendFunc::None;
  BlendEq currentBlendEquationAlpha = BlendEq::None;
  BlendFunc currentBlendSrcAlpha = BlendFunc::None;
  BlendFunc currentBlendDstAlpha = BlendFunc::None;
  bool currentPremultipledAlpha = false;

  FrontFaceDirection currentFlipSided = FrontFaceDirection::CW;
  CullFace currentCullFace = CullFace::None;

  GLfloat currentLineWidth = 0;

  GLfloat currentPolygonOffsetFactor = 0;
  GLfloat currentPolygonOffsetUnits = 0;

  bool currentScissorTest = false;

  GLuint maxTextures;

  int currentTextureSlot = -1;

  struct BoundTexture {
    GLenum type;
    GLuint texture;
  };
  std::unordered_map<GLuint, BoundTexture> currentBoundTextures;

  math::Vector4 currentScissor = {0, 0, 0, 0};
  math::Vector4 currentViewport = {0, 0, 0, 0};

  GLuint createTexture(GLenum type, GLenum target, unsigned count)
  {
    uint8_t data[4]; // 4 is required to match default unpack alignment of 4.
    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(type, texture);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    for (unsigned i = 0; i < count; i++) {
      glTexImage2D(target + i, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    return texture;
  }

  std::unordered_map<GLenum, GLuint> emptyTextures = {};

  // init
  State(QOpenGLContext *context) :
     glfunctions(context), colorBuffer(context), stencilBuffer(context, this), depthBuffer(this)
  {
    emptyTextures[GL_TEXTURE_2D] = createTexture(GL_TEXTURE_2D, GL_TEXTURE_2D, 1);
    emptyTextures[GL_TEXTURE_CUBE_MAP] = createTexture(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_POSITIVE_X, 6);

    colorBuffer.setClear(0, 0, 0, 1);
    depthBuffer.setClear(1);
    stencilBuffer.setClear(0);

    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, (GLint *)&maxTextures);

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttributes);
    newAttributes.resize(maxVertexAttributes);
    enabledAttributes.resize(maxVertexAttributes);
    attributeDivisors.resize(maxVertexAttributes);

    enable(GL_DEPTH_TEST);
    depthBuffer.setFunc(Func::LessEqual);

    setFlipSided(FrontFaceDirection::CW);
    setCullFace(CullFace::Back);
    enable(GL_CULL_FACE);

    enable(GL_BLEND);
    setBlending(Blending::Normal);
  }

  State &initAttributes()
  {
    newAttributes.assign(newAttributes.size(), 0);
    return *this;
  }

  State &enableAttribute(uint8_t attribute)
  {
    newAttributes[attribute] = 1;

    if (enabledAttributes[attribute] == 0) {
      glEnableVertexAttribArray(attribute);
      enabledAttributes[attribute] = 1;
    }

    if (attributeDivisors[attribute] != 0) {
      glVertexAttribDivisor(attribute, 0);
      attributeDivisors[attribute] = 0;
    }
    return *this;
  }

  State &enableAttributeAndDivisor(uint8_t attribute, GLuint meshPerAttribute)
  {
    newAttributes[attribute] = 1;

    if (enabledAttributes[attribute] == 0) {
      glEnableVertexAttribArray(attribute);
      enabledAttributes[attribute] = 1;
    }

    if (attributeDivisors[attribute] != meshPerAttribute) {
      glVertexAttribDivisor(attribute, meshPerAttribute);
      attributeDivisors[attribute] = meshPerAttribute;
    }
    return *this;
  }

  State &disableUnusedAttributes()
  {
    for (size_t i = 0, l = enabledAttributes.size(); i != l; ++i) {
      if (enabledAttributes[i] != newAttributes[i]) {
        glDisableVertexAttribArray(i);
        enabledAttributes[i] = 0;
      }
    }
  }

  void enable(GLenum id)
  {
    if (capabilities.find(id) == capabilities.end()) {
      glEnable(id);
      capabilities.insert(id);
    }
  }

  void disable(GLenum id)
  {
    if (capabilities.find(id) != capabilities.end()) {
      glDisable(id);
      capabilities.erase(id);
    }
  }

  const std::vector<GLint> &getCompressedTextureFormats()
  {
    if (compressedTextureFormats.empty()) {
      GLint numFormats;
      glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numFormats);
      compressedTextureFormats.resize(numFormats);
      glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, compressedTextureFormats.data());
    }

    return compressedTextureFormats;
  }

  bool useProgram(GLuint program)
  {
    if (currentProgram != program) {

      glUseProgram(program);
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
              glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
              glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
            }
            else {
              glBlendEquation(GL_FUNC_ADD);
              glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            }
            break;

          case Blending::Subtractive:
            if (premultipliedAlpha) {
              glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
              glBlendFuncSeparate(GL_ZERO, GL_ZERO, GL_ONE_MINUS_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
            }
            else {
              glBlendEquation(GL_FUNC_ADD);
              glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
            }
            break;

          case Blending::Multiply:
            if (premultipliedAlpha) {
              glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
              glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_SRC_ALPHA);
            }
            else {
              glBlendEquation(GL_FUNC_ADD);
              glBlendFunc(GL_ZERO, GL_SRC_COLOR);
            }
            break;

          default:
            if (premultipliedAlpha) {
              glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
              glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            }
            else {
              glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
              glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
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

        glBlendEquationSeparate((GLenum) blendEquation, (GLenum) blendEquationAlpha);

        currentBlendEquation = blendEquation;
        currentBlendEquationAlpha = blendEquationAlpha;
      }

      if (blendSrc != currentBlendSrc || blendDst != currentBlendDst || blendSrcAlpha != currentBlendSrcAlpha ||
          blendDstAlpha != currentBlendDstAlpha) {

        glBlendFuncSeparate((GLenum) blendSrc, (GLenum) blendDst, (GLenum) blendSrcAlpha, (GLenum) blendDstAlpha);

        currentBlendSrc = blendSrc;
        currentBlendDst = blendDst;
        currentBlendSrcAlpha = blendSrcAlpha;
        currentBlendDstAlpha = blendDstAlpha;
      }
    }

    currentBlending = blending;
    currentPremultipledAlpha = premultipliedAlpha;
  }

  State &setMaterial(const Material::Ptr material)
  {
    material->side == Side::Double
    ? disable(GL_CULL_FACE)
    : enable(GL_CULL_FACE);

    setFlipSided(material->side == Side::Back ? FrontFaceDirection::CW : FrontFaceDirection::CCW);

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

  State &setFlipSided(FrontFaceDirection flipSided)
  {
    if (currentFlipSided != flipSided) {
      glFrontFace((GLenum) flipSided);

      currentFlipSided = flipSided;
    }
  }

  State &setCullFace(CullFace cullFace)
  {
    if (cullFace != CullFace::None) {
      enable(GL_CULL_FACE);

      if (cullFace != currentCullFace) {
        glCullFace((GLenum) cullFace);
      }

    }
    else {
      disable(GL_CULL_FACE);
    }

    currentCullFace = cullFace;
  }

  State &setLineWidth(GLfloat width)
  {
    if (width != currentLineWidth) {
      glLineWidth(width);
      currentLineWidth = width;
    }
  }

  State &setPolygonOffset(bool polygonOffset, GLfloat factor, GLfloat units)
  {
    if (polygonOffset) {
      enable(GL_POLYGON_OFFSET_FILL);

      if (currentPolygonOffsetFactor != factor || currentPolygonOffsetUnits != units) {

        glPolygonOffset(factor, units);

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
      glActiveTexture( glSlot );
      currentTextureSlot = glSlot;
    }
  }

  void bindTexture(GLenum webglType, GLint webglTexture=-1)
  {
    if(currentTextureSlot < 0)
      activeTexture();

    BoundTexture *boundTexture;
    auto find = currentBoundTextures.find(currentTextureSlot);
    if (find == currentBoundTextures.end()) {
      boundTexture = &currentBoundTextures[currentTextureSlot];
    }
    else
      boundTexture = &find->second;

    if(boundTexture->type != webglType || boundTexture->texture != webglTexture ) {

      glBindTexture( webglType, webglTexture >= 0 ? webglTexture : emptyTextures[webglType]);

      boundTexture->type = webglType;
      boundTexture->texture = webglTexture;
    }
  }

#if 0
  void compressedTexImage2D()
  {
    try {
      glCompressedTexImage2D.apply( gl, arguments );
    } catch ( error ) {
      console.error( 'THREE.WebGLState:', error );
    }
  }

  function texImage2D() {
    try {
      gl.texImage2D.apply( gl, arguments );
    } catch ( error ) {
      console.error( 'THREE.WebGLState:', error );
    }
  }
#endif

  void scissor(const math::Vector4 &scissor)
  {
    if(currentScissor != scissor) {
      glScissor( scissor.x(), scissor.y(), scissor.z(), scissor.w() );
      currentScissor = scissor;
    }
  }

  void viewport(const math::Vector4 &viewport)
  {
    if(currentViewport != viewport) {
      glViewport( viewport.x(), viewport.y(), viewport.z(), viewport.w());
      currentViewport = viewport;
    }
  }

  void reset()
  {
    for(size_t i=0; i < enabledAttributes.size(); i ++ ) {
      if (enabledAttributes[ i ] == 1) {
        glDisableVertexAttribArray( i );
        enabledAttributes[ i ] = 0;
      }
    }

    capabilities.clear();

    compressedTextureFormats.clear();

    currentTextureSlot = -1;
    currentBoundTextures.clear();

    currentProgram = 0;

    currentBlending = Blending::None;

    currentFlipSided = FrontFaceDirection::CW;
    currentCullFace = CullFace::None;

    colorBuffer.reset();
    depthBuffer.reset();
    stencilBuffer.reset();
  }
};

}
}

#endif //THREE_QT_GLSTATE_H
