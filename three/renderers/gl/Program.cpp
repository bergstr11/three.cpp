//
// Created by byter on 01.10.17.
//

#include <iostream>
#include <sstream>
#include <regex>
#include <helper/utils.h>
#include "Program.h"
#include "Renderer_impl.h"
#include "shader/ShaderChunk.h"

namespace three {
namespace gl {

unsigned Program::programIdCount = 0;

using namespace std;

vector<string> getEncodingComponents(Encoding encoding)
{
  switch (encoding) {

    case Encoding::Linear:
      return {"Linear", "( value )"};
    case Encoding::sRGB:
      return {"sRGB", "( value )"};
    case Encoding::RGBE:
      return {"RGBE", "( value )"};
    case Encoding::RGBM7:
      return {"RGBM", "( value, 7.0 )"};
    case Encoding::RGBM16:
      return {"RGBM", "( value, 16.0 )"};
    case Encoding::RGBD:
      return {"RGBD", "( value, 256.0 )"};
    case Encoding::Gamma:
      return {"Gamma", "( value, float( GAMMA_FACTOR ) )"};
    default:
      throw invalid_argument(string("unsupported encoding"));
  }
}

string getTexelDecodingFunction(const char *functionName, Encoding encoding)
{
  vector<string> components = getEncodingComponents(encoding);
  stringstream ss;
  ss << "vec4 " << functionName << "( vec4 value ) { return " << components[0] << "ToLinear"
     << components[1] << "; }";
  return ss.str();
}

string getTexelEncodingFunction(const char *functionName, Encoding encoding)
{
  vector<string> components = getEncodingComponents(encoding);
  stringstream ss;
  ss << "vec4 " << functionName << "( vec4 value ) { return LinearTo" << components[0]
     << components[1] << "; }";
  return ss.str();
}

string getToneMappingFunction(const char *functionName, ToneMapping toneMapping)
{
  string toneMappingName;

  switch (toneMapping) {
    case ToneMapping::Linear:
      toneMappingName = "Linear";
      break;

    case ToneMapping::Reinhard:
      toneMappingName = "Reinhard";
      break;

    case ToneMapping::Uncharted2:
      toneMappingName = "Uncharted2";
      break;

    case ToneMapping::Cineon:
      toneMappingName = "OptimizedCineon";
      break;

    default:
      throw invalid_argument(string("unsupported tonemapping"));
  }

  stringstream ss;
  ss << "vec3 " << functionName << "( vec3 color ) { return " << toneMappingName << "ToneMapping( color ); }";
  return ss.str();
}

string
generateExtensions(Extensions &extensions, const ProgramParameters &parameters)
{
  stringstream ss;
  if (extensions.get(Extension::OES_standard_derivatives) || parameters.envMapCubeUV || parameters.bumpMap || parameters.normalMap || parameters.flatShading)
    ss << "#extension GL_OES_standard_derivatives : enable" << endl;
  if ((extensions.get(Extension::EXT_frag_depth) || parameters.logarithmicDepthBuffer) && parameters.extensions.get(Extension::EXT_frag_depth))
    ss << "#extension GL_EXT_frag_depth : enable" << endl;
  if (extensions.get(Extension::GL_EXT_draw_buffers) && parameters.extensions.get(Extension::GL_EXT_draw_buffers))
    ss << "#extension GL_EXT_draw_buffers : require" << endl;
  if ((extensions.get(Extension::EXT_shader_texture_lod) || parameters.envMap) && parameters.extensions.get(Extension::EXT_shader_texture_lod))
    ss << "#extension GL_EXT_shader_texture_lod : enable" << endl;

  return ss.str();

}

string generateDefines(unordered_map<string, string> defines)
{
  stringstream ss;

  for(const auto &pair : defines) {

    if (!pair.second.empty())
      ss << "#define " << pair.first << " " << pair.second << endl;
  }

  return ss.str();
}

struct AttribInfo
{
  GLsizei length;
  GLint size;
  GLenum type;
  GLchar name[101];
};

unordered_map<string, GLint> Program::fetchAttributeLocations()
{

  unordered_map<string, GLint> attributes;

  GLint numActive;
  _renderer.glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &numActive);

