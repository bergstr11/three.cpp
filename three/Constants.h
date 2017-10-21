//
// Created by byter on 05.08.17.
//

#ifndef THREE_QT_CONSTANTS
#define THREE_QT_CONSTANTS

#include <GL/gl.h>

namespace three {

enum class ToneMapping
{
  None = 0, Linear = 1, Reinhard = 2, Uncharted2 = 3, Cineon = 4
};

enum class TextureFilter
{
  Nearest = 1003,
  NearestMipMapNearest = 1004,
  NearestMipMapLinear = 1005,
  Linear = 1006,
  LinearMipMapNearest = 1007,
  LinearMipMapLinear = 1008
};

enum class TextureMapping
{
  UV = 300,
  CubeReflection = 301,
  CubeRefraction = 302,
  EquirectangularReflection = 303,
  EquirectangularRefraction = 304,
  SphericalReflection = 305,
  CubeUVReflection = 306,
  CubeUVRefraction = 307
};

enum class TextureWrapping
{
  Repeat = 1000,
  ClampToEdge = 1001,
  MirroredRepeat = 1002
};

enum class TextureFormat
{
  Alpha = GL_ALPHA,
  RGB = GL_RGB,
  RGBA = GL_RGBA,
  Luminance = GL_LUMINANCE,
  LuminanceAlpha = GL_LUMINANCE4_ALPHA4,
  Depth = GL_DEPTH,
  DepthStencil = GL_DEPTH_STENCIL,

  RGB_S3TC_DXT1 = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
  RGBA_S3TC_DXT1 = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
  RGBA_S3TC_DXT3 = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
  RGBA_S3TC_DXT5 = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
};

enum class TextureType
{
  UnsignedByte = GL_UNSIGNED_BYTE,
  Byte = GL_BYTE,
  Short = GL_SHORT,
  UnsignedShort = GL_UNSIGNED_SHORT,
  Int = GL_INT,
  UnsignedInt = GL_UNSIGNED_INT,
  Float = GL_FLOAT,
  HalfFloat = GL_HALF_FLOAT,
  UnsignedShort4444 = GL_UNSIGNED_SHORT_4_4_4_4,
  UnsignedShort5551 = GL_UNSIGNED_SHORT_5_5_5_1,
  UnsignedShort565 = GL_UNSIGNED_SHORT_5_6_5,
  UnsignedInt248 = GL_UNSIGNED_INT_24_8
};

enum class Encoding
{
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

enum class CombineOperation
{
  Multiply = 0,
  Mix = 1,
  Add = 2
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
  CW = GL_CW, CCW = GL_CCW
};

enum class TextureTarget : GLenum
{
  twoD=GL_TEXTURE_2D, //A two-dimensional texture.
  cubeMap=GL_TEXTURE_CUBE_MAP, //Positive X face for a cube-mapped texture.
  cubeMapNegativeX=GL_TEXTURE_CUBE_MAP_NEGATIVE_X, //Negative X face for a cube-mapped texture.
  cubeMapPositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // Positive Y face for a cube-mapped texture.
  cubeMapNegativeY=GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, //Negative Y face for a cube-mapped texture.
  cubeMapPositiveZ=GL_TEXTURE_CUBE_MAP_POSITIVE_Z, //Positive Z face for a cube-mapped texture.
  cubeMapNegativeZ=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

enum class ShadowMapType
{
  Basic=0, PCF=1, PCFSoft=2
};

enum class DepthPacking {
  Basic=3200, RGBA=3201
};

enum class BufferType {
  Array=GL_ARRAY_BUFFER, ElementArray=GL_ELEMENT_ARRAY_BUFFER
};

enum class ShaderType {
  Vertex=GL_VERTEX_SHADER,
  Fragment=GL_FRAGMENT_SHADER
};

enum class Precision
{
  lowp, mediump, highp
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

struct UpdateRange
{
  uint32_t offset;
  int32_t count;

  UpdateRange(uint32_t offset, int32_t count) : offset(offset), count(count) {}
};

}

#endif //THREE_QT_CONSTANTS
