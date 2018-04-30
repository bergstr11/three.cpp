//
// Created by byter on 4/19/18.
//

#include "Collada.h"

#include <3rdparty/tinyxml2/tinyxml2.h>
#include <QString>
#include <QFile>
#include <cerrno>
#include <cstdlib>

namespace three {
namespace loader {

using namespace std;
using namespace tinyxml2;
using namespace collada;

enum Library {
   animations,
   clips,
   controllers,
   images,
   effects,
   materials,
   cameras,
   lights,
   geometries,
   nodes,
   visualScenes,
   kinematicsModels,
   kinematicsScenes
};

template <Library lib>
struct ParseLibrary {};

struct Builder {
  using Ptr = std::shared_ptr<Builder>;
  virtual void build(Collada &collada) = 0;
};

inline string parseId(const char *value)
{
  return value ? string(value+1) : "";
}

inline bool has_name(XMLElement *element, const char *name) {
  return !strcmp(element->Name(), name);
}

void parseFloatArray(const char *text, vector<float> data)
{
  const char *p = text;
  char *end;
  data.clear();

  for (double f = std::strtod(p, &end); p != end; f = std::strtod(p, &end))
  {
    p = end;
    if (errno == ERANGE){
      errno = 0;
      throw range_error(text);
    }
    data.push_back(f);
  }
}

void parseMatrix(const char *text, math::Matrix4 &matrix)
{
  const char *p = text;
  char *end;

  unsigned i=0;
  for (double f = std::strtod(p, &end); p != end && i < 16; f = std::strtod(p, &end))
  {
    p = end;
    if (errno == ERANGE){
      errno = 0;
      throw range_error(text);
    }
    matrix.elements()[i++] = (float)f;
  }
}

void parseStringArray(const char *text, const char *delim, vector<string> data)
{
  const char *str = text, *pos;

  do {
    pos = str;
    auto sz = strcspn(str, delim);
    if (sz) data.push_back(string(str, sz));
    str += strspn(pos + sz, delim);
  } while(str != pos);
}

void parseSource(XMLElement *xml, Source &source)
{
  XMLElement *floatarray = xml->FirstChildElement("float_array");
  if(floatarray)
    parseFloatArray(floatarray->GetText(), source.float_array);
  XMLElement *namearray = xml->FirstChildElement("Name_array");
  if(namearray)
    parseStringArray(namearray->GetText(), " \t", source.name_array);
  XMLElement *technique = xml->FirstChildElement("technique_common");
  if(technique) {
    XMLElement *accessor = technique->FirstChildElement("accessor");
    if ( accessor )
      source.stride = strtoul( accessor->Attribute( "stride" ), nullptr, 10 );
  }
}

void parseAnimationSampler(XMLElement *xml, AnimationSampler &sampler)
{
  XMLElement *input = xml->FirstChildElement("input");
  while(input) {

    string id = parseId( input->Attribute( "source" ) );
    string semantic = input->Attribute( "semantic" );
    sampler.inputs[ semantic ] = id;

    input = input->NextSiblingElement("input");
  }
}

void parseAnimationChannel(XMLElement *xml, AnimationChannel &sampler)
{
  const char *target = xml->Attribute( "target" );

  // parsing SID Addressing Syntax
  vector<string> parts;
  parseStringArray(target, "/", parts);

  sampler.id = parts[0];
  string sid = parts[1];

  // check selection syntax

  if(( sid.find( '(' ) >= 0)) {
    sampler.syntax = AnimationChannel::Syntax::array;

    // array-access syntax. can be used to express fields in one-dimensional vectors or two-dimensional matrices.
    vector<string> indices;
    parseStringArray(sid.c_str(), "(", parts);
    sid = indices[0];

    for (unsigned i = 1; i < indices.size(); i ++ ) {
      sampler.indices.push_back(strtoul(indices[ i ].c_str(), NULL, 10));
    }
  }
  else if( sid.find( '.' ) >= 0 ) {
    sampler.syntax = AnimationChannel::Syntax::member;

    //  member selection access
    parseStringArray(sid.c_str(), ".", parts);
    sid = parts[0];
    sampler.member = parts[1];
  }

  sampler.sid = sid;

  sampler.sampler = parseId( xml->Attribute( "source" ) );
}

void parseSkin(XMLElement *xml, Skin &skin)
{
  XMLElement *child = xml->FirstChildElement();

  while(child) {
    if(has_name(child, "bind_shape_matrix")) {
      parseMatrix(child->GetText(), skin.bindShapeMatrix);
    }
    else if(has_name(child, "source")) {
      string id = child->Attribute("id");
      parseSource(child, skin.sources[id]);
    }
    else if(has_name(child, "joints")) {
      //parseJoints(child, skin.joints);
    }
    else if(has_name(child, "vertex_weights")) {
      //parseVertexWeights(child, skin.vertexWeights);
    }
    child = child->NextSiblingElement();
  }
}

template <>
struct ParseLibrary<animations> : public Builder
{
  static constexpr char const *root_node = "library_animations";
  static constexpr char const *element_node = "animation";

