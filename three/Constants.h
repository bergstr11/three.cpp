//
// Created by byter on 05.08.17.
//

#ifndef THREE_QT_CONSTANTS
#define THREE_QT_CONSTANTS

namespace three {

enum class ToneMapping {
  None=0, Linear=1, Reinhard=2, Uncharted2=3, Cineon=4
};

enum class TextureFilter {
  Nearest=1003,
  NearestMipMapNearest=1004,
  NearestMipMapLinear=1005,
  Linear=1006,
  LinearMipMapNearest=1007,
  LinearMipMapLinear=1008
};

enum class TextureMapping {
  UV=300,
  CubeReflection=301,
  CubeRefraction=302,
  EquirectangularReflection=303,
  EquirectangularRefraction=304,
  SphericalReflection=305,
  CubeUVReflection=306,
  CubeUVRefraction=307
};

enum class TextureWrapping {
  Repeat=1000,
  ClampToEdge=1001,
  MirroredRepeat=1002
};

enum class TextureFormat {
  Alpha=1021,
  RGB=1022,
  RGBA=1023,
  Luminance=1024,
  LuminanceAlpha=1025,
  Depth=1026,
  DepthStencil=1027
};

enum class TextureType {
  UnsignedByte=1009,
  Byte=1010,
  Short=1011,
  UnsignedShort=1012,
  Int=1013,
  UnsignedInt=1014,
  Float=1015,
  HalfFloat=1016,
  UnsignedShort4444=1017,
  UnsignedShort5551=1018,
  UnsignedShort565=1019,
  UnsignedInt248=1020
  
};

enum class Encoding {
  Linear=3000,
  sRGB=3001,
  Gamma=3007,
  RGBE=3002,
  LogLuv=3003,
  RGBM7=3004,
  RGBM16=3005,
  RGBD=3006
};

enum class Blending {
  None= 0,
  Normal = 1,
  Additive = 2,
  Subtractive = 3,
  Multiply = 4,
  Custom = 5
};

enum class Side {
  Front = 0,
  Back = 1,
  Double = 2
};

enum class Colors {
  None = 0,
  Face = 1,
  Vertex = 2
};

enum class BlendFactor {
  None = 0,
  Zero = 200,
  One = 201,
  SrcColor = 202,
  OneMinusSrcColor = 203,
  SrcAlpha = 204,
  OneMinusSrcAlpha = 205,
  DstAlpha = 206,
  OneMinusDstAlpha = 207,
  DstColor = 208,
  OneMinusDstColor = 209,
  SrcAlphaSaturate = 210
};

enum class BlendEquation {
  None = 0,
  Add = 100,
  Subtract = 101,
  ReverseSubtract = 102,
  Min = 103,
  Max = 104
};

enum class DepthFunc {
  Never = 0,
  Always = 1,
  Less = 2,
  LessEqual = 3,
  Equal = 4,
  GreaterEqual = 5,
  Greater = 6,
  NotEqual = 7
};

enum class DrawMode {
  Triangles = 0,
  TriangleStrip = 1,
  TriangleFan = 2
};

}

#endif //THREE_QT_CONSTANTS
