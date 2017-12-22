//
// Created by byter on 19.08.17.
//

#ifndef THREE_CPP_COLOR_H
#define THREE_CPP_COLOR_H

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace three {

struct ColorKey
{
  const char *const name;
  const unsigned rgb;

  ColorKey(const char *name, unsigned rgb) : name(name), rgb(rgb)
  {}
};

enum class NamedColor : std::uint32_t
{
  aliceblue = 0xF0F8FF, antiquewhite = 0xFAEBD7, aqua = 0x00FFFF, aquamarine = 0x7FFFD4, azure = 0xF0FFFF,
  beige = 0xF5F5DC, bisque = 0xFFE4C4, black = 0x000000, blanchedalmond = 0xFFEBCD, blue = 0x0000FF,
  blueviolet = 0x8A2BE2, brown = 0xA52A2A, burlywood = 0xDEB887, cadetblue = 0x5F9EA0, chartreuse = 0x7FFF00,
  chocolate = 0xD2691E, coral = 0xFF7F50, cornflowerblue = 0x6495ED, cornsilk = 0xFFF8DC, crimson = 0xDC143C,
  cyan = 0x00FFFF, darkblue = 0x00008B, darkcyan = 0x008B8B, darkgoldenrod = 0xB8860B, darkgray = 0xA9A9A9,
  darkgreen = 0x006400, darkgrey = 0xA9A9A9, darkkhaki = 0xBDB76B, darkmagenta = 0x8B008B,
  darkolivegreen = 0x556B2F, darkorange = 0xFF8C00, darkorchid = 0x9932CC, darkred = 0x8B0000,
  darksalmon = 0xE9967A, darkseagreen = 0x8FBC8F, darkslateblue = 0x483D8B, darkslategray = 0x2F4F4F,
  darkslategrey = 0x2F4F4F, darkturquoise = 0x00CED1, darkviolet = 0x9400D3, deeppink = 0xFF1493,
  deepskyblue = 0x00BFFF, dimgray = 0x696969, dimgrey = 0x696969, dodgerblue = 0x1E90FF, firebrick = 0xB22222,
  floralwhite = 0xFFFAF0, forestgreen = 0x228B22, fuchsia = 0xFF00FF, gainsboro = 0xDCDCDC, ghostwhite = 0xF8F8FF,
  gold = 0xFFD700, goldenrod = 0xDAA520, gray = 0x808080, green = 0x008000, greenyellow = 0xADFF2F,
  grey = 0x808080, honeydew = 0xF0FFF0, hotpink = 0xFF69B4, indianred = 0xCD5C5C, indigo = 0x4B0082,
  ivory = 0xFFFFF0, khaki = 0xF0E68C, lavender = 0xE6E6FA, lavenderblush = 0xFFF0F5, lawngreen = 0x7CFC00,
  lemonchiffon = 0xFFFACD, lightblue = 0xADD8E6, lightcoral = 0xF08080, lightcyan = 0xE0FFFF,
  lightgoldenrodyellow = 0xFAFAD2, lightgray = 0xD3D3D3, lightgreen = 0x90EE90, lightgrey = 0xD3D3D3,
  lightpink = 0xFFB6C1, lightsalmon = 0xFFA07A, lightseagreen = 0x20B2AA, lightskyblue = 0x87CEFA,
  lightslategray = 0x778899, lightslategrey = 0x778899, lightsteelblue = 0xB0C4DE, lightyellow = 0xFFFFE0,
  lime = 0x00FF00, limegreen = 0x32CD32, linen = 0xFAF0E6, magenta = 0xFF00FF, maroon = 0x800000,
  mediumaquamarine = 0x66CDAA, mediumblue = 0x0000CD, mediumorchid = 0xBA55D3, mediumpurple = 0x9370DB,
  mediumseagreen = 0x3CB371, mediumslateblue = 0x7B68EE, mediumspringgreen = 0x00FA9A,
  mediumturquoise = 0x48D1CC, mediumvioletred = 0xC71585, midnightblue = 0x191970, mintcream = 0xF5FFFA,
  mistyrose = 0xFFE4E1, moccasin = 0xFFE4B5, navajowhite = 0xFFDEAD, navy = 0x000080, oldlace = 0xFDF5E6,
  olive = 0x808000, olivedrab = 0x6B8E23, orange = 0xFFA500, orangered = 0xFF4500, orchid = 0xDA70D6,
  palegoldenrod = 0xEEE8AA, palegreen = 0x98FB98, paleturquoise = 0xAFEEEE, palevioletred = 0xDB7093,
  papayawhip = 0xFFEFD5, peachpuff = 0xFFDAB9, peru = 0xCD853F, pink = 0xFFC0CB, plum = 0xDDA0DD,
  powderblue = 0xB0E0E6, purple = 0x800080, red = 0xFF0000, rosybrown = 0xBC8F8F,
  royalblue = 0x4169E1, saddlebrown = 0x8B4513, salmon = 0xFA8072, sandybrown = 0xF4A460, seagreen = 0x2E8B57,
  seashell = 0xFFF5EE, sienna = 0xA0522D, silver = 0xC0C0C0, skyblue = 0x87CEEB, slateblue = 0x6A5ACD,
  slategray = 0x708090, slategrey = 0x708090, snow = 0xFFFAFA, springgreen = 0x00FF7F, steelblue = 0x4682B4,
  tan = 0xD2B48C, teal = 0x008080, thistle = 0xD8BFD8, tomato = 0xFF6347, turquoise = 0x40E0D0,
  violet = 0xEE82EE, wheat = 0xF5DEB3, white = 0xFFFFFF, whitesmoke = 0xF5F5F5, yellow = 0xFFFF00,
  yellowgreen = 0x9ACD32
};

inline float hue2rgb(float p, float q, float t)
{
  if ( t < 0 ) t += 1;
  if ( t > 1 ) t -= 1;
  if ( t < 1 / 6 ) return p + ( q - p ) * 6 * t;
  if ( t < 1 / 2 ) return q;
  if ( t < 2 / 3 ) return p + ( q - p ) * 6 * ( 2 / 3 - t );
  return p;
}

struct Color
{
  union
  {
    struct
    {
      float r, g, b;
    };
    float elements[3];
  };

  Color(float r, float g, float b) : r(r), g(g), b(b)
  {}

  Color(const Color &color) : r(color.r), g(color.g), b(color.b)
  {}

  Color(unsigned hex) : r((hex >> 16 & 255) / 255.0f), g((hex >> 8 & 255) / 255.0f), b((hex & 255) / 255.0f)
  {}

  Color() :
     r(std::numeric_limits<float>::infinity()),
     g(std::numeric_limits<float>::infinity()),
     b(std::numeric_limits<float>::infinity()) {}

  static Color fromName(NamedColor name)
  {
    auto hex = (std::uint32_t)name;

    float r = ( hex >> 16 & 255 ) / 255.0f;
    float g = ( hex >> 8 & 255 ) / 255.0f;
    float b = ( hex & 255 ) / 255.0f;

    return Color(r, g, b);
  }

  bool isNull() const {
    return r == g && g == b && b == std::numeric_limits<float>::infinity();
  }

  Color &operator = (const Color &color) {
    r = color.r;
    g = color.g;
    b = color.b;

    return *this;
  }

  Color &operator *=(const Color &color)
  {
    r *= color.r;
    g *= color.g;
    b *= color.b;

    return *this;
  }

  Color &operator *=(float s)
  {
    r *= s;
    g *= s;
    b *= s;

    return *this;
  }

#if 0
  static Color fromHsl(unsigned h, float s, float l)
  {
    // h,s,l ranges are in 0.0 - 1.0
    h = euclideanModulo(h, 1);
    s = clamp( s, 0.0f, 1.0f );
    l = clamp( l, 0.0f, 1.0f );

    float r, g, b;
    if ( s == 0 ) {
      r = g = b = l;
    }
    else {
      float p = l <= 0.5 ? l * ( 1 + s ) : l + s - ( l * s );
      float q = ( 2 * l ) - p;

      r = hue2rgb( q, p, h + 1 / 3 );
      g = hue2rgb( q, p, h );
      b = hue2rgb( q, p, h - 1 / 3 );
    }
    return Color(r, g, b);
  }

  copyGammaToLinear: function ( color, gammaFactor ) {

    if ( gammaFactor === undefined ) gammaFactor = 2.0;

    this.r = Math.pow( color.r, gammaFactor );
    this.g = Math.pow( color.g, gammaFactor );
    this.b = Math.pow( color.b, gammaFactor );

    return this;

  },

  copyLinearToGamma: function ( color, gammaFactor ) {

    if ( gammaFactor === undefined ) gammaFactor = 2.0;

    var safeInverse = ( gammaFactor > 0 ) ? ( 1.0 / gammaFactor ) : 1.0;

    this.r = Math.pow( color.r, safeInverse );
    this.g = Math.pow( color.g, safeInverse );
    this.b = Math.pow( color.b, safeInverse );

    return this;

  },

  convertGammaToLinear: function () {

    var r = this.r, g = this.g, b = this.b;

    this.r = r * r;
    this.g = g * g;
    this.b = b * b;

    return this;

  },

  convertLinearToGamma: function () {

    this.r = Math.sqrt( this.r );
    this.g = Math.sqrt( this.g );
    this.b = Math.sqrt( this.b );

    return this;

  },

  getHex: function () {

    return ( this.r * 255 ) << 16 ^ ( this.g * 255 ) << 8 ^ ( this.b * 255 ) << 0;

  },

  getHexString: function () {

    return ( '000000' + this.getHex().toString( 16 ) ).slice( - 6 );

  },

  getHSL: function ( optionalTarget ) {

    // h,s,l ranges are in 0.0 - 1.0

    var hsl = optionalTarget || { h: 0, s: 0, l: 0 };

    var r = this.r, g = this.g, b = this.b;

    var max = Math.max( r, g, b );
    var min = Math.min( r, g, b );

    var hue, saturation;
    var lightness = ( min + max ) / 2.0;

    if ( min === max ) {

      hue = 0;
      saturation = 0;

    } else {

      var delta = max - min;

      saturation = lightness <= 0.5 ? delta / ( max + min ) : delta / ( 2 - max - min );

      switch ( max ) {

        case r: hue = ( g - b ) / delta + ( g < b ? 6 : 0 ); break;
        case g: hue = ( b - r ) / delta + 2; break;
        case b: hue = ( r - g ) / delta + 4; break;

      }

      hue /= 6;

    }

    hsl.h = hue;
    hsl.s = saturation;
    hsl.l = lightness;

    return hsl;

  },

  getStyle: function () {

    return 'rgb(' + ( ( this.r * 255 ) | 0 ) + ',' + ( ( this.g * 255 ) | 0 ) + ',' + ( ( this.b * 255 ) | 0 ) + ')';

  },

  offsetHSL: function ( h, s, l ) {

    var hsl = this.getHSL();

    hsl.h += h; hsl.s += s; hsl.l += l;

    this.setHSL( hsl.h, hsl.s, hsl.l );

    return this;

  },

  add: function ( color ) {

    this.r += color.r;
    this.g += color.g;
    this.b += color.b;

    return this;

  },

  addColors: function ( color1, color2 ) {

    this.r = color1.r + color2.r;
    this.g = color1.g + color2.g;
    this.b = color1.b + color2.b;

    return this;

  },

  addScalar: function ( s ) {

    this.r += s;
    this.g += s;
    this.b += s;

    return this;

  },

  sub: function( color ) {

    this.r = Math.max( 0, this.r - color.r );
    this.g = Math.max( 0, this.g - color.g );
    this.b = Math.max( 0, this.b - color.b );

    return this;

  },

  lerp: function ( color, alpha ) {

    this.r += ( color.r - this.r ) * alpha;
    this.g += ( color.g - this.g ) * alpha;
    this.b += ( color.b - this.b ) * alpha;

    return this;

  },

  equals: function ( c ) {

    return ( c.r === this.r ) && ( c.g === this.g ) && ( c.b === this.b );

  },

  fromArray: function ( array, offset ) {

    if ( offset === undefined ) offset = 0;

    this.r = array[ offset ];
    this.g = array[ offset + 1 ];
    this.b = array[ offset + 2 ];

    return this;

  },

  toArray: function ( array, offset ) {

    if ( array === undefined ) array = [];
    if ( offset === undefined ) offset = 0;

    array[ offset ] = this.r;
    array[ offset + 1 ] = this.g;
    array[ offset + 2 ] = this.b;

    return array;

  }
#endif
};

inline Color operator *(const Color &color1, const Color &color2)
{
  Color result(color1);
  result *= color2;
  return result;
}

inline Color operator *(const Color &color1, const float scalar)
{
  Color result(color1);
  result *= scalar;
  return result;
}

}


#endif //THREE_CPP_COLOR_H
