//
// Created by byter on 8/10/18.
//

#include <unordered_map>
#include <regex>
#include "SVG.h"
#include <3rdparty/tinyxml2/tinyxml2.h>
#include <sstream>
#include <ctgmath>

namespace three {
namespace extras {

using namespace std;
using namespace tinyxml2;

struct StyleData
{
  unordered_map<string, string> styles {{"fill", "#000"}};
  unordered_map<string, unordered_map<string, string>> classes;

  void resetLocal() {
    styles = {{"fill", "#000"}};
  }
};

float _stof(const string &s) {
  stringstream ss(s);
  float ret;
  ss >> ret;
  return ret;
}

float reflection( float a, float b )
{
  return a - ( b - a );
}

bool isVisible(const StyleData &data)
{
  return data.styles.find("fill") != data.styles.cend() && data.styles.at("fill") != "transparent";
}

void parseStyle(const XMLElement *element, StyleData &styleData )
{
  styleData.resetLocal();

  const char *style = element->Attribute( "style" );
  if(style) {
    static const regex rex(R"(;?(([^:]+)\:([^;]+)))");

    cregex_iterator rex_it(style, style + strlen(style), rex);
    cregex_iterator rex_end;

    while(rex_it != rex_end) {
      cmatch match = *rex_it;

      styleData.styles[match[2]] = match[3];
      rex_it++;
    }
  }
}

void applyColorStyle(const XMLElement *element, Color &color, StyleData &style)
{
  string fill = style.styles["fill"];

  const char *clazz = element->Attribute("class");
  if(clazz) {
    string cls(clazz);
    if(style.classes.find(cls) != style.classes.end()) {
      const auto &classStyle = style.classes[cls];
      if(classStyle.find("fill") != classStyle.end())
        fill = style.classes[cls].at("fill");
    }
  }
  color.setStyle( fill );
}

vector<float> parseFloats( string data )
{
  static const regex rex(R"([+-]?\d*\.\d+|[+-]?\d+)");

  vector<float> floats;

  sregex_iterator rex_it(data.begin(), data.end(), rex);
  sregex_iterator rex_end;

  while(rex_it != rex_end) {
    smatch match = *rex_it;

    floats.push_back( _stof(match.str()) );
    rex_it++;
  }

  return floats;
}

float svgAngle( float ux, float uy, float vx, float vy )
{
  float dot = ux * vx + uy * vy;
  float len = sqrt( ux * ux + uy * uy ) *  sqrt( vx * vx + vy * vy );
  float ang = acos( max( -1.0f, min( 1.0f, dot / len ) ) ); // floating point precision, slightly over values appear
  if ( ( ux * vy - uy * vx ) < 0 ) ang = - ang;
  return ang;
}

/**
 * https://www.w3.org/TR/SVG/implnote.html#ArcImplementationNotes
 * https://mortoray.com/2017/02/16/rendering-an-svg-elliptical-arc-as-bezier-curves/ Appendix: Endpoint to center arc conversion
 * From
 * rx ry x-axis-rotation large-arc-flag sweep-flag x y
 * To
 * aX, aY, xRadius, yRadius, aStartAngle, aEndAngle, aClockwise, aRotation
 */

void parseArcCommand( extras::ShapePath &path, float rx, float ry, float x_axis_rotation, float large_arc_flag,
                      float sweep_flag, const math::Vector2 &start, const math::Vector2 &end )
{
  x_axis_rotation = x_axis_rotation * M_PI / 180;

  // Ensure radii are positive
  rx = abs( rx );
  ry = abs( ry );

  // Compute (x1′, y1′)
  float dx2 = ( start.x() - end.x() ) / 2.0f;
  float dy2 = ( start.y() - end.y() ) / 2.0f;
  float x1p = cos( x_axis_rotation ) * dx2 + sin( x_axis_rotation ) * dy2;
  float y1p = - sin( x_axis_rotation ) * dx2 + cos( x_axis_rotation ) * dy2;

  // Compute (cx′, cy′)
  float rxs = rx * rx;
  float rys = ry * ry;
  float x1ps = x1p * x1p;
  float y1ps = y1p * y1p;

  // Ensure radii are large enough
  float cr = x1ps / rxs + y1ps / rys;

  if ( cr > 1 ) {

    // scale up rx,ry equally so cr == 1
    float s = sqrt( cr );
    rx = s * rx;
    ry = s * ry;
    rxs = rx * rx;
    rys = ry * ry;

  }

  float dq = ( rxs * y1ps + rys * x1ps );
  float pq = ( rxs * rys - dq ) / dq;
  float q = sqrt( max( 0.0f, pq ) );
  if ( large_arc_flag == sweep_flag ) q = - q;
  float cxp = q * rx * y1p / ry;
  float cyp = - q * ry * x1p / rx;

  // Step 3: Compute (cx, cy) from (cx′, cy′)
  float cx = cos( x_axis_rotation ) * cxp - sin( x_axis_rotation ) * cyp + ( start.x() + end.x() ) / 2.0f;
  float cy = sin( x_axis_rotation ) * cxp + cos( x_axis_rotation ) * cyp + ( start.y() + end.y() ) / 2.0f;

  // Step 4: Compute θ1 and Δθ
  float theta = svgAngle( 1, 0, ( x1p - cxp ) / rx, ( y1p - cyp ) / ry );
  float delta = svgAngle( ( x1p - cxp ) / rx, ( y1p - cyp ) / ry, ( - x1p - cxp ) / rx, fmod(( - y1p - cyp ) / ry , (float)M_PI * 2.0f ));

  path.currentPath->absellipse( cx, cy, rx, ry, theta, theta + delta, sweep_flag == 0, x_axis_rotation );

}

void parsePathNode(const XMLElement *element, StyleData &style, vector<extras::ShapePath> &paths)
{
  static const regex rex("([a-df-z])([^a-df-z]*)", regex::icase);

  extras::ShapePath path;
  applyColorStyle(element, path.color, style);

  math::Vector2 point;
  math::Vector2 control;

  const char *d = element->Attribute( "d" );

  cregex_iterator rex_it(d, d + strlen(d), rex);
  cregex_iterator rex_end;

  vector<float> numbers;

  while(rex_it != rex_end) {
    cmatch match = *rex_it;
    rex_it++;

    char command = match[1].str().at(0);
    string data = match[2];

    switch ( command ) {

      case 'M':
        numbers = parseFloats( data );
        for ( unsigned j = 0, jl = numbers.size(); j < jl; j += 2 ) {
          point.set(numbers[ j + 0 ], numbers[ j + 1 ]);
          control = point;

          path.moveTo( point.x(), point.y() );
        }
        break;

      case 'H':
        numbers = parseFloats( data );
        for ( unsigned j = 0, jl = numbers.size(); j < jl; j ++ ) {
          point.x() = numbers[ j ];
          control = point;
          path.lineTo( point.x(), point.y(), true );
        }
        break;

      case 'V':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j ++ ) {
          point.y() = numbers[ j ];
          control = point;
          path.lineTo( point.x(), point.y(), true );
        }
        break;

      case 'L':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 2 ) {
          point.set(numbers[ j + 0 ], numbers[ j + 1 ]);
          control = point;
          path.lineTo( point.x(), point.y(), true );
        }
        break;

      case 'C':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 6 ) {
          path.bezierCurveTo(
             numbers[ j + 0 ],
             numbers[ j + 1 ],
             numbers[ j + 2 ],
             numbers[ j + 3 ],
             numbers[ j + 4 ],
             numbers[ j + 5 ]
          );
          control.set(numbers[ j + 2 ], numbers[ j + 3 ]);
          point.set(numbers[ j + 4 ], numbers[ j + 5 ]);
        }
        break;

