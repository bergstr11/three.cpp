//
// Created by byter on 4/19/18.
//

#ifndef THREE_PP_COLLADA_H
#define THREE_PP_COLLADA_H

#include <vector>
#include <istream>
#include <string>
#include <unordered_map>

#include <threepp/material/Material.h>
#include <threepp/objects/Mesh.h>
#include <threepp/scene/Scene.h>
#include <threepp/util/simplesignal.h>
#include <threepp/math/Triangle.h>

#include "Loader.h"

namespace three {

namespace collada {

struct Source
{
  std::vector<float> float_array;
  std::vector<std::string> name_array;
  unsigned stride;
};
using SourceMap = std::unordered_map<std::string, Source>;

struct AnimationSampler
{
  std::unordered_map<std::string, std::string> inputs;
};
struct AnimationChannel
{
  enum class Syntax {array, member};

  std::string id;
  std::string sid;
  std::string member;
  std::string sampler;
  Syntax syntax;
  std::vector<unsigned> indices;
};
struct Animation
{
  SourceMap sources;
  std::unordered_map<std::string, AnimationSampler> samplers;
  std::unordered_map<std::string, AnimationChannel> channels;
};

struct Clip
{
   std::string name;
   float start;
   float end;
   std::vector<std::string> animations;
};

struct Skin
{
  math::Matrix4 bindShapeMatrix;
  SourceMap sources;
};

struct Controller
{
  std::string id;
  Skin skin;
};

struct EffectProfile
{
  std::unordered_map<std::string, std::string> surfaces;
  std::unordered_map<std::string, std::string> samplers;
};

struct Material
{
  std::string name;
  std::string url;
};

struct Camera
{
  std::string name;
  std::string optics;
};

struct Light
{
  std::string name;
  std::string technique;
};

struct Input
{
  std::string id;
  unsigned offset;
};

struct Primitive
{
   std::string type;
   std::string material;
   unsigned count;
   std::unordered_map<std::string, Input> inputs;
   unsigned stride = 0;
};

struct Geometry
{
  std::string name;
  SourceMap sources;
  std::vector<Vertex> vertices;
  std::vector<Primitive> primitives;
};

struct Node
{
  std::string name;
  std::string type;
  std::string id;
  std::string sid;
  math::Matrix4 matrix;
  std::vector<std::string> nodeIds;
  std::vector<std::string> instanceCameras;
  std::vector<std::string> instanceControllers;
  std::vector<std::string> instanceLights;
  std::vector<std::string> instanceGeometries;
  std::vector<std::string> instanceNodes;
  std::unordered_map<std::string, std::string> transforms;
};

struct VisualScene
{
  std::string name;
  std::vector<Node> children;
};

struct KinematicsModel
{
  std::string name;
  //std::unordered_map<Joint> joints;
  //std::vector<Link> links
};

struct KinematicsScene
{
  std::string id;
};

}

namespace loader {

struct ColladaOptions
{
};

/**
 * Collada-based scene Loader
 */
class Collada : public Loader, private ColladaOptions
{
protected:
  std::vector<Material::Ptr> _materials;
  std::vector<Mesh::Ptr> _meshes;

  std::unordered_map<std::string, collada::Animation> animations;
  std::unordered_map<std::string, collada::Clip> clips;
  std::unordered_map<std::string, collada::Controller> controllers;
  std::unordered_map<std::string, std::string> images;
  std::unordered_map<std::string, collada::EffectProfile> effects;
  std::unordered_map<std::string, collada::Material> materials;
  std::unordered_map<std::string, collada::Camera> cameras;
  std::unordered_map<std::string, collada::Light> lights;
  std::unordered_map<std::string, collada::Geometry> geometries;
  std::unordered_map<std::string, collada::Node> nodes;
  std::unordered_map<std::string, collada::VisualScene> visualScenes;
  std::unordered_map<std::string, collada::KinematicsModel> kinematicsModels;
  std::unordered_map<std::string, collada::KinematicsScene> kinematicsScenes;

  float _version;

  void loadScene(std::string name, ResourceLoader &loader);

public:

  Collada(const ColladaOptions &options = ColladaOptions()) : ColladaOptions(options) {}
  ~Collada() override;

  float version() const {return _version;}

  void load(std::string name, ResourceLoader &loader) override;
  void load(std::string name, Color background, ResourceLoader &loader) override;
  void load(std::string name, Texture::Ptr background, ResourceLoader &loader) override;
};

}
}

#endif //THREE_PP_COLLADA_H
