//
// Created by byter on 22.10.17.
//

#include "FlareRenderer.h"
#include <math/Box2.h>
#include <QByteArray>
#include <array>
#include <sstream>

namespace three {
namespace gl {

using namespace std;
using namespace math;

struct FlareRenderer::Data
{
  GLuint vertex_att;
  GLuint uv_att;

  GLint renderType;
  GLint map;
  GLint occlusionMap;
  GLint opacity;
  GLint color;
  GLint scale;
  GLint rotation;
  GLint screenPosition;

  Data(QOpenGLFunctions *f, GLuint program) {
    vertex_att = f->glGetAttribLocation(program, "position" );
    uv_att = f->glGetAttribLocation( program, "uv" );
    renderType = f->glGetUniformLocation(program, "renderType" ),
       map = f->glGetUniformLocation( program, "map" );
    occlusionMap = f->glGetUniformLocation( program, "occlusionMap" );
    opacity = f->glGetUniformLocation( program, "opacity" );
    color = f->glGetUniformLocation( program, "color" );
    scale = f->glGetUniformLocation( program, "scale" );
    rotation = f->glGetUniformLocation( program, "rotation" );
    screenPosition = f->glGetUniformLocation( program, "screenPosition" );
  }
};

FlareRenderer::~FlareRenderer()
{
  if(_data) delete _data;
}

void FlareRenderer::init()
{
  array<float, 16> vertices = {
     -1.0f, -1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
     1.0f, 1.0f, 1.0f, 1.0f,
     -1.0f, 1.0f, 0.0f, 1.0f
  };

  array<uint16_t, 6> faces = {
     0, 1, 2,
     0, 2, 3
  };

  // buffers

  _fn->glGenBuffers(1, &_vertexBuffer);
  _fn->glGenBuffers(1, &_elementBuffer);

  _fn->glBindBuffer( GL_ARRAY_BUFFER, _vertexBuffer );
  _fn->glBufferData( GL_ARRAY_BUFFER, 16 * sizeof(float), vertices.data(), GL_STATIC_DRAW );

  _fn->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _elementBuffer );
  _fn->glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(float), faces.data(), GL_STATIC_DRAW );

  // textures
  _fn->glGenTextures(1, &_tempTexture);
  _fn->glGenTextures(1, &_occlusionTexture);

  _state.bindTexture( GL_TEXTURE_2D, _tempTexture );
  _fn->glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 16, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  _fn->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  _fn->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  _fn->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  _fn->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  _state.bindTexture( GL_TEXTURE_2D, _occlusionTexture );
  _fn->glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  _fn->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  _fn->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  _fn->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  _fn->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  static const char * vertexShader =
     "'precision {precision} float;\n"

     "uniform lowp int renderType;\n"
  
     "uniform vec3 screenPosition;\n"
     "uniform vec2 scale;\n"
     "uniform float rotation;\n"

     "uniform sampler2D occlusionMap;\n"

     "attribute vec2 position;\n"
     "attribute vec2 uv;\n"

     "varying vec2 vUV;\n"
     "varying float vVisibility;\n"

     "void main() {\n"

     "	vUV = uv;\n"

     "	vec2 pos = position;\n"

     "	if ( renderType == 2 ) {\n"

     "		vec4 visibility = texture2D( occlusionMap, vec2( 0.1, 0.1 ) );\n"
     "		visibility += texture2D( occlusionMap, vec2( 0.5, 0.1 ) );\n"
     "		visibility += texture2D( occlusionMap, vec2( 0.9, 0.1 ) );\n"
     "		visibility += texture2D( occlusionMap, vec2( 0.9, 0.5 ) );\n"
     "		visibility += texture2D( occlusionMap, vec2( 0.9, 0.9 ) );\n"
     "		visibility += texture2D( occlusionMap, vec2( 0.5, 0.9 ) );\n"
     "		visibility += texture2D( occlusionMap, vec2( 0.1, 0.9 ) );\n"
     "		visibility += texture2D( occlusionMap, vec2( 0.1, 0.5 ) );\n"
     "		visibility += texture2D( occlusionMap, vec2( 0.5, 0.5 ) );\n"