      case 'S':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 4 ) {
          path.bezierCurveTo(
             reflection( point.x(), control.x() ),
             reflection( point.y(), control.y() ),
             numbers[ j + 0 ],
             numbers[ j + 1 ],
             numbers[ j + 2 ],
             numbers[ j + 3 ]
          );
          control.set(numbers[ j + 0 ], numbers[ j + 1 ]);
          point.set(numbers[ j + 2 ], numbers[ j + 3 ]);
        }
        break;

      case 'Q':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 4 ) {
          path.quadraticCurveTo(
             numbers[ j + 0 ],
             numbers[ j + 1 ],
             numbers[ j + 2 ],
             numbers[ j + 3 ]
          );
          control.set(numbers[ j + 0 ], numbers[ j + 1 ]);
          point.set(numbers[ j + 2 ], numbers[ j + 3 ]);
        }
        break;

      case 'T':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 2 ) {
          float rx = reflection( point.x(), control.x() );
          float ry = reflection( point.y(), control.y() );
          path.quadraticCurveTo(
             rx,
             ry,
             numbers[ j + 0 ],
             numbers[ j + 1 ]
          );
          control.set(rx, ry);
          point.set(numbers[ j + 0 ], numbers[ j + 1 ]);
        }
        break;

      case 'A':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 7 ) {
          math::Vector2 start = point;
          point.set(numbers[ j + 5 ], numbers[ j + 6 ]);
          control = point;
          parseArcCommand(
             path, numbers[ j ], numbers[ j + 1 ], numbers[ j + 2 ], numbers[ j + 3 ], numbers[ j + 4 ], start, point
          );
        }
        break;

        //

      case 'm':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 2 ) {
          point.x() += numbers[ j + 0 ];
          point.y() += numbers[ j + 1 ];
          control = point;
          path.moveTo( point.x(), point.y() );
        }
        break;

      case 'h':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j ++ ) {
          point.x() += numbers[ j ];
          control = point;
          path.lineTo( point.x(), point.y(), true );
        }
        break;

      case 'v':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j ++ ) {
          point.y() += numbers[ j ];
          control = point;
          path.lineTo( point.x(), point.y(), true );
        }
        break;

      case 'l':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 2 ) {
          point.x() += numbers[ j + 0 ];
          point.y() += numbers[ j + 1 ];
          control = point;
          path.lineTo( point.x(), point.y(), true );
        }
        break;

      case 'c':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 6 ) {
          path.bezierCurveTo(
             point.x() + numbers[ j + 0 ],
             point.y() + numbers[ j + 1 ],
             point.x() + numbers[ j + 2 ],
             point.y() + numbers[ j + 3 ],
             point.x() + numbers[ j + 4 ],
             point.y() + numbers[ j + 5 ]
          );
          control.x() = point.x() + numbers[ j + 2 ];
          control.y() = point.y() + numbers[ j + 3 ];
          point.x() += numbers[ j + 4 ];
          point.y() += numbers[ j + 5 ];
        }
        break;

      case 's':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 4 ) {
          path.bezierCurveTo(
             reflection( point.x(), control.x() ),
             reflection( point.y(), control.y() ),
             point.x() + numbers[ j + 0 ],
             point.y() + numbers[ j + 1 ],
             point.x() + numbers[ j + 2 ],
             point.y() + numbers[ j + 3 ]
          );
          control.x() = point.x() + numbers[ j + 0 ];
          control.y() = point.y() + numbers[ j + 1 ];
          point.x() += numbers[ j + 2 ];
          point.y() += numbers[ j + 3 ];
        }
        break;

      case 'q':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 4 ) {
          path.quadraticCurveTo(
             point.x() + numbers[ j + 0 ],
             point.y() + numbers[ j + 1 ],
             point.x() + numbers[ j + 2 ],
             point.y() + numbers[ j + 3 ]
          );
          control.x() = point.x() + numbers[ j + 0 ];
          control.y() = point.y() + numbers[ j + 1 ];
          point.x() += numbers[ j + 2 ];
          point.y() += numbers[ j + 3 ];
        }
        break;

      case 't':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 2 ) {
          float rx = reflection( point.x(), control.x() );
          float ry = reflection( point.y(), control.y() );
          path.quadraticCurveTo(
             rx,
             ry,
             point.x() + numbers[ j + 0 ],
             point.y() + numbers[ j + 1 ]
          );
          control.set(rx, ry);
          point.x() = point.x() + numbers[ j + 0 ];
          point.y() = point.y() + numbers[ j + 1 ];
        }
        break;

      case 'a':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 7 ) {
          math::Vector2 start = point;
          point.x() += numbers[ j + 5 ];
          point.y() += numbers[ j + 6 ];
          control = point;
          parseArcCommand(
             path, numbers[ j ], numbers[ j + 1 ], numbers[ j + 2 ], numbers[ j + 3 ], numbers[ j + 4 ], start, point
          );
        }
        break;

        //

      case 'Z':
      case 'z':
        path.close(true);
        break;
    }
  }

  paths.push_back(path);
}

