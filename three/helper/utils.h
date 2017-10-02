//
// Created by byter on 30.09.17.
//

#ifndef THREE_QT_UTILS_H
#define THREE_QT_UTILS_H

#include <vector>
#include <cstring>
#include <unordered_map>

namespace three {

template <typename Value>
static void vector_append(std::vector<Value> &container, std::initializer_list<Value> args) {
  container.insert(container.end(), std::begin(args), std::end(args));
}

inline std::string replace_all(std::string target, const std::string &find, const std::string &replace)
{
  for(auto it = target.begin(), end = target.end()-find.length(); it < end; ) {
    if(!strncmp(&(*it), find.data(), find.length())) {
      target.replace(it, it+find.length(), replace);
      it += replace.length();
    }
    else it++;
  }
  return target;
}

inline std::string replace_all(std::string target, const std::unordered_map<std::string, std::string> &values)
{
  for(auto &pair : values) {
    const std::string &find = pair.first, &replace = pair.second;
    for(auto it = target.begin(), end = target.end()-find.length(); it < end; ) {
      if(!strncmp(&(*it), find.data(), find.length())) {
        target.replace(it, it+find.length(), replace);
        it += replace.length();
      }
      else it++;
    }
  }
  return target;
}

}
#endif //THREE_QT_UTILS_H
