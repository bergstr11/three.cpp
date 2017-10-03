#ifndef SIMPLE_SIGNAL_H
#define SIMPLE_SIGNAL_H

#include <stdint.h>
#include <assert.h>
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>

namespace three {

/** A counter used for:
    - counting depth of emit() call recursion
    - the number of slots called in an emission
*/
using counter_t = unsigned int;

namespace aggregation {

/** Aggregation that ignores slot return type and value, returning
    void. This is the default aggregation used by a signal's `emit()`
    function. */
template <typename Result>
class Void {
 public:
  /** Discards slot return value */
  void aggregate(Result r) {}
  /** @returns nothing, irrespective of the slot return type */
  void get() { return; }
};

/** Specialisation of Void for void slots. */
template <>
class Void<void> {
 public:
  /** Does nothing */
  void aggregate() {}
  /** @returns nothing */
  void get() { return; }
};

/** Aggregation that returns the value returned by the last slot
    called in a signal emission. If a signal has no connected slots,
    the return value is a value-initialized `Result`. */
template <typename Result>
class Last {
 public:
  /** Keeps hold of only the latest slot return value. */
  void aggregate(Result r) { m_latest = std::move(r); }
  /** @returns the return value of the last slot called from the
      signal emission. */
  Result get() { return std::move(m_latest); }

 private:
  Result m_latest{};  // Ensure value-initialisation
};

/** Aggregation that collates the values returned by each slot in a
    container. This template class is suitable for most standard
    library containers. By default, a `std::vector` is used.  If a
    signal has no connected slots, the return value is an empty
    container. */
template <typename Result,
          template <typename, typename> class Container = std::vector,
          typename Allocator = std::allocator<Result>>
class Collation {
  using Containr = Container<Result, Allocator>;
 public:
  /** Append the slot return value within the container. */
  void aggregate(Result r) { m_result.insert(m_result.end(), std::move(r)); }
  /** @returns the container of collated slot return values. */
  Containr get() { return std::move(m_result); }

 private:
  Containr m_result;
};

/** Aggregation that counts the number of slots called. */
template <typename Result>
class Counter {
 public:
  /** Increment counter and discard slots return value */
  void aggregate(Result r) { ++m_result; }
  /** @returns the number of slots called. */
  counter_t get() { return m_result; }

 private:
  counter_t m_result{};
};

/** Aggregation that counts the number of slots called,
    specialised for void slots. */
template <>
class Counter<void> {
 public:
  /** Increment counter */
  void aggregate() { ++m_result; }
  /** @returns the number of slots called. */
  counter_t get() { return m_result; }

 private:
  counter_t m_result{};
};

}  // namespace aggregation

namespace controller {

/** Continue signal emission regardless of the slot return value */
template <typename Result>
struct All {
  bool operator()(const Result & result) { return true; }
};

/** Continue signal emission -- specialisation for void slots. */
template <>
struct All<void> {
  bool operator()() { return true; }
};

/** Continue signal emission iff the slot returns a result that
    (when converted to a boolean) equals `T`. */
template <typename Result, bool T = true>
struct While {
  bool operator()(const Result & result) { return result == T; }
};

}  // namespace controller

namespace detail {

/** Undefined template class for invoking slots differently
    depending on their return type. */
template <typename Signature>
class ConnectionInvoker;

/** ConnectionInvoker specialisation for value-returning slots. */
template <typename Result, typename... Args>
class ConnectionInvoker<Result(Args...)> {
 public:
  /** Call the slot, handing its return value over to the aggregation.
      @returns `true` iff the aggregation determines that emission
      should continue. */
  template <typename Aggregation, typename Controller>
  static bool invoke(const std::function<Result(Args...)> & f,
                     Args... args,
                     Aggregation & aggregation,
                     Controller & controller) {
    Result result = f(args...);
    bool ok = controller(result);
    aggregation.aggregate(std::move(result));
    return ok;
  }
};

/** ConnectionInvoker specialisation for void slots. */
template <typename... Args>
class ConnectionInvoker<void(Args...)> {
 public:
  /** Call the slot.
      @returns `true` iff the aggregation determines that emission
      should continue. */
  template <typename Aggregation, typename Controller>
  static bool invoke(const std::function<void(Args...)> & f,
                     Args... args,
                     Aggregation & aggregation,
                     Controller & controller) {
    f(args...);
    bool ok = controller();
    aggregation.aggregate();
    return ok;
  }
};

/** Node implements a doubly-linked ring, with each node containing a
    function of type `Func`. */
template <typename Func>
class Node {
 public:
  /** Constructor */
  explicit Node(Func f)
      : m_next(this),
        m_prev(this),
        m_function(std::move(f)) {}

