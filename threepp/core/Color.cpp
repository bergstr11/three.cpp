//
// Created by byter on 19.08.17.
//

#include "Color.h"
#include <threepp/math/Math.h>
#include <regex>
#include <iomanip>

namespace three {

using namespace std;

void parseColorHex(const string &hexString, Color &color)
{
  long rgb;
  if(hexString.length() == 3) {
    stringstream ss;
    ss << hexString.at(0) << hexString.at(0) << hexString.at(1) << hexString.at(1) << hexString.at(2) << hexString.at(2);
    rgb = strtol(ss.str().c_str(), 0, 16);
  }
  else
    rgb = strtol(hexString.c_str(), 0, 16);
  color.r = rgb / 0x10000;
  color.g = (rgb / 0x100) % 0x100;
  color.b = rgb % 0x100;
}

int _stoi(const string &s) {
  return strtol(s.c_str(), 0, 10);
}

// h,s,l ranges are in 0.0 - 1.0
void Color::setHsl(float h, float s, float l)
{
  if ( s == 0 ) {

    this->r = this->g = this->b = l;

  } else {

    float p = l <= 0.5 ? l * ( 1 + s ) : l + s - ( l * s );
    float q = ( 2 * l ) - p;

    this->r = hue2rgb( q, p, h + 1.0f / 3 );
    this->g = hue2rgb( q, p, h );
    this->b = hue2rgb( q, p, h - 1.0f / 3 );
  }
}

void Color::setStyle( const std::string &style )
{
  static const regex hr_rex(R"(^((?:rgb|hsl)a?)\(\s*([^\)]*)\))");
  static const regex rgb_rex(R"(^(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*(,\s*([0-9]*\.?[0-9]+)\s*)?$)");
  static const regex rgbp_rex(R"(^(\d+)\%\s*,\s*(\d+)\%\s*,\s*(\d+)\%\s*(,\s*([0-9]*\.?[0-9]+)\s*)?$)");
  static const regex hsla_rex(R"(^([0-9]*\.?[0-9]+)\s*,\s*(\d+)\%\s*,\s*(\d+)\%\s*(,\s*([0-9]*\.?[0-9]+)\s*)?$)");
  static const regex hex_rex(R"(^\#([A-Fa-f0-9]+)$)");

  smatch m;
  if ( regex_match(style, m, hr_rex ) ) {

    // rgb / hsl
    smatch color;
    string name = m[1];
    string components = m[2];

    if (name == "rgb" || name == "rgba") {

      if (regex_match(components, color, rgb_rex)) {

        // rgb(255,0,0) rgba(255,0,0,0.5)
        this->r = min(255, _stoi(color[1])) / 255;
        this->g = min(255, _stoi(color[2])) / 255;
        this->b = min(255, _stoi(color[3])) / 255;

        return;
      }

      if (regex_match(components, color, rgbp_rex)) {

        // rgb(100%,0%,0%) rgba(100%,0%,0%,0.5)
        this->r = min(100, _stoi(color[1])) / 100;
        this->g = min(100, _stoi(color[2])) / 100;
        this->b = min(100, _stoi(color[3])) / 100;

        return;

      }
    }
    else if (name == "hsl" || name == "hsla") {
      if (regex_match(components, color, hsla_rex)) {

        // hsl(120,50%,50%) hsla(120,50%,50%,0.5)
        float h = atof(((string) color[1]).c_str()) / 360;
        int s = _stoi(color[2]) / 100;
        int l = _stoi(color[3]) / 100;

        setHsl(h, s, l);
        return;
      }
    }
  }

  else if ( regex_match(style, m, hex_rex ) ) {

    // hex color
    string hex = m[ 1 ];
    parseColorHex(hex, *this);
  }
}

}