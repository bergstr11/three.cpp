//
// Created by byter on 20.09.17.
//

#ifndef THREE_QT_HELPERS_H
#define THREE_QT_HELPERS_H

namespace three {
namespace gl {

struct InfoRender
{
  unsigned frame = 0;
  unsigned  calls = 0;
  unsigned  vertices = 0;
  unsigned  faces = 0;
  unsigned  points = 0;
};

}
}
#endif //THREE_QT_HELPERS_H
