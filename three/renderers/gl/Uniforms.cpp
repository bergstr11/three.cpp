//
// Created by byter on 12.10.17.
//

#include "Uniforms.h"
#include "shader/UniformsLib.h"
#include "Renderer_impl.h"
#include <regex>

namespace three {
namespace gl {

using namespace std;

Uniforms::Uniforms(Renderer_impl &renderer, GLuint program) : _renderer(renderer)
{
  GLint numUniforms;
  renderer.glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

  for (unsigned i = 0; i < numUniforms; ++i) {
    parseUniform(program, i, this);
  }
}

std::vector<int32_t> &Uniforms::allocTexUnits(Renderer_impl &renderer, size_t n)
{
  if(arrayCacheI32.find(n) == arrayCacheI32.end()) {
    arrayCacheI32.emplace(n, std::vector<int32_t>(n));
    arrayCacheI32[n].resize(n);
  }
  auto &r = arrayCacheI32[ n ];

  for ( size_t i = 0; i < n; ++i)
    r[i] = renderer.allocTextureUnit();

  return r;
}

std::vector<Uniform::Ptr> Uniforms::sequenceUniforms(const UniformValues &values)
{
  std::vector<Uniform::Ptr> result;
  for(auto name : _sequence) {

    if(values.contains(name)) {
      result.push_back(_map[name]);
    }
  }
  return result;
}

#define MATCH_NAME(nm) {#nm, UniformName::nm}

UniformName toUniformName(string name, bool isIndex)
{
  static std::unordered_map<string, UniformName> string_to_name {
     MATCH_NAME(tCube),
     MATCH_NAME(tEquirect),
     MATCH_NAME(tFlip),
     MATCH_NAME(opacity),
     MATCH_NAME(diffuse),
     MATCH_NAME(emissive),
     MATCH_NAME(specular),
     MATCH_NAME(shininess),
     MATCH_NAME(projectionMatrix),
     MATCH_NAME(viewMatrix),
     MATCH_NAME(modelViewMatrix),
     MATCH_NAME(normalMatrix),
     MATCH_NAME(modelMatrix),
     MATCH_NAME(logDepthBufFC),
     MATCH_NAME(boneMatrices),
     MATCH_NAME(bindMatrix),
     MATCH_NAME(bindMatrixInverse),
     MATCH_NAME(toneMappingExposure),
     MATCH_NAME(toneMappingWhitePoint),
     MATCH_NAME(cameraPosition),
     MATCH_NAME(map),
     MATCH_NAME(uvTransform),
     MATCH_NAME(alphaMap),
     MATCH_NAME(specularMap),
     MATCH_NAME(envMap),
     MATCH_NAME(flipEnvMap),
     MATCH_NAME(reflectivity),
     MATCH_NAME(refractionRatio),
     MATCH_NAME(aoMap),
     MATCH_NAME(aoMapIntensity),
     MATCH_NAME(lightMap),
     MATCH_NAME(lightMapIntensity),
     MATCH_NAME(emissiveMap),
     MATCH_NAME(bumpMap),
     MATCH_NAME(bumpScale),
     MATCH_NAME(normalMap),
     MATCH_NAME(normalScale),
     MATCH_NAME(displacementMap),
     MATCH_NAME(displacementScale),
     MATCH_NAME(displacementBias),
     MATCH_NAME(roughnessMap),
     MATCH_NAME(metalnessMap),
     MATCH_NAME(gradientMap),
     MATCH_NAME(roughness),
     MATCH_NAME(metalness),
     MATCH_NAME(clearCoat),
     MATCH_NAME(clearCoatRoughness),
     MATCH_NAME(envMapIntensity),
     MATCH_NAME(fogDensity),
     MATCH_NAME(fogNear),
     MATCH_NAME(fogFar),
     MATCH_NAME(fogColor),
     MATCH_NAME(ambientLightColor),
     MATCH_NAME(direction),
     MATCH_NAME(color),
     MATCH_NAME(shadow),
     MATCH_NAME(shadowBias),
     MATCH_NAME(shadowRadius),
     MATCH_NAME(shadowMapSize),
     MATCH_NAME(size),
     MATCH_NAME(scale),
     MATCH_NAME(dashSize),
     MATCH_NAME(totalSize),
     MATCH_NAME(referencePosition),
     MATCH_NAME(nearDistance),
     MATCH_NAME(farDistance),
     MATCH_NAME(clippingPlanes),
     MATCH_NAME(directionalLights),
     MATCH_NAME(spotLights),
     MATCH_NAME(rectAreaLights),
     MATCH_NAME(pointLights),
     MATCH_NAME(hemisphereLights),
     MATCH_NAME(directionalShadowMap),
     MATCH_NAME(directionalShadowMatrix),
     MATCH_NAME(spotShadowMap),
     MATCH_NAME(spotShadowMatrix),
     MATCH_NAME(pointShadowMap),
     MATCH_NAME(pointShadowMatrix),
     MATCH_NAME(distance),
     MATCH_NAME(position),
     MATCH_NAME(coneCos),
     MATCH_NAME(penumbraCos),
     MATCH_NAME(decay)
  };
  if(isIndex) {
    unsigned index = stoi(name);
    if((unsigned)UniformName::index_15 >= index) {
      return (UniformName)((unsigned)UniformName::index_0 + index);
    }
    throw std::invalid_argument(std::string("unsupported index ")+name);
  }
  try {
    return string_to_name.at(name);
  }
  catch(const out_of_range &r) {
    throw std::invalid_argument(std::string("unknown variable ")+name);
  }
}

void Uniforms::parseUniform(GLuint program, unsigned index, UniformContainer *container)
{
  static regex rex(R"(([\w\d_]+)(\])?(\[|\.)?)");

  GLchar uname[100];
  GLsizei length;
  GLint size;
  GLenum type;

  _renderer.glGetActiveUniform( program, index, 100, &length, &size, &type, uname);
  GLint addr = _renderer.glGetUniformLocation(program, uname);

  string name(uname);
  sregex_iterator rex_it(name.cbegin(), name.cend(), rex);
  sregex_iterator rex_end;

  while(rex_it != rex_end) {
    smatch match = *rex_it;

    bool isIndex = match[2] == "]";
    string subscript = match[3];

    UniformName id = toUniformName(match[1], isIndex);

    if(!match[3].matched || subscript == "[" && match[3].second == name.end()) {
      // bare name or "pure" bottom-level array "[0]" suffix
      container->add(match[3].matched ?
                     ArrayUniform::make(_renderer, id, (UniformType)type, addr) :
                     Uniform::make(_renderer, id, (UniformType)type, addr));
    }
    else {
      // step into inner node / create it in case it doesn't exist
      if(container->_map.count(id) == 0) {
        StructuredUniform::Ptr next = StructuredUniform::make(_renderer, id, (UniformType)type, addr);
        container->add(next);
      }
      container = container->_map[id]->asContainer();
      assert(container != nullptr);
    }
    rex_it++;
  }
}

void Uniform::setValue(GLfloat v) {
  _renderer.glUniform1f( _addr, v );
  check_glerror(&_renderer);
}

void Uniform::setValue(GLint v) {
  _renderer.glUniform1i( _addr, v );
  check_glerror(&_renderer);
}

void Uniform::setValue(GLuint v) {
  _renderer.glUniform1i( _addr, v );
  check_glerror(&_renderer);
}

void Uniform::setValue(const three::Color &c) {
  _renderer.glUniform3f(_addr, c.r, c.g, c.b);
  check_glerror(&_renderer);
}

void Uniform::setValue(const math::Vector2 &v) {
  _renderer.glUniform2fv(_addr, 1, v.elements());
  check_glerror(&_renderer);
}

void Uniform::setValue(const math::Vector3 &v) {
  _renderer.glUniform3fv(_addr, 1, v.elements());
  check_glerror(&_renderer);
}

void Uniform::setValue(const math::Vector4 &v) {
  _renderer.glUniform4fv(_addr, 1, v.elements());
  check_glerror(&_renderer);
}

void Uniform::setValue(const math::Matrix3 &v) {
  _renderer.glUniformMatrix3fv( _addr, 1, GL_FALSE, v.elements());
  check_glerror(&_renderer);
}

void Uniform::setValue(const math::Matrix4 &v) {
  _renderer.glUniformMatrix4fv( _addr, 1, GL_FALSE, v.elements());
  check_glerror(&_renderer);
}

void Uniform::setValue(const GLint * array, size_t size) {
  _renderer.glUniform2iv(_addr, size, array);
  check_glerror(&_renderer);
}

void Uniform::setValue(const std::vector<math::Matrix4> &matrices)
{
  auto *data = new float[matrices.size() * sizeof(math::Matrix4)];

  unsigned offset = 0;
  for(const auto &mat : matrices) {
    mat.writeTo(data, offset);
    offset += sizeof(math::Matrix4);
  }
  _renderer.glUniformMatrix4fv( _addr, matrices.size(), GL_FALSE, data);
  delete[] data;
  check_glerror(&_renderer);
}

void Uniform::setValue(const std::vector<float> &vector)
{
  _renderer.glUniform1fv(_addr, vector.size(), vector.data());
  check_glerror(&_renderer);
}

void Uniform::setValue(const std::vector<Texture::Ptr> &textures)
{
  vector<GLint> units = _renderer.allocTextureUnits(textures.size());

  _renderer.glUniform1iv(_addr, textures.size(), units.data());

  for (size_t i = 0; i < textures.size(); ++ i ) {

    _renderer.setTexture2D( textures[i], units[ i ] );
  }
  check_glerror(&_renderer);
}

void Uniform::setValue(const Texture::Ptr &texture)
{
  unsigned unit = _renderer.allocTextureUnit();
  _renderer.glUniform1i( _addr, unit );
  _renderer.setTexture2D(texture, unit );
  check_glerror(&_renderer);
}

void Uniform::setValue(const CubeTexture::Ptr &texture)
{
  unsigned unit = _renderer.allocTextureUnit();
  _renderer.glUniform1i( _addr, unit );
  _renderer.setTextureCube(texture, unit );
  check_glerror(&_renderer);
}

}
}
