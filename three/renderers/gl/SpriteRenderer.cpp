//
// Created by byter on 22.10.17.
//
#include "SpriteRenderer.h"
#include <algorithm>
#include <array>
#include <sstream>
#include "Renderer_impl.h"

namespace three {
namespace gl {

using namespace std;

struct SpriteRenderer::Data
{
  GLint position_att;
  GLint uv_att;

  GLint projectionMatrix;
  GLint map;
  GLint opacity;
  GLint color;
  GLint scale;
  GLint rotation;
  GLint uvOffset;
  GLint uvScale;
  GLint modelViewMatrix;
  GLint fogType, fogDensity, fogNear, fogFar, fogColor, fogDepth;
  GLint alphaTest;

  Data(QOpenGLFunctions *f, GLuint program)
  {
    position_att = f->glGetAttribLocation(program, "position");
    uv_att = f->glGetAttribLocation(program, "uv");

    uvOffset = f->glGetUniformLocation(program, "uvOffset");
    uvScale = f->glGetUniformLocation(program, "uvScale");

    rotation = f->glGetUniformLocation(program, "rotation");
    scale = f->glGetUniformLocation(program, "scale");

    color = f->glGetUniformLocation(program, "color");
    map = f->glGetUniformLocation(program, "map");
    opacity = f->glGetUniformLocation(program, "opacity");

    modelViewMatrix = f->glGetUniformLocation(program, "modelViewMatrix");
    projectionMatrix = f->glGetUniformLocation(program, "projectionMatrix");

    fogType = f->glGetUniformLocation(program, "fogType");
    fogDensity = f->glGetUniformLocation(program, "fogDensity");
    fogNear = f->glGetUniformLocation(program, "fogNear");
    fogFar = f->glGetUniformLocation(program, "fogFar");
    fogColor = f->glGetUniformLocation(program, "fogColor");
    fogDepth = f->glGetUniformLocation(program, "fogDepth");

    alphaTest = f->glGetUniformLocation(program, "alphaTest");
  }
};

SpriteRenderer::~SpriteRenderer()
{
  if(_data) delete _data;
}

void SpriteRenderer::init()
{
  array<float, 16> vertices = {
     -0.5f, -0.5f, 0, 0,
     0.5f, -0.5f, 1, 0,
     0.5f, 0.5f, 1, 1,
     -0.5f, 0.5f, 0, 1
  };

  array<uint16_t, 6> faces = {
     0, 1, 2,
     0, 2, 3
  };

  _r.glGenBuffers(1, &_vertexBuffer);
  _r.glGenBuffers(1, &_elementBuffer);

  _r.glBindBuffer( GL_ARRAY_BUFFER, _vertexBuffer );
  _r.glBufferData( GL_ARRAY_BUFFER, 16 * sizeof(float), vertices.data(), GL_STATIC_DRAW );

  _r.glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _elementBuffer );
  _r.glBufferData( GL_ELEMENT_ARRAY_BUFFER, 12, faces.data(), GL_STATIC_DRAW );

  GLuint vshader = _r.glCreateShader( GL_VERTEX_SHADER );
  GLuint fshader = _r.glCreateShader( GL_FRAGMENT_SHADER );

  static const char * vertexShader =
     "#define SHADER_NAME SpriteMaterial"

     "uniform mat4 modelViewMatrix;"
     "uniform mat4 projectionMatrix;"
     "uniform float rotation;"
     "uniform vec2 scale;"
     "uniform vec2 uvOffset;"
     "uniform vec2 uvScale;"

     "attribute vec2 position;"
     "attribute vec2 uv;"

     "varying vec2 vUV;"
     "varying float fogDepth;"

     "void main() {"

     "	vUV = uvOffset + uv * uvScale;"

     "	vec2 alignedPosition = position * scale;"

     "	vec2 rotatedPosition;"
     "	rotatedPosition.x = cos( rotation ) * alignedPosition.x - sin( rotation ) * alignedPosition.y;"
     "	rotatedPosition.y = sin( rotation ) * alignedPosition.x + cos( rotation ) * alignedPosition.y;"

