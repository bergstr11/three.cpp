//
// Created by byter on 30.09.17.
//

#ifndef THREE_QT_UTILS_H
#define THREE_QT_UTILS_H

#include <vector>

namespace three {

template <typename Value>
static void vector_append(std::vector<Value> &container, std::initializer_list<Value> args) {
  container.insert(container.end(), std::begin(args), std::end(args));
}

}
#endif //THREE_QT_UTILS_H
