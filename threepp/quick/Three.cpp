//
// Created by byter on 12/29/17.
//

#include <QMetaType>
#include <threepp/math/Euler.h>
Q_DECLARE_METATYPE(three::math::Euler);

#include "Three.h"
#include "ThreeDItem.h"
#include <threepp/quick/scene/Scene.h>
#include <threepp/quick/scene/Model.h>
#include "elements/LightShadow.h"
#include "elements/RayCaster.h"
#include "cameras/OrtographicCamera.h"
#include "threepp/quick/lights/AmbientLight.h"
#include "threepp/quick/lights/SpotLight.h"
#include "threepp/quick/lights/PointLight.h"
#include "threepp/quick/lights/HemisphereLight.h"
#include "threepp/quick/lights/DirectionalLight.h"
#include "objects/Axes.h"
#include "objects/GridLines.h"
#include "objects/Ring.h"
#include "objects/Torus.h"
#include "objects/Box.h"
#include "objects/Plane.h"
#include "objects/Sphere.h"
#include "objects/Cylinder.h"
#include "objects/Mesh.h"
#include <threepp/quick/objects/ModelRef.h>
#include "materials/ShaderMaterial.h"
#include "textures/ImageTexture.h"
#include "textures/ImageCubeTexture.h"
#include "interact/OrbitController.h"

namespace three {
namespace quick {

static QObject *three_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
  Q_UNUSED(engine)
  Q_UNUSED(scriptEngine)

  return new Three();
}

void init()
{
  qRegisterMetaType<three::math::Euler>();
  qmlRegisterSingletonType<three::quick::Three>("three.quick", 1, 0, "Three", three_provider);
  qmlRegisterType<three::quick::ThreeDItem>("three.quick", 1, 0, "ThreeD");
  qmlRegisterType<three::quick::Scene>("three.quick", 1, 0, "Scene");
  qmlRegisterType<three::quick::Model>("three.quick", 1, 0, "Model");
  qmlRegisterUncreatableType<three::quick::ThreeQObjectRoot>("three.quick", 1, 0, "ThreeQObjectRoot", "abstract class");
  qmlRegisterUncreatableType<three::quick::ThreeQObject>("three.quick", 1, 0, "ThreeQObject", "abstract class");
  qmlRegisterUncreatableType<three::quick::Material>("three.quick", 1, 0, "Material", "abstract class");
  qmlRegisterUncreatableType<three::quick::Camera>("three.quick", 1, 0, "Camera", "abstract class");
  qmlRegisterUncreatableType<three::quick::Light>("three.quick", 1, 0, "Light", "abstract class");
  qmlRegisterUncreatableType<three::quick::LightShadow>("three.quick", 1, 0, "LightShadow", "internal class");
  qmlRegisterUncreatableType<three::quick::LightShadowOC>("three.quick", 1, 0, "LightShadowOC", "internal class");
  qmlRegisterUncreatableType<three::quick::LightShadowPC>("three.quick", 1, 0, "LightShadowPC", "internal class");
  qmlRegisterUncreatableType<three::quick::FogBase>("three.quick", 1, 0, "FogBase", "abstract class");
  qmlRegisterUncreatableType<three::quick::Mesh>("three.quick", 1, 0, "Mesh", "internal class");
  qmlRegisterType<three::quick::Intersect>();
  qmlRegisterType<three::quick::RayCaster>();
  qmlRegisterType<three::quick::Fog>("three.quick", 1, 0, "Fog");
  qmlRegisterType<three::quick::FogExp2>("three.quick", 1, 0, "FogExp2");
  qmlRegisterType<three::quick::Axes>("three.quick", 1, 0, "Axes");
  qmlRegisterType<three::quick::GridLines>("three.quick", 1, 0, "GridLines");
  qmlRegisterType<three::quick::Box>("three.quick", 1, 0, "Box");
  qmlRegisterType<three::quick::Ring>("three.quick", 1, 0, "Ring");
  qmlRegisterType<three::quick::Torus>("three.quick", 1, 0, "Torus");
  qmlRegisterType<three::quick::Plane>("three.quick", 1, 0, "Plane");
  qmlRegisterType<three::quick::Sphere>("three.quick", 1, 0, "Sphere");
  qmlRegisterType<three::quick::Cylinder>("three.quick", 1, 0, "Cylinder");
  qmlRegisterType<three::quick::ModelRef>("three.quick", 1, 0, "ModelRef");
  qmlRegisterType<three::quick::AmbientLight>("three.quick", 1, 0, "AmbientLight");
  qmlRegisterType<three::quick::SpotLight>("three.quick", 1, 0, "SpotLight");
  qmlRegisterType<three::quick::PointLight>("three.quick", 1, 0, "PointLight");
  qmlRegisterType<three::quick::HemisphereLight>("three.quick", 1, 0, "HemisphereLight");
  qmlRegisterType<three::quick::DirectionalLight>("three.quick", 1, 0, "DirectionalLight");
  qmlRegisterType<three::quick::MeshBasicMaterial>("three.quick", 1, 0, "MeshBasicMaterial");
  qmlRegisterType<three::quick::MeshLambertMaterial>("three.quick", 1, 0, "MeshLambertMaterial");
  qmlRegisterType<three::quick::MeshPhongMaterial>("three.quick", 1, 0, "MeshPhongMaterial");
  qmlRegisterType<three::quick::ShaderMaterial>("three.quick", 1, 0, "ShaderMaterial");
  qmlRegisterType<three::quick::PerspectiveCamera>("three.quick", 1, 0, "PerspectiveCamera");
  qmlRegisterType<three::quick::OrtographicCamera>("three.quick", 1, 0, "OrtographicCamera");
  qmlRegisterUncreatableType<three::quick::Texture>("three.quick", 1, 0, "Texture", "abstract class");
  qmlRegisterType<three::quick::Image>("three.quick", 1, 0, "Image");
  qmlRegisterType<three::quick::ImageTexture>("three.quick", 1, 0, "ImageTexture");
  qmlRegisterType<three::quick::ImageCubeTexture>("three.quick", 1, 0, "ImageCubeTexture");
  qmlRegisterUncreatableType<three::quick::Controller>("three.quick", 1, 0, "Controller", "abstract class");
  qmlRegisterType<three::quick::OrbitController>("three.quick", 1, 0, "OrbitController");
}

using namespace std;

QVariant Three::raycaster(QVariant value)
{
  Camera *camera = value.value<Camera *>();
  if(camera) {
    QVariant var;
    var.setValue(new RayCaster(camera));
    return var;
  }
  return QVariant();
}

QVariant Three::color(QString hex)
{
  QVariant var;
  var.setValue(QColor(hex));
  return var;
}

}
}
