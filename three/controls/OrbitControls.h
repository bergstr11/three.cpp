//
// Created by byter on 12/24/17.
//

#ifndef THREEPP_ORBITCONTROLS_H
#define THREEPP_ORBITCONTROLS_H

#include <camera/Camera.h>
#include <math/Spherical.h>
#include <helper/simplesignal.h>
#include <QDebug>

//var controls = new THREE.OrbitControls( camera, renderer.domElement );

namespace three {

/**
 * @author qiao / https://github.com/qiao
 * @author mrdoob / http://mrdoob.com
 * @author alteredq / http://alteredqualia.com/
 * @author WestLangley / http://github.com/WestLangley
 * @author erich666 / http://erichaines.com
 */

// This set of controls performs orbiting, dollying (zooming), and panning.
// Unlike TrackballControls, it maintains the "up" direction object.up (+Y by default).
//
//    Orbit - left mouse / touch: one finger move
//    Zoom - middle mouse, or mousewheel / touch: two finger spread or squish
//    Pan - right mouse, or arrow keys / touch: three finger swipe
class OrbitControls
{
protected:
  Camera::Ptr _camera;

  // for reset
  math::Vector3 _target0;
  math::Vector3 _position0;
  float _zoom0;

  enum class State
  {
    NONE = -1, ROTATE = 0, DOLLY = 1, PAN = 2, TOUCH_ROTATE = 3, TOUCH_DOLLY = 4, TOUCH_PAN = 5
  };

  State state = State::NONE;

  // current position in spherical coordinates
  math::Spherical _spherical;
  math::Spherical _sphericalDelta;

  float scale = 1;
  math::Vector3 _panOffset;
  bool _zoomChanged = false;

  math::Vector2 _rotateStart;
  math::Vector2 _rotateEnd;
  math::Vector2 _rotateDelta;

  math::Vector2 _panStart;
  math::Vector2 _panEnd;
  math::Vector2 _panDelta;

  math::Vector2 _dollyStart;
  math::Vector2 _dollyEnd;
  math::Vector2 _dollyDelta;

  math::Vector3 _offset;

  // so camera.up is the orbit axis
  math::Quaternion _quat;
  math::Quaternion _quatInverse;

  math::Vector3 lastPosition;
  math::Quaternion lastQuaternion;

  virtual void init()
  {
    _quat = math::Quaternion::fromUnitVectors(_camera->up(), math::Vector3(0, 1, 0));
    _quatInverse = _quat.inverse();
  }

  explicit OrbitControls(Camera::Ptr camera)
     : _camera(camera), _target0(target), _position0(camera->position()), _zoom0(camera->zoom())
  {
    init();
  }

  virtual unsigned clientWidth() = 0;
  virtual unsigned clientHeight() = 0;

  // deltaX and deltaY are in pixels; right and down are positive
  void pan(float deltaX, float deltaY);
  void dollyIn(float dollyScale);
  void dollyOut(float dollyScale);

  bool update();

  void startRotate(unsigned x, unsigned y);
  void startZoom(unsigned x, unsigned y);
  void startPan(unsigned x, unsigned y);

  void doRotate(unsigned x, unsigned y);
  void doDolly(unsigned x, unsigned y);
  void doPan(unsigned x, unsigned y);

public:
  using Ptr = std::shared_ptr<OrbitControls>;

  Signal<void()> onChanged;
  Signal<void()> onStart;
  Signal<void()> onEnd;

  // Set to false to disable this control
  bool enabled = true;

  // "target" sets the location of focus, where the object orbits around
  math::Vector3 target;

  // How far you can dolly in and out ( PerspectiveCamera only )
  float minDistance = 0;
  float maxDistance = std::numeric_limits<float>::infinity();

  // How far you can zoom in and out ( OrthographicCamera only )
  float minZoom = 0;
  float maxZoom = std::numeric_limits<float>::infinity();

  // How far you can orbit vertically, upper and lower limits.
  // Range is 0 to Math.PI radians.
  float minPolarAngle = 0; // radians
  float maxPolarAngle = (float) M_PI; // radians

  // How far you can orbit horizontally, upper and lower limits.
  // If set, must be a sub-interval of the interval [ - Math.PI, Math.PI ].
  float minAzimuthAngle = -std::numeric_limits<float>::infinity(); // radians
  float maxAzimuthAngle = std::numeric_limits<float>::infinity(); // radians