  AttribInfo info;
  for (unsigned i = 0; i < numActive; i++) {

    _renderer.glGetActiveAttrib(_program, i, 100, &info.length, &info.size, &info.type, info.name);
    info.name[info.length] = 0;
    string name(info.name);

    // console.log("THREE.WebGLProgram: ACTIVE VERTEX ATTRIBUTE:", name, i );
    attributes[name] = _renderer.glGetAttribLocation(_program, info.name);
  }

  return attributes;
}

string replaceLightNums(string value, const ProgramParameters &parameters)
{
  return replace_all(value, {{"NUM_DIR_LIGHTS",       to_string(parameters.numDirLights)},
                             {"NUM_SPOT_LIGHTS",      to_string(parameters.numSpotLights)},
                             {"NUM_RECT_AREA_LIGHTS", to_string(parameters.numRectAreaLights)},
                             {"NUM_POINT_LIGHTS",     to_string(parameters.numPointLights)},
                             {"NUM_HEMI_LIGHTS",      to_string(parameters.numHemiLights)}});
}

string parseIncludes(string lookat)
{
  static regex rex("[ \r\n\t]*#include +<([\\w\\d.]+)>");

  sregex_iterator rex_it(lookat.begin(), lookat.end(), rex);
  sregex_iterator rex_end;

  while(rex_it != rex_end) {
    ssub_match sub = (*rex_it)[1];
    string r = getShaderChunk(sub.str());
    if(r.empty()) {
      stringstream ss;
      ss << "unable to resolve #include <" << sub.str() << ">";
      throw logic_error(ss.str());
    }
    lookat.replace(sub.first, sub.second, r);

    rex_it = sregex_iterator(sub.first+r.length(), lookat.end(), rex);
  }
  return lookat;
}

string unrollLoops(string loops)
{
  static regex rex(R"(for \( int i = (\d+)\; i < (\d+); i \+\+ \) \{[\r\n]?([\s\S]+?)(?=\})\})");
  static regex rex2(R"(\[ i \])");

  stringstream unroll;
  sregex_iterator rex_it(loops.begin(), loops.end(), rex);
  sregex_iterator rex_end;

  while(rex_it != rex_end) {
    int start = stoi((*rex_it)[1].str());
    int end = stoi((*rex_it)[2].str());
    ssub_match snippet = (*rex_it)[3];

    for(int i=start; i<end; i++) {
      stringstream ss2;
      ss2 << "[ " << i << " ]";
      unroll << regex_replace(snippet.str(), rex2, ss2.str());
    }
    rex_it++;
  }

  return unroll.str();
}

enum class InfoObject {program, shader};
string getInfoLog(QOpenGLFunctions *f, InfoObject obj, GLuint handle)
{
  GLint length = 0;
  if(obj == InfoObject::shader) {
    f->glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
  }
  else {
    f->glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);
  }
  if(length == 0) return "";

  string info;
  info.resize(length);

  if(obj == InfoObject::shader)
    f->glGetShaderInfoLog(handle, length, nullptr, &info.front());
  else
    f->glGetProgramInfoLog(handle, length, nullptr, &info.front());

  return info;
}

GLuint createShader(QOpenGLFunctions *f, GLenum type, string glsl)
{
  GLuint shader = f->glCreateShader( type );

  const char * source = glsl.data();
  f->glShaderSource( shader, 1, &source, nullptr);
  f->glCompileShader( shader );

  GLint value;
  f->glGetShaderiv( shader, GL_COMPILE_STATUS, &value);

  string info = getInfoLog(f, InfoObject::shader, shader);
  if(!info.empty()) {
    cout << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader compilation result: " << info<< endl;
  }

  if(value != GL_TRUE) {
    throw logic_error("GLSL compile error");
  }

  // --enable-privileged-webgl-extension
  // console.log( type, gl.getExtension( 'WEBGL_debug_shaders' ).getTranslatedShaderSource( shader ) );

  return shader;

}

