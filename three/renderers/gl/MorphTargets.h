//
// Created by byter on 07.10.17.
//

#ifndef THREE_QT_MORPHTARGETS_H
#define THREE_QT_MORPHTARGETS_H

#include <QOpenGLFunctions>
#include <array>
#include <vector>
#include <unordered_map>
#include <three/objects/Mesh.h>
#include <three/core/BufferGeometry.h>
#include <three/material/Material.h>
#include "Program.h"

namespace three {
namespace gl {

class MorphTargets
{
  QOpenGLFunctions * const _fn;
  std::array<float, 8> _morphInfluences;

  using Influence = std::pair<size_t, float>;
  std::unordered_map<uint32_t, std::vector<Influence>> _influencesList;

public:
  MorphTargets(QOpenGLFunctions *fn) : _fn(fn) {}

  void update(Mesh::Ptr object, BufferGeometry::Ptr geometry, Material::Ptr material, Program::Ptr program)
  {
    auto objectInfluences = object->morphTargetInfluences();

    size_t length = objectInfluences.size();

    if (_influencesList.find(geometry->id) == _influencesList.end()) {

      // initialise list
      auto res = _influencesList.emplace(geometry->id, std::vector<Influence>(length));
      res.first->second.resize(length);

      for ( size_t i = 0; i < length; i ++ ) {

        res.first->second[ i ].first = i;
        res.first->second[ i ].second = 0;
      }
    }
    std::vector<Influence> &influences = _influencesList[ geometry->id ];

    bool morphTargets = material->morphTargets && !geometry->morphPositions().empty();
    bool morphNormals = material->morphNormals && !geometry->morphNormals().empty();

    // Remove current morphAttributes
    for ( unsigned i = 0; i < length; i ++ ) {

      auto &influence = influences[ i ];

      if (influence.first != 0 ) {
        if (morphTargets) geometry->removeAttribute(IndexedAttributeName::morphTarget, i);
        if (morphNormals) geometry->removeAttribute(IndexedAttributeName::morphNormal, i);
      }
    }

    // Collect influences
    for (unsigned i = 0; i < length; i ++ ) {

      auto &influence = influences[ i ];

      influence.first = i;
      influence.second = objectInfluences[ i ];
    }

    std::sort(influences.begin(), influences.end(), [](const Influence &a, const Influence &b) {
      return std::abs(b.second) > std::abs(a.second);
    });

    // Add morphAttributes
    for (unsigned i = 0; i < 8; i ++ ) {

      Influence influence = influences[ i ];

      auto index = influence.first;
      auto value = influence.second;

      if(value > 0) {
        if (morphTargets) geometry->addAttribute(IndexedAttributeName::morphTarget, i, geometry->morphPositions()[index]);
        if (morphNormals) geometry->addAttribute(IndexedAttributeName::morphNormal, i, geometry->morphNormals()[index]);

        _morphInfluences[ i ] = value;
        continue;

      }
      _morphInfluences[ i ] = 0;
    }

    //program->getUniforms().setValue( gl, 'morphTargetInfluences', morphInfluences );
  }
};

}
}
#endif //THREE_QT_MORPHTARGETS_H
