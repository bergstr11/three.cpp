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
  Alpha = 1021,
  RGB = 1022,
  RGBA = 1023,
  Luminance = 1024,
  LuminanceAlpha = 1025,
  Depth = 1026,
  DepthStencil = 1027
};

enum class TextureType
{
  UnsignedByte = 1009,
  Byte = 1010,
  Short = 1011,
  UnsignedShort = 1012,
  Int = 1013,
  UnsignedInt = 1014,
  Float = 1015,
  HalfFloat = 1016,
  UnsignedShort4444 = 1017,
  UnsignedShort5551 = 1018,
  UnsignedShort565 = 1019,
  UnsignedInt248 = 1020

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
  Front = 0,
  Back = 1,
  Double = 2
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
  Triangles = 0,
  TriangleStrip = 1,
  TriangleFan = 2
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

enum class BindingTarget : GLenum
{
  texture2D=GL_TEXTURE_2D,
  textureCubeMap=GL_TEXTURE_CUBE_MAP
};

enum class ShadowMapType
{
  Basic=0, PCF=1, PCFSoft=2
};

enum class DepthPacking {
  Basic=3200, RGBA=3201
};

}

#endif //THREE_QT_CONSTANTS
