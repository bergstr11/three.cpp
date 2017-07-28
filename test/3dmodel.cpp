//
// Created by byter on 02.07.17.
//
#include "3dmodel.h"

namespace lo {
namespace threed {

using namespace std;

void Model::findObjectDimensions(Node::Ptr node, QMatrix4x4 transformation, QVector3D &minDimension, QVector3D &maxDimension)
{
  transformation *= node->getTransformation();

  for (int ii = 0; ii < node->meshes.size(); ++ii) {
    int start = node->meshes[ii]->indexOffset;
    int end = start + node->meshes[ii]->indexCount;
    for (int ii = start; ii < end; ++ii) {
      int ind = m_indices[ii] * 3;
      QVector4D vec(m_vertices[ind], m_vertices[ind + 1], m_vertices[ind + 2], 1.0);
      vec = transformation * vec;

      if (vec.x() < minDimension.x())
        minDimension.setX(vec.x());
      if (vec.y() < minDimension.y())
        minDimension.setY(vec.y());
      if (vec.z() < minDimension.z())
        minDimension.setZ(vec.z());
      if (vec.x() > maxDimension.x())
        maxDimension.setX(vec.x());
      if (vec.y() > maxDimension.y())
        maxDimension.setY(vec.y());
      if (vec.z() > maxDimension.z())
        maxDimension.setZ(vec.z());
    }
  }

  for (int ii = 0; ii < node->children.size(); ++ii) {
    findObjectDimensions(node->children[ii], transformation, minDimension, maxDimension);
  }
}

// This will transform the model to unit coordinates, so a model of any size or shape will fit on screen
void Model::transformToUnitCoordinates()
{
  double amin = std::numeric_limits<double>::max();
  double amax = std::numeric_limits<double>::min();
  QVector3D minDimension(amin, amin, amin);
  QVector3D maxDimension(amax, amax, amax);

  // Get the minimum and maximum x,y,z values for the model
  findObjectDimensions(m_rootNode, QMatrix4x4(), minDimension, maxDimension);

  // Calculate scale and translation needed to center and fit on screen
  float dist = qMax(maxDimension.x() - minDimension.x(),
                    qMax(maxDimension.y() - minDimension.y(), maxDimension.z() - minDimension.z()));
  float sc = 1.0 / dist;
  QVector3D center = (maxDimension - minDimension) / 2;
  QVector3D trans = -(maxDimension - center);

  // Apply the scale and translation to a matrix
  QMatrix4x4 transformation;
  transformation.setToIdentity();
  transformation.scale(sc);
  transformation.translate(trans);

  // Multiply the transformation to the root node transformation matrix
  m_rootNode->multiplyTransform(transformation);
}

void Model::getBufferData(const vector<float> **vertices, const vector<float> **normals,
                          const vector<unsigned int> **indices) const
{
  if (vertices != 0)
    *vertices = &m_vertices;

  if (normals != 0)
    *normals = &m_normals;

  if (indices != 0)
    *indices = &m_indices;
}

void Model::getTextureData(const vector<vector<float> > **textureUV, const vector<float> **tangents,
                           const vector<float> **bitangents) const
{
  if (textureUV != 0)
    *textureUV = &m_textureUV;

  if (tangents != 0)
    *tangents = &m_tangents;

  if (bitangents != 0)
    *bitangents = &m_bitangents;
}

}
}