     "		vVisibility =        visibility.r / 9.0;\n"
     "		vVisibility *= 1.0 - visibility.g / 9.0;\n"
     "		vVisibility *=       visibility.b / 9.0;\n"
     "		vVisibility *= 1.0 - visibility.a / 9.0;\n"

     "		pos.x = cos( rotation ) * position.x - sin( rotation ) * position.y;\n"
     "		pos.y = sin( rotation ) * position.x + cos( rotation ) * position.y;\n"

     "	}\n"

     "	gl_Position = vec4( ( pos * scale + screenPosition.xy ).xy, screenPosition.z, 1.0 );\n"

     "}";
  
  static const char * fragmentShader =
     "uniform lowp int renderType;\n"

     "uniform sampler2D map;\n"
     "uniform float opacity;\n"
     "uniform vec3 color;\n"

     "varying vec2 vUV;\n"
     "varying float vVisibility;\n"

     "void main() {\n"

     // pink square

     "	if ( renderType == 0 ) {\n"

     "		gl_FragColor = vec4( 1.0, 0.0, 1.0, 0.0 );\n"

     // restore

     "	} else if ( renderType == 1 ) {\n"

     "		gl_FragColor = texture2D( map, vUV );\n"

     // flare

     "	} else {\n"

     "		vec4 texture = texture2D( map, vUV );\n"
     "		texture.a *= opacity * vVisibility;\n"
     "		gl_FragColor = texture;\n"
     "		gl_FragColor.rgb *= color;\n"

     "	}\n"

     "}";

  _program = _fn->glCreateProgram();

  GLuint fshader = _fn->glCreateShader( GL_FRAGMENT_SHADER );
  GLuint vshader = _fn->glCreateShader( GL_VERTEX_SHADER );

  stringstream ss;
  ss << "precision " << _capabilities.precisionS() << " float;" << endl << vertexShader;
  const char *vsource = ss.str().data();
  _fn->glShaderSource(vshader, 1, &vsource, nullptr);

  ss << "precision " << _capabilities.precisionS() << " float;" << endl << fragmentShader;
  const char *fsource = ss.str().data();
  _fn->glShaderSource(fshader, 1, &fsource, nullptr);

  _fn->glCompileShader(fshader);
  _fn->glCompileShader(vshader);

  _fn->glAttachShader( _program, fshader );
  _fn->glAttachShader( _program, vshader );

  _fn->glLinkProgram( _program );

  _data = new Data(_fn, _program);
}

