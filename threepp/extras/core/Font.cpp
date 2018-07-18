//
// Created by byter on 6/17/18.
//

#include "Font.h"

namespace three {
namespace extras {

std::vector<ShapePath> Font::createPaths(const std::wstring &text, float size) const
{
  float scale = size / _data.resolution;
  float line_height = (_data.boundingBox.max().y() - _data.boundingBox.min().y() + _data.underlineThickness) * scale;

  std::vector<ShapePath> paths;

  float offsetX = 0, offsetY = 0;

  for (unsigned i = 0; i < text.length(); i++) {

    auto c = text.at(i);

    if (c == '\n') {

      offsetX = 0;
      offsetY -= line_height;
    }
    else {

      offsetX += createPath(c, scale, offsetX, offsetY, paths);
    }
  }

  return paths;
}

float Font::createPath(wchar_t c, float scale, float offsetX, float offsetY, std::vector<ShapePath> &paths) const
{
  Glyph glyph = _data.glyphAt(c);

  paths.emplace_back();
  ShapePath &path = paths.back();

  glyph.apply(path, scale, offsetX, offsetY);
  return glyph.ha * scale;
}

void Glyph::apply(ShapePath &path, float scale, float offsetX, float offsetY)
{
  float x, y, cpx, cpy, cpx1, cpy1, cpx2, cpy2;

  for (unsigned i = 0, l = data.size(); i < l; i++) {

    int16_t action = data[i];

    switch ((char) action) {

      case 'm': // moveTo

        x = data[++i] * scale + offsetX;
        y = data[++i] * scale + offsetY;

        path.moveTo(x, y);

        break;

      case 'l': // lineTo

        x = data[++i] * scale + offsetX;
        y = data[++i] * scale + offsetY;

        path.lineTo(x, y);

        break;

      case 'q': // quadraticCurveTo

        cpx = data[++i] * scale + offsetX;
        cpy = data[++i] * scale + offsetY;
        cpx1 = data[++i] * scale + offsetX;
        cpy1 = data[++i] * scale + offsetY;

        path.quadraticCurveTo(cpx1, cpy1, cpx, cpy);

        break;

      case 'b': // bezierCurveTo

        cpx = data[++i] * scale + offsetX;
        cpy = data[++i] * scale + offsetY;
        cpx1 = data[++i] * scale + offsetX;
        cpy1 = data[++i] * scale + offsetY;
        cpx2 = data[++i] * scale + offsetX;
        cpy2 = data[++i] * scale + offsetY;

        path.bezierCurveTo(cpx1, cpy1, cpx2, cpy2, cpx, cpy);

        break;
    }
  }
}

void Glyph::moveTo(int x, int y)
{
  data.push_back('m');
  data.push_back(x);
  data.push_back(y);
}

void Glyph::lineTo(int x, int y)
{
  data.push_back('l');
  data.push_back(x);
  data.push_back(y);
}

void Glyph::quadraticCurveTo(int x1, int y1, int x2, int y2)
{
  data.push_back('q');
  data.push_back(x1);
  data.push_back(y1);
  data.push_back(x2);
  data.push_back(y2);
}

void Glyph::bezierCurveTo(int x1, int y1, int x2, int y2, int x3, int y3)
{
  data.push_back('b');
  data.push_back(x1);
  data.push_back(y1);
  data.push_back(x2);
  data.push_back(y2);
  data.push_back(x3);
  data.push_back(y3);
}

}
}