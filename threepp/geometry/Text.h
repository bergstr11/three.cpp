//
// Created by byter on 5/27/18.
//

#ifndef THREE_PP_TEXT_H
#define THREE_PP_TEXT_H

#include <string>
#include <threepp/core/LinearGeometry.h>
#include <threepp/core/BufferGeometry.h>
#include <threepp/geometry/Extrude.h>
#include <threepp/extras/core/Font.h>

namespace three {
namespace geometry {

struct TextOptions
{
  extras::Font font;
  float size = 100;
  float height = 50;
  unsigned curveSegments = 4;

  bool bevelEnabled = true;
  float bevelThickness = 10;
  float bevelSize = 8;

  explicit TextOptions(const extras::Font &font) : font(font) {}
};

class Text : public LinearGeometry
{
protected:
  std::wstring _text;

  const TextOptions _options;

  Text(const std::wstring &text, const TextOptions &options);

public:
  using Ptr = std::shared_ptr<Text>;

  static Ptr make(const std::wstring &text, const TextOptions &options) {
    return Ptr(new Text(text, options));
  }

  Text *cloned() const override {
    return LinearGeometry::setTyper(new Text(*this));
  }
};

namespace buffer {

class Text : public Extrude
{
  friend class three::geometry::Text;

protected:
  Text(const std::wstring &text, const TextOptions &options);

public:
  using Ptr = std::shared_ptr<Text>;

  static Ptr make(const std::wstring &text, const TextOptions &options) {
    return Ptr(new Text(text, options));
  }

  Text *cloned() const override {
    return BufferGeometry::setTyper(new Text(*this));
  }
};

}
}
}

#endif //THREE_PP_TEXT_H