     "	vec4 mvPosition;"

     "	mvPosition = modelViewMatrix * vec4( 0.0, 0.0, 0.0, 1.0 );"
     "	mvPosition.xy += rotatedPosition;"

     "	gl_Position = projectionMatrix * mvPosition;"

     "	fogDepth = - mvPosition.z;"

     "}";

  stringstream ss;
  ss << "precision " << _capabilities.precisionS() << " float;" << endl << vertexShader;
  const char *vsource = ss.str().data();
  _r.glShaderSource(vshader, 1, &vsource, nullptr);

  static const char * fragmentShader =
    "#define SHADER_NAME  SpriteMaterial"

    "uniform vec3 color;"
    "uniform sampler2D map;"
    "uniform float opacity;"

    "uniform int fogType;"
    "uniform vec3 fogColor;"
    "uniform float fogDensity;"
    "uniform float fogNear;"
    "uniform float fogFar;"
    "uniform float alphaTest;"

    "varying vec2 vUV;"
    "varying float fogDepth;"

    "void main() {"

    "	vec4 texture = texture2D( map, vUV );"

    "	gl_FragColor = vec4( color * texture.xyz, texture.a * opacity );"

    "	if ( gl_FragColor.a < alphaTest ) discard;"

    "	if ( fogType > 0 ) {"

    "		float fogFactor = 0.0;"

    "		if ( fogType == 1 ) {"

    "			fogFactor = smoothstep( fogNear, fogFar, fogDepth );"

    "		} else {"

    "			const float LOG2 = 1.442695;"
    "			fogFactor = exp2( - fogDensity * fogDensity * fogDepth * fogDepth * LOG2 );"
    "			fogFactor = 1.0 - clamp( fogFactor, 0.0, 1.0 );"

    "		}"

    "		gl_FragColor.rgb = mix( gl_FragColor.rgb, fogColor, fogFactor );"

    "	}"

    "}";

  ss.clear();
  ss << "precision " << _capabilities.precisionS() << " float;" << endl << fragmentShader;
  const char *fsource = ss.str().data();
  _r.glShaderSource(fshader, 1, &fsource, nullptr);

  _r.glCompileShader(vshader);
  _r.glCompileShader(fshader);

  _r.glAttachShader( _program, vshader );
  _r.glAttachShader( _program, fshader );

  _r.glLinkProgram( _program );

  /*var canvas = document.createElementNS( 'http://www.w3.org/1999/xhtml', 'canvas' );
  canvas.width = 8;
  canvas.height = 8;

  var context = canvas.getContext( '2d' );
  context.fillStyle = 'white';
  context.fillRect( 0, 0, 8, 8 );

  texture = new CanvasTexture( canvas );*/
}

void SpriteRenderer::render(vector<Sprite::Ptr> &sprites, Scene::Ptr scene, Camera::Ptr camera)
{
  if (sprites.empty()) return;

  // setup gl
  if (!_linked) {
    init();
    _linked = true;
  }

  _state.useProgram(_program);

  _state.initAttributes();
  _state.enableAttribute( _data->position_att );
  _state.enableAttribute( _data->uv_att );
  _state.disableUnusedAttributes();

  _state.disable( GL_CULL_FACE );
  _state.enable( GL_BLEND );

  _r.glBindBuffer( GL_ARRAY_BUFFER, _vertexBuffer );
  _r.glVertexAttribPointer( _data->position_att, 2, GL_FLOAT, GL_FALSE, 2 * 8, (const void *)0 );
  _r.glVertexAttribPointer( _data->uv_att, 2, GL_FLOAT, GL_FALSE, 2 * 8, (const void *)8 );

  _r.glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _elementBuffer );

  _r.glUniformMatrix4fv(_data->projectionMatrix, sizeof(camera->projectionMatrix()), GL_FALSE,
                          camera->projectionMatrix().elements());

  _state.activeTexture( GL_TEXTURE0 );
  _r.glUniform1i( _data->map, 0 );

  GLint oldFogType = 0;
  GLint sceneFogType = 0;
  Fog::Ptr fog = scene->fog();

