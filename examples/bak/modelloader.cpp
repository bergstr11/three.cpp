#include "modelloader.h"
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <QDebug>
#include <QThread>
#include <limits>

#define DEBUGOUTPUT_NORMALS(nodeIndex) (false)//( QList<int>{1}.contains(nodeIndex) )//(false)

namespace lo {
namespace threed {

using namespace std;

/*
 * offload work to a thread
 */
class LoaderThread : public QThread
{
Q_OBJECT
  ModelLoader *const m_loader;
  const QFileInfo m_file;

public:
  LoaderThread(QFileInfo file, ModelLoader *loader) : m_file(file), m_loader(loader)
  {}

protected:
  void run() override
  {
    m_loader->synchLoad(m_file);
    emit done();
  }

signals:

  void done();
};

ModelLoader::ModelLoader(bool transformToUnitCoordinates) :
   m_transformToUnitCoordinates(transformToUnitCoordinates)
{

}

void ModelLoader::asynchLoad(QFileInfo file)
{
  LoaderThread *loaderThread = new LoaderThread(file, this);
  connect(loaderThread, &LoaderThread::done, this, &ModelLoader::modelLoaded);
  connect(loaderThread, &LoaderThread::finished, loaderThread, &QObject::deleteLater);

  qDebug() << "thread start";
  loaderThread->start();
}

Model::Ptr ModelLoader::synchLoad(QFileInfo file)
{
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(file.absoluteFilePath().toStdString(),
                                           aiProcess_GenSmoothNormals |
                                           aiProcess_CalcTangentSpace |
                                           aiProcess_Triangulate |
                                           aiProcess_JoinIdenticalVertices |
                                           aiProcess_SortByPType
  );

  if (!scene) {
    qDebug() << "Error loading file: (assimp:) " << importer.GetErrorString();
    return nullptr;
  }

  Model::Ptr model = make_shared<Model>();

  if (scene->HasMaterials()) {
    for (unsigned int ii = 0; ii < scene->mNumMaterials; ++ii) {
      processMaterial(model, scene->mMaterials[ii]);
    }
  }

  if (scene->HasMeshes()) {
    for (unsigned int ii = 0; ii < scene->mNumMeshes; ++ii) {
      processMesh(model, scene->mMeshes[ii]);
    }
  }
  else {
    qDebug() << "Error: No meshes found";
    return nullptr;
  }

  if (scene->HasLights()) {
    qDebug() << "Has Lights";
  }

  if (scene->mRootNode != NULL) {
    processNode(model, scene, scene->mRootNode, nullptr);
  }
  else {
    qDebug() << "Error loading model";
    return nullptr;
  }

  // This will transform the model to unit coordinates, so a model of any size or shape will fit on screen
  if (m_transformToUnitCoordinates)
    model->transformToUnitCoordinates();

  m_loaded = model;
  return model;
}

void loadTextureStack(Material::Ptr material, aiMaterial *aiMat, aiTextureType textureType, unsigned int count)
{
  for (uint i = 0; i < count; i++) {
    aiString texPath;
    aiTextureMapping mapping;
    unsigned int uvindex;
    float blend;
    aiTextureOp op;
    aiTextureMapMode mapmode;

    if (aiMat->GetTexture(textureType, i, &texPath, &mapping, &uvindex, &blend, &op, &mapmode) == AI_SUCCESS) {

      Texture::Ptr texture = make_shared<Texture>(texPath.data, mapping, uvindex, blend, op, mapmode);
      material->textures[textureType].push_back(texture);
      qDebug() << "loaded texture" << i << "type" << textureType << ": " << texture->path;
    }
    else
      qWarning() << "Failed to get texture" << i << "for material";
  }
}

void ModelLoader::processMaterial(Model::Ptr model, aiMaterial *aiMat)
{
  Material::Ptr material = make_shared<Material>();

  aiString mname;
  aiMat->Get(AI_MATKEY_NAME, mname);
  if (mname.length > 0)
    material->name = mname.C_Str();

  int shadingModel;
  aiMat->Get(AI_MATKEY_SHADING_MODEL, shadingModel);

  if (shadingModel != aiShadingMode_Phong && shadingModel != aiShadingMode_Gouraud) {
    qWarning() << "This mesh's shading model is not implemented in this loader, setting to default material";
    material->name = "DefaultMaterial";
  }
  else {
    aiColor3D dif(0.f, 0.f, 0.f);
    aiColor3D amb(0.f, 0.f, 0.f);
    aiColor3D spec(0.f, 0.f, 0.f);

    aiMat->Get(AI_MATKEY_COLOR_AMBIENT, amb);
    aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, dif);
    aiMat->Get(AI_MATKEY_COLOR_SPECULAR, spec);
    aiMat->Get(AI_MATKEY_OPACITY, material->opacity);
    aiMat->Get(AI_MATKEY_SHININESS, material->shininess);
    aiMat->Get(AI_MATKEY_TWOSIDED, material->twosided);

    material->ambient = QVector3D(amb.r, amb.g, amb.b);
    material->diffuse = QVector3D(dif.r, dif.g, dif.b);
    material->specular = QVector3D(spec.r, spec.g, spec.b);

    material->ambient *= .2f;
    if (material->shininess == 0.0) material->shininess = 30;

    qDebug() << "material" << material->name << "ambient:" << material->ambient << "diffuse:" << material->diffuse
             << "specular:" << material->specular << "shininess:" << material->shininess;

    unsigned int tcount;
    if ((tcount = aiMat->GetTextureCount(aiTextureType_DIFFUSE)) > 0) {
      loadTextureStack(material, aiMat, aiTextureType_DIFFUSE, tcount);
    }
    if ((tcount = aiMat->GetTextureCount(aiTextureType_SPECULAR)) > 0) {
      loadTextureStack(material, aiMat, aiTextureType_SPECULAR, tcount);
    }
    if ((tcount = aiMat->GetTextureCount(aiTextureType_AMBIENT)) > 0) {
      loadTextureStack(material, aiMat, aiTextureType_AMBIENT, tcount);
    }
    if ((tcount = aiMat->GetTextureCount(aiTextureType_EMISSIVE)) > 0) {
      loadTextureStack(material, aiMat, aiTextureType_EMISSIVE, tcount);
    }
    if ((tcount = aiMat->GetTextureCount(aiTextureType_HEIGHT)) > 0) {
      loadTextureStack(material, aiMat, aiTextureType_HEIGHT, tcount);
    }
    if ((tcount = aiMat->GetTextureCount(aiTextureType_NORMALS)) > 0) {
      loadTextureStack(material, aiMat, aiTextureType_NORMALS, tcount);
    }
    if ((tcount = aiMat->GetTextureCount(aiTextureType_SHININESS)) > 0) {
      loadTextureStack(material, aiMat, aiTextureType_SHININESS, tcount);
    }
    if ((tcount = aiMat->GetTextureCount(aiTextureType_OPACITY)) > 0) {
      loadTextureStack(material, aiMat, aiTextureType_OPACITY, tcount);
    }
    if ((tcount = aiMat->GetTextureCount(aiTextureType_DISPLACEMENT)) > 0) {
      loadTextureStack(material, aiMat, aiTextureType_DISPLACEMENT, tcount);
    }
    if ((tcount = aiMat->GetTextureCount(aiTextureType_LIGHTMAP)) > 0) {
      loadTextureStack(material, aiMat, aiTextureType_LIGHTMAP, tcount);
    }
    if ((tcount = aiMat->GetTextureCount(aiTextureType_REFLECTION)) > 0) {
      loadTextureStack(material, aiMat, aiTextureType_REFLECTION, tcount);
    }
  }
  model->m_materials.push_back(material);
}

void ModelLoader::processMesh(Model::Ptr model, aiMesh *aimesh)
{
  QString name(aimesh->mName.length != 0 ? aimesh->mName.C_Str() : "");

  Mesh::Ptr mesh = make_shared<Mesh>(name);

  mesh->indexOffset = model->m_indices.size();
  unsigned int indexCountBefore = model->m_indices.size();
  int vertindexoffset = model->m_vertices.size() / 3;

  mesh->numUVChannels = aimesh->GetNumUVChannels();
  mesh->hasTangentsAndBitangents = aimesh->HasTangentsAndBitangents();
  mesh->hasNormals = aimesh->HasNormals();
  mesh->hasBones = aimesh->HasBones();

  // Get Vertices
  if (aimesh->mNumVertices > 0) {
    for (uint ii = 0; ii < aimesh->mNumVertices; ++ii) {
      aiVector3D &vec = aimesh->mVertices[ii];

      model->m_vertices.push_back(vec.x);
      model->m_vertices.push_back(vec.y);
      model->m_vertices.push_back(vec.z);
    }
  }

  // Get Normals
  if (aimesh->HasNormals()) {
    model->m_normals.resize(model->m_vertices.size());

    int nind = vertindexoffset * 3;

    for (uint ii = 0; ii < aimesh->mNumVertices; ++ii) {
      aiVector3D &vec = aimesh->mNormals[ii];
      model->m_normals[nind] = vec.x;
      model->m_normals[nind + 1] = vec.y;
      model->m_normals[nind + 2] = vec.z;
      nind += 3;
    };
  }

  // Get Texture coordinates
  if (aimesh->GetNumUVChannels() > 0) {
    if ((unsigned int) model->m_textureUV.size() <
        aimesh->GetNumUVChannels()) // Caution, assumes all meshes in this model have same number of uv channels
    {
      model->m_textureUV.resize(aimesh->GetNumUVChannels());
      model->m_numUVComponents.resize(aimesh->GetNumUVChannels());
    }

    for (unsigned int mchanInd = 0; mchanInd < aimesh->GetNumUVChannels(); ++mchanInd) {
      Q_ASSERT(aimesh->mNumUVComponents[mchanInd] == 2 && "Error: Texture Mapping Component Count must be 2. Others not supported");

      model->m_numUVComponents[mchanInd] = aimesh->mNumUVComponents[mchanInd];
      model->m_textureUV[mchanInd].resize((model->m_vertices.size() / 3) * 2);

      int uvind = vertindexoffset * model->m_numUVComponents[mchanInd];

      for (uint iind = 0; iind < aimesh->mNumVertices; ++iind) {
        // U
        model->m_textureUV[mchanInd][uvind] = aimesh->mTextureCoords[mchanInd][iind].x;
        if (aimesh->mNumUVComponents[mchanInd] > 1) {
          // V
          model->m_textureUV[mchanInd][uvind + 1] = aimesh->mTextureCoords[mchanInd][iind].y;
          if (aimesh->mNumUVComponents[mchanInd] > 2) {
            // W
            model->m_textureUV[mchanInd][uvind + 2] = aimesh->mTextureCoords[mchanInd][iind].z;
          }
        }
        uvind += model->m_numUVComponents[mchanInd];
      }
    }
  }

  // Get Tangents and bitangents
  if (aimesh->HasTangentsAndBitangents()) {
    model->m_tangents.resize(model->m_vertices.size());
    model->m_bitangents.resize(model->m_vertices.size());

    int tind = vertindexoffset * 3;

    for (uint ii = 0; ii < aimesh->mNumVertices; ++ii) {
      aiVector3D &vec = aimesh->mTangents[ii];
      model->m_tangents[tind] = vec.x;
      model->m_tangents[tind + 1] = vec.y;
      model->m_tangents[tind + 2] = vec.z;

      aiVector3D &vec2 = aimesh->mBitangents[ii];
      model->m_bitangents[tind] = vec2.x;
      model->m_bitangents[tind + 1] = vec2.y;
      model->m_bitangents[tind + 2] = vec2.z;

      tind += 3;
    };
  }

  // Get mesh indexes
  for (uint t = 0; t < aimesh->mNumFaces; ++t) {
    aiFace *face = &aimesh->mFaces[t];
    if (face->mNumIndices != 3) {
      qDebug() << "Warning: Mesh face with not exactly 3 indices, ignoring this primitive." << face->mNumIndices;
      continue;
    }

    model->m_indices.push_back(face->mIndices[0] + vertindexoffset);
    model->m_indices.push_back(face->mIndices[1] + vertindexoffset);
    model->m_indices.push_back(face->mIndices[2] + vertindexoffset);
  }

  mesh->indexCount = model->m_indices.size() - indexCountBefore;
  mesh->material = model->m_materials.at(aimesh->mMaterialIndex);

  model->m_meshes.push_back(mesh);
}

void ModelLoader::processNode(Model::Ptr model, const aiScene *scene, aiNode *node, Node::Ptr parentNode)
{
  static int nodeIndex = 0;

  QString name(node->mName.length != 0 ? node->mName.C_Str() : "");
  Node::Ptr newNode = make_shared<Node>(name, QMatrix4x4(node->mTransformation[0]));

  if(parentNode != nullptr)
    parentNode->children.push_back(newNode);
  else
    model->m_rootNode = newNode;

  newNode->meshes.resize(node->mNumMeshes);
  for (uint imesh = 0; imesh < node->mNumMeshes; ++imesh) {
    shared_ptr<Mesh> mesh = model->m_meshes[node->mMeshes[imesh]];
    newNode->meshes[imesh] = mesh;
  }

#ifdef DEBUG
  qDebug() << "NodeName" << newNode->name;
  qDebug() << "  NodeIndex" << nodeIndex;
  qDebug() << "  NumChildren" << node->mNumChildren;
  qDebug() << "  NumMeshes" << newNode->meshes.size();
  for (int ii = 0; ii < newNode->meshes.size(); ++ii) {
    qDebug() << "    MeshName" << newNode->meshes[ii]->name;
    qDebug() << "    MaterialName" << newNode->meshes[ii]->material->Name;
    qDebug() << "    MeshVertices" << newNode->meshes[ii]->indexCount;
    qDebug() << "    numUVChannels" << newNode->meshes[ii]->numUVChannels;
    qDebug() << "    hasTangAndBit" << newNode->meshes[ii]->hasTangentsAndBitangents;
    qDebug() << "    hasNormals" << newNode->meshes[ii]->hasNormals;
    qDebug() << "    hasBones" << newNode->meshes[ii]->hasBones;
  }
#endif

  ++nodeIndex;

  for (uint ich = 0; ich < node->mNumChildren; ++ich) {
    processNode(model, scene, node->mChildren[ich], newNode);
  }
}

shared_ptr<Model> ModelLoader::getLoaded()
{
  return m_loaded;
}

} //threed
} //lo

#include "modelloader.moc"