Program::Program(Renderer_impl &renderer,
                 Extensions &extensions,
                 const std::string code,
                 const Material::Ptr material,
                 Shader &shader,
                 const ProgramParameters &parameters )
   : _renderer(renderer), _cachedAttributes(&renderer)
{
  //var gl = renderer.context;
  //var defines = material.defines;

  //var vertexShader = shader.vertexShader;
  //var fragmentShader = shader.fragmentShader;

  const char *shadowMapTypeDefine;

  switch(parameters.shadowMapType) {
    case ShadowMapType::PCF:
      shadowMapTypeDefine = "SHADOWMAP_TYPE_PCF";
      break;
    case ShadowMapType::PCFSoft:
      shadowMapTypeDefine = "SHADOWMAP_TYPE_PCF_SOFT";
      break;
    default:
      shadowMapTypeDefine = "SHADOWMAP_TYPE_BASIC";
      break;
  }

  const char *envMapTypeDefine = "ENVMAP_TYPE_CUBE";
  const char *envMapModeDefine = "ENVMAP_MODE_REFLECTION";
  const char *envMapBlendingDefine = "ENVMAP_BLENDING_MULTIPLY";

  if ( parameters.envMap ) {
    switch ( parameters.envMap->mapping() ) {

      case TextureMapping::CubeReflection:
      case TextureMapping::CubeRefraction:
        envMapTypeDefine = "ENVMAP_TYPE_CUBE";
        break;

      case TextureMapping::CubeUVReflection:
      case TextureMapping::CubeUVRefraction:
        envMapTypeDefine = "ENVMAP_TYPE_CUBE_UV";
        break;

      case TextureMapping::EquirectangularReflection:
      case TextureMapping::EquirectangularRefraction:
        envMapTypeDefine = "ENVMAP_TYPE_EQUIREC";
        break;

      case TextureMapping::SphericalReflection:
        envMapTypeDefine = "ENVMAP_TYPE_SPHERE";
        break;

      default: break;
    }

    switch( parameters.envMap->mapping() ) {

      case TextureMapping::CubeRefraction:
      case TextureMapping::EquirectangularRefraction:
        envMapModeDefine = "ENVMAP_MODE_REFRACTION";
        break;

      default: break;
    }

    switch ( parameters.combine ) {

      case CombineOperation::Multiply:
        envMapBlendingDefine = "ENVMAP_BLENDING_MULTIPLY";
        break;

      case CombineOperation::Mix:
        envMapBlendingDefine = "ENVMAP_BLENDING_MIX";
        break;

      case CombineOperation::Add:
        envMapBlendingDefine = "ENVMAP_BLENDING_ADD";
        break;
    }
  }

  float gammaFactorDefine = renderer._gammaFactor > 0 ? renderer._gammaFactor : 1.0;

  string customExtensions = generateExtensions(extensions, parameters);

  string customDefines = generateDefines( parameters.defines );

  //

  GLuint program = _renderer.glCreateProgram();

  string prefixVertex, prefixFragment;

  if(parameters.rawShaderMaterial) {

    prefixVertex =  customDefines;
    prefixFragment = customExtensions + customDefines;

  } else {
    stringstream ss;
    ss << "precision " << parameters.precision << " float;" << endl;
    ss << "precision " << parameters.precision << " int;" << endl;
    ss << "#define SHADER_NAME " << shader.name() << endl;
    ss << customDefines;
    if(parameters.supportsVertexTextures) ss << "#define VERTEX_TEXTURES" << endl;
    ss << "#define GAMMA_FACTOR " << gammaFactorDefine << endl;
    ss << "#define MAX_BONES " << parameters.maxBones << endl;
    if( parameters.useFog && parameters.fog ) ss << "#define USE_FOG" << endl;
    if( parameters.useFog && parameters.fogExp ) ss << "#define FOG_EXP2" << endl;
    if(parameters.map) ss << "#define USE_MAP" << endl;
    if(parameters.envMap) ss << "#define USE_ENVMAP" << endl;
    if(parameters.envMap) ss << "#define " << envMapModeDefine << endl;
    if(parameters.lightMap) ss << "#define USE_LIGHTMAP" << endl;
    if(parameters.aoMap) ss << "#define USE_AOMAP" << endl;
    if(parameters.emissiveMap) ss << "#define USE_EMISSIVEMAP" << endl;
    if(parameters.bumpMap) ss << "#define USE_BUMPMAP" << endl;
    if(parameters.normalMap) ss << "#define USE_NORMALMAP" << endl;
    if(parameters.displacementMap && parameters.supportsVertexTextures) ss << "#define USE_DISPLACEMENTMAP" << endl;
    if(parameters.specularMap) ss << "#define USE_SPECULARMAP" << endl;
    if(parameters.roughnessMap) ss << "#define USE_ROUGHNESSMAP" << endl;
    if(parameters.metalnessMap) ss << "#define USE_METALNESSMAP" << endl;
    if(parameters.alphaMap) ss << "#define USE_ALPHAMAP" << endl;
    if(parameters.vertexColors != Colors::None) ss << "#define USE_COLOR" << endl;

    if(parameters.flatShading) ss << "#define FLAT_SHADED" << endl;

    if(parameters.skinning) ss << "#define USE_SKINNING" << endl;
    if(parameters.useVertexTexture) ss << "#define BONE_TEXTURE" << endl;

    if(parameters.morphTargets) ss << "#define USE_MORPHTARGETS" << endl;
    if(parameters.morphNormals && !parameters.flatShading) ss << "#define USE_MORPHNORMALS" << endl;
    if(parameters.doubleSided) ss << "#define DOUBLE_SIDED" << endl;
    if(parameters.flipSided) ss << "#define FLIP_SIDED" << endl;

    ss << "#define NUM_CLIPPING_PLANES " << parameters.numClippingPlanes << endl;

    if(parameters.shadowMapEnabled) ss << "#define USE_SHADOWMAP" << endl;
    if(parameters.shadowMapEnabled) ss << "#define " << shadowMapTypeDefine << endl;

    if(parameters.sizeAttenuation) ss << "#define USE_SIZEATTENUATION" << endl;

    if(parameters.logarithmicDepthBuffer) ss << "#define USE_LOGDEPTHBUF" << endl;
    if(parameters.logarithmicDepthBuffer && extensions.get(Extension::EXT_frag_depth)) ss << "#define USE_LOGDEPTHBUF_EXT" << endl;

    ss << "uniform mat4 modelMatrix;" << endl;
    ss << "uniform mat4 modelViewMatrix;" << endl;
    ss << "uniform mat4 projectionMatrix;" << endl;
    ss << "uniform mat4 viewMatrix;" << endl;
    ss << "uniform mat3 normalMatrix;" << endl;
    ss << "uniform vec3 cameraPosition;" << endl;

    ss << "attribute vec3 position;" << endl;
    ss << "attribute vec3 normal;" << endl;
    ss << "attribute vec2 uv;" << endl;

    ss << "#ifdef USE_COLOR" << endl;

    ss << "	attribute vec3 color;" << endl;

    ss << "#endif" << endl;

    ss << "#ifdef USE_MORPHTARGETS" << endl;

    ss << "	attribute vec3 morphTarget0;" << endl;
    ss << "	attribute vec3 morphTarget1;" << endl;
    ss << "	attribute vec3 morphTarget2;" << endl;
    ss << "	attribute vec3 morphTarget3;" << endl;

    ss << "	#ifdef USE_MORPHNORMALS" << endl;

    ss << "		attribute vec3 morphNormal0;" << endl;
    ss << "		attribute vec3 morphNormal1;" << endl;
    ss << "		attribute vec3 morphNormal2;" << endl;
    ss << "		attribute vec3 morphNormal3;" << endl;

    ss << "	#else" << endl;

    ss << "		attribute vec3 morphTarget4;" << endl;
    ss << "		attribute vec3 morphTarget5;" << endl;
    ss << "		attribute vec3 morphTarget6;" << endl;
    ss << "		attribute vec3 morphTarget7;" << endl;

    ss << "	#endif" << endl;

    ss << "#endif" << endl;

    ss << "#ifdef USE_SKINNING" << endl;

    ss << "	attribute vec4 skinIndex;" << endl;
    ss << "	attribute vec4 skinWeight;" << endl;

    ss << "#endif" << endl;

    prefixVertex = ss.str();

    ss.clear();
    ss << customExtensions;

    ss << "precision " << parameters.precision << " float;" << endl;
    ss << "precision " << parameters.precision << " int;" << endl;

    ss << "#define SHADER_NAME " << shader.name();

    ss << customDefines;

    if(parameters.alphaTest) ss << "#define ALPHATEST " + parameters.alphaTest << endl;

    ss << "#define GAMMA_FACTOR " << gammaFactorDefine;

    if(( parameters.useFog && parameters.fog )) ss << "#define USE_FOG" << endl;
    if(( parameters.useFog && parameters.fogExp )) ss << "#define FOG_EXP2" << endl;

    if(parameters.map) ss << "#define USE_MAP" << endl;
    if(parameters.envMap) ss << "#define USE_ENVMAP" << endl;
    if(parameters.envMap) ss << "#define " << envMapTypeDefine << endl;
    if(parameters.envMap) ss << "#define " << envMapModeDefine << endl;
    if(parameters.envMap) ss << "#define " << envMapBlendingDefine << endl;
    if(parameters.lightMap) ss << "#define USE_LIGHTMAP" << endl;
    if(parameters.aoMap) ss << "#define USE_AOMAP" << endl;
    if(parameters.emissiveMap) ss << "#define USE_EMISSIVEMAP" << endl;
    if(parameters.bumpMap) ss << "#define USE_BUMPMAP" << endl;
    if(parameters.normalMap) ss << "#define USE_NORMALMAP" << endl;
    if(parameters.specularMap) ss << "#define USE_SPECULARMAP" << endl;
    if(parameters.roughnessMap) ss << "#define USE_ROUGHNESSMAP" << endl;
    if(parameters.metalnessMap) ss << "#define USE_METALNESSMAP" << endl;
    if(parameters.alphaMap) ss << "#define USE_ALPHAMAP" << endl;
    if(parameters.vertexColors != Colors::None) ss << "#define USE_COLOR" << endl;

    if(parameters.gradientMap) ss << "#define USE_GRADIENTMAP" << endl;

    if(parameters.flatShading) ss << "#define FLAT_SHADED" << endl;

    if(parameters.doubleSided) ss << "#define DOUBLE_SIDED" << endl;
    if(parameters.flipSided) ss << "#define FLIP_SIDED" << endl;

    ss << "#define NUM_CLIPPING_PLANES " + parameters.numClippingPlanes;
    ss << "#define UNION_CLIPPING_PLANES " + ( parameters.numClippingPlanes - parameters.numClipIntersection );

    if(parameters.shadowMapEnabled) ss << "#define USE_SHADOWMAP" << endl;
    if(parameters.shadowMapEnabled) ss << "#define " << shadowMapTypeDefine;

    if(parameters.premultipliedAlpha) ss << "#define PREMULTIPLIED_ALPHA" << endl;

    if(parameters.physicallyCorrectLights) ss << "#define PHYSICALLY_CORRECT_LIGHTS" << endl;

    if(parameters.logarithmicDepthBuffer) ss << "#define USE_LOGDEPTHBUF" << endl;
    if(parameters.logarithmicDepthBuffer && extensions.get(Extension::EXT_frag_depth)) ss << "#define USE_LOGDEPTHBUF_EXT" << endl;

    if(parameters.envMap && extensions.get(Extension::EXT_shader_texture_lod)) ss << "#define TEXTURE_LOD_EXT" << endl;

    ss << "uniform mat4 viewMatrix;" << endl;
    ss << "uniform vec3 cameraPosition;" << endl;

    if(( parameters.toneMapping != ToneMapping::None)) {
      ss << "#define TONE_MAPPING" << endl;

      // this code is required here because it is used by the toneMapping() function defined below
      ss << getShaderChunk(ShaderChunk::tonemapping_pars_fragment);

      ss << getToneMappingFunction( "toneMapping", parameters.toneMapping );
    }

    if(parameters.dithering) ss << "#define DITHERING" << endl;

    if( parameters.outputEncoding != Encoding::Unknown
        || parameters.mapEncoding  != Encoding::Unknown
        || parameters.envMapEncoding  != Encoding::Unknown
        || parameters.emissiveMapEncoding  != Encoding::Unknown)
      // this code is required here because it is used by the various encoding/decoding function defined below
      ss << getShaderChunk(ShaderChunk::encodings_pars_fragment);

    if(parameters.mapEncoding != Encoding::Unknown)
      ss << getTexelDecodingFunction( "mapTexelToLinear", parameters.mapEncoding );
    if(parameters.envMapEncoding != Encoding::Unknown)
      ss << getTexelDecodingFunction( "envMapTexelToLinear", parameters.envMapEncoding );
    if(parameters.emissiveMapEncoding != Encoding::Unknown)
      ss << getTexelDecodingFunction( "emissiveMapTexelToLinear", parameters.emissiveMapEncoding );
    if(parameters.outputEncoding != Encoding::Unknown)
      ss << getTexelEncodingFunction( "linearToOutputTexel", parameters.outputEncoding );

    if(parameters.depthPacking != DepthPacking::Unknown)
      ss << "#define DEPTH_PACKING " << parameters.depthPacking;

    prefixFragment = ss.str();
  }

  string vertexShader = parseIncludes( shader.vertexShader() );
  vertexShader = replaceLightNums( shader.vertexShader(), parameters );

  string fragmentShader = parseIncludes( shader.fragmentShader() );
  fragmentShader = replaceLightNums( fragmentShader, parameters );

  if ( ! parameters.shaderMaterial ) {

    vertexShader = unrollLoops( vertexShader );
    fragmentShader = unrollLoops( fragmentShader );
  }

  string vertexGlsl = prefixVertex + vertexShader;
  string fragmentGlsl = prefixFragment + fragmentShader;

  GLuint glVertexShader = createShader(&_renderer, GL_VERTEX_SHADER, vertexGlsl );
  GLuint glFragmentShader = createShader(&_renderer, GL_FRAGMENT_SHADER, fragmentGlsl );

  _renderer.glAttachShader( program, glVertexShader );
  _renderer.glAttachShader( program, glFragmentShader );

  // Force a particular attribute to index 0.

  if (!parameters.index0AttributeName.empty()) {

    _renderer.glBindAttribLocation( program, 0, parameters.index0AttributeName.data());

  } else if (parameters.morphTargets) {

    // programs with morphTargets displace position out of attribute 0
    _renderer.glBindAttribLocation( program, 0, "position" );
  }

  _renderer.glLinkProgram( program );

  string programLog = getInfoLog(&_renderer, InfoObject::program, program );

  bool runnable = true;
  bool haveDiagnostics = true;

  // console.log( '**VERTEX**', gl.getExtension( 'WEBGL_debug_shaders' ).getTranslatedShaderSource( glVertexShader ) );
  // console.log( '**FRAGMENT**', gl.getExtension( 'WEBGL_debug_shaders' ).getTranslatedShaderSource( glFragmentShader ) );

  GLint value;
  _renderer.glGetProgramiv( program, GL_LINK_STATUS, &value);
  if(value != GL_TRUE) {

    runnable = false;
    GLint status;
    _renderer.glGetProgramiv(program, GL_VALIDATE_STATUS, &status);

    cerr << "shader compilation failed: " << _renderer.glGetError() << endl << "VALIDATE_STATUS: " << status << endl
         << "ProgramInfoLog: ", programLog;
  }
  else if ( !programLog.empty()) {

    cerr << "ProgramInfoLog: " << programLog;

  }
  // clean up
  _renderer.glDeleteShader( glVertexShader );
  _renderer.glDeleteShader( glFragmentShader );
}

#if 0
// set up caching for uniform locations
this.getUniforms = function () {

  if ( cachedUniforms === undefined ) {

    cachedUniforms = new WebGLUniforms( gl, program, renderer );

  }

  return cachedUniforms;
}

// set up caching for attribute locations
this.getAttributes = function () {

  if ( cachedAttributes === undefined ) {

    cachedAttributes = fetchAttributeLocations( gl, program );

  }

  return cachedAttributes;
}
#endif

Uniforms::Ptr Program::getUniforms()
{
  if (_cachedUniforms == nullptr) {
    _cachedUniforms = Uniforms::make(&_renderer, _program);
  }

  return _cachedUniforms;
}

Program::~Program() {
  _renderer.glDeleteProgram(_program);
  _program = 0;
}

}
}