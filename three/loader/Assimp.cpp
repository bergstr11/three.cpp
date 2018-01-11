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
#include <material/MeshPhongMaterial.h>
#include <material/MeshLambertMaterial.h>
#include <material/MeshToonMaterial.h>
#include <textures/DataTexture.h>

namespace three {
namespace loader {

using namespace std;
namespace as = ::Assimp;

class IOStream : public as::IOStream
{
  friend class IOSystem;

  istream &_in;

protected:
  IOStream(istream &in) : _in(in)
  {}

public:
  ~IOStream()
  {}

  size_t Read(void *pvBuffer, size_t pSize, size_t pCount) override
  {
    _in.read((char *) pvBuffer, pSize * pCount);
    return (size_t) _in.gcount();
  }

  size_t Write(const void *pvBuffer, size_t pSize, size_t pCount) override
  {
    throw logic_error("write not supported");
  }

  aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override
  {
    switch (pOrigin) {
      case aiOrigin_SET:
        _in.seekg(pOffset, ios_base::beg);
        break;
      case aiOrigin_CUR:
        _in.seekg(pOffset, ios_base::cur);
        break;
      case aiOrigin_END:
        _in.seekg(pOffset, ios_base::end);
        break;
    }
    return _in.fail() ? aiReturn_FAILURE : aiReturn_SUCCESS;
  }

  size_t Tell() const override
  {
    return (size_t) _in.tellg();
  }

  size_t FileSize() const override
  {
    auto pos = _in.tellg();
    _in.seekg(0, ios_base::end);
    size_t size = (size_t) _in.tellg();
    _in.seekg(pos, ios_base::beg);
    return size;
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
      return new IOStream(_loader.istream(pFile, ios_base::binary));
    }
    else if (!strcmp(pMode, "rt") || !strcmp(pMode, "r")) {
      return new IOStream(_loader.istream(pFile, ios_base::in));
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
    ProgressHandler::UpdateFileRead(currentStep, numberOfSteps);
  }

