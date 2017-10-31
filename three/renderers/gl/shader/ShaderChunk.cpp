//
// Created by byter on 10/31/17.
//

#include "ShaderChunk.h"

#include <QResource>
#include <unordered_map>

namespace three {
namespace gl {

using namespace std;

const char *getShaderChunk(ShaderChunk chunk)
{
  switch(chunk) {
    case ShaderChunk::encodings_pars_fragment:
      return getShaderChunk("encodings_pars_fragment");
    case ShaderChunk::tonemapping_pars_fragment:
      return getShaderChunk("tonemapping_pars_fragment");
  }
}

const char *getShaderChunk(std::string chunk)
{
  static unordered_map<string, QByteArray> shader_chunks;

  if(shader_chunks.find(chunk) != shader_chunks.end()) {
    QByteArray &qb = shader_chunks[chunk];
    return qb.data();
  }
  QString resName = QString(":/chunk/") + QString::fromStdString(chunk) + ".glsl";
  QResource res(resName);

  if(!res.isValid()) throw invalid_argument(string("invalid resource: ")+chunk);

  if(res.isCompressed()) {
    QByteArray uncompressed = qUncompress(res.data(), res.size());
    shader_chunks[chunk] = uncompressed;
    return uncompressed.data();
  }
  else {
    QByteArray ba = QByteArray::fromRawData((const char *)res.data(), res.size());
    shader_chunks[chunk] = ba;
    return ba;
  }
}

}
}
