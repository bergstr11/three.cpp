#ifndef THREEPP_EXTRAS_FONT_H
#define THREEPP_EXTRAS_FONT_H

#include "ShapePath.h"
#include <threepp/math/Box2.h>
#include <unordered_map>

namespace three {
namespace extras {

/**
* @author zz85 / http://www.lab4games.net/zz85/blog
* @author mrdoob / http://mrdoob.com/
*/

/**
 * a glyph, consiting of shape commands
 */
class Glyph
{
  std::vector<int16_t> data;

public:
  int ha;

  /** apply the commands to the given path */
  void apply(ShapePath &path, float scale, float offsetX, float offsetY);

  /** add a move command */
  void moveTo(int x, int y);

  /** add a lineTo command */
  void lineTo(int x, int y);

  /** add a quadraticCurve command */
  void quadraticCurveTo(int px1, int py1, int x2, int y2);

  /** add a bezierCurve command */
  void bezierCurveTo(int x1, int y1, int x2, int y2, int x3, int y3);
};

/**
 * FontData describes a font
 */
class FontData
{
  Glyph unknown;
  std::unordered_map<wchar_t, Glyph> glyphs;

public:
  float resolution;
  math::Box2 boundingBox;
  float underlineThickness;

  const Glyph &glyphAt(wchar_t c) const
  {
    if (glyphs.count(c)) return glyphs.at(c);
    return unknown;
  }

  Glyph &put(wchar_t c)
  {
    return glyphs[c];
  }
};

/**
 * Font wraps a FontData and adds the capability of creating Shapes
 */
class Font
{
  FontData _data;

  float createPath(wchar_t c,
                   float scale,
                   float offsetX,
                   float offsetY,
                   std::vector<ShapePath> &paths) const;

  std::vector<ShapePath> createPaths(const std::wstring &text, float size) const;

public:
  explicit Font(const FontData &data) : _data(data)
  {}

  std::vector<Shape::Ptr> generateShapes(const std::wstring &text, float size) const
  {
    const auto &paths = createPaths(text, size);

    std::vector<Shape::Ptr> shapes;

    for (const auto &path : paths) {

      const auto &pshapes = path.toShapes();
      shapes.insert(shapes.end(), pshapes.begin(), pshapes.end());
    }

    return shapes;
  }
};

}
}

#endif //THREEPP_EXTRAS_FONT_H