  void UpdatePostProcess(int currentStep, int numberOfSteps) override
  {
    ProgressHandler::UpdatePostProcess(currentStep, numberOfSteps);
  }
};

void ai2Three(math::Matrix4 &matrix, const aiMatrix4x4 &ai)
{
  matrix.set(ai.a1, ai.a2, ai.a3, ai.a4, ai.b1, ai.b2, ai.b3, ai.b4, ai.c1, ai.c2,
             ai.c3, ai.c4, ai.d1, ai.d2, ai.d3, ai.d4);
}

static Texture::Ptr loadTexture(const aiString &path, const aiScene *aiscene)
{
  return nullptr;
}

template <typename ... Maps>
struct ReadMaterial;

template <typename Map, typename ... Maps>
struct ReadMaterial<Map, Maps...>
{
  template <typename ... Ms>
  static void mixin(MaterialT<Ms...> &material, const aiMaterial *ai, const aiScene *aiscene) {
    ReadMaterial<Map>::mixin(material, ai, aiscene);
    ReadMaterial<Maps...>::mixin(material, ai, aiscene);
  }
  static void read(MaterialT<Map, Maps...> &material, const aiMaterial *ai, const aiScene *aiscene) {
    ReadMaterial<Map>::mixin(material, ai, aiscene);
    ReadMaterial<Maps...>::mixin(material, ai, aiscene);
  }
};

#define FORWARD_MIXIN(x) \
static void read(MaterialT<x> &material, const aiMaterial *ai, const aiScene *aiscene) { \
  mixin(material, ai, aiscene); \
}

template <>
struct ReadMaterial<>
{
  FORWARD_MIXIN()
  static void mixin(Material &material, const aiMaterial *ai, const aiScene *aiscene) {
    //material.map = ai;
  }
};
template <>
struct ReadMaterial<material::Colored>
{
  FORWARD_MIXIN(material::Colored)
  static void mixin(material::Colored &material, const aiMaterial *ai, const aiScene *aiscene) {
    ai->Get(AI_MATKEY_COLOR_DIFFUSE, material.color);
    ai->Get(AI_MATKEY_OPACITY, material.opacity);
  }
};
template <>
struct ReadMaterial<material::LightMap>
{
  FORWARD_MIXIN(material::LightMap)
  static void mixin(material::LightMap &material, const aiMaterial *ai, const aiScene *aiscene) {
    aiString path;	// filename
    if(ai->GetTexture(aiTextureType_LIGHTMAP, 0, &path) == AI_SUCCESS)
      material.lightMap = loadTexture(path, aiscene);
    //material.lightMapIntensity;
  }
};
template <>
struct ReadMaterial<material::EmissiveMap>
{
  FORWARD_MIXIN(material::EmissiveMap)
  static void mixin(material::EmissiveMap &material, const aiMaterial *ai, const aiScene *aiscene) {
    aiString path;
    if(ai->GetTexture(aiTextureType_EMISSIVE, 0, &path) == AI_SUCCESS)
      material.emissiveMap = loadTexture(path, aiscene);
    ai->Get(AI_MATKEY_COLOR_EMISSIVE, material.emissive);
    //material.emissive *= material.emissiveIntensity;
  }
};
template <>
struct ReadMaterial<material::AoMap>
{
  FORWARD_MIXIN(material::AoMap)
  static void mixin(material::AoMap &material, const aiMaterial *ai, const aiScene *aiscene) {
    aiString path;
    if(ai->GetTexture(aiTextureType_AMBIENT, 0, &path) == AI_SUCCESS)
      material.aoMap = loadTexture(path, aiscene);
    //material.aoMapIntensity;
    ai->Get(AI_MATKEY_COLOR_AMBIENT, material.ambient);
  }
};
template <>
struct ReadMaterial<material::EnvMap>
{
  FORWARD_MIXIN(material::EnvMap)
  static void mixin(material::EnvMap &material, const aiMaterial *ai, const aiScene *aiscene) {
    aiString path;
    if(ai->GetTexture(aiTextureType_REFLECTION, 0, &path) == AI_SUCCESS)
      material.envMap = loadTexture(path, aiscene);

    //material.reflectivity;
    //material.refractionRatio;
    ai->Get(AI_MATKEY_COLOR_REFLECTIVE, material.reflective);
  }
};
template <>
struct ReadMaterial<material::AlphaMap>
{
  FORWARD_MIXIN(material::AlphaMap)
  static void mixin(material::AlphaMap &material, const aiMaterial *ai, const aiScene *aiscene) {
    aiString path;
    if(ai->GetTexture(aiTextureType_OPACITY, 0, &path) == AI_SUCCESS)
      material.alphaMap = loadTexture(path, aiscene);
    //ai->Get(AI_MATKEY_SHININESS, material.shininess);
  }
};
template <>
struct ReadMaterial<material::SpecularMap>
{
  FORWARD_MIXIN(material::SpecularMap)
  static void mixin(material::SpecularMap &material, const aiMaterial *ai, const aiScene *aiscene) {
    aiString path;
    if(ai->GetTexture(aiTextureType_SPECULAR, 0, &path) == AI_SUCCESS)
      material.specularMap = loadTexture(path, aiscene);
    ai->Get(AI_MATKEY_COLOR_SPECULAR, material.specular);
  }
};
template <>
struct ReadMaterial<material::DisplacementMap>
{
  FORWARD_MIXIN(material::DisplacementMap)
  static void mixin(material::DisplacementMap &material, const aiMaterial *ai, const aiScene *aiscene) {
    aiString path;
    if(ai->GetTexture(aiTextureType_DISPLACEMENT, 0, &path) == AI_SUCCESS)
      material.displacementMap = loadTexture(path, aiscene);
  }
};
template <>
struct ReadMaterial<material::BumpMap>
{
  FORWARD_MIXIN(material::BumpMap)
  static void mixin(material::BumpMap &material, const aiMaterial *ai, const aiScene *aiscene) {
    aiString path;
    if(ai->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS)
      material.bumpMap = loadTexture(path, aiscene);
  }
};
template <>
struct ReadMaterial<material::NormalMap>
{
  FORWARD_MIXIN(material::NormalMap)
  static void mixin(material::NormalMap &material, const aiMaterial *ai, const aiScene *aiscene) {
    aiString path;
    if(ai->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
      material.normalMap = loadTexture(path, aiscene);
  }
};

class MeshMaker
{
public:
  using Ptr = shared_ptr<MeshMaker>;

