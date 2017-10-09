//
// Created by byter on 01.10.17.
//

#include "Program.h"
#include <sstream>
#include <regex>
#include <helper/utils.h>

namespace three {
namespace gl {

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
generateExtensions(const Extensions &extensions, const UseExtension &use, const Parameters &parameters)
{
  stringstream ss;
  if (extensions.get(Extension::OES_standard_derivatives) || parameters.envMapCubeUV || parameters.bumpMap || parameters.normalMap || parameters.flatShading)
    ss << "#extension GL_OES_standard_derivatives : enable" << endl;
  if ((extensions.get(Extension::EXT_frag_depth) || parameters.logarithmicDepthBuffer) && use.get(Extension::EXT_frag_depth))
    ss << "#extension GL_EXT_frag_depth : enable" << endl;
  if (extensions.get(Extension::GL_EXT_draw_buffers) && use.get(Extension::GL_EXT_draw_buffers))
    ss << "#extension GL_EXT_draw_buffers : require" << endl;
  if ((extensions.get(Extension::EXT_shader_texture_lod) || parameters.envMap) && use.get(Extension::EXT_shader_texture_lod))
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
  glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &numActive);

  AttribInfo info;
  for (unsigned i = 0; i < numActive; i++) {

    glGetActiveAttrib(_program, i, 100, &info.length, &info.size, &info.type, info.name);
    info.name[info.length] = 0;
    string name(info.name);

    // console.log("THREE.WebGLProgram: ACTIVE VERTEX ATTRIBUTE:", name, i );
    attributes[name] = glGetAttribLocation(_program, info.name);
  }

  return attributes;
}

string replaceLightNums(string value, const Parameters &parameters)
{
  return replace_all(value, {{"NUM_DIR_LIGHTS",       to_string(parameters.numDirLights)},
                             {"NUM_SPOT_LIGHTS",      to_string(parameters.numSpotLights)},
                             {"NUM_RECT_AREA_LIGHTS", to_string(parameters.numRectAreaLights)},
                             {"NUM_POINT_LIGHTS",     to_string(parameters.numPointLights)},
                             {"NUM_HEMI_LIGHTS",      to_string(parameters.numHemiLights)}});
}

string parseIncludes(string lookat, unordered_map<string, string> ShaderChunk)
{
  static regex rex("[ \r\n\t]*#include +<([\\w\\d.]+)>");

  sregex_iterator rex_it(lookat.begin(), lookat.end(), rex);
  sregex_iterator rex_end;

  while(rex_it != rex_end) {
    ssub_match sub = (*rex_it)[1];
    string r = ShaderChunk[sub.str()];
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
  }

  return unroll.str();
}

}
}