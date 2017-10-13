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

struct pair_hash {
  template <class T1, class T2>
  std::size_t operator () (const std::pair<T1,T2> &p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);

    // Mainly for demonstration purposes, i.e. works but is overly simple
    // In the real world, use sth. like boost.hash_combine
    return h1 ^ h2;
  }
};

}
#endif //THREE_QT_UTILS_H
