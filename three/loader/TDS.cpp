//
// Created by byter on 1/5/18.
//

#include "TDS.h"
#include <fstream>
#include "reader.h"

namespace three {
namespace loader {

enum class ChunkId : uint16_t
{
  NULL_CHUNK = 0x0000,
  M3DMAGIC = 0x4D4D,
  SMAGIC = 0x2D2D,
  LMAGIC = 0x2D3D,
  MLIBMAGIC = 0x3DAA,
  MATMAGIC = 0x3DFF,
  CMAGIC = 0xC23D,
  M3D_VERSION = 0x0002,
  M3D_KFVERSION = 0x0005,
  COLOR_F = 0x0010,
  COLOR_24 = 0x0011,
  LIN_COLOR_24 = 0x0012,
  LIN_COLOR_F = 0x0013,
  INT_PERCENTAGE = 0x0030,
  FLOAT_PERCENTAGE = 0x0031,
  MDATA = 0x3D3D,
  MESH_VERSION = 0x3D3E,
  MASTER_SCALE = 0x0100,
  LO_SHADOW_BIAS = 0x1400,
  HI_SHADOW_BIAS = 0x1410,
  SHADOW_MAP_SIZE = 0x1420,
  SHADOW_SAMPLES = 0x1430,
  SHADOW_RANGE = 0x1440,
  SHADOW_FILTER = 0x1450,
  RAY_BIAS = 0x1460,
  O_CONSTS = 0x1500,
  AMBIENT_LIGHT = 0x2100,
  BIT_MAP = 0x1100,
  SOLID_BGND = 0x1200,
  V_GRADIENT = 0x1300,
  USE_BIT_MAP = 0x1101,
  USE_SOLID_BGND = 0x1201,
  USE_V_GRADIENT = 0x1301,
  FOG = 0x2200,
  FOG_BGND = 0x2210,
  LAYER_FOG = 0x2302,
  DISTANCE_CUE = 0x2300,
  DCUE_BGND = 0x2310,
  USE_FOG = 0x2201,
  USE_LAYER_FOG = 0x2303,
  USE_DISTANCE_CUE = 0x2301,
  MAT_ENTRY = 0xAFFF,
  MAT_NAME = 0xA000,
  MAT_AMBIENT = 0xA010,
  MAT_DIFFUSE = 0xA020,
  MAT_SPECULAR = 0xA030,
  MAT_SHININESS = 0xA040,
  MAT_SHIN2PCT = 0xA041,
  MAT_TRANSPARENCY = 0xA050,
  MAT_XPFALL = 0xA052,
  MAT_USE_XPFALL = 0xA240,
  MAT_REFBLUR = 0xA053,
  MAT_SHADING = 0xA100,
  MAT_USE_REFBLUR = 0xA250,
  MAT_SELF_ILLUM = 0xA084,
  MAT_TWO_SIDE = 0xA081,
  MAT_DECAL = 0xA082,
  MAT_ADDITIVE = 0xA083,
  MAT_WIRE = 0xA085,
  MAT_FACEMAP = 0xA088,
  MAT_TRANSFALLOFF_IN = 0xA08A,
  MAT_PHONGSOFT = 0xA08C,
  MAT_WIREABS = 0xA08E,
  MAT_WIRE_SIZE = 0xA087,
  MAT_TEXMAP = 0xA200,
  MAT_SXP_TEXT_DATA = 0xA320,
  MAT_TEXMASK = 0xA33E,
  MAT_SXP_TEXTMASK_DATA = 0xA32A,
  MAT_TEX2MAP = 0xA33A,
  MAT_SXP_TEXT2_DATA = 0xA321,
  MAT_TEX2MASK = 0xA340,
  MAT_SXP_TEXT2MASK_DATA = 0xA32C,
  MAT_OPACMAP = 0xA210,
  MAT_SXP_OPAC_DATA = 0xA322,
  MAT_OPACMASK = 0xA342,
  MAT_SXP_OPACMASK_DATA = 0xA32E,
  MAT_BUMPMAP = 0xA230,
  MAT_SXP_BUMP_DATA = 0xA324,
  MAT_BUMPMASK = 0xA344,
  MAT_SXP_BUMPMASK_DATA = 0xA330,
  MAT_SPECMAP = 0xA204,
  MAT_SXP_SPEC_DATA = 0xA325,
  MAT_SPECMASK = 0xA348,
  MAT_SXP_SPECMASK_DATA = 0xA332,
  MAT_SHINMAP = 0xA33C,
  MAT_SXP_SHIN_DATA = 0xA326,
  MAT_SHINMASK = 0xA346,
  MAT_SXP_SHINMASK_DATA = 0xA334,
  MAT_SELFIMAP = 0xA33D,
  MAT_SXP_SELFI_DATA = 0xA328,
  MAT_SELFIMASK = 0xA34A,
  MAT_SXP_SELFIMASK_DATA = 0xA336,
  MAT_REFLMAP = 0xA220,
  MAT_REFLMASK = 0xA34C,
  MAT_SXP_REFLMASK_DATA = 0xA338,
  MAT_ACUBIC = 0xA310,
  MAT_MAPNAME = 0xA300,
  MAT_MAP_TILING = 0xA351,
  MAT_MAP_TEXBLUR = 0xA353,
  MAT_MAP_USCALE = 0xA354,
  MAT_MAP_VSCALE = 0xA356,
  MAT_MAP_UOFFSET = 0xA358,
  MAT_MAP_VOFFSET = 0xA35A,
  MAT_MAP_ANG = 0xA35C,
  MAT_MAP_COL1 = 0xA360,
  MAT_MAP_COL2 = 0xA362,
  MAT_MAP_RCOL = 0xA364,
  MAT_MAP_GCOL = 0xA366,
  MAT_MAP_BCOL = 0xA368,
  NAMED_OBJECT = 0x4000,
  N_DIRECT_LIGHT = 0x4600,
  DL_OFF = 0x4620,
  DL_OUTER_RANGE = 0x465A,
  DL_INNER_RANGE = 0x4659,
  DL_MULTIPLIER = 0x465B,
  DL_EXCLUDE = 0x4654,
  DL_ATTENUATE = 0x4625,
  DL_SPOTLIGHT = 0x4610,
  DL_SPOT_ROLL = 0x4656,
  DL_SHADOWED = 0x4630,
  DL_LOCAL_SHADOW2 = 0x4641,
  DL_SEE_CONE = 0x4650,
  DL_SPOT_RECTANGULAR = 0x4651,
  DL_SPOT_ASPECT = 0x4657,
  DL_SPOT_PROJECTOR = 0x4653,
  DL_SPOT_OVERSHOOT = 0x4652,
  DL_RAY_BIAS = 0x4658,
  DL_RAYSHAD = 0x4627,
  N_CAMERA = 0x4700,
  CAM_SEE_CONE = 0x4710,
  CAM_RANGES = 0x4720,
  OBJ_HIDDEN = 0x4010,
  OBJ_VIS_LOFTER = 0x4011,
  OBJ_DOESNT_CAST = 0x4012,
  OBJ_DONT_RECVSHADOW = 0x4017,
  OBJ_MATTE = 0x4013,
  OBJ_FAST = 0x4014,
  OBJ_PROCEDURAL = 0x4015,
  OBJ_FROZEN = 0x4016,
  N_TRI_OBJECT = 0x4100,
  POINT_ARRAY = 0x4110,
  POINT_FLAG_ARRAY = 0x4111,
  FACE_ARRAY = 0x4120,
  MSH_MAT_GROUP = 0x4130,
  SMOOTH_GROUP = 0x4150,
  MSH_BOXMAP = 0x4190,
  TEX_VERTS = 0x4140,
  MESH_MATRIX = 0x4160,
  MESH_COLOR = 0x4165,
  MESH_TEXTURE_INFO = 0x4170,
  KFDATA = 0xB000,
  KFHDR = 0xB00A,
  KFSEG = 0xB008,
  KFCURTIME = 0xB009,
  AMBIENT_NODE_TAG = 0xB001,
  OBJECT_NODE_TAG = 0xB002,
  CAMERA_NODE_TAG = 0xB003,
  TARGET_NODE_TAG = 0xB004,
  LIGHT_NODE_TAG = 0xB005,
  L_TARGET_NODE_TAG = 0xB006,
  SPOTLIGHT_NODE_TAG = 0xB007,
  NODE_ID = 0xB030,
  NODE_HDR = 0xB010,
  PIVOT = 0xB013,
  INSTANCE_NAME = 0xB011,
  MORPH_SMOOTH = 0xB015,
  BOUNDBOX = 0xB014,
  POS_TRACK_TAG = 0xB020,
  COL_TRACK_TAG = 0xB025,
  ROT_TRACK_TAG = 0xB021,
  SCL_TRACK_TAG = 0xB022,
  MORPH_TRACK_TAG = 0xB026,
  FOV_TRACK_TAG = 0xB023,
  ROLL_TRACK_TAG = 0xB024,
  HOT_TRACK_TAG = 0xB027,
  FALL_TRACK_TAG = 0xB028,
  HIDE_TRACK_TAG = 0xB029,
  POLY_2D = 0x5000,
  SHAPE_OK = 0x5010,
  SHAPE_NOT_OK = 0x5011,
  SHAPE_HOOK = 0x5020,
  PATH_3D = 0x6000,
  PATH_MATRIX = 0x6005,
  SHAPE_2D = 0x6010,
  M_SCALE = 0x6020,
  M_TWIST = 0x6030,
  M_TEETER = 0x6040,
  M_FIT = 0x6050,
  M_BEVEL = 0x6060,
  XZ_CURVE = 0x6070,
  YZ_CURVE = 0x6080,
  INTERPCT = 0x6090,
  DEFORM_LIMIT = 0x60A0,
  USE_CONTOUR = 0x6100,
  USE_TWEEN = 0x6110,
  USE_SCALE = 0x6120,
  USE_TWIST = 0x6130,
  USE_TEETER = 0x6140,
  USE_FIT = 0x6150,
  USE_BEVEL = 0x6160,
  DEFAULT_VIEW = 0x3000,
  VIEW_TOP = 0x3010,
  VIEW_BOTTOM = 0x3020,
  VIEW_LEFT = 0x3030,
  VIEW_RIGHT = 0x3040,
  VIEW_FRONT = 0x3050,
  VIEW_BACK = 0x3060,
  VIEW_USER = 0x3070,
  VIEW_CAMERA = 0x3080,
  VIEW_WINDOW = 0x3090,
  VIEWPORT_LAYOUT_OLD = 0x7000,
  VIEWPORT_DATA_OLD = 0x7010,
  VIEWPORT_LAYOUT = 0x7001,
  VIEWPORT_DATA = 0x7011,
  VIEWPORT_DATA_3 = 0x7012,
  VIEWPORT_SIZE = 0x7020,
  NETWORK_VIEW = 0x7030,
};

using namespace std;
namespace r = reader;

struct Access
{
  static Scene::Ptr test()
  {
    return nullptr;
  }
};

using word = uint16_t;
inline word readWord(istream &in) {return r::read_int<uint16_t, 2>(in);}

struct Chunk
{
  ChunkId id;
  istream::pos_type position;
  uint32_t size;
  istream::pos_type end;

