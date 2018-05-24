//
// Created by byter on 5/24/18.
//

#ifndef THREE_PP_TRACKINGPROPERTY_H
#define THREE_PP_TRACKINGPROPERTY_H

namespace three {
namespace quick {

template <typename Value> class TrackingPropertyBase
{
protected:
  bool _isSet = false;
  Value _value;

public:
  explicit TrackingPropertyBase(const Value &init) : _value(init) {}

  operator Value () const {return _value;}

  bool operator == (const Value &value) {return _value == value;}
  bool operator != (const Value &value) {return _value != value;}

  bool isSet() const {return _isSet;}
};

template <typename Value, typename Enable = void>
class TrackingProperty;

template <typename Value>
struct TrackingProperty<Value, typename std::enable_if<std::is_pointer<Value>::value>::type> : TrackingPropertyBase<Value>
{
  using VNoPtr = typename std::remove_pointer<Value>::type;
  using Super = TrackingPropertyBase<Value>;

  explicit TrackingProperty(const Value &init) : Super(init) {}

  TrackingProperty &operator = (const Value &value) {
    Super::_value = value;
    Super::_isSet = true;
    return *this;
  }

  VNoPtr &operator ()() {return *Super::_value;}

  const VNoPtr &operator ()() const {return *Super::_value;}
};

template <typename Value>
struct TrackingProperty<Value, typename std::enable_if<!std::is_pointer<Value>::value>::type> : TrackingPropertyBase<Value>
{
  using Super = TrackingPropertyBase<Value>;

  explicit TrackingProperty(const Value &init) : Super(init) {}

  TrackingProperty &operator = (const Value &value) {
    Super::_value = value;
    Super::_isSet = true;
    return *this;
  }

  Value &operator ()() {return Super::_value;}

  const Value &operator ()() const {return Super::_value;}
};

}
}
#endif //THREE_PP_TRACKINGPROPERTY_H
