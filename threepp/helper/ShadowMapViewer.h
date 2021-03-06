//
// Created by byter on 9/18/18.
//

#ifndef THREE_PP_SHADOWMAPVIEWER_H
#define THREE_PP_SHADOWMAPVIEWER_H

#include <QRectF>
#include <threepp/light/LightShadow.h>
#include <threepp/scene/Scene.h>
#include <threepp/geometry/Plane.h>
#include <threepp/objects/Mesh.h>
#include <threepp/renderers/gl/shader/UniformsLib.h>
#include <threepp/renderers/gl/shader/ShaderLib.h>
#include <threepp/material/ShaderMaterial.h>
#include <threepp/material/MeshPhongMaterial.h>
#include <threepp/camera/OrthographicCamera.h>
#include <threepp/renderers/OpenGLRenderer.h>

namespace three {


/**
 * @author alteredq / http://alteredqualia.com/
 *
 * Unpack RGBA depth shader
 * - show RGBA encoded depth as monochrome color
 */
constexpr const char * const vertexShader =

"out vec2 vUv;"
"void main() {"
"  vUv = uv;"
"  gl_Position = projectionMatrix * modelViewMatrix * vec4( position, 1.0 );"
"}";

constexpr const char * const fragmentShader =

"uniform float opacity;"
"uniform sampler2D diffuse;"
"in vec2 vUv;"
"out vec4 fragColor;"
"#include <packing>"
"void main() {"
"  float depth = 1.0 - unpackRGBAToDepth( texture( diffuse, vUv ) );"
"  fragColor = vec4( vec3( depth ), opacity );"
"}";

/**
 * @author arya-s / https://github.com/arya-s
 *
 * This is a helper for visualising a given light's shadow map.
 * It works for shadow casting lights: THREE.DirectionalLight and THREE.SpotLight.
 * It renders out the shadow map and displays it on a HUD.
 */
class ShadowMapViewer
{
protected:
  quick::Light *_light;

  QPointF _position {0, 0};
  float _scale = 1;
  float _itemWidth, _itemHeight;

  OrthographicCamera::Ptr _camera;

  Scene::Ptr _scene;
  DynamicMesh::Ptr _mesh;

  ShaderMaterial::Ptr _material;

  ShadowMapViewer(quick::Light *light, const QPointF &position, float scale, float itemWidth, float itemHeight)
     : _light(light),
       _position(position),
       _scale(scale),
       _itemWidth(itemWidth),
       _itemHeight(itemHeight),
       _camera(OrthographicCamera::make(itemWidth / -2, itemWidth / 2, itemHeight / 2, itemHeight / - 2, 1, 10)),
       _scene(Scene::make())
  {
    _camera->position().set( 0, 0, 2 );

    gl::UniformValues uniforms;
    uniforms.set(gl::UniformName::opacity, 1.0f);
    _material = ShaderMaterial::make(uniforms, vertexShader, fragmentShader, Side::Front);

    geometry::buffer::Plane::Ptr geometry = geometry::buffer::Plane::make(_itemWidth, _itemHeight);
    _mesh = DynamicMesh::make(geometry, _material);

    _scene->add(_mesh);
  }

  void setGeometry(float itemWidth, float itemHeight)
  {
    const auto &sz = _light->light()->shadow()->mapSize();

    if(_itemWidth != itemWidth || _itemHeight != itemHeight) {
      _itemWidth = itemWidth;
      _itemHeight = itemHeight;

      geometry::buffer::Plane::Ptr geometry = geometry::buffer::Plane::make(sz.width(), sz.height());
      _mesh->setGeometry(geometry);
    }

    _mesh->scale().set(_scale, _scale, 1);

    float width = sz.width() * _scale;
    float height = sz.height() * _scale;

    _mesh->position().set(-_itemWidth / 2 + width / 2 + _position.x(), _itemHeight / 2 - height / 2 - _position.y(), 0);

    _camera->setLeft(_itemWidth / -2);
    _camera->setRight(_itemWidth / 2);
    _camera->setTop(_itemHeight / 2);
    _camera->setBottom(_itemHeight / -2);
    _camera->updateProjectionMatrix();
  }

public:
  using Ptr = std::shared_ptr<ShadowMapViewer>;

  static Ptr make(quick::Light *light, const QPointF &position, float scale, float itemWidth, float itemHeight) {
    return Ptr(new ShadowMapViewer(light, position, scale, itemWidth, itemHeight));
  }

  // Set to false to disable displaying this shadow map
  bool enabled = true;

  void setPosition(const QPointF &pos) {
    _position = pos;
  }

  void setLight(quick::Light *light) {
    _light = light;
  }

  void setScale(float scale) {
    _scale = scale;
  }

  QPointF &position() {return _position;}
  float &scale() {return _scale;}

  void render(OpenGLRenderer::Ptr renderer, three::Renderer::Target::Ptr target)
  {
    auto shadow = _light && _light->light() ? _light->light()->shadow() : nullptr;
    if (enabled && shadow && shadow->map()) {

      setGeometry(target->width(), target->height());

      //Because a light's .shadowMap is only initialised after the first render pass
      //we have to make sure the correct map is sent into the shader, otherwise we
      //always end up with the scene's first added shadow casting light's shadowMap
      //in the shader
      //See: https://github.com/mrdoob/three.js/issues/5932
      _material->uniforms.set(gl::UniformName::diffuse, shadow->map()->texture());

      bool tmpAutoClear = renderer->autoClear;
      renderer->autoClear = false; // To allow render overlay
      renderer->clearDepth();
      renderer->render(_scene, _camera, target);
      renderer->autoClear = tmpAutoClear;  //Restore user's setting
    }
  }
};

}

#endif //THREE_PP_SHADOWMAPVIEWER_H
