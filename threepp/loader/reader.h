//
// Created by byter on 1/6/18.
//

#ifndef THREEPP_READER_H
#define THREEPP_READER_H

#include <istream>
#include <sstream>
#include <type_traits>

namespace three {
namespace reader {

/**
 * @tparam T integral type
 * @tparam Bytes number of bytes
 * @param in input stream
 * @param value the value to read
 * @param bigEndian whether the stream data is bigEndian
 * @return true if read
 */
template <typename T, size_t Bytes=sizeof(T), typename = std::enable_if<std::is_integral<T>::value>>
bool read(std::istream &in, T *value, bool littleEndian=true)
{
  unsigned char data[Bytes];
  in.read((char *)data, Bytes);
  if(in.gcount() != Bytes) return false;

  *value = 0;
  if(littleEndian) {
    for(unsigned i=0; i<Bytes; i++)
      *value |= ((T)data[i] << (i * 8));
  }
  else {
    for(unsigned i=Bytes; i>0; i--)
      *value |= ((T)data[i-1] << ((Bytes - i) * 8));
  }

  return true;
}

/**
 *
 * @param in the tream to read from
 * @param value the stream to read into
 * @return true if successfully read
 */
bool read(std::istream &in, std::stringstream &value, size_t maxLength)
{
  size_t i=0;
  for(std::istream::char_type c=in.get(); c != 0 && i < maxLength; c = in.get(), i++){
    if(in.fail()) return false;
    value << c;
  }
  return true;
}

/**
 * @param in input stream
 * @param value the value to read
 * @param bigEndian whether the stream data is bigEndian
 * @return true if read
 */
bool read(std::istream &in, float *value, bool littleEndian=true)
{
  return read<uint32_t, 4>(in, (uint32_t *)value, littleEndian);
}

/**
 * @param in input stream
 * @param value the value to read
 * @param bigEndian whether the stream data is bigEndian
 * @return true if read
 */
bool read(std::istream &in, double *value, bool littleEndian=true)
{
  return read<uint64_t, 8>(in, (uint64_t *)value, littleEndian);
}

template <typename T, size_t Bytes=sizeof(T)>
T read_int(std::istream &in, bool littleEndian=true)
{
  T value;
  if(!read<T, Bytes>(in, (T *)&value, littleEndian))
    throw std::logic_error("IO error or read beyond end of stream");
  return value;
}

float read_float(std::istream &in, bool littleEndian=true)
{
  float value;
  if(!read<uint32_t, 4>(in, (uint32_t *)&value, littleEndian))
    throw std::logic_error("IO error or read beyond end of stream");

  return value;
}

double read_double(std::istream &in, bool littleEndian=true)
{
  double value;
  if(!read<uint64_t, 8>(in, (uint64_t *)&value, littleEndian))
    throw std::logic_error("IO error or read beyond end of stream");

  return value;
}

std::string read_string(std::istream &in, size_t maxLength)
{
  std::stringstream ss;
  if(!read(in, ss, maxLength))
    throw std::logic_error("IO error or read beyond end of stream");
  return ss.str();
}

}
}
#endif //THREEPP_READER_H
