//
// Created by csell on 03.06.2018.
//

#include "Grid.h"

namespace three {
namespace helper {

Grid::Ptr Grid::make(const Options &options) {
  return Grid::Ptr(new Grid(options));
}

}
}