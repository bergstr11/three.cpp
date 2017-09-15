//
// Created by byter on 29.07.17.
//

#ifndef THREEQT_LAYERS
#define THREEQT_LAYERS

namespace three {

class Layers
{
  unsigned int mask;

public:

  void set(unsigned int channel) {
    mask = 1 << channel | 0;
  }

  void enable(unsigned int channel) {
    mask |= 1 << channel | 0;
  }

  void toggle(unsigned int channel) {
    mask ^= 1 << channel | 0;
  }

  void disable(unsigned int channel) {
    mask &= ~ ( 1 << channel | 0 );
  }

  bool test(const Layers &layers) const {
    return (mask & layers.mask ) != 0;
  }
};

}

#endif //THREEQT_LAYERS
