//
// Created by byter on 10/27/18.
//

#ifndef THREE_PP_TIMER_H
#define THREE_PP_TIMER_H

#include <QDateTime>
#include <cassert>

namespace three {
namespace react3d {

/**
 * This class will take care of the time in the physics engine.
 */
class Timer
{

private :
  float _timeStep = 1.0f / 60.0f;

  /// Last time the timer has been updated
  long double mLastUpdateTime;

  /// Time difference between the two last timer update() calls
  long double mDeltaTime;

  /// Used to fix the time step and avoid strange time effects
  double mAccumulator;

  /// True if the timer is running
  bool mIsRunning;

  /// Private copy-constructor
  Timer(const Timer &timer);

  /// Private assignment operator
  Timer &operator=(const Timer &timer);

public :
  Timer() : mLastUpdateTime(0), mDeltaTime(0), mAccumulator(0), mIsRunning(false) {}

  /// Return the current time of the physics engine
  long double getPhysicsTime() const
  {
    return mLastUpdateTime;
  }

  /// Start the timer
  void start()
  {
    if (!mIsRunning) {

      // Get the current system time
      mLastUpdateTime = getCurrentSystemTime();

      mAccumulator = 0.0;
      mIsRunning = true;
    }
  }

  /// Stop the timer
  void stop()
  {
    mIsRunning = false;
  }

  void reset()
  {
    // Get the current system time
    mLastUpdateTime = getCurrentSystemTime();
    mAccumulator = 0.0;
  }

  /// Return true if the timer is running
  bool isRunning() const
  {
    return mIsRunning;
  }

  /// True if it's possible to take a new step
  bool isPossibleToTakeStep() const
  {
    return (mAccumulator >= _timeStep);
  }

  /// Compute the time since the last update() call and add it to the accumulator
  void update()
  {
    // Get the current system time
    long double currentTime = getCurrentSystemTime();

    // Compute the delta display time between two display frames
    mDeltaTime = currentTime - mLastUpdateTime;

    // Update the current display time
    mLastUpdateTime = currentTime;

    // Update the accumulator value
    mAccumulator += mDeltaTime;
  }

  /// Take a new step => update the timer by adding the timeStep value to the current time
  void nextStep()
  {
    assert(mIsRunning);

    // Update the accumulator value
    mAccumulator -= _timeStep;
  }

  /// Compute the interpolation factor
  float computeInterpolationFactor()
  {
    return (float(mAccumulator) / _timeStep);
  }

  float timeStep()
  {
    return _timeStep;
  }

  /// Return the current time of the system in seconds
  static long double getCurrentSystemTime()
  {
    return (long double)QDateTime::currentMSecsSinceEpoch() / 1000;
  }
};

}
}

#endif //THREE_PP_TIMER_H
