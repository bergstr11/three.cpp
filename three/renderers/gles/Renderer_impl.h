//
// Created by byter on 05.08.17.
//

#ifndef THREE_QT_OPENGLRENDERER_H
#define THREE_QT_OPENGLRENDERER_H

#include <renderers/OpenGLRenderer.h>
#include <light/Light.h>
#include <math/Frustum.h>
#include <objects/Sprite.h>
#include <objects/LensFlare.h>
#include "RenderTarget.h"
#include "State.h"
#include "Clipping.h"
#include "RenderLists.h"

namespace three {
namespace gl {

class Renderer_impl : public OpenGLRenderer
{
  std::vector<Light::Ptr> _lightsArray;
  std::vector<Light::Ptr> _shadowsArray;

  //var currentRenderList = null;

  std::vector<Sprite::Ptr> _spritesArray;
  std::vector<LensFlare::Ptr> _flaresArray;

  // clearing
  bool _autoClear = true;
  bool _autoClearColor = true;
  bool _autoClearDepth = true;
  bool _autoClearStencil = true;

  // scene graph
  bool _sortObjects = true;

  // user-defined clipping
  std::vector<math::Plane> _clippingPlanes;
  bool _localClippingEnabled = false;

  // physically based shading
  float _gammaFactor = 2.0;  // for backwards compatibility
  bool _gammaInput = false;
  bool _gammaOutput = false;

  // physical lights
  bool _physicallyCorrectLights = false;

  // tone mapping
  ToneMapping _toneMapping = ToneMapping::Linear;
  float _toneMappingExposure = 1.0;
  float _toneMappingWhitePoint = 1.0;

  // morphs
  unsigned _maxMorphTargets = 8;
  unsigned _maxMorphNormals = 4;

  bool _isContextLost = false;

  // internal state cache
  RenderTarget::Ptr _currentRenderTarget = nullptr;
  //_currentFramebuffer = null,
  int _currentMaterialId = -1;
  unsigned _currentGeometryProgram = 0;

  Camera::Ptr _currentCamera;
  Camera::Ptr _currentArrayCamera;

  QVector4D _currentViewport;
  QVector4D _currentScissor;
  //_currentScissorTest = null,

  //

  unsigned _usedTextureUnits = 0;

  unsigned _width;
  unsigned _height;

  float _pixelRatio = 1;

  QVector4D _viewport;// = new Vector4( 0, 0, _width, _height ),
  QVector4D _scissor; // = new Vector4( 0, 0, _width, _height ),
  bool _scissorTest = false;

  // frustum
  //Frustum _frustum = new Frustum(),

  // clipping
  Clipping _clipping;
  bool _clippingEnabled = false;
  //bool _localClippingEnabled = false;

  // camera matrices cache
  math::Matrix4 _projScreenMatrix;
  math::Frustum _frustum;
  math::Vector3 _vector3;

  RenderLists _renderLists;
  RenderList *_currentRenderList = nullptr;

  float getTargetPixelRatio()
  {
    return _currentRenderTarget ? _pixelRatio : 1;
  }

  gl::State _state;

  void projectObject(Object3D::Ptr object, Camera::Ptr camera, bool sortObjects );

public:

  //OpenGLRenderer_impl(QOpenGLContext *context, Options options=Options());
  Renderer_impl(QOpenGLContext *context, unsigned width, unsigned height);

  void render(const Scene::Ptr scene, const Camera::Ptr camera) override;

  gl::State &state() {return _state;}
};

}
}
#endif //THREE_QT_OPENGLRENDERER_H
