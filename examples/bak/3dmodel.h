//
// Created by cse on 6/3/17.
//

#ifndef QGL_TEST_3DMODEL_H
#define QGL_TEST_3DMODEL_H

#include <assimp/scene.h>
#include <map>
#include <vector>
#include <memory>

#include <QVector3D>
#include <QString>
#include <QVector4D>
#include <QMatrix4x4>

namespace lo {
namespace threed {

struct Texture
{
  using Ptr = std::shared_ptr<Texture>;

  Texture(const char *path, aiTextureMapping mapping, unsigned int uvindex, float blend, aiTextureOp op, aiTextureMapMode mapmode)
     : path(path), mapping(mapping), uvindex(uvindex), blend(blend), op(op), mapmode(mapmode) {
  }

  const QString path;
  const aiTextureMapping mapping;
  const unsigned int uvindex;
  const float blend;
  const aiTextureOp op;
  const aiTextureMapMode mapmode;
};

struct Material
{
  using Ptr = std::shared_ptr<Material>;

  QString name;
  QVector3D ambient;
  QVector3D diffuse;
  QVector3D specular;
  float shininess;
  float opacity;
  bool twosided;

  std::map<aiTextureType, std::vector<Texture::Ptr>> textures;
};

struct Light
{
  QVector4D position;
  QVector3D intensity;
};

struct Mesh
{
  using Ptr = std::shared_ptr<Mesh>;

  const QString name;

  Mesh(QString name) : name(name) {}

  unsigned int indexCount;
  unsigned int indexOffset;
  std::shared_ptr<Material> material;

  unsigned int numUVChannels;
  bool hasTangentsAndBitangents;
  bool hasNormals;
  bool hasBones;
};

class Node
{
public:
  using Ptr = std::shared_ptr<Node>;

  Node(QString name, QMatrix4x4 transform) : name(name), transformation(transform) {}

  const QString name;

  std::vector<Mesh::Ptr> meshes;
  std::vector<Node::Ptr> children;

  const QMatrix4x4 &getTransformation() {
    return transformation;
  }
  void multiplyTransform(QMatrix4x4 mult) {
    transformation = mult * transformation;
  }

private:
  QMatrix4x4 transformation;
};

class Model
{
  friend class ModelLoader;
public:
  using Ptr = std::shared_ptr<Model>;

  void getBufferData(const std::vector<float> **vertices, const std::vector<float> **normals, const std::vector<unsigned int> **indices) const;

  /**
   * @param textureUV For texture mapping
   * @param tangents  For normal mapping
   * @param bitangents For normal mapping
   */
  void getTextureData(const std::vector<std::vector<float> > **textureUV, const std::vector<float> **tangents, const std::vector<float> **bitangents) const;

  lo::threed::Node::Ptr getNodeData() const { return m_rootNode; }

  // Texture information
  int numUVChannels() { return m_textureUV.size(); }

  int numUVComponents(int channel) { return m_numUVComponents.at(channel); }

  void transformToUnitCoordinates();

private:
  void findObjectDimensions(std::shared_ptr<Node> node, QMatrix4x4 transformation, QVector3D &minDimension, QVector3D &maxDimension);

  std::vector<float> m_vertices;
  std::vector<float> m_normals;
  std::vector<unsigned int> m_indices;

  std::vector<std::vector<float/*texture mapping coords*/> > m_textureUV; // m_textureUV[uvChannelIndex] is vector of texture mapping coords
  // m_textureUV[uvChannelIndex][ii+n] == if(n==0&&numCmpnts>0) U; if(n==0&&numCmpnts>0) V; if(n==0&&numCmpnts>0) W.
  std::vector<float> m_tangents;
  std::vector<float> m_bitangents;
  std::vector<unsigned int /*num components*/> m_numUVComponents; // m_numUVComponents[uvChannelIndex]

  std::vector<lo::threed::Material::Ptr> m_materials;
  std::vector<lo::threed::Mesh::Ptr> m_meshes;
  lo::threed::Node::Ptr m_rootNode;
};

}
}

#endif //QGL_TEST_3DMODEL_H
