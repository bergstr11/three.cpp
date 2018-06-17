//
// Created by byter on 6/18/18.
//

#include "Text3D.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

namespace three {
namespace quick {

three::Object3D::Ptr Text3D::_create()
{
  QFile file(_font);
  QJsonDocument doc = QJsonDocument::fromBinaryData(file.readAll());
  //doc.object().

  extras::FontData fontData;

  geometry::TextOptions options {extras::Font(fontData)};
  options.height = _height;
  options.size = _size;
  options.curveSegments = _curveSegments;
  options.bevelThickness = _bevelThickness;
  options.bevelSize = _bevelSize;
  options.bevelEnabled = _bevelEnabled;

  _creator.set(geometry::Text::make(_text.toStdWString(), options));

  material()->identify(_creator);

  return _creator.mesh;
}

}
}