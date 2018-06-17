//
// Created by byter on 6/17/18.
//

#include "Font.h"

namespace three {
namespace extras {

std::vector<ShapePath> Font::createPaths( const std::wstring &text, float size, unsigned divisions ) const
{
  float scale = size / _data.resolution;
  float line_height = ( _data.boundingBox.max().y() - _data.boundingBox.min().y() + _data.underlineThickness ) * scale;

  std::vector<ShapePath> paths;

  float offsetX = 0, offsetY = 0;

  for ( unsigned i = 0; i < text.length(); i ++ ) {

    auto c = text.at(i);

    if ( c == '\n' ) {

      offsetX = 0;
      offsetY -= line_height;

    } else {

      offsetX += createPath( c, divisions, scale, offsetX, offsetY, paths );
    }
  }

  return paths;
}

unsigned Font::createPath( wchar_t c,
                           unsigned divisions,
                           float scale,
                           float offsetX,
                           float offsetY,
                           std::vector<ShapePath> &paths) const
{
  Glyph glyph = _data.glyphAt(c);

  paths.emplace_back();
  ShapePath &path = paths.back();

  float x, y, cpx, cpy, cpx1, cpy1, cpx2, cpy2;

  for ( unsigned i = 0, l = glyph.outline.size(); i < l; ) {

    uint16_t action = glyph.outline[ i ++ ];

    switch ( (char)action ) {

      case 'm': // moveTo

        x = glyph.outline[ i ++ ] * scale + offsetX;
        y = glyph.outline[ i ++ ] * scale + offsetY;

        path.moveTo( x, y );

        break;

      case 'l': // lineTo

        x = glyph.outline[ i ++ ] * scale + offsetX;
        y = glyph.outline[ i ++ ] * scale + offsetY;

        path.lineTo( x, y );

        break;

      case 'q': // quadraticCurveTo

        cpx = glyph.outline[ i ++ ] * scale + offsetX;
        cpy = glyph.outline[ i ++ ] * scale + offsetY;
        cpx1 = glyph.outline[ i ++ ] * scale + offsetX;
        cpy1 = glyph.outline[ i ++ ] * scale + offsetY;

        path.quadraticCurveTo( cpx1, cpy1, cpx, cpy );

        break;

      case 'b': // bezierCurveTo

        cpx = glyph.outline[ i ++ ] * scale + offsetX;
        cpy = glyph.outline[ i ++ ] * scale + offsetY;
        cpx1 = glyph.outline[ i ++ ] * scale + offsetX;
        cpy1 = glyph.outline[ i ++ ] * scale + offsetY;
        cpx2 = glyph.outline[ i ++ ] * scale + offsetX;
        cpy2 = glyph.outline[ i ++ ] * scale + offsetY;

        path.bezierCurveTo( cpx1, cpy1, cpx2, cpy2, cpx, cpy );

        break;
    }
  }
  return glyph.ha * scale;
}

}
}