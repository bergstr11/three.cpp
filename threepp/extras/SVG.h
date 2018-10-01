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

/**
 * SVG 2D Loader
 */
class DLX SVG
{
protected:
  std::vector<extras::ShapePath> _paths;

public:
  static SVG fromFile(const std::string &file);
  static SVG fromXML(const std::string &xml);
  static SVG fromXML(const char *xml, size_t size);

  const std::vector<extras::ShapePath> &paths() const {return _paths;}
};

}
}


#endif //THREE_PP_SVG_H
