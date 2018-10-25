//
// Created by byter on 5/27/18.
//

#include "Text.h"

namespace three {
namespace geometry {

Text::Text(const std::wstring &text, const TextOptions &options)
   : LinearGeometry(geometry::Typer(this)), _text(text), _options(options)
{
  set(buffer::Text(_text, _options));
  mergeVertices();
}

namespace buffer {

static inline ExtrudeOptions extrudeOptions(const TextOptions &textOptions)
{
  ExtrudeOptions options;
  options.depth = textOptions.height;
  options.curveSegments = textOptions.curveSegments;
  options.bevelEnabled = textOptions.bevelEnabled;
  options.bevelThickness = textOptions.bevelThickness;
  options.bevelSize = textOptions.bevelSize;

  return options;
}

static inline std::vector<extras::Shape::Ptr> makeShapes(const std::wstring &text, const TextOptions &options)
{
  return options.font.generateShapes( text, options.size );
}

Text::Text(const std::wstring &text, const TextOptions &options)
   : Extrude(makeShapes(text, options), extrudeOptions(options))
{}

}
}
}