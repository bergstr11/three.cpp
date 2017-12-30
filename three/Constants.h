//
// Created by byter on 05.08.17.
//

#ifndef THREE_QT_CONSTANTS
#define THREE_QT_CONSTANTS

#include <GL/gl.h>
#include <iostream>
#include <assert.h>

namespace three {

enum class ToneMapping
{
  None = 0, Linear = 1, Reinhard = 2, Uncharted2 = 3, Cineon = 4
};

enum class TextureFilter : GLint
{
  Nearest = GL_NEAREST,
  NearestMipMapNearest = GL_NEAREST_MIPMAP_NEAREST,
  NearestMipMapLinear = GL_NEAREST_MIPMAP_LINEAR,
  Linear = GL_LINEAR,
  LinearMipMapNearest = GL_LINEAR_MIPMAP_NEAREST,
  LinearMipMapLinear = GL_LINEAR_MIPMAP_LINEAR
};

enum class TextureMapping : GLenum
{
  Unknown = 0,
  UV = 300,
  CubeReflection = 301,
  CubeRefraction = 302,
  EquirectangularReflection = 303,
  EquirectangularRefraction = 304,
  SphericalReflection = 305,
  CubeUVReflection = 306,
  CubeUVRefraction = 307
};

enum class TextureWrapping : GLint
{
  Repeat = GL_REPEAT,
  ClampToEdge = GL_CLAMP_TO_EDGE,
  MirroredRepeat = GL_MIRRORED_REPEAT
};

enum class TextureFormat : GLenum
{
  Undefined = 0,
  Alpha = GL_ALPHA,
  RGB = GL_RGB,
  BGR = GL_BGR,
  RGBA = GL_RGBA,
  BGRA = GL_BGRA,
  Luminance = GL_LUMINANCE,
  LuminanceAlpha = GL_LUMINANCE4_ALPHA4,
  Depth = GL_DEPTH,
  DepthComponent=GL_DEPTH_COMPONENT,
  DepthComponent32 = GL_DEPTH_COMPONENT32F,
  DepthComponent16 = GL_DEPTH_COMPONENT16,
  DepthStencil = GL_DEPTH_STENCIL,
  RGB_S3TC_DXT1 = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
  RGBA_S3TC_DXT1 = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
  RGBA_S3TC_DXT3 = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
  RGBA_S3TC_DXT5 = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
};

enum class TextureType : GLenum
{
  UnsignedByte = GL_UNSIGNED_BYTE,
  Byte = GL_BYTE,
  Short = GL_SHORT,
  UnsignedShort = GL_UNSIGNED_SHORT,
  Int = GL_INT,
  UnsignedInt = GL_UNSIGNED_INT,
  Float = GL_FLOAT,
  HalfFloat = GL_HALF_FLOAT,
  HalfFloatOES = 0x8D61, //GL_HALF_FLOAT_OES
  UnsignedShort4444 = GL_UNSIGNED_SHORT_4_4_4_4,
  UnsignedShort5551 = GL_UNSIGNED_SHORT_5_5_5_1,
  UnsignedShort565 = GL_UNSIGNED_SHORT_5_6_5,
  UnsignedInt248 = GL_UNSIGNED_INT_24_8,
};

enum class Encoding
{
  Unknown=0,
  Linear = 3000,
  sRGB = 3001,
  Gamma = 3007,
  RGBE = 3002,
  LogLuv = 3003,
  RGBM7 = 3004,
  RGBM16 = 3005,
  RGBD = 3006
};

enum class Blending
{
  None = 0,
  Normal = 1,
  Additive = 2,
  Subtractive = 3,
  Multiply = 4,
  Custom = 5
};

enum class Side
{
  Front = GL_FRONT,
  Back = GL_BACK,
  Double = GL_FRONT_AND_BACK
};

enum class Equation
{
  Subtract = GL_FUNC_SUBTRACT,
  ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT,
  Min = GL_MIN_EXT,
  Max = GL_MAX
};

enum class Colors
{
  None = 0,
  Face = 1,
  Vertex = 2
};

enum class BlendFunc
{
  None = -1,
  Zero = GL_ZERO,
  One = GL_ONE,
  SrcColor = GL_SRC_COLOR,
  OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
  DstColor=GL_DST_COLOR,
  OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
  SrcAlpha = GL_SRC_ALPHA,
  OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
  DstAlpha = GL_DST_ALPHA,
  OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
  ConstColor=GL_CONSTANT_COLOR,
  OneMinusConstColor = GL_ONE_MINUS_CONSTANT_COLOR,
  ConstAlpha = GL_CONSTANT_ALPHA,
  OneMinusConstAlpha = GL_ONE_MINUS_CONSTANT_ALPHA,
  SrcAlphaSaturate = GL_SRC_ALPHA_SATURATE
};

enum class BlendEq
{
  None = 0,
  Add = GL_FUNC_ADD,
  Subtract = GL_FUNC_SUBTRACT,
  ReverseSubtract = GL_FUNC_REVERSE_SUBTRACT
};

enum class Func
{
  Never = GL_NEVER,
  Less = GL_LESS,
  Always = GL_ALWAYS,
  LessEqual = GL_LEQUAL,
  Equal = GL_EQUAL,
  GreaterEqual = GL_GEQUAL,
  Greater = GL_GREATER,
  NotEqual = GL_NOTEQUAL
};

enum class Op
{
  Keep = 0x1E00,
  Zero = 0,
  Replace = 0x1E01,
  Incr = 0x1E02,
  Decr = 0x1E03,
  Invert = 0x150A
};

enum class DrawMode
{
  Points = GL_POINTS,
  LineStrip = GL_LINE_STRIP,
  LineLoop = GL_LINE_LOOP,
  Lines = GL_LINES,
  Triangles = GL_TRIANGLES,
  TriangleStrip = GL_TRIANGLE_STRIP,
  TriangleFan = GL_TRIANGLE_FAN
};

enum class CombineOperation : GLenum
{
  Unknown = 0,
  Multiply = 1,
  Mix = 2,
  Add = 3
};

enum class LineCap
{
  round
};

enum class LineJoin
{
  round
};

enum class CullFace
{
  None = GL_NONE,
  Back = GL_BACK,
  Front = GL_FRONT,
  FrontBack = GL_FRONT_AND_BACK,
};

enum class FrontFaceDirection
{
  CW = GL_CW, CCW = GL_CCW, Undefined
};

enum class TextureTarget : GLenum
{
  twoD=GL_TEXTURE_2D, //A two-dimensional texture.
  cubeMap=GL_TEXTURE_CUBE_MAP, //Positive X face for a cube-mapped texture.