/*
* According to https://www.w3.org/TR/SVG/shapes.html#RectElementRXAttribute
* rounded corner should be rendered to elliptical arc, but bezier curve does the job well enough
*/
void parseRectNode(const XMLElement *element, StyleData &style, vector<extras::ShapePath> &paths)
{
  float x = atof(element->Attribute("x", "0"));
  float y = atof(element->Attribute("y", "0"));
  float rx = atof(element->Attribute("rx", "0"));
  float ry = atof(element->Attribute("ry", "0"));
  float w = atof(element->Attribute("width"));
  float h = atof(element->Attribute("height"));

  extras::ShapePath path;
  applyColorStyle(element, path.color, style);
  path.moveTo( x + 2 * rx, y );
  path.lineTo( x + w - 2 * rx, y );
  if ( rx != 0 || ry != 0 ) path.bezierCurveTo( x + w, y, x + w, y, x + w, y + 2 * ry );
  path.lineTo( x + w, y + h - 2 * ry );
  if ( rx != 0 || ry != 0 ) path.bezierCurveTo( x + w, y + h, x + w, y + h, x + w - 2 * rx, y + h );
  path.lineTo( x + 2 * rx, y + h );

  if ( rx != 0 || ry != 0 ) {

    path.bezierCurveTo( x, y + h, x, y + h, x, y + h - 2 * ry );
    path.lineTo( x, y + 2 * ry );
    path.bezierCurveTo( x, y, x, y, x + 2 * rx, y );
  }
  paths.push_back(path);
}