  // Set to true to enable damping (inertia)
  // If damping is enabled, you must call controls.update() in your animation loop
  bool enableDamping = false;
  float dampingFactor = 0.25f;

  // This option actually enables dollying in and out; left as "zoom" for backwards compatibility.
  // Set to false to disable zooming
  bool enableZoom = true;
  float zoomSpeed = 1.0f;

  // Set to false to disable rotating
  bool enableRotate = true;
  float rotateSpeed = 1.0f;

  // Set to false to disable panning
  bool enablePan = true;
  float keyPanSpeed = 7.0f;  // pixels moved per arrow key push

  // Set to true to automatically rotate around the target
  // If auto-rotate is enabled, you must call controls.update() in your animation loop
  bool autoRotate = false;
  float autoRotateSpeed = 2.0f; // 30 seconds per round when fps is 60

  // Set to false to disable use of the keys
  bool enableKeys = true;

  float getPolarAngle() const
  {
    return _spherical.phi();
  }

  float getAzimuthalAngle() const
  {
    return _spherical.theta();
  }

  float getAutoRotationAngle()
  {
    return 2 * (float)M_PI / 60 / 60 * autoRotateSpeed;
  }

  float getZoomScale()
  {
    return std::pow( 0.95f, zoomSpeed );
  }

  void rotateLeft(float angle)
  {
    _sphericalDelta.theta() -= angle;
  }

  void rotateUp(float angle)
  {
    _sphericalDelta.phi() -= angle;
  }

  void panLeft(float distance, const math::Matrix4 &objectMatrix)
  {

    math::Vector3 v = math::Vector3::fromMatrixColumn( objectMatrix, 0 ); // get X column of objectMatrix
    v *= -distance;
    qDebug() << "panLeft" << distance << "|" << v.x() << v.y() << v.z();

    _panOffset += v;
  }

  void panUp(float distance, const math::Matrix4 &objectMatrix)
  {
    math::Vector3 v = math::Vector3::fromMatrixColumn( objectMatrix, 1 ); // get Y column of objectMatrix
    v *= distance;
    qDebug() << "panUp" << distance << "|" << v.x() << v.y() << v.z();

    _panOffset += v;
  }

  void saveState()
  {
    _target0 = target;
    _position0 = _camera->position();
    _zoom0 = _camera->zoom();
  }

  virtual void reset()
  {
    target = _target0;
    _camera->position() = _position0;
    _camera->setZoom(_zoom0);
    _camera->updateProjectionMatrix();

    onChanged.emitSignal();

    update();

    state = State::NONE;
  }

  bool handleEvent(unsigned x, unsigned y);