  virtual Mesh::Ptr makeMesh(BufferGeometry::Ptr geometry) = 0;
  virtual Material &material() = 0;
};

template <typename Mat>
class MeshMakerT : public MeshMaker
{
  using MatP = const typename Mat::Ptr;
  MatP _material;

protected:
  explicit MeshMakerT(MatP mat) : _material(mat) {}

  template <typename ... Maps>
  static void read(MaterialT<Maps...> &material, const aiMaterial *ai, const aiScene *aiscene)
  {
    ReadMaterial<Maps...>::read(material, ai, aiscene);
  }

public:
  Mesh::Ptr makeMesh(BufferGeometry::Ptr geometry) override {
    return MeshT<BufferGeometry, Mat>::make(geometry, _material);
  }

  Material &material() override {return *_material;}

  static Ptr make(const aiScene *aiscene, const aiMaterial *ai)
  {
    auto material = Mat::make();
    read(*material, ai, aiscene);

    return Ptr(new MeshMakerT(material));
  }
};

struct Access
{
  Scene::Ptr scene;
  const aiScene * aiscene;

  unordered_map<unsigned, Mesh::Ptr> meshes;
  unordered_map<unsigned, MeshMaker::Ptr> makers;

  Access(Scene::Ptr scene, const aiScene * aiscene)
     : scene(scene), aiscene(aiscene) {}

  void readMaterial(unsigned materialIndex)
  {
    MeshMaker::Ptr maker;
    const aiMaterial *ai = aiscene->mMaterials[materialIndex];

    aiString name;
    ai->Get(AI_MATKEY_NAME,name);

    int shadingModel;
    if(ai->Get(AI_MATKEY_SHADING_MODEL, shadingModel) == AI_SUCCESS) {
      if(shadingModel == aiShadingMode_Phong) {
        maker = MeshMakerT<MeshPhongMaterial>::make(aiscene, ai);
      }
      else if(shadingModel == aiShadingMode_Toon) {
        maker = MeshMakerT<MeshToonMaterial>::make(aiscene, ai);
      }
      else {
        maker = MeshMakerT<MeshLambertMaterial>::make(aiscene, ai);
      }
    }
    else {
      float shininess;
      if(ai->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS && shininess > 0.0f) {
        maker = MeshMakerT<MeshPhongMaterial>::make(aiscene, ai);
      }
      else {
        maker = MeshMakerT<MeshLambertMaterial>::make(aiscene, ai);
      }
    }

    unsigned wireframe;
    if(ai->Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe) == AI_SUCCESS)
      maker->material().wireframe = (bool) wireframe;

    int twosided;
    if(ai->Get(AI_MATKEY_TWOSIDED, twosided) == AI_SUCCESS)
      maker->material().side = twosided ? Side::Double : Side::Front;

    int blendFunc;
    ai->Get(AI_MATKEY_BLEND_FUNC, blendFunc);

    //ai->GetTexture(aiTextureType_DIFFUSE, 0, &path);
    //ai->GetTexture(aiTextureType_SHININESS, 0, &path);

    makers[materialIndex] = maker;
  }

  Mesh::Ptr readMesh(int index)
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
      mesh = MeshT<BufferGeometry, MeshLambertMaterial>::make(geometry, mat);
    }