void parsePolyNode(const XMLElement *element, StyleData &style, vector<extras::ShapePath> &paths, bool closed)
{
  static const regex rex(R"((-?[\d\.?]+)[,|\s](-?[\d\.?]+))");

  extras::ShapePath path;
  applyColorStyle(element, path.color, style);

  unsigned index = 0;

  const char *points = element->Attribute( "points" );
  cregex_iterator rex_it(points, points + strlen(points), rex);
  cregex_iterator rex_end;

  while(rex_it != rex_end) {
    std::cmatch match = *rex_it;

    float x = _stof(match[1]);
    float y = _stof(match[2]);

    if(index == 0) path.moveTo(x, y);
    else path.lineTo(x, y);

    index++;
    rex_it++;
  }

  path.currentPath->setAutoClose(closed);
  paths.push_back(path);
}

void parsePolygonNode(const XMLElement *element, StyleData &style, vector<extras::ShapePath> &paths)
{
  parsePolyNode(element, style, paths, true);
}

void parsePolylineNode(const XMLElement *element, StyleData &style, vector<extras::ShapePath> &paths)
{
  parsePolyNode(element, style, paths, false);
}

void parseCircleNode(const XMLElement *element, StyleData &style, vector<extras::ShapePath> &paths)
{
  float x = atof( element->Attribute( "cx" ) );
  float y = atof( element->Attribute( "cy" ) );
  float r = atof( element->Attribute( "r" ) );

  extras::Path::Ptr subpath = extras::Path::make();
  subpath->absarc( x, y, r, 0, M_PI * 2 );

  extras::ShapePath path;
  applyColorStyle(element, path.color, style);
  path.subPaths.push_back( subpath );

  paths.push_back(path);
}

void parseEllipseNode(const XMLElement *element, StyleData &style, vector<extras::ShapePath> &paths)
{
  float x = atof( element->Attribute( "cx" ) );
  float y = atof( element->Attribute( "cy" ) );
  float rx = atof( element->Attribute( "rx" ) );
  float ry = atof( element->Attribute( "ry" ) );

  extras::Path::Ptr subpath = extras::Path::make();
  subpath->absellipse( x, y, rx, ry, 0, M_PI * 2 );

  extras::ShapePath path;
  applyColorStyle(element, path.color, style);
  path.subPaths.push_back( subpath );

  paths.push_back(path);
}

