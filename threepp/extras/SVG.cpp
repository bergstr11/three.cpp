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

bool isVisible(const unordered_map<string, string> &style)
{
  return style.find("fill") != style.cend() && style.at("fill") != "transparent";
}

void parseStyle( XMLElement *element, unordered_map<string, string> &style )
{
  if ( element->Attribute( "fill" ) ) style["fill"] = element->Attribute( "fill" );
}

vector<float> parseFloats( string data )
{
  static const regex rex(R"([\s,]+|(?=\s?[+\-]))");
  static const regex rex2(R"((\d*)?\.(\d+))");

  vector<float> floats;

  sregex_iterator rex_it(data.begin(), data.end(), rex);
  sregex_iterator rex_end;

  while(rex_it != rex_end) {
    smatch match = *rex_it;

    string number = match.str();

    // Handle values like 48.6037.7.8
    sregex_iterator rex2_it(number.begin(), number.end(), rex2);
    while(rex2_it != rex_end) {
      smatch match2 = *rex2_it;

      floats.push_back( _stof(number) );
    }
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
  float cx = cos( x_axis_rotation ) * cxp - sin( x_axis_rotation ) * cyp + ( start.x() + end.x() ) / 2;
  float cy = sin( x_axis_rotation ) * cxp + cos( x_axis_rotation ) * cyp + ( start.y() + end.y() ) / 2;

  // Step 4: Compute θ1 and Δθ
  float theta = svgAngle( 1, 0, ( x1p - cxp ) / rx, ( y1p - cyp ) / ry );
  float delta = svgAngle( ( x1p - cxp ) / rx, ( y1p - cyp ) / ry, ( - x1p - cxp ) / rx, fmod(( - y1p - cyp ) / ry , (float)M_PI * 2.0f ));

  path.currentPath->absellipse( cx, cy, rx, ry, theta, theta + delta, sweep_flag == 0, x_axis_rotation );

}

void parsePathNode(XMLElement *element, unordered_map<string, string> &style, vector<extras::ShapePath> &paths)
{
  static const regex rex(R"(([a-df-z])([^a-df-z]*))", regex::icase || regex::ECMAScript);

  extras::ShapePath path;
  path.color.setStyle( style["fill"] );

  math::Vector2 point;
  math::Vector2 control;

  string d = element->Attribute( "d" );

  sregex_iterator rex_it(d.begin(), d.end(), rex);
  sregex_iterator rex_end;

  vector<float> numbers;

  while(rex_it != rex_end) {
    smatch match = *rex_it;

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
          path.lineTo( point.x(), point.y() );
        }
        break;

      case 'V':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j ++ ) {
          point.y() = numbers[ j ];
          control = point;
          path.lineTo( point.x(), point.y() );
        }
        break;

      case 'L':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 2 ) {
          point.set(numbers[ j + 0 ], numbers[ j + 1 ]);
          control = point;
          path.lineTo( point.x(), point.y() );
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
          path.lineTo( point.x(), point.y() );
        }
        break;

      case 'v':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j ++ ) {
          point.y() += numbers[ j ];
          control = point;
          path.lineTo( point.x(), point.y() );
        }
        break;

      case 'l':
        numbers = parseFloats( data );
        for ( unsigned j=0, jl=numbers.size(); j < jl; j += 2 ) {
          point.x() += numbers[ j + 0 ];
          point.y() += numbers[ j + 1 ];
          control = point;
          path.lineTo( point.x(), point.y() );
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
        path.currentPath->setAutoClose(true);
        if ( path.currentPath->curves().size() > 0 ) {
          // Reset point to beginning of Path
          auto &curve = path.currentPath->curve(0);
          point = curve->start();
          path.currentPath->moveTo(point.x(), point.y());
        }
        break;

    }
  }

  paths.push_back(path);
}

/*
* According to https://www.w3.org/TR/SVG/shapes.html#RectElementRXAttribute
* rounded corner should be rendered to elliptical arc, but bezier curve does the job well enough
*/
void parseRectNode(XMLElement *element, unordered_map<string, string> &style, vector<extras::ShapePath> &paths)
{
  float x = atof(element->Attribute("x", "0"));
  float y = atof(element->Attribute("y", "0"));
  float rx = atof(element->Attribute("rx", "0"));
  float ry = atof(element->Attribute("ry", "0"));
  float w = atof(element->Attribute("width"));
  float h = atof(element->Attribute("height"));

  extras::ShapePath path;
  path.color.setStyle( style["fill"] );
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

void parsePolyNode(XMLElement *element, unordered_map<string, string> &style, vector<extras::ShapePath> &paths, bool closed)
{
  static const regex rex(R"((-?[\d\.?]+)[,|\s](-?[\d\.?]+))");

  extras::ShapePath path;
  path.color.setStyle( style["fill"] );

  unsigned index = 0;

  string points = element->Attribute( "points" );
  sregex_iterator rex_it(points.begin(), points.end(), rex);
  sregex_iterator rex_end;

  while(rex_it != rex_end) {
    std::smatch match = *rex_it;

    float x = _stof(match[1]);
    float y = _stof(match[2]);

    if(index == 0) path.moveTo(x, y);
    else path.lineTo(x, y);

    index++;
  }

  path.currentPath->setAutoClose(closed);
  paths.push_back(path);
}

void parsePolygonNode(XMLElement *element, unordered_map<string, string> &style, vector<extras::ShapePath> &paths)
{
  parsePolyNode(element, style, paths, true);
}

void parsePolylineNode(XMLElement *element, unordered_map<string, string> &style, vector<extras::ShapePath> &paths)
{
  parsePolyNode(element, style, paths, false);
}

void parseCircleNode(XMLElement *element, unordered_map<string, string> &style, vector<extras::ShapePath> &paths)
{
  float x = atof( element->Attribute( "cx" ) );
  float y = atof( element->Attribute( "cy" ) );
  float r = atof( element->Attribute( "r" ) );

  extras::Path::Ptr subpath = extras::Path::make();
  subpath->absarc( x, y, r, 0, M_PI * 2 );

  extras::ShapePath path;
  path.color.setStyle( style["fill"] );
  path.subPaths.push_back( subpath );

  paths.push_back(path);
}

void parseEllipseNode(XMLElement *element, unordered_map<string, string> &style, vector<extras::ShapePath> &paths)
{
  float x = atof( element->Attribute( "cx" ) );
  float y = atof( element->Attribute( "cy" ) );
  float rx = atof( element->Attribute( "rx" ) );
  float ry = atof( element->Attribute( "ry" ) );

  extras::Path::Ptr subpath = extras::Path::make();
  subpath->absellipse( x, y, rx, ry, 0, M_PI * 2 );

  extras::ShapePath path;
  path.color.setStyle( style["fill"] );
  path.subPaths.push_back( subpath );

  paths.push_back(path);
}

void parseLineNode(XMLElement *element, unordered_map<string, string> &style, vector<extras::ShapePath> &paths)
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

void parse(XMLElement *element, unordered_map<string, string> &style, vector<extras::ShapePath> &paths)
{
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

  for(XMLElement *child = element->FirstChildElement(); child; child = element->NextSiblingElement())
    parse(child, style, paths);
}

void SVG::load(std::string name)
{
  tinyxml2::XMLDocument doc;
  doc.LoadFile( name.c_str() );

  XMLElement *svg = doc.FirstChildElement("svg");

  unordered_map<string, string> style {{ "fill", "#000" }};
  parse( svg, style, paths);
}

}
}