    vector<uint32_t> indices;
    for(unsigned i=0; i<ai->mNumFaces; i++) {
      aiFace &f = ai->mFaces[i];
      if ( f.mNumIndices == 3 ) {

        indices.push_back( f.mIndices[ 0 ] );
        indices.push_back( f.mIndices[ 1 ] );
        indices.push_back( f.mIndices[ 2 ] );
      }
      else if ( f.mNumIndices == 4 ) {

        indices.push_back( f.mIndices[ 0 ] );
        indices.push_back( f.mIndices[ 1 ] );
        indices.push_back( f.mIndices[ 2 ] );
        indices.push_back( f.mIndices[ 2 ] );
        indices.push_back( f.mIndices[ 3 ] );
        indices.push_back( f.mIndices[ 0 ] );
      }
    }
    geometry->setIndex(indices);

    vector<Vertex> vertices;
    for(unsigned i=0; i<ai->mNumVertices; i++) {
      aiVector3D &vertex = ai->mVertices[i];
      vertices.emplace_back(vertex.x, vertex.y, vertex.z);
    }
    geometry->setPosition(BufferAttributeT<float>::make(vertices) );

    if(ai->mNormals) {
      vector<Vertex> normals;
      for(unsigned i=0; i<ai->mNumVertices; i++) {
        aiVector3D &normal = ai->mNormals[i];
        normals.emplace_back(normal.x, normal.y, normal.z);
      }
      geometry->setNormal(BufferAttributeT<float>::make(normals) );
    }

    if(ai->mColors[0]) {
      vector<Color> colors;
      for(unsigned i=0; i<ai->mNumVertices; i++) {
        aiColor4D &color = ai->mColors[0][i];
        colors.emplace_back(color.r, color.g, color.b);
      }
      geometry->setColor(BufferAttributeT<float>::make(colors) );
    }

    if(ai->mTextureCoords[0]) {
      vector<UV> uvs;
      for(unsigned i=0; i<ai->mNumVertices; i++) {
        aiVector3D &coord = ai->mTextureCoords[0][i];
        uvs.emplace_back(coord.x, coord.y);
      }
      geometry->setUV((BufferAttributeT<float>::make(uvs)));
    }
    if(ai->mTextureCoords[1]) {
      vector<UV> uvs;
      for(unsigned i=0; i<ai->mNumVertices; i++) {
        aiVector3D *coord = ai->mTextureCoords[0];
        uvs.emplace_back(coord->x, coord->y);
      }
      geometry->setUV2((BufferAttributeT<float>::make(uvs)));
    }
    if(ai->mTangents) {

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

  Object3D::Ptr readObject(const aiNode *ai, Object3D::Ptr object=nullptr)
  {
    if(!object) {
      object = Objects::make();
    }
    object->_name = ai->mName.C_Str();
    ai2Three(object->_matrix, ai->mTransformation);

    for(unsigned i=0; i<ai->mNumChildren; i++) {
      auto child = readObject(ai->mChildren[i]);
      object->add(child);
    }

    for(unsigned i=0; i<ai->mNumMeshes; i++) {
      Mesh::Ptr mesh = readMesh(ai->mMeshes[i]);
      object->add(mesh);
    }
    object->_matrix.decompose(object->_position, object->_quaternion, object->_scale);

    return object;
  }

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
};

void Assimp::loadScene(string name, ResourceLoader &loader)
{
  as::Importer importer;
  importer.SetIOHandler(new IOSystem(loader));
  importer.SetProgressHandler(new ProgressHandler());

  const aiScene *aiscene = importer.ReadFile(name.c_str(),
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_RemoveRedundantMaterials |
                                             aiProcess_GenUVCoords |
                                             aiProcess_OptimizeMeshes |
                                             aiProcess_OptimizeGraph |
                                             aiProcess_SortByPType);

  if (!aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
    onError.emitSignal(importer.GetErrorString());
    return;
  }

  Access access(_scene, aiscene);
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

}
}