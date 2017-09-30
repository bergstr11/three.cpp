//
// Created by byter on 13.09.17.
//

#ifndef THREE_QT_GLPROGRAM_H
#define THREE_QT_GLPROGRAM_H

#include <memory>

namespace three {
namespace gl {

class Program {
  unsigned _id;

public:
  using Ptr = std::shared_ptr<Program>;

  unsigned id() const {return _id;}
};

}
}
#endif //THREE_QT_GLPROGRAM_H