  bool resetState();
};

}
#if 0
//
	// event callbacks - update the object state
	//

	function handleKeyDown( event ) {

		//console.log( 'handleKeyDown' );

		switch ( event.keyCode ) {

			case scope.keys.UP:
				pan( 0, scope.keyPanSpeed );
				scope.update();
				break;

			case scope.keys.BOTTOM:
				pan( 0, - scope.keyPanSpeed );
				scope.update();
				break;

			case scope.keys.LEFT:
				pan( scope.keyPanSpeed, 0 );
				scope.update();
				break;

			case scope.keys.RIGHT:
				pan( - scope.keyPanSpeed, 0 );
				scope.update();
				break;

		}

	}

	function handleTouchStartRotate( event ) {

		//console.log( 'handleTouchStartRotate' );

		rotateStart.set( event.touches[ 0 ].pageX, event.touches[ 0 ].pageY );

	}

	function handleTouchStartDolly( event ) {

		//console.log( 'handleTouchStartDolly' );

		var dx = event.touches[ 0 ].pageX - event.touches[ 1 ].pageX;
		var dy = event.touches[ 0 ].pageY - event.touches[ 1 ].pageY;

		var distance = Math.sqrt( dx * dx + dy * dy );

		dollyStart.set( 0, distance );

	}

	function handleTouchStartPan( event ) {

		//console.log( 'handleTouchStartPan' );

		panStart.set( event.touches[ 0 ].pageX, event.touches[ 0 ].pageY );

	}

	function handleTouchMoveRotate( event ) {

		//console.log( 'handleTouchMoveRotate' );

		rotateEnd.set( event.touches[ 0 ].pageX, event.touches[ 0 ].pageY );
		rotateDelta.subVectors( rotateEnd, rotateStart );

		var element = scope.domElement === document ? scope.domElement.body : scope.domElement;

		// rotating across whole screen goes 360 degrees around
		rotateLeft( 2 * Math.PI * rotateDelta.x / element.clientWidth * scope.rotateSpeed );

		// rotating up and down along whole screen attempts to go 360, but limited to 180
		rotateUp( 2 * Math.PI * rotateDelta.y / element.clientHeight * scope.rotateSpeed );

		rotateStart.copy( rotateEnd );

		scope.update();

	}

	function handleTouchMoveDolly( event ) {

		//console.log( 'handleTouchMoveDolly' );

		var dx = event.touches[ 0 ].pageX - event.touches[ 1 ].pageX;
		var dy = event.touches[ 0 ].pageY - event.touches[ 1 ].pageY;

		var distance = Math.sqrt( dx * dx + dy * dy );

		dollyEnd.set( 0, distance );

		dollyDelta.subVectors( dollyEnd, dollyStart );

		if ( dollyDelta.y > 0 ) {

			dollyOut( getZoomScale() );

		} else if ( dollyDelta.y < 0 ) {

			dollyIn( getZoomScale() );

		}

		dollyStart.copy( dollyEnd );

		scope.update();

	}

	function handleTouchMovePan( event ) {

		//console.log( 'handleTouchMovePan' );

		panEnd.set( event.touches[ 0 ].pageX, event.touches[ 0 ].pageY );

		panDelta.subVectors( panEnd, panStart );

		pan( panDelta.x, panDelta.y );

		panStart.copy( panEnd );

		scope.update();

	}

	function handleTouchEnd( event ) {

		//console.log( 'handleTouchEnd' );

	}

	//
	// event handlers - FSM: listen for events and reset state
	//


	function onKeyDown( event ) {

		if ( scope.enabled === false || scope.enableKeys === false || scope.enablePan === false ) return;

		handleKeyDown( event );

	}

	function onTouchStart( event ) {

		if ( scope.enabled === false ) return;

		switch ( event.touches.length ) {

			case 1:	// one-fingered touch: rotate

				if ( scope.enableRotate === false ) return;

				handleTouchStartRotate( event );

				state = STATE.TOUCH_ROTATE;

				break;

			case 2:	// two-fingered touch: dolly

				if ( scope.enableZoom === false ) return;

				handleTouchStartDolly( event );

				state = STATE.TOUCH_DOLLY;

				break;

			case 3: // three-fingered touch: pan

				if ( scope.enablePan === false ) return;

				handleTouchStartPan( event );

				state = STATE.TOUCH_PAN;

				break;

			default:

				state = STATE.NONE;

		}

		if ( state !== STATE.NONE ) {

			scope.dispatchEvent( startEvent );

		}

	}

	function onTouchMove( event ) {

		if ( scope.enabled === false ) return;

		event.preventDefault();
		event.stopPropagation();

		switch ( event.touches.length ) {

			case 1: // one-fingered touch: rotate

				if ( scope.enableRotate === false ) return;
				if ( state !== STATE.TOUCH_ROTATE ) return; // is this needed?...

				handleTouchMoveRotate( event );

				break;

			case 2: // two-fingered touch: dolly

				if ( scope.enableZoom === false ) return;
				if ( state !== STATE.TOUCH_DOLLY ) return; // is this needed?...

				handleTouchMoveDolly( event );

				break;

			case 3: // three-fingered touch: pan

				if ( scope.enablePan === false ) return;
				if ( state !== STATE.TOUCH_PAN ) return; // is this needed?...

				handleTouchMovePan( event );

				break;

			default:

				state = STATE.NONE;

		}

	}

	function onTouchEnd( event ) {

		if ( scope.enabled === false ) return;

		handleTouchEnd( event );

		scope.dispatchEvent( endEvent );

		state = STATE.NONE;

	}

	function onContextMenu( event ) {

		if ( scope.enabled === false ) return;

		event.preventDefault();

	}

	//

	scope.domElement.addEventListener( 'contextmenu', onContextMenu, false );

	scope.domElement.addEventListener( 'mousedown', onMouseDown, false );
	scope.domElement.addEventListener( 'wheel', onMouseWheel, false );

	scope.domElement.addEventListener( 'touchstart', onTouchStart, false );
	scope.domElement.addEventListener( 'touchend', onTouchEnd, false );
	scope.domElement.addEventListener( 'touchmove', onTouchMove, false );

	window.addEventListener( 'keydown', onKeyDown, false );

	// force an update at start

	this.update();

};

}
#endif

#endif //THREEPP_ORBITCONTROLS_H