  /** Constructor */
  Node() : Node(nullptr) {}

  /** Destructor */
  ~Node() {}

  /** Insert `node` immediately before this node in the ring.
      @returns the inserted node */
  Node * insert(Node * node) {
    if (node) {
      node->m_prev = m_prev;
      node->m_next = this;
      m_prev->m_next = node;
      m_prev = node;
    }
    return node;
  }

  /** Extracts this node from the list.
      @returns this node */
  Node * extract() {
    m_next->m_prev = m_prev;
    m_prev->m_next = m_next;
    m_prev = m_next = nullptr;
    return this;
  }

  /** Deactivates this node (by nullifying its function) thereby
      marking this node as suitable for extraction from the list when
      convenient.
      @returns this node */
  Node * deactivate() {
    m_function = nullptr;
    return this;
  }

  /** @returns this node's function */
  const Func & function() const { return m_function; }

  /** @returns the node which follows this one in the ring. If this
      is the only node in the ring, then returns this node. */
  Node * next() { return m_next; }

 private:
  /** The next node in the ring. */
  Node * m_next;
  /** The previous node in the ring. */
  Node * m_prev;
  /** The node's function */
  Func m_function;
};

/** Exception safe counter incrementer, i.e. ensures counter is
    decremented if exception is thrown while this object is in
    scope. */
template <typename T>
class ScopedIncrement {
 public:
  /** Constructor
      @param[in,out] counter The counter to be incremented */
  explicit ScopedIncrement(T & counter) : m_counter(counter) { ++m_counter; }

  /** Destructor. Decrements the counter */
  ~ScopedIncrement() { --m_counter; }

 private:
  T & m_counter;
};

}  // namespace detail


/** Undefined template class used for specialization. */
template <typename Signature>
class Signal;

/** Signal is a template class providing an implementation of the
    infrastructure for the Observer Pattern.

    The Observer Pattern's subject contains or references one or more
    _signal_ objects. A signal is emitted when some event occurs.

    Each observer connects a _slot_ to the subject's appropriate
    signal. This is conceptually identical to Qt's celebrated 'signals
    and slots' mechanism.

    A slot is an arbitrary function, lambda expression, bind
    expression, other function object, or a pointer to a member
    function. All of a signal's connected slots must have identical
    signatures. The Signal template class is instantiated with the
    function signature shared by all of its slots.

    While the term _slot_ is used herein, alternative, commonly used
    and equally good terms include _callback_, _callback function_,
    _notification callback_, _receiver_ as well as _observer_.

    A signal is connected to a slot using `connect()`. These
    connections form a many-to-many relationship between signals and
    slots. A connection's existence implies that the emission of the
    signal will be transmitted to the slot.

    The Signal class is responsible for managing its connections to
    slots. Signal-slot connections can be removed with `disconnect()`.

    The signal is emitted to its connected slots using the `emit()`. */
template <typename Result, typename... Args>
class Signal<Result(Args...)> {
  using Slot = std::function<Result(Args...)>;
  using Connection = detail::Node<Slot>;

 public:
  using ConnectionId = Connection *;

  /** Constructor. Constructs a Signal with no connections to
      slots. */
  Signal() : m_recursionDepth(0), m_deactivations(false) {}

  /** Constructor. Constructs a Signal with a single connection to the
      given slot */
  Signal(Slot f) : Signal() { connect(std::move(f)); }

  /** Destructor. */
  ~Signal() {
    if (m_head) {
      while (m_head->next() != m_head.get())
        disconnect(m_head->next());
    }
  }

  /** Copy constructor */
  Signal(const Signal &) = delete;

  /** Copy assignment operator */
  Signal & operator=(const Signal &) = delete;

