//
// Created by byter on 1/6/18.
//

#include "Assimp.h"

#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <threepp/core/BufferGeometry.h>
#include <threepp/material/MeshPhongMaterial.h>
#include <threepp/material/MeshLambertMaterial.h>
#include <threepp/material/MeshToonMaterial.h>
#include <threepp/textures/ImageTexture.h>
#include <threepp/textures/DataTexture.h>

#include <QDebug>

namespace three {
namespace loader {

using namespace std;
namespace as = ::Assimp;

class IOStream : public as::IOStream
{
  friend class IOSystem;

  Resource::Ptr _resource;

protected:
  IOStream(Resource::Ptr loader) : _resource(loader)
  {}

public:
  ~IOStream() {}

  size_t Read(void *pvBuffer, size_t pSize, size_t pCount) override
  {
    _resource->in().read((char *) pvBuffer, pSize * pCount);
    return (size_t) _resource->in().gcount();
  }

  size_t Write(const void *pvBuffer, size_t pSize, size_t pCount) override
  {
    throw logic_error("write not supported");
  }

  aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override
  {
    switch (pOrigin) {
      case aiOrigin_SET:
        _resource->in().seekg(pOffset, ios_base::beg);
        break;
      case aiOrigin_CUR:
        _resource->in().seekg(pOffset, ios_base::cur);
        break;
      case aiOrigin_END:
        _resource->in().seekg(pOffset, ios_base::end);
        break;
    }
    return _resource->in().fail() ? aiReturn_FAILURE : aiReturn_SUCCESS;
  }

  size_t Tell() const override
  {
    return (size_t) _resource->in().tellg();
  }

  size_t FileSize() const override
  {
    return _resource->size();
  }

  void Flush() override
  {
    throw logic_error("flush not supported");
  }
};

class IOSystem : public as::IOSystem
{
  ResourceLoader &_loader;

public:
  IOSystem(ResourceLoader &loader) : _loader(loader)
  {}

  ~IOSystem()
  {}

  bool Exists(const char *pFile) const override
  {
    return _loader.exists(pFile);
  }

  char getOsSeparator() const override
  {
    return '/';
  }

  as::IOStream *Open(const char *pFile, const char *pMode) override
  {
    if (!strcmp(pMode, "rb")) {
      return new IOStream(_loader.get(pFile, ios_base::in | ios_base::binary));
    }
    else if (!strcmp(pMode, "rt") || !strcmp(pMode, "r")) {
      return new IOStream(_loader.get(pFile, ios_base::in));
    }
    else throw logic_error("flush not supported");
  }

  void Close(as::IOStream *pFile) override
  {
    delete pFile;
  }
};

class ProgressHandler : public as::ProgressHandler
{
public:
  bool Update(float percentage) override
  {
    return false;
  }

  void UpdateFileRead(int currentStep, int numberOfSteps) override
  {
    as::ProgressHandler::UpdateFileRead(currentStep, numberOfSteps);
  }

  void UpdatePostProcess(int currentStep, int numberOfSteps) override
  {
    as::ProgressHandler::UpdatePostProcess(currentStep, numberOfSteps);
  }
};

class MeshMaker
{
public:
  using Ptr = shared_ptr<MeshMaker>;

  virtual Mesh::Ptr makeMesh(BufferGeometry::Ptr geometry) = 0;
  virtual Material &material() = 0;
};

struct Access
{
  friend class MeshMaker;

  Scene::Ptr scene;
  const aiScene * aiscene;
  ResourceLoader &loader;

  unordered_map<string, QImage> images;
  unordered_map<unsigned, Mesh::Ptr> meshes;
  unordered_map<unsigned, MeshMaker::Ptr> makers;

  Access(Scene::Ptr scene, const aiScene * aiscene, ResourceLoader &loader)
     : scene(scene), aiscene(aiscene), loader(loader) {}

  void readMaterial(unsigned materialIndex);

  BufferAttributeT<float>::Ptr readUVChannel(unsigned index, const aiMesh *mesh);

  Mesh::Ptr readMesh(int index);

  void readObject(const aiNode *ai, Object3D::Ptr object);

  void readScene()
  {
    for(unsigned i=0; i<aiscene->mNumMaterials; i++) {
      readMaterial(i);
    }

    readObject(aiscene->mRootNode, scene);

    for(int i=0; i<aiscene->mNumMeshes; i++) {
      //hookupSkeletons(ai->mMeshes[i], scene);
    }
    if(aiscene->mNumAnimations > 0) {
      //Animation::Ptr animation = Animation::make();
      //readAnimation(ai->mAnimations[0], animation);
    }
  }

