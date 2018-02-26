//
// Created by byter on 30.09.17.
//

#ifndef THREEPP_UTILS_H
#define THREEPP_UTILS_H

#include <vector>
#include <cstring>
#include <sstream>
#include <unordered_map>

namespace three {

template <typename Value>
static void operator +=(std::vector<Value> &container, const std::initializer_list<Value> &args) {
  container.insert(container.end(), std::begin(args), std::end(args));
}

template<typename _Arg>
inline std::string to_string(_Arg&& _arg)
{
  std::stringstream ss;
  ss << std::forward<_Arg>(_arg);
  return ss.str();
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

#ifdef QDEBUG_H
inline void printMatrix(const char *prefix, const three::math::Matrix4 &m)
{
  auto q(qDebug().nospace());
  q << prefix << " ";
  for(int i=0; i<16; i++) {
    q << m[i]; if(i<15) q << '|';
  }
}
inline void printMatrix(const char *prefix, const three::math::Matrix3 &m)
{
  auto q(qDebug().nospace());
  q << prefix << " ";
  for(int i=0; i<9; i++) {
    q << m[i]; if(i<8) q << '|';
  }
}
inline void printEuler(const char *prefix, const three::math::Euler &e)
{
  auto q(qDebug().nospace());
  q << prefix << " " << e.x() << '|' << e.y() << '|' << e.z();
}
inline void printVertex(const char *prefix, const three::math::Vector3 &v)
{
  auto q(qDebug().nospace());
  q << prefix << " " << v.x() << '|' << v.y() << '|' << v.z();
}
inline void printQuaternion(const char *prefix, const three::math::Quaternion &m)
{
  auto q(qDebug().nospace());
  q << prefix << " ";
  for(int i=0; i<4; i++) {
    q << m[i]; if(i<3) q << '|';
  }
}
#endif

}
#endif //THREEPP_UTILS_H
