/**
 * @author zz85 / http://www.lab4games.net/zz85/blog
 * @author mrdoob / http://mrdoob.com/
 */

#include "ShapePath.h"
#include <threepp/math/Box2.h>
#include <unordered_map>

namespace three {
namespace extras {

struct Glyph
{
  std::vector<uint16_t> outline;
  unsigned ha;
};

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
    if(glyphs.count(c)) return glyphs.at(c);
    return unknown;
  }
};

class Font
{
  FontData _data;

  unsigned createPath( wchar_t c,
                       unsigned divisions,
                       float scale,
                       float offsetX,
                       float offsetY,
                       std::vector<ShapePath> &paths) const;

  std::vector<ShapePath> createPaths( const std::wstring &text,
                                      float size,
                                      unsigned divisions ) const;

public:
  explicit Font(const FontData &data) : _data(data) {}

  std::vector<Shape::Ptr> generateShapes( const std::wstring &text, float size, unsigned divisions ) const
  {
    const auto &paths = createPaths( text, size, divisions );

    std::vector<Shape::Ptr> shapes;

    for ( const auto &path : paths ) {

      const auto &pshapes = path.toShapes();
      shapes.insert(shapes.end(), pshapes.begin(), pshapes.end() );
    }

    return shapes;
  }
};

}
}