  Texture::Ptr loadTexture(aiTextureType type, unsigned index, const aiMaterial *material);
};

template <typename ... Maps>
struct ReadMaterial;

template <typename Map, typename ... Maps>
struct ReadMaterial<Map, Maps...>
{
  template <typename ... Ms>
  static void mixin(MaterialT<Ms...> &material, const aiMaterial *ai, Access *access) {
    ReadMaterial<Map>::mixin(material, ai, access);
    ReadMaterial<Maps...>::mixin(material, ai, access);
  }
  static void read(MaterialT<Map, Maps...> &material, const aiMaterial *ai, Access *access) {
    ReadMaterial<Map>::mixin(material, ai, access);
    ReadMaterial<Maps...>::mixin(material, ai, access);
  }
};

#define FORWARD_MIXIN(x) \
static void read(MaterialT<x> &material, const aiMaterial *ai, Access *access) { \
  mixin(material, ai, access); \
}

inline void read_color(const char* pKey,unsigned int type, unsigned int idx, const aiMaterial *ai, Color &color)
{
  aiColor4D c4D;
  if(ai->Get(pKey, type, idx, c4D) == AI_SUCCESS) {
    color.r = c4D.r;
    color.g = c4D.g;
    color.b = c4D.b;
  }
}

template <>
struct ReadMaterial<material::Colored>
{
  FORWARD_MIXIN(material::Colored)
  static void mixin(material::Colored &material, const aiMaterial *ai, Access *access) {
    read_color(AI_MATKEY_COLOR_DIFFUSE, ai, material.color);
    ai->Get(AI_MATKEY_OPACITY, material.opacity);
  }
};
template <>
struct ReadMaterial<material::LightMap>
{
  FORWARD_MIXIN(material::LightMap)
  static void mixin(material::LightMap &material, const aiMaterial *ai, Access *access) {
    material.lightMap = access->loadTexture(aiTextureType_LIGHTMAP, 0, ai);
  }
};
template <>
struct ReadMaterial<material::Emissive>
{
  FORWARD_MIXIN(material::Emissive)
  static void mixin(material::Emissive &material, const aiMaterial *ai, Access *access) {
    material.emissiveMap = access->loadTexture(aiTextureType_EMISSIVE, 0, ai);
    read_color(AI_MATKEY_COLOR_EMISSIVE, ai, material.emissive);
  }
};
template <>
struct ReadMaterial<material::AoMap>
{
  FORWARD_MIXIN(material::AoMap)
  static void mixin(material::AoMap &material, const aiMaterial *ai, Access *access) {
    material.aoMap = access->loadTexture(aiTextureType_AMBIENT, 0, ai);
  }
};
template <>
struct ReadMaterial<material::EnvMap>
{
  FORWARD_MIXIN(material::EnvMap)
  static void mixin(material::EnvMap &material, const aiMaterial *ai, Access *access) {
    material.envMap = access->loadTexture(aiTextureType_REFLECTION, 0, ai);
    ai->Get(AI_MATKEY_REFLECTIVITY, material.reflectivity);
    ai->Get(AI_MATKEY_REFRACTI, material.refractionRatio);
  }
};
template <>
struct ReadMaterial<material::AlphaMap>
{
  FORWARD_MIXIN(material::AlphaMap)
  static void mixin(material::AlphaMap &material, const aiMaterial *ai, Access *access) {
    material.alphaMap = access->loadTexture(aiTextureType_OPACITY, 0, ai);
  }
};
template <>
struct ReadMaterial<material::SpecularMap>
{
  FORWARD_MIXIN(material::SpecularMap)
  static void mixin(material::SpecularMap &material, const aiMaterial *ai, Access *access) {
    material.specularMap = access->loadTexture(aiTextureType_SPECULAR, 0, ai);
  }
};
template <>
struct ReadMaterial<material::Specular>
{
  FORWARD_MIXIN(material::Specular)
  static void mixin(material::Specular &material, const aiMaterial *ai, Access *access) {
    material.specularMap = access->loadTexture(aiTextureType_SPECULAR, 0, ai);
    read_color(AI_MATKEY_COLOR_SPECULAR, ai, material.specular);
    ai->Get(AI_MATKEY_SHININESS, material.shininess);

    float scale;
    if(ai->Get(AI_MATKEY_SHININESS_STRENGTH, scale) == AI_SUCCESS) material.specular *= scale;
  }
};
template <>
struct ReadMaterial<material::DisplacementMap>
{
  FORWARD_MIXIN(material::DisplacementMap)
  static void mixin(material::DisplacementMap &material, const aiMaterial *ai, Access *access) {
    material.displacementMap = access->loadTexture(aiTextureType_DISPLACEMENT, 0, ai);
  }
};
template <>
struct ReadMaterial<material::BumpMap>
{
  FORWARD_MIXIN(material::BumpMap)
  static void mixin(material::BumpMap &material, const aiMaterial *ai, Access *access) {
    material.bumpMap = access->loadTexture(aiTextureType_HEIGHT, 0, ai);
    ai->Get(AI_MATKEY_BUMPSCALING, material.bumpScale);
  }
};
template <>
struct ReadMaterial<material::NormalMap>
{
  FORWARD_MIXIN(material::NormalMap)
  static void mixin(material::NormalMap &material, const aiMaterial *ai, Access *access) {
    material.normalMap = access->loadTexture(aiTextureType_NORMALS, 0, ai);
  }
};

template <typename Mat>
class MeshMakerT : public MeshMaker
{
  using MatP = const typename Mat::Ptr;
  MatP _material;

protected:
  explicit MeshMakerT(MatP mat) : _material(mat) {}