  /** Add a connection from this signal to the given slot. The slot
      can be a function, lambda expression, bind expression or another
      function object. Member functions can be connected as slots
      using alternative overloads of `connect()`.
      @returns an identifier for the newly added signal-slot connection. */
  ConnectionId connect(Slot f) {
    if (!m_head) m_head = std::unique_ptr<Connection>(new Connection());
    if (f != nullptr)
      return m_head->insert(new Connection(std::move(f)));
    else
      return nullptr;
  }

  /** Add a connection from this signal to a member function of the
      referenced object.
      @returns an identifier for the newly added signal-slot connection. */
  template <class C>
  ConnectionId connect(C & obj, Result (C::*mf)(Args...)) {
    return connect([&obj, mf](Args... args) { return (obj .* mf)(args...); });
  }

  /** Add a connection from this signal to a member function of the
      referenced const object.
      @returns an identifier for the newly added signal-slot connection. */
  template <class C>
  ConnectionId connect(const C & obj, Result (C::*mf)(Args...)) {
    return connect([&obj, mf](Args... args) { return (obj .* mf)(args...); });
  }

  /** Add a connection from this signal to a member function of the
      pointed-to object.
      @returns an identifier for the newly added signal-slot connection. */
  template <class C>
  ConnectionId connect(C * obj, Result (C::*mf)(Args...)) {
    return obj ? connect(*obj, mf) : nullptr;
  }

  /** Add a connection from this signal to a member function of the
      pointed-to const object.
      @returns an identifier for the newly added signal-slot connection. */
  template <class C>
  ConnectionId connect(const C * obj, Result (C::*mf)(Args...)) {
    return obj ? connect(*obj, mf) : nullptr;
  }

  /** Remove a connection from this signal to a slot. The connection
      is identified using the `ConnectionId` value originally returned
      by `connect()`. If more than one connection exists between this
      signal and a slot, only the connection with the given Id is
      removed, meaning the slot will still be called during emission,
      albeit one time less.
      @param[in] id The signal-slot connection Id, as returned by `connect()`.
      @returns true if the identified connection was removed. */
  bool disconnect(ConnectionId id) {
    if (id && m_head) {
      // Scan the list for the matching connection
      Connection * node = m_head->next();
      while (node != m_head.get() && node != id)
        node = node->next();

      if (node == id) {
        // Delete or deactivate the connection
        if (m_recursionDepth == 0) {
          delete node->extract();
        } else {
          node->deactivate();
          m_deactivations = true;
        }
        return true;
      }
    }
    return false;
  }

  /** remove all connections */
  void disconnectAll() {
    if (m_head) {
      Connection * node = m_head->next();
      while (node != m_head.get())
        node = node->next();

      if (node != m_head.get()) {
        // Delete or deactivate the connection
        if (m_recursionDepth == 0) {
          delete node->extract();
        } else {
          node->deactivate();
          m_deactivations = true;
        }
      }
    }
  }

  /** Emit a signal to all its connected slots, optionally collating
      the values they return.

      ##### Slot calling order #####

      Slots are called in the order in which they are connected to the
      signal, using the `connect()` function.

      ##### Signal modification and recursion during emission #####

      The implementation is safe against recursion, so slots may be
      connected to and disconnected from the signal during signal
      emission and recursive `emit()` calls are also safe.

      Slots connected to this signal during an emission will be called
      in that emission.

      Slots disconnected from this signal during an emission will not
      be called in that emission, unless they were called prior to
      their disconnection.

      The result of emission depends on the aggregation used to
      instantiate the `emit()` template function. By default, nothing
      is returned, i.e. `emit()` is a void function.

      ##### Aggregation #####

      An Aggregation is a class that collates the slot return values
      into a single return value. Examples are:

        - Ignore the return value (if any) from the slots and return
          void [Default].
        - Return the result of the last slot called during emission.
        - Return the results of all the calls to each of a signal's
          connected slots in a collection, such as a vector.
        - Return the number of slots which returned true/non-null
          values.

      An implementation of an Aggregation class must implement the
      following member functions:

      - `void aggregate(TypeA) {...}` -- where `TypeA` is the slot
        return type, or is implicitly convertable from it.
      - `TypeB get() {...}` -- where `TypeB` is at the
        implementer's discretion, though it is often the same as
        `TypeA`.

      An aggregation's `aggregate()` function accepts the value
      returned from each slot and either does something with it or
      ignores it.

      An aggregation's `get()` function returns the aggregated result.

      Some example Aggregation implementations are available in the
      `aggregation` namespace.

      ##### Controller #####

      A controller is a callable object that determines if or when to
      halt running emissions. Typically, though not necessarily, this
      decision is based on the slots' return value.

      The controller function should have the form:

      - `bool func(const Result &)` or
      - `bool func()`

      For value-returning and void slots respectively, where `Result`
      is the return type of the slots (or is implicitly convertable
      from it).

      @returns the output of the aggregation's `get()` function.
  */
#ifdef _MSC_VER
  template <typename Aggregation = aggregation::Void<Result>, typename Controller = controller::All<Result>>
  auto emitSignal(Args... args) -> decltype(Aggregation().get())
  {
    Aggregation aggregation = Aggregation();
    Controller controller = Controller();
#else
  template <typename Aggregation = aggregation::Void<Result>, typename Controller = controller::All<Result>>
  auto emitSignal(Args... args, Aggregation aggregation = Aggregation(), Controller controller = Controller())
  -> decltype(aggregation.get()) {
#endif
    using Invoker = detail::ConnectionInvoker<Result(Args...)>;
    using Increment = detail::ScopedIncrement<decltype(m_recursionDepth)>;

    if (m_head) {
      bool ok = true;
      Connection * node = m_head->next();
      while (node != m_head.get() && ok) {
        /* Increment the recursion counter to ensure that slots cannot
           recursively delete connections from the ring (thereby
           potentially invalidating this iteration), rather they only
           deactivate them. Adding connections is okay. */
        Increment scopedIncrement{m_recursionDepth};
        if (node->function() != nullptr)
          ok = Invoker::invoke(
              node->function(), args..., aggregation, controller);
        node = node->next();
      }

      if (m_recursionDepth == 0 && m_deactivations) {
        /* A slot has recursively deactivated one or more connections
           from this signal. Delete these deactivated connections
           now. */
        node = m_head->next();
        while (node != m_head.get()) {
          Connection * next = node->next();
          if (node->function() == nullptr)
            delete node->extract();
          node = next;
        }
        m_deactivations = false;
      }
    }
    return aggregation.get();
  }

 private:
  /** Inert entry point into connection ring */
  std::unique_ptr<Connection> m_head;
  /** Guard against iteration invalidation by slots that alter the Signal */
  counter_t m_recursionDepth;
  /** Have any connections been deactivated (but not deleted)? */
  bool m_deactivations;
};

template <typename Result, typename... Args>
class SignalRegistry {
  using signal_t = Signal<Result(Args...)>;
  using connection_t = typename Signal<Result(Args...)>::ConnectionId;
  using slot_t = std::function<Result(Args...)>;

  std::unordered_map<connection_t, signal_t *> connections;
public:
  ~SignalRegistry() {
    disconnectAll();
  }

  connection_t connect(signal_t &signal, slot_t slot) {
    connection_t conn = signal.connect(slot);
    connections[conn] = &signal;
    return conn;
  }

  template <class C>
  connection_t connect(signal_t &signal, C & obj, Result (C::*mf)(Args...)) {
    connection_t conn = signal.connect(obj, mf);
    connections[conn] = &signal;
    return conn;
  }

  template <class C>
  connection_t connect(signal_t &signal, const C & obj, Result (C::*mf)(Args...)) {
    connection_t conn = signal.connect(obj, mf);
    connections[conn] = &signal;
    return conn;
  }

  template <class C>
  connection_t connect(signal_t &signal, C * obj, Result (C::*mf)(Args...)) {
    connection_t conn = signal.connect(obj, mf);
    connections[conn] = &signal;
    return conn;
  }

  template <class C>
  connection_t connect(signal_t &signal, const C * obj, Result (C::*mf)(Args...)) {
    connection_t conn = signal.connect(obj, mf);
    connections[conn] = &signal;
    return conn;
  }

  void disconnect(connection_t conn) {
    if(connections.count(conn)) {
      connections[conn]->disconnect(conn);
      connections.erase(conn);
    }
  }

  void disconnectAll() {
    for(auto &conn : connections) {
      conn.second->disconnect(conn.first);
    }
    connections.clear();
  }
};

using VoidSignal = Signal<void()>;

}  // namespace lo

#endif //SIMPLE_SIGNAL_H