#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <string>
#include <QMatrix4x4>
#include <vector>
#include <QFile>
#include <QDir>
#include <QFileInfo>

#include "3dmodel.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

namespace lo {
namespace threed {

class ModelLoader : public QObject {
Q_OBJECT
  friend class LoaderThread;

public:
  ModelLoader(bool transformToUnitCoordinates = true);

  void asynchLoad(QFileInfo fileInfo);
  Model::Ptr synchLoad(QFileInfo fileInfo);

  Model::Ptr getLoaded();

signals:
  void modelLoaded();

private:
  void processMaterial(Model::Ptr model, aiMaterial *aiMat);

  void processMesh(Model::Ptr model, aiMesh *aimesh);

  void processNode(Model::Ptr model, const aiScene *scene, aiNode *node, Node::Ptr parentNode);

  Model::Ptr m_loaded;
  const bool m_transformToUnitCoordinates;
};

}
}

#endif // MODELLOADER_H