  template <typename ... Maps>
  static void read(MaterialT<Maps...> &material, const aiMaterial *ai, Access *access)
  {
    ReadMaterial<Maps...>::read(material, ai, access);

    aiColor4D tmp_c;
    aiString tmp_s;
    if(ai->Get(AI_MATKEY_COLOR_REFLECTIVE, tmp_c) == AI_SUCCESS)
      qWarning() << "COLOR_REFLECTIVE found, currently not used";
    if(ai->Get(AI_MATKEY_COLOR_TRANSPARENT, tmp_c) == AI_SUCCESS)
      qWarning() << "COLOR_TRANSPARENT found, currently not used";
    if(ai->Get(AI_MATKEY_GLOBAL_BACKGROUND_IMAGE, tmp_s) == AI_SUCCESS)
      qWarning() << "GLOBAL_BACKGROUND_IMAGE found, currently not used";

    read_color(AI_MATKEY_COLOR_AMBIENT, ai, material.ambientColor);

    material.map = access->loadTexture(aiTextureType_DIFFUSE, 0, ai);
    if(!material.map)
      material.map = access->loadTexture(aiTextureType_UNKNOWN, 0, ai);

    aiUVTransform transform;
    if(ai->Get(AI_MATKEY_UVTRANSFORM_DIFFUSE(0), transform) == AI_SUCCESS) {
      if(transform.mRotation != 0 || transform.mScaling.x != 1 || transform.mScaling.y != 1
         || transform.mTranslation.x != 0 || transform.mTranslation.y != 0)
      qWarning() << "UVTRANSFORM_DIFFUSE found, currently not supported";
    }

    auto tex = access->loadTexture(aiTextureType_SHININESS, 0, ai);
    if(tex) qWarning() << "found SHININESS texture, currently not supported";

    aiString name;
    ai->Get(AI_MATKEY_NAME, name);
    material.name = name.C_Str();

    int twosided;
    if(ai->Get(AI_MATKEY_TWOSIDED, twosided) == AI_SUCCESS)
      material.side = twosided ? Side::Double : Side::Front;

    int blendFunc;
    if(ai->Get(AI_MATKEY_BLEND_FUNC, blendFunc) == AI_SUCCESS) {
      switch(blendFunc) {
        case aiBlendMode_Default:
          material.blendSrcAlpha = BlendFunc::SrcAlpha;
          material.blendDstAlpha = BlendFunc::OneMinusSrcAlpha;
          material.blendEquationAlpha = BlendEq::Add;
          break;
        case aiBlendMode_Additive:
          material.blendSrcAlpha = BlendFunc::None;
          material.blendDstAlpha = BlendFunc::None;
          material.blendEquationAlpha = BlendEq::Add;
          break;
      }
    }

    ai->Get(AI_MATKEY_ENABLE_WIREFRAME, material.wireframe);
  }

public:
  Mesh::Ptr makeMesh(BufferGeometry::Ptr geometry) override {
    Mesh::Ptr m = MeshT<BufferGeometry, Mat>::make(geometry, _material);
    m->setName(_material->name);
    return m;
  }

