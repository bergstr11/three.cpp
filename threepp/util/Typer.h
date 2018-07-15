//
// Created by byter on 4/29/18.
//

#ifndef THREE_PP_TYPER_H
#define THREE_PP_TYPER_H

#include <type_traits>

namespace three {

namespace _union {

typedef unsigned int index_type;

/**
 * variadic union storage
 *
 * @tparam ...
 */
template<typename...>
struct variadic_union;

template<typename Head_>
struct variadic_union<Head_>
{
  using Head = Head_;
  alignas(Head *) Head *data;

  template<typename T>
  void set(T *head_init, index_type &union_index)
  {
    static_assert(std::is_same<T, Head>::value, "unable to store object of the given type");
    data = head_init;
    union_index = 0;
  }
};

template<typename Head_, typename... Tail>
struct variadic_union<Head_, Tail...>
{
  using Head = Head_;
  union
  {
    alignas(Head *) Head *data;
    variadic_union<Tail...> tail;
  };

  void set(Head *head_init, index_type &union_index)
  {
    data = head_init;
    union_index = 0;
  }

  template<typename T>
  void set(T *t, index_type &union_index)
  {
    tail.set(t, union_index);
    ++union_index;
  }
};

/**
 * data structure used for calculating the per-class index
 * @tparam ...
 */
template<typename...>
struct variadic_index;

template<typename Head, typename... Tail>
struct variadic_index<Head, variadic_union<Head, Tail...>>
{
  static constexpr
  const index_type index = 0;
};

template<typename Target, typename Head, typename... Tail>
struct variadic_index<Target, variadic_union<Head, Tail...>>
{
  static constexpr
  const index_type index = variadic_index<Target, variadic_union<Tail...>>::index + 1;
};

}

/**
 * stores a pointer to an object of one of the types listed in the template parameter types,
 * and allows safely retrieving the pointer through an (implicit) cast, like so:
 * <pre>
 * using MyTyper = three::Typer<A, B, C>;
 *
 * class A {
 * protected:
 *    A(const MyTyper &typer) : typer(typer) {}
 *
 * public:
 *    A() : typer(MyTyper(this)) {}
 *    MyTyper typer;
 *    ...
 * };
 *
 * class B : public A {
 * public:
 *    B() : A(MyTyper(this)) {}
 *    ...
 * }
 *
 * class C : public B {
 * public:
 *    C() : B(MyTyper(this).allow<B>()) {}
 *    ...
 * }
 *
 * A *ab = get_A_which_happens_to_be_a_B();
 *
 * B *b = ab->typer; //implicit cast, returns non-null if ab is a non-null pointer to B
 * C *c = ab->typer; //implicit cast, returns null because ab is not a pointer to C
 *
 * A *ac = get_A_which_happens_to_be_a_C();
 *
 * B *b = ac->typer; //implicit cast, returns non-null because ac allows casting to B
 * C *c = ac->typer; //implicit cast, returns non-null because ac is a pointer to C
 *
 * </pre>
 *
 * @tparam Ts list of types pointer storeable in this object
 */
template <typename... Ts>
class Typer
{
  _union::variadic_union<Ts...> data;
  _union::index_type union_mask {0};

public:
  template<typename T>
  Typer(T *t)
  {
    _union::index_type index;
    data.set(t, index);
    union_mask |= 1 << index;
  }

  Typer() = default;

  /**
   * declare the given type's pointer to be reinterpret_cast-compatible with the pointer already stored
   * in this object. This should be used for superclasses only, as shown in the class comment
   *
   * @tparam T
   */
  template<typename T>
  Typer &allow()
  {
    _union::index_type index = _union::variadic_index<T, _union::variadic_union<Ts...>>::index;
    union_mask |= 1 << index;
    return *this;
  }

  /**
   * @tparam T the type of pointer to retrieve
   * @return the pointer, if such a pointer was stored through the constructor, or nullptr otherwise
   */
  template<typename T>
  operator T *() const
  {
    return (union_mask & (1 << _union::variadic_index<T, _union::variadic_union<Ts...>>::index))
           ? reinterpret_cast<T *>(data.data) : nullptr;
  }
};

}

#endif //THREE_PP_TYPER_H