  bool read(istream &in)
  {
    position = in.tellg();
    if(!r::read<uint16_t, 2>(in, (uint16_t *)&id))
      return false;
    if(!r::read<uint32_t, 4>(in, &size))
      return false;
    end = position + (long)size;
    position += 6;
    return true;
  }

  bool readNext(istream &in, const Chunk &prev)
  {
    in.seekg(prev.position);
    return read(in);
  }
};

void TDS::load(string name, ResourceLoader &loader)
{
  //fstream in(name, ios_base::in | ios_base::binary);
  istream & in = loader.istream(name, ios_base::in | ios_base::binary);

  Chunk chunk;
  chunk.read(in);

  if (chunk.id == ChunkId::MLIBMAGIC || chunk.id == ChunkId::CMAGIC || chunk.id == ChunkId::M3DMAGIC ) {

    while(chunk.read(in)) {

      if ( chunk.id == ChunkId::M3D_VERSION ) {

        uint32_t version;
        r::read<uint32_t, 2>(in, &version);
      }
      else if (chunk.id == ChunkId::MDATA) {

        readMeshData( in, loader, chunk );
      }
      else {

        stringstream ss;
        ss << "Unknown main chunk: " << hex << (uint32_t)chunk.id;
        throw invalid_argument(ss.str());
      }
    }
  }
}

/**
 * Read mesh data chunk.
 *
 * @method readMeshData
 * @param {Dataview} data Dataview in use.
 */
void TDS::readMeshData(std::istream &in, ResourceLoader &loader, Chunk &chunk)
{
  Chunk next;
  while (next.readNext(in, chunk)) {

    uint32_t meshVersion;

    if ( next.id == ChunkId::MESH_VERSION ) {
      r::read<uint32_t, 4>(in, &meshVersion);
    }
    else if (next.id == ChunkId::MASTER_SCALE) {

      float masterScale = r::read_float(in);
      _objects->scale().set(masterScale, masterScale, masterScale);

    } else if (next.id == ChunkId::NAMED_OBJECT ) {

      readNamedObject(in, loader, next);

    } else if(next.id == ChunkId::MAT_ENTRY) {

      readMaterialEntry(in, loader, next);
    }
  }
}

void TDS::readNamedObject(std::istream &in, ResourceLoader &loader, Chunk &chunk)
{
  string name = r::read_string(in, 64);

  Chunk next;
  while (next.readNext(in, chunk)) {

    if (next.id == ChunkId::N_TRI_OBJECT ) {

      Mesh::Ptr mesh = readMesh(in, name, next);
      _meshes.push_back( mesh );

    }
  }
}

Mesh::Ptr TDS::readMesh(std::istream &in, string name, Chunk &chunk)
{
  Chunk next;

  //Geometry::Ptr geometry;
  vector<UV> uvs;

  //var material = new THREE.MeshPhongMaterial();

  while (next.readNext(in, chunk)) {

    if (next.id == ChunkId::POINT_ARRAY) {

      word points = readWord(in);

      vector<Vertex> vertices;
      for (uint16_t i = 0; i < points; i ++ )		{

        float x = r::read_float(in);
        float y = r::read_float(in);
        float z = r::read_float(in);
        vertices.emplace_back(x, y, z);
      }

      if (_useBufferGeometry)	{

        BufferGeometry::Ptr bg = BufferGeometry::make();
        bg->setPosition(BufferAttributeT<float>::make(vertices));
      }
      else	{ //Geometry

        //Geometry::Ptr geometry = Geometry::make(vertices);
      }

    }
#if 0
    else if (next.id == ChunkId::FACE_ARRAY ) {

      readFaceArray( in, mesh, next);
    }
    else if (next.id == ChunkId::TEX_VERTS) {

      word texels = readWord(in);

      if (_useBufferGeometry )	{

        vector<UV> uvs;
        for (auto i = 0; i < texels; i ++ )		{

          float u = r::read_float(in);
          float v = r::read_float(in);
          uvs.emplace_back( u, v );
        }
        geometry.addAttribute( 'uv', new THREE.BufferAttribute( new Float32Array( uvs ), 2 ) );

      } else { //Geometry

        uvs = [];
        for ( var i = 0; i < texels; i ++ )		{

          uvs.push( new THREE.Vector2( this.readFloat( data ), this.readFloat( data ) ) );

        }

      }

    } else if ( next === MESH_MATRIX ) {

      this.debugMessage( '   Tranformation Matrix (TODO)' );

      var values = [];
      for ( var i = 0; i < 12; i ++ ) {

        values[ i ] = this.readFloat( data );

      }

      var matrix = new THREE.Matrix4();

      //X Line
      matrix.elements[ 0 ] = values[ 0 ];
      matrix.elements[ 1 ] = values[ 6 ];
      matrix.elements[ 2 ] = values[ 3 ];
      matrix.elements[ 3 ] = values[ 9 ];

      //Y Line
      matrix.elements[ 4 ] = values[ 2 ];
      matrix.elements[ 5 ] = values[ 8 ];
      matrix.elements[ 6 ] = values[ 5 ];
      matrix.elements[ 7 ] = values[ 11 ];

      //Z Line
      matrix.elements[ 8 ] = values[ 1 ];
      matrix.elements[ 9 ] = values[ 7 ];
      matrix.elements[ 10 ] = values[ 4 ];
      matrix.elements[ 11 ] = values[ 10 ];

      //W Line
      matrix.elements[ 12 ] = 0;
      matrix.elements[ 13 ] = 0;
      matrix.elements[ 14 ] = 0;
      matrix.elements[ 15 ] = 1;

      matrix.transpose();

      var inverse = new THREE.Matrix4();
      inverse.getInverse( matrix, true );
      geometry.applyMatrix( inverse );

      matrix.decompose( mesh.position, mesh.quaternion, mesh.scale );

    } else {

      this.debugMessage( '   Unknown mesh chunk: ' + next.toString( 16 ) );

    }

    next = this.nextChunk( data, chunk );
#endif
  }
#if 0
  if ( ! useBufferGeometry ) {

    //geometry.faceVertexUvs[0][faceIndex][vertexIndex]

    if ( uvs.length > 0 ) {

      var faceUV = [];

      for ( var i = 0; i < geometry.faces.length; i ++ ) {

        faceUV.push( [ uvs[ geometry.faces[ i ].a ], uvs[ geometry.faces[ i ].b ], uvs[ geometry.faces[ i ].c ] ] );

      }

      geometry.faceVertexUvs[ 0 ] = faceUV;

    }
    geometry.computeVertexNormals();
  }
#endif

  //auto mesh = MeshT<BufferGeometry, MeshPhongMaterial>::make(name, );
  return nullptr;
}

void TDS::readMaterialEntry(std::istream &in, ResourceLoader &loader, Chunk &chunk)
{
}

}
}