  static void parse(XMLElement *xml, std::unordered_map<std::string, Animation> &animations)
  {
    string id(xml->Attribute( "id" ));
    Animation &data = animations[id];

    XMLElement *child = xml->FirstChildElement();
    while(child) {

      if(has_name(child, "source")) {
        const char *id = child->Attribute( "id" );
        parseSource( child, data.sources[ id ] );
      }
      else if(has_name(child, "sampler")) {
        const char *id = child->Attribute( "id" );
        parseAnimationSampler( child, data.samplers[ id ] );
      }
      else if(has_name(child, "channel")) {
        const char *id = child->Attribute( "target" );
        parseAnimationChannel( child, data.channels[ id ] );
      }
      child = child->NextSiblingElement();
    }
  }

  void build(Collada &collada) override
  {
  }
};
template <>
struct ParseLibrary<clips> : public Builder
{
  static constexpr char const *root_node = "library_animation_clips";
  static constexpr char const *element_node = "animation_clip";

  static void parse(XMLElement *xml, std::unordered_map<std::string, Clip> &clips)
  {
    string id(xml->Attribute( "id" ));
    Clip &data = clips[id];

    data.name = id;
    data.start = xml->FloatAttribute("start", 0.0f);
    data.end = xml->FloatAttribute("end", 0.0f);

    XMLElement *child = xml->FirstChildElement();
    while(child) {

      if(has_name(child, "instance_animation")) {
          data.animations.push_back( parseId( child->Attribute( "url" ) ) );
      }
      child = child->NextSiblingElement();
    }
  }

  void build(Collada &collada) override
  {
  }
};
template <>
struct ParseLibrary<controllers> : public Builder
{
  static constexpr char const *root_node = "library_controllers";
  static constexpr char const *element_node = "controller";

  static void parse(XMLElement *xml, std::unordered_map<std::string, Controller> &controllers)
  {
    string id(xml->Attribute( "id" ));
    Controller &data = controllers[id];

    XMLElement *skin = xml->FirstChildElement("skin");
    if(skin) {
      // there is exactly one skin per controller
      data.id = parseId( skin->Attribute( "source" ) );
      parseSkin(skin, data.skin);
    }
    else {
      XMLElement *morph = xml->FirstChildElement("morph");
      if(morph) {
        data.id = parseId( morph->Attribute( "source" ) );
        qWarning() << "ColladaLoader: Morph target animation not supported yet";
      }
    }
  }

  void build(Collada &collada) override
  {
  }
};
template <>
struct ParseLibrary<images> : public Builder
{
  static constexpr char const *root_node = "library_images";
  static constexpr char const *element_node = "image";

  static void parse(XMLElement *xml, std::unordered_map<std::string, std::string> &images)
  {
    XMLElement *child = xml->FirstChildElement("init_from");
    if(child) {
      string id(xml->Attribute( "id" ));
      images[id] = child->GetText();
    }
  }

  void build(Collada &collada) override
  {
  }
};
template <>
struct ParseLibrary<effects> : public Builder
{
  static constexpr char const *root_node = "library_effects";
  static constexpr char const *element_node = "effect";

  static void parse(XMLElement *xml, std::unordered_map<std::string, EffectProfile> &effects)
  {
    XMLElement *child = xml->FirstChildElement("profile_COMMON");
    if(child) {
      string id(xml->Attribute( "id" ));
      EffectProfile &effect = effects[id];
      //parseEffectProfileCOMMON(child, effect);
    }
  }

  void build(Collada &collada) override
  {
  }
};
template <>
struct ParseLibrary<materials> : public Builder
{
  static constexpr char const *root_node = "library_materials";
  static constexpr char const *element_node = "material";

  static void parse(XMLElement *xml, std::unordered_map<std::string, collada::Material> &materials)
  {
    string id(xml->Attribute( "id" ));
    collada::Material &material = materials[id];

    material.name = xml->Attribute("name");

    XMLElement *child = xml->FirstChildElement("instance_effect");
    if(child) {
      material.url = parseId(child->Attribute("url"));
    }
  }

  void build(Collada &collada) override
  {
  }
};
template <>
struct ParseLibrary<cameras> : public Builder
{
  static constexpr char const *root_node = "library_cameras";
  static constexpr char const *element_node = "camera";

  static void parse(XMLElement *xml, std::unordered_map<std::string, collada::Camera> &cameras)
  {
    string id(xml->Attribute( "id" ));
    collada::Camera &camera = cameras[id];

    camera.name = xml->Attribute("name");

    XMLElement *child = xml->FirstChildElement("optics");
    if(child) {
      //parseCameraOptics( child, camera.optics);
    }
  }