  Material &material() override {return *_material;}

  static Ptr make(Access *access, const aiMaterial *ai)
  {
    auto material = Mat::make();
    read(*material, ai, access);

    return Ptr(new MeshMakerT(material));
  }
};

Texture::Ptr Access::loadTexture(aiTextureType type, unsigned index, const aiMaterial *material)
{
  aiString path;
  aiTextureMapping mapping;
  unsigned int uvindex = numeric_limits<unsigned>::max();
  ai_real blend;
  aiTextureOp op = (aiTextureOp)numeric_limits<int>::max();
  aiTextureMapMode mapmode[3];

  if(material->GetTexture(type, index, &path, &mapping, &uvindex, &blend, &op, mapmode) == AI_SUCCESS) {

    if(uvindex != numeric_limits<unsigned>::max()) {
      qWarning() << "explicit UV index not used";
    }
    if(path.data[0] == '*') {
      unsigned index = atoi(path.data + 1);
      aiTexture *tex = aiscene->mTextures[index];

      TextureOptions options = DataTexture::options();
      //return DataTexture::make(options, image);
      qWarning() << path.C_Str()  << ": embedded textures not (yet) supported";
    }
    else {
      string imageFile(path.C_Str());
      QImage image;
      if(images.count(imageFile) > 0)
        image = images[imageFile];
      else {
        loader.load(image, imageFile);
        images[imageFile] = image;
      }
      const char *textype;
      switch(type) {
        case aiTextureType_DIFFUSE:
          textype = "diffuse";
          break;
        case aiTextureType_SPECULAR:
          textype = "specular";
          break;
        case aiTextureType_AMBIENT:
          textype = "ambient";
          break;
        case aiTextureType_EMISSIVE:
          textype = "emissive";
          break;
        case aiTextureType_HEIGHT:
          textype = "height";
          break;
        case aiTextureType_NORMALS:
          textype = "normals";
          break;
        case aiTextureType_SHININESS:
          textype = "shininess";
          break;
        case aiTextureType_OPACITY:
          textype = "opacity";
          break;
        case aiTextureType_DISPLACEMENT:
          textype = "displacement";
          break;
        case aiTextureType_LIGHTMAP:
          textype = "lightmap";
          break;
        case aiTextureType_REFLECTION:
          textype = "reflection";
          break;
        case aiTextureType_UNKNOWN:
          textype = "unknown";
          break;
        default:
          textype = "undefined";
      }
      qDebug() << "loaded texture " << imageFile.c_str() << "(" << textype << ")";
      TextureOptions options = ImageTexture::options();

      switch(mapping) {
        case aiTextureMapping_UV:
          break;
        default:
          qWarning() << path.C_Str()  << ": unsupported texture mapping (non-UV)";
      }
      bool isPo2 = math::isPowerOfTwo(image.width()) && math::isPowerOfTwo(image.height());
      if(isPo2) {
        switch(mapmode[0]) {
          case aiTextureMapMode_Wrap:
            options.wrapS = TextureWrapping::Repeat;
            break;
          case aiTextureMapMode_Clamp:
            options.wrapS = TextureWrapping::ClampToEdge;
            break;
          case aiTextureMapMode_Mirror:
            options.wrapS = TextureWrapping::MirroredRepeat;
            break;
        }
        switch(mapmode[1]) {
          case aiTextureMapMode_Wrap:
            options.wrapT = TextureWrapping::Repeat;
            break;
          case aiTextureMapMode_Clamp:
            options.wrapT = TextureWrapping::ClampToEdge;
            break;
          case aiTextureMapMode_Mirror:
            options.wrapT = TextureWrapping::MirroredRepeat;
            break;
        }
      }
      else {
        options.minFilter = TextureFilter::Linear;
        options.wrapS = options.wrapT = TextureWrapping::ClampToEdge;
      }
      if(op < (aiTextureOp)numeric_limits<int>::max()) {
        switch(op) {
          case aiTextureOp_Multiply:
            options.blending = Blending::Multiply;
            break;
          case aiTextureOp_SmoothAdd:
          case aiTextureOp_SignedAdd:
          case aiTextureOp_Add:
            options.blending = Blending::Additive;
            break;
          case aiTextureOp_Subtract:
            options.blending = Blending::Subtractive;
            break;
          case aiTextureOp_Divide:
            options.blending = Blending::Divide;
            break;
        }
      }
      return ImageTexture::make(options, image);
    }
  }
  return nullptr;
}

void Access::readObject(const aiNode *ai, Object3D::Ptr object)
{
  object->_name = ai->mName.C_Str();

  const aiMatrix4x4 & m = ai->mTransformation;
  object->_matrix.set(
     m.a1, m.a2, m.a3, m.a4, m.b1, m.b2, m.b3, m.b4, m.c1, m.c2, m.c3, m.c4, m.d1, m.d2, m.d3, m.d4);

  for(unsigned i=0; i<ai->mNumMeshes; i++) {
    Mesh::Ptr mesh = readMesh(ai->mMeshes[i]);
    object->add(mesh);
  }

  for(unsigned i=0; i<ai->mNumChildren; i++) {
    Node::Ptr node = Node::make();
    readObject(ai->mChildren[i], node);
    object->add(node);
  }

  object->_matrix.decompose(object->_position, object->_quaternion, object->_scale);
}

BufferAttributeT<float>::Ptr Access::readUVChannel(unsigned index, const aiMesh *ai)
{
  if(ai->mTextureCoords[index]) {
    if(ai->mNumUVComponents[index] != 2)
      qWarning() << ai->mNumUVComponents[index] << "UV components found, 2 used";

    auto uvs = attribute::prealloc<float, UV>(ai->mNumVertices);

    for(unsigned i=0; i<ai->mNumVertices; i++) {
      aiVector3t<float> &aiuv = ai->mTextureCoords[index][i];
      uvs->next().set(aiuv.x, aiuv.y);
    }
    return uvs;
  }
  return nullptr;
}

Mesh::Ptr Access::readMesh(int index)
{
  if(meshes.count(index) > 0) return meshes[index];

  aiMesh *ai = aiscene->mMeshes[index];
  Mesh::Ptr mesh;

  BufferGeometry::Ptr geometry = BufferGeometry::make();
  if(makers.count(ai->mMaterialIndex)) {
    mesh = makers[ai->mMaterialIndex]->makeMesh(geometry);
  }
  else {
    MeshLambertMaterial::Ptr mat = MeshLambertMaterial::make();
    mesh = Mesh::make(geometry, mat);
  }

  if(mesh->_name.empty())
    mesh->_name = ai->mName.C_Str();

  auto indices = attribute::growing<uint32_t>(true);

  for(unsigned i=0; i<ai->mNumFaces; i++) {
    aiFace &f = ai->mFaces[i];
    if ( f.mNumIndices == 3 ) {

      indices->next() = f.mIndices[ 0 ];
      indices->next() = f.mIndices[ 1 ];
      indices->next() = f.mIndices[ 2 ];
    }
    else if ( f.mNumIndices == 4 ) {

      indices->next() = f.mIndices[ 0 ];
      indices->next() = f.mIndices[ 1 ];
      indices->next() = f.mIndices[ 2 ];
      indices->next() = f.mIndices[ 2 ];
      indices->next() = f.mIndices[ 3 ];
      indices->next() = f.mIndices[ 0 ];
    }
  }
  geometry->setIndex(indices);

  geometry->setPosition(attribute::external<float, Vertex>(ai->mVertices, ai->mNumVertices));

  if(ai->mNormals) {
    geometry->setNormal(attribute::external<float, Vertex>(ai->mNormals, ai->mNumVertices));
  }
  if(ai->mColors[0]) {
    geometry->setColor(attribute::external<float, Color>(ai->mColors[0], ai->mNumVertices));
  }

  geometry->setUV(readUVChannel(0, ai));
  geometry->setUV2(readUVChannel(1, ai));

  if(ai->mTangents) {
    geometry->setTangents(attribute::external<float, Vertex>(ai->mTangents, ai->mNumVertices));
  }
  if(ai->mBitangents) {
    geometry->setBitangents(attribute::external<float, Vertex>(ai->mBitangents, ai->mNumVertices));
  }
#if 0
  if ( this.mTangentBuffer && this.mTangentBuffer.length > 0 )
      geometry.addAttribute( 'tangents', new THREE.BufferAttribute( this.mTangentBuffer, 3 ) );
    if ( this.mBitangentBuffer && this.mBitangentBuffer.length > 0 )
      geometry.addAttribute( 'bitangents', new THREE.BufferAttribute( this.mBitangentBuffer, 3 ) );
    if ( this.mBones.length > 0 ) {

      var weights = [];
      var bones = [];

      for ( var i = 0; i < this.mBones.length; i ++ ) {

        for ( var j = 0; j < this.mBones[ i ].mWeights.length; j ++ ) {

          var weight = this.mBones[ i ].mWeights[ j ];
          if ( weight ) {

            if ( ! weights[ weight.mVertexId ] ) weights[ weight.mVertexId ] = [];
            if ( ! bones[ weight.mVertexId ] ) bones[ weight.mVertexId ] = [];
            weights[ weight.mVertexId ].push( weight.mWeight );
            bones[ weight.mVertexId ].push( parseInt( i ) );

          }

        }

      }

      for ( var i in bones ) {

        sortWeights( bones[ i ], weights[ i ] );

      }

      var _weights = [];
      var _bones = [];

      for ( var i = 0; i < weights.length; i ++ ) {

        for ( var j = 0; j < 4; j ++ ) {

          if ( weights[ i ] && bones[ i ] ) {

            _weights.push( weights[ i ][ j ] );
            _bones.push( bones[ i ][ j ] );

          } else {

            _weights.push( 0 );
            _bones.push( 0 );

          }

        }

      }

      geometry.addAttribute( 'skinWeight', new THREE.BufferAttribute( new Float32Array( _weights ), BONESPERVERT ) );
      geometry.addAttribute( 'skinIndex', new THREE.BufferAttribute( new Float32Array( _bones ), BONESPERVERT ) );

    }

    var mesh;

    if ( this.mBones.length == 0 )
      mesh = new THREE.Mesh( geometry, mat );

    if ( this.mBones.length > 0 )
      mesh = new THREE.SkinnedMesh( geometry, mat );

    this.threeNode = mesh;
#endif
  //mesh.matrixAutoUpdate = false;
  return mesh;
}

void Access::readMaterial(unsigned materialIndex)
{
  MeshMaker::Ptr maker;
  const aiMaterial *ai = aiscene->mMaterials[materialIndex];

  int shadingModel;
  if(ai->Get(AI_MATKEY_SHADING_MODEL, shadingModel) == AI_SUCCESS) {

    switch(shadingModel) {
      case aiShadingMode_Phong:
      case aiShadingMode_Gouraud:
        maker = MeshMakerT<MeshPhongMaterial>::make(this, ai);
        break;
      case aiShadingMode_Toon:
        maker = MeshMakerT<MeshToonMaterial>::make(this, ai);
        break;
      default:
        maker = MeshMakerT<MeshLambertMaterial>::make(this, ai);
        break;
    }
  }
  else {
    float shininess;
    if(ai->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS && shininess > 0.0f) {
      maker = MeshMakerT<MeshPhongMaterial>::make(this, ai);
    }
    else {
      maker = MeshMakerT<MeshLambertMaterial>::make(this, ai);
    }
  }

  makers[materialIndex] = maker;
}

void Assimp::loadScene(string name, ResourceLoader &loader)
{
  _importer = make_shared<as::Importer>();
  _importer->SetIOHandler(new IOSystem(loader));
  _importer->SetProgressHandler(new ProgressHandler());

  const aiScene *aiscene = _importer->ReadFile(name.c_str(),
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_RemoveRedundantMaterials |
                                             aiProcess_GenUVCoords |
                                             aiProcess_OptimizeMeshes |
                                             aiProcess_GenNormals |
                                             aiProcess_FlipUVs |
                                             //aiProcess_RemoveComponent | //see AI_CONFIG_PP_RVC_FLAGS
                                             //aiProcess_OptimizeGraph |
                                             aiProcess_SortByPType);

  if (!aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
    onError.emitSignal(_importer->GetErrorString());
    qWarning() << _importer->GetErrorString();
    return;
  }

  Access access(_scene, aiscene, loader);
  access.readScene();
}

void Assimp::load(std::string name, Color background, ResourceLoader &loader)
{
  _scene = SceneT<Color>::make(name, background);
  loadScene(name, loader);
}

void Assimp::load(std::string name, Texture::Ptr background, ResourceLoader &loader)
{
  _scene = SceneT<Texture::Ptr>::make(name, background);
  loadScene(name, loader);
}

void Assimp::load(string name, ResourceLoader &loader)
{
  _scene = Scene::make(name);
  loadScene(name, loader);
}

Assimp::~Assimp()
{
  _importer = nullptr;
}

}
}
