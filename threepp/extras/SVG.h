//
// Created by byter on 8/10/18.
//

#ifndef THREE_PP_SVG_H
#define THREE_PP_SVG_H

#include <vector>
#include <string>

#include <threepp/extras/core/ShapePath.h>

namespace three {

namespace extras {

struct SVGOptions
{
};

/**
 * SVG 2D geometry Loader
 */
class SVG : private SVGOptions
{
protected:
  std::vector<extras::ShapePath> paths;
public:

  SVG(const SVGOptions &options = SVGOptions()) : SVGOptions(options) {}

  void load(std::string name);
};

}
}


#endif //THREE_PP_SVG_H