  cubeMapPositiveX=GL_TEXTURE_CUBE_MAP_POSITIVE_X, // Positive Y face for a cube-mapped texture.
  cubeMapNegativeX=GL_TEXTURE_CUBE_MAP_NEGATIVE_X, //Negative X face for a cube-mapped texture.
  cubeMapPositiveY=GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // Positive Y face for a cube-mapped texture.
  cubeMapNegativeY=GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, //Negative Y face for a cube-mapped texture.
  cubeMapPositiveZ=GL_TEXTURE_CUBE_MAP_POSITIVE_Z, //Positive Z face for a cube-mapped texture.
  cubeMapNegativeZ=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  //Negative Z face for a cube-mapped texture.
};

inline TextureTarget operator +(const TextureTarget &value, unsigned dist) {
  assert((GLenum)value >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && GL_TEXTURE_CUBE_MAP_POSITIVE_X + dist <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
  return (TextureTarget)((GLenum)value + dist);
}

enum class ShadowMapType
{
  Basic=0, PCF=1, PCFSoft=2, NoShadow=3
};

enum class DepthPacking {
  Basic=3200, RGBA=3201, Unknown=0
};

enum class BufferType {
  Array=GL_ARRAY_BUFFER, ElementArray=GL_ELEMENT_ARRAY_BUFFER
};

enum class ShaderType {
  Vertex=GL_VERTEX_SHADER,
  Fragment=GL_FRAGMENT_SHADER
};

enum class Precision : int
{
  lowp, mediump, highp, unknown
};

enum class UniformType
{
  Float=GL_FLOAT,
  FloatVect2=GL_FLOAT_VEC2,
  FloatVec3=GL_FLOAT_VEC3,
  FloatVec4=GL_FLOAT_VEC4,
  Int=GL_INT,
  IntVec2=GL_INT_VEC2,
  IntVec3=GL_INT_VEC3,
  IntVect4=GL_INT_VEC4,
  Bool=GL_BOOL,
  BoolVec2=GL_BOOL_VEC2,
  BoolVec3=GL_BOOL_VEC3,
  BoolVec4=GL_BOOL_VEC4,
  FloatMat2=GL_FLOAT_MAT2,
  FloatMat3=GL_FLOAT_MAT3,
  FloatMat4=GL_FLOAT_MAT4,
  Sampler2D=GL_SAMPLER_2D,
  SamplerCube=GL_SAMPLER_CUBE
};

namespace numeric_out {

template <typename Enumeration>
auto numeric(Enumeration const value)
-> typename std::underlying_type<Enumeration>::type
{
  return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

template <class Enumeration, class = typename std::enable_if<std::is_enum<Enumeration>::value>::type>
std::ostream &operator<<(std::ostream &os, const Enumeration &_enum)
{
  os << numeric(_enum);
}

}

namespace string_out {

inline std::ostream &operator<<(std::ostream &os, const TextureMapping &mapping)
{
  switch (mapping) {
    case TextureMapping::Unknown: os << "Unknown"; break;
    case TextureMapping::UV: os << "UV"; break;
    case TextureMapping::CubeReflection: os << "CubeReflection"; break;
    case TextureMapping::CubeRefraction: os << "CubeRefraction"; break;
    case TextureMapping::EquirectangularReflection: os << "EquirectangularReflection"; break;
    case TextureMapping::EquirectangularRefraction: os << "EquirectangularRefraction"; break;
    case TextureMapping::SphericalReflection: os << "SphericalReflection"; break;
    case TextureMapping::CubeUVReflection: os << "CubeUVReflection"; break;
    case TextureMapping::CubeUVRefraction: os << "CubeUVRefraction"; break;
  }
}

inline std::ostream &operator<<(std::ostream &os, const Encoding &encoding)
{
  switch (encoding) {
    case Encoding::Unknown: os << "Unknown"; break;
    case Encoding::Linear: os << "Linear"; break;
    case Encoding::sRGB: os << "sRGB"; break;
    case Encoding::Gamma: os << "Gamma"; break;
    case Encoding::RGBE: os << "RGBE"; break;
    case Encoding::LogLuv: os << "LogLuv"; break;
    case Encoding::RGBM7: os << "RGBM7"; break;
    case Encoding::RGBM16: os << "RGBM16"; break;
    case Encoding::RGBD: os << "RGBD"; break;
  }
}

inline std::ostream &operator<<(std::ostream &stream, const DepthPacking &depthPacking)
{
  switch (depthPacking) {
    case DepthPacking::Basic: stream << "BASIC"; break;
    case DepthPacking::RGBA: stream << "RGBA"; break;
    default:
      throw std::invalid_argument("unknown depthPacking");
  }
}

inline std::ostream &operator<<(std::ostream &stream, const Precision &precision)
{
  switch (precision) {
    case Precision::lowp: stream << "lowp"; break;
    case Precision::mediump: stream << "mediump"; break;
    case Precision::highp: stream << "highp"; break;
    default:
      throw std::invalid_argument("unknown precision");
  }
}

}
}

#endif //THREE_QT_CONSTANTS