  if ( fog ) {

    _r.glUniform3f( _data->fogColor, fog->color().r, fog->color().g, fog->color().b );

    fog::Dispatch dispatch;
    dispatch.func<DefaultFog>() = [&](DefaultFog &df) {

      _r.glUniform1f( _data->fogNear, df.near() );
      _r.glUniform1f( _data->fogFar, df.far() );

      _r.glUniform1i( _data->fogType, 1 );
      oldFogType = 1;
      sceneFogType = 1;
    };
    dispatch.func<FogExp2>() = [&] (FogExp2 &fe) {
      _r.glUniform1f( _data->fogDensity, fe.density() );

      _r.glUniform1i( _data->fogType, 2 );
      oldFogType = 2;
      sceneFogType = 2;
    };
  }
  else {

    _r.glUniform1i( _data->fogType, 0 );
    oldFogType = 0;
    sceneFogType = 0;

  }


  // update positions and sort

  for (const Sprite::Ptr sprite : sprites) {

    sprite->modelViewMatrix = camera->matrixWorldInverse() * sprite->matrixWorld();
    //sprite->z() = - sprite.modelViewMatrix.elements[ 14 ];
  }

  sort(sprites.begin(), sprites.end(), [] (const Sprite::Ptr &a, const Sprite::Ptr &b) -> bool {
    if ( a->renderOrder() != b->renderOrder()) {

      return a->renderOrder() < b->renderOrder();

    } else {
      float za = a->modelViewMatrix.elements()[ 14 ];
      float zb = b->modelViewMatrix.elements()[ 14 ];

      if (za != zb)
        return zb < za;
      else
        return b->id() < a->id();
    }
  });

  // render all sprites

  float scale[2];

  for (Sprite::Ptr sprite : sprites) {

    SpriteMaterial::Ptr material = sprite->material<0>();

    if (!material->visible) continue;

    sprite->onBeforeRender.emitSignal(_r, scene, camera, nullptr, material, nullptr);

    _r.glUniform1f( _data->alphaTest, material->alphaTest );
    _r.glUniformMatrix4fv(_data->modelViewMatrix, 1, GL_FALSE, sprite->modelViewMatrix.elements() );

    sprite->matrixWorld().decompose( _spritePosition, _spriteRotation, _spriteScale );

    scale[ 0 ] = _spriteScale.x();
    scale[ 1 ] = _spriteScale.y();

    GLint fogType = scene->fog() && material->fog ? sceneFogType : 0;

    if ( oldFogType != fogType ) {

      _r.glUniform1i( _data->fogType, fogType );
      oldFogType = fogType;
    }

    if (material->map()) {

      _r.glUniform2f( _data->uvOffset, material->map()->offset().x(), material->map()->offset().y());
      _r.glUniform2f( _data->uvScale, material->map()->repeat().x(), material->map()->repeat().y());

    } else {

      _r.glUniform2f( _data->uvOffset, 0, 0 );
      _r.glUniform2f( _data->uvScale, 1, 1 );

    }

    _r.glUniform1f( _data->opacity, material->opacity);
    _r.glUniform3f( _data->color, material->color().r, material->color().g, material->color().b );

    _r.glUniform1f( _data->rotation, material->rotation() );
    _r.glUniform2fv(_data->scale, 1, scale);

    _state.setBlending( material->blending, material->blendEquation, material->blendSrc, material->blendDst,
                        material->blendEquationAlpha, material->blendSrcAlpha, material->blendDstAlpha,
                        material->premultipliedAlpha );
    _state.depthBuffer.setTest( material->depthTest );
    _state.depthBuffer.setMask( material->depthWrite );
    _state.colorBuffer.setMask( material->colorWrite );

    if(material->map())
      _textures.setTexture2D(material->map(), 0);
    else if(_texture)
    _textures.setTexture2D( _texture, 0 );

    _r.glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

    sprite->onAfterRender.emitSignal( _r, scene, camera, nullptr, material, nullptr );
  }

  // restore gl
  _state.enable( GL_CULL_FACE );
  _state.reset();
}

}
}