  void build(Collada &collada) override
  {
  }
};
template <>
struct ParseLibrary<lights> : public Builder
{
  static constexpr char const *root_node = "library_lights";
  static constexpr char const *element_node = "light";

  static void parse(XMLElement *xml, std::unordered_map<std::string, collada::Light> &lights)
  {
    string id(xml->Attribute( "id" ));
    collada::Light &light = lights[id];

    XMLElement *child = xml->FirstChildElement("technique_common");
    if(child) {
      //parseLightTechnique( child, light.technique);
    }
  }

  void build(Collada &collada) override
  {
  }
};
template <>
struct ParseLibrary<geometries> : public Builder
{
  static constexpr char const *root_node = "library_geometries";
  static constexpr char const *element_node = "geometry";

  static void parse(XMLElement *xml, std::unordered_map<std::string, collada::Geometry> &geometries)
  {
    XMLElement *mesh = xml->FirstChildElement("mesh");

    // the following tags inside geometry are not supported yet:
    // (see https://github.com/mrdoob/three.js/pull/12606): convex_mesh, spline, brep
    if (!mesh) return;

    string id(xml->Attribute( "id" ));
    collada::Geometry &geometry = geometries[id];

    XMLElement *child = mesh->FirstChildElement();
    while(child) {

      string childId(child->Attribute( "id" ));

      /*if(has_name(child, "source"))
        parseSource( child, geometry.sources[ id ] );

      else if(has_name(child, "vertices"))
        parseGeometryVertices( child, geometry.vertices );

      else if(has_name(child, "lines"))
        parseGeometryPrimitive( child, geometry.primitives );

      else if(has_name(child, "linestrips"))
        parseGeometryPrimitive( child, geometry.primitives );

      else if(has_name(child, "polylist"))
        parseGeometryPrimitive( child, geometry.primitives );

      else if(has_name(child, "triangles"))
        parseGeometryPrimitive( child, geometry.primitives );

      else
        qWarning() << "ColladaLoader: unsupported geometry element" << child->Name();*/

      child = child->NextSiblingElement();
    }
  }

  void build(Collada &collada) override
  {
  }
};
template <>
struct ParseLibrary<nodes> : public Builder
{
  static constexpr char const *root_node = "library_nodes";
  static constexpr char const *element_node = "node";

  static void parse(XMLElement *xml, std::unordered_map<std::string, collada::Node> &nodes)
  {
    string id(xml->Attribute( "id" ));
    collada::Node &node = nodes[id];
    node.name = xml->Attribute("name");
    node.type = xml->Attribute("type");
    node.id = xml->Attribute("id");
    node.sid = xml->Attribute("sid");

    XMLElement *child = xml->FirstChildElement();
    while(child) {

      if(has_name(child, "node")) {
        node.nodeIds.push_back(child->Attribute("id"));
        //parseNode(child, node);
      }
      else if(has_name(child, "instance_camera")) {
        node.instanceCameras.push_back(parseId(child->Attribute("url")));
      }
      else if(has_name(child, "instance_controller")) {
        //parseNodeInstance(child, node.instanceControllers);
      }
      else if(has_name(child, "instance_light")) {
        node.instanceLights.push_back(parseId(child->Attribute("url")));
      }
      else if(has_name(child, "instance_geometry")) {
        //parseNodeInstance(child,node.instanceGeometries);
      }
      else if(has_name(child, "instance_node")) {
        node.instanceNodes.push_back(parseId(child->Attribute("url")));
      }
      else if(has_name(child, "matrix")) {
        math::Matrix4 matrix;
        parseMatrix(child->GetText(), matrix);
        node.matrix *= matrix.transpose();
        node.transforms[child->Attribute("sid")] = child->Name();
      }
      else if(has_name(child, "translate")) {
        math::Vector3 vector;
        //parseVector(child->GetText(), vector);
        node.matrix *= math::Matrix4::translation(vector.x(), vector.y(), vector.z());
        node.transforms[child->Attribute("sid")] = child->Name();
      }
      else if(has_name(child, "rotate")) {
        math::Vector3 vector;
        float deg;
        //parseVectorAndAngle(child->GetText(), vector, deg);
        node.matrix *= math::Matrix4::axisRotation(vector, math::degToRad(deg));
        node.transforms[child->Attribute("sid")] = child->Name();
      }
      else if(has_name(child, "scale")) {
        math::Vector3 vector;
        //parseVector(child->GetText(), vector);
        node.matrix.scale(vector);
        node.transforms[child->Attribute("sid")] = child->Name();
      }
      else {
        qWarning() << "ColladaLoader: unused node element" << child->Name();
      }

      child = child->NextSiblingElement();
    }
  }

  void build(Collada &collada) override
  {
  }
};

static void prepareNodes(XMLElement *element)
{
  static vector<string> ids;

  XMLElement *node = element->FirstChildElement("node");
  while(node) {
    if(!node->Attribute("id", nullptr)) {
      stringstream ss; ss << "three_default_" << ids.size()+1;
      string id = ss.str();
      ids.push_back(id);
      node->SetAttribute("id", id.c_str());
    }
    prepareNodes(node);
    node = node->NextSiblingElement("node");
  }
}

template <>
struct ParseLibrary<visualScenes> : public Builder
{
  static constexpr char const *root_node = "library_visual_scenes";
  static constexpr char const *element_node = "visual_scene";

  static void parse(XMLElement *xml, std::unordered_map<std::string, collada::VisualScene> &scenes)
  {
    string id(xml->Attribute( "id" ));
    collada::VisualScene &scene = scenes[id];

    //XMLElement *
    prepareNodes(xml);

    XMLElement *node = xml->FirstChildElement("node");

    while(node) {
      //parseNode(node, scene.children);

      node = node->NextSiblingElement("node");
    }
  }

  void build(Collada &collada) override
  {
  }
};
template <>
struct ParseLibrary<kinematicsModels> : public Builder
{
  static constexpr char const *root_node = "library_kinematics_models";
  static constexpr char const *element_node = "kinematics_model";

  static void parse(XMLElement *xml, std::unordered_map<std::string, collada::KinematicsModel> &models)
  {
    string id(xml->Attribute( "id" ));
    collada::KinematicsModel &model = models[id];

    XMLElement *child = xml->FirstChildElement("technique_common");

    while(child) {
      //parseKinematicsTechniqueCommon( child, model );

      child = child->NextSiblingElement("technique_common");
    }
  }

  void build(Collada &collada) override
  {
  }
};
template <>
struct ParseLibrary<kinematicsScenes> : public Builder
{
  static constexpr char const *root_node = "scene";
  static constexpr char const *element_node = "instance_kinematics_scene";

  static void parse(XMLElement *xml, std::unordered_map<std::string, collada::KinematicsScene> &scenes)
  {
    //var data = {bindJointAxis: []};

    XMLElement *child = xml->FirstChildElement("bind_joint_axis");

    while(child) {
      //parseKinematicsBindJointAxis( child );

      child = child->NextSiblingElement("bind_joint_axis");
    }
    //library.kinematicsScenes[ parseId( xml.getAttribute( 'url' ) ) ] = data;
  }

  void build(Collada &collada) override
  {
  }
};

struct Access
{
};

template <Library lib, typename Data> Builder::Ptr parse(XMLElement *xml, unordered_map<string, Data> &result)
{
  using ParseLib = ParseLibrary<lib>;

  XMLElement *root = xml->FirstChildElement(ParseLib::root_node);
  if (root) {
    XMLElement *element = root->FirstChildElement(ParseLib::element_node);

    while(element) {
      ParseLib::parse(element, result);
      element = element->NextSiblingElement(ParseLib::element_node);
    }
  }
  return std::make_shared<ParseLib>();
}

void Collada::loadScene(string name, ResourceLoader &loader)
{
  XMLDocument doc;
  doc.LoadFile( name.c_str() );

  XMLElement *collada = doc.FirstChildElement("COLLADA");
  _version = collada->FloatAttribute("version", 0);

  std::vector<Builder::Ptr> builders {
    parse<Library::animations>(collada, animations),
    parse<Library::clips>(collada, clips),
    parse<Library::controllers>(collada, controllers),
    parse<Library::images>(collada, images),
    parse<Library::effects>(collada, effects),
    parse<Library::materials>(collada, materials),
    parse<Library::cameras>(collada, cameras),
    parse<Library::lights>(collada, lights),
    parse<Library::geometries>(collada, geometries),
    parse<Library::nodes>(collada, nodes),
    parse<Library::visualScenes>(collada, visualScenes),
    parse<Library::kinematicsModels>(collada, kinematicsModels),
    parse<Library::kinematicsScenes>(collada, kinematicsScenes)
  };

  for(auto builder : builders) {
    builder->build(*this);
  }
  //Access access(_scene, aiscene, loader, modelMap);
}

void Collada::load(std::string name, const Color &background, ResourceLoader &loader)
{
  _scene = SceneT<Color>::make(name, background);
  loadScene(name, loader);
}

void Collada::load(std::string name, Texture::Ptr background, ResourceLoader &loader)
{
  _scene = SceneT<Texture::Ptr>::make(name, background);
  loadScene(name, loader);
}

void Collada::load(string name, ResourceLoader &loader)
{
  _scene = Scene::make(name);
  loadScene(name, loader);
}

}
}