void parseLineNode(const XMLElement *element, StyleData &style, vector<extras::ShapePath> &paths)
{
  float x1 = atof( element->Attribute( "x1" ) );
  float y1 = atof( element->Attribute( "y1" ) );
  float x2 = atof( element->Attribute( "x2" ) );
  float y2 = atof( element->Attribute( "y2" ) );

  extras::ShapePath path;
  path.moveTo( x1, y1 );
  path.lineTo( x2, y2 );
  path.currentPath->setAutoClose(false);

  paths.push_back(path);
}

void parseDefs(const XMLElement *element, StyleData &styles)
{
  static const regex rex(R"((\.[^\{]+)\{([^}]+)\})");
  static const regex rex2(R"((\w+)\:([\w#]+))");

  const XMLElement *style = element->FirstChildElement("style");
  if(style) {
    const char *text = style->GetText();

    cregex_iterator rex_it(text, text + strlen(text), rex);
    cregex_iterator rex_end;

    while(rex_it != rex_end) {
      cmatch match = *rex_it;

      string cls = match[1].str().substr(1);
      cregex_iterator rex2_it(match[2].first, match[2].first+match[2].length(), rex2);
      while(rex2_it != rex_end) {
        string match = rex2_it->str();
        auto pos = match.find(':');

        string key = match.substr(0, pos);
        string value = match.substr(pos+1, match.length());

        unordered_map<string, string> clsdata;
        clsdata.emplace(key, value);
        styles.classes.emplace(cls, clsdata);
        rex2_it++;
      }

      rex_it++;
    }

  }
}

void parse(const XMLElement *element, StyleData &style, vector<extras::ShapePath> &paths)
{
  if(!strcmp(element->Name(), "defs")) {
    parseDefs(element, style);
  }
  if(!strcmp(element->Name(), "g")) {
    parseStyle(element, style);
  }
  else if(!strcmp(element->Name(), "path")) {
    parseStyle(element, style);
    if(element->Attribute("d") && isVisible(style)) parsePathNode(element, style, paths);
  }
  else if(!strcmp(element->Name(), "rect")) {
    parseStyle(element, style);
    if(isVisible(style)) parseRectNode(element, style, paths);
  }
  else if(!strcmp(element->Name(), "polygon")) {
    parseStyle(element, style);
    if(isVisible(style)) parsePolygonNode(element, style, paths);
  }
  else if(!strcmp(element->Name(), "polyline")) {
    parseStyle(element, style);
    if(isVisible(style)) parsePolylineNode(element, style, paths);
  }
  else if(!strcmp(element->Name(), "circle")) {
    parseStyle(element, style);
    if(isVisible(style)) parseCircleNode(element, style, paths);
  }
  else if(!strcmp(element->Name(), "ellipse")) {
    parseStyle(element, style);
    if(isVisible(style)) parseEllipseNode(element, style, paths);
  }
  else if(!strcmp(element->Name(), "line")) {
    parseStyle(element, style);
    if(isVisible(style)) parseLineNode(element, style, paths);
  }

  for(const XMLElement *child = element->FirstChildElement(); child; child = child->NextSiblingElement())
    parse(child, style, paths);
}

void read(const tinyxml2::XMLDocument &doc, vector<extras::ShapePath> &paths)
{
  const XMLElement *elem = doc.FirstChildElement("svg");
  if(!elem) return;

  StyleData style;
  parse(elem, style, paths);
}

SVG SVG::fromFile(const string &file)
{
  SVG svg;
  tinyxml2::XMLDocument doc;
  doc.LoadFile( file.c_str() );

  read(doc, svg._paths);
  return svg;
}

SVG SVG::fromXML(const string &xml)
{
  SVG svg;
  tinyxml2::XMLDocument doc;
  doc.Parse( xml.c_str(), xml.length() );

  read(doc, svg._paths);
  return svg;
}

SVG SVG::fromXML(const char *xml, size_t size)
{
  SVG svg;
  tinyxml2::XMLDocument doc;
  doc.Parse( xml, size );

  read(doc, svg._paths);
  return svg;
}

}
}