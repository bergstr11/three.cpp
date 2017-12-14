//
// Created by byter on 12/14/17.
//

#ifndef THREEPP_MATERIAL_H
#define THREEPP_MATERIAL_H

#include <QObject>

namespace three {
namespace quick {

class Material : public QObject
{
Q_OBJECT

public:

  virtual three::Material::Ptr create() = 0;
};

}
}

#endif //THREEPP_MATERIAL_H