void FlareRenderer::render(std::vector<LensFlare::Ptr> &flares,
                           Scene::Ptr scene, Camera::Ptr camera, const Vector4 &viewport)
{
  if (flares.empty()) return;

  Vector3 tempPosition;

  float invAspect = viewport.w() / viewport.z();
  float halfViewportWidth = viewport.z() * 0.5f;
  float halfViewportHeight = viewport.w() * 0.5f;

  float size = 16 / viewport.w();
  Vector2 scale(size * invAspect, size);

  Vector3 screenPosition( 1, 1, 0 );
  Vector2 screenPositionPixels( 1, 1 );

  Box2 validArea(
     {viewport.x(), viewport.y()},
     {viewport.x() + (viewport.z() - 16), viewport.y() + (viewport.w() - 16)});

  if (!_linked) {
    init();
    _linked = true;
  }

  _state.useProgram(_program);

  _state.initAttributes();
  _state.enableAttribute( _data->vertex_att );
  _state.enableAttribute( _data->uv_att );
  _state.disableUnusedAttributes();

  // loop through all lens flares to update their occlusion and positions
  // setup gl and common used attribs/uniforms

  _fn->glUniform1i( _data->occlusionMap, 0 );
  _fn->glUniform1i( _data->map, 1 );

  _fn->glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer );
  _fn->glVertexAttribPointer( _data->vertex_att, 2, GL_FLOAT, false, 2 * 8, (const void *)0 );
  _fn->glVertexAttribPointer( _data->uv_att, 2, GL_FLOAT, false, 2 * 8, (const void *)8 );

  _fn->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBuffer );

  _state.disable( GL_CULL_FACE );
  _state.depthBuffer.setMask( false );

  for (LensFlare::Ptr flare : flares) {

    size = 16 / viewport.w();
    scale.set( size * invAspect, size );

    // calc object screen position

    tempPosition.set( flare->matrixWorld().elements()[ 12 ],
                      flare->matrixWorld().elements()[ 13 ],
                      flare->matrixWorld().elements()[ 14 ] );

    tempPosition.apply( camera->matrixWorldInverse() );
    tempPosition.apply( camera->projectionMatrix() );

    // setup arrays for gl programs

    screenPosition = tempPosition;

    // horizontal and vertical coordinate of the lower left corner of the pixels to copy

    screenPositionPixels.x() = viewport.x() + ( screenPosition.x() * halfViewportWidth ) + halfViewportWidth - 8;
    screenPositionPixels.y() = viewport.y() + ( screenPosition.y() * halfViewportHeight ) + halfViewportHeight - 8;

    // screen cull

    if ( validArea.containsPoint( screenPositionPixels )) {

      // save current RGB to temp texture

      _state.activeTexture( GL_TEXTURE0 );
      _state.bindTexture( GL_TEXTURE_2D );
      _state.activeTexture( GL_TEXTURE1 );
      _state.bindTexture( GL_TEXTURE_2D, _tempTexture );
      _fn->glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, screenPositionPixels.x(), screenPositionPixels.y(), 16, 16, 0 );


      // render pink quad

      _fn->glUniform1i( _data->renderType, 0 );
      _fn->glUniform2f( _data->scale, scale.x(), scale.y() );
      _fn->glUniform3f( _data->screenPosition, screenPosition.x(), screenPosition.y(), screenPosition.z() );

      _state.disable( GL_BLEND );
      _state.enable( GL_DEPTH_TEST );

      _fn->glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );


      // copy result to occlusionMap

      _state.activeTexture( GL_TEXTURE0 );
      _state.bindTexture( GL_TEXTURE_2D, _occlusionTexture );
      _fn->glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, screenPositionPixels.x(), screenPositionPixels.y(), 16, 16, 0 );


      // restore graphics

      _fn->glUniform1i( _data->renderType, 1 );
      _state.disable( GL_DEPTH_TEST );

      _state.activeTexture( GL_TEXTURE1 );
      _state.bindTexture( GL_TEXTURE_2D, _tempTexture );
      _fn->glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );


      // update object positions

      flare->setScreenPosition(screenPosition);

      // render flares

      _fn->glUniform1i( _data->renderType, 2 );
      _state.enable( GL_BLEND );

      for (const LensFlare::Flare &sprite : flare->flares()) {

        if ( sprite.opacity > 0.001 && sprite.scale > 0.001 ) {

          screenPosition.set(sprite.x, sprite.y, sprite.z);

          size = sprite.size * sprite.scale / viewport.w();

          scale.set(size * invAspect, size);

          _fn->glUniform3f( _data->screenPosition, screenPosition.x(), screenPosition.y(), screenPosition.z());
          _fn->glUniform2f( _data->scale, scale.x(), scale.y());
          _fn->glUniform1f( _data->rotation, sprite.rotation );

          _fn->glUniform1f( _data->opacity, sprite.opacity );
          _fn->glUniform3f( _data->color, sprite.color.r, sprite.color.g, sprite.color.b );

          _state.setBlending(sprite.blending);

          _textures.setTexture2D(sprite.texture, 1);

          _fn->glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

        }

      }

    }

  }

  // restore gl

  _state.enable( GL_CULL_FACE );
  _state.enable( GL_DEPTH_TEST );
  _state.depthBuffer.setMask( true );

  _state.reset();
}

}
}
