//
// Created by byter on 30.09.17.
//

#ifndef THREEPP_UTILS_H
#define THREEPP_UTILS_H

#include <vector>
#include <cstring>
#include <unordered_map>

namespace three {

template <typename Value>
static void operator +=(std::vector<Value> &container, const std::initializer_list<Value> &args) {
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

template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct pair_hash {
  template <class T1, class T2>
  std::size_t operator () (const std::pair<T1,T2> &p) const {
    auto h = std::hash<T1>{}(p.first);
    hash_combine(h, p.second);

    return h;
  }
};

}
#endif //THREEPP_UTILS_H
