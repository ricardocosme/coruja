
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>
#include <boost/hof/is_invocable.hpp>
#include <range/v3/range_concepts.hpp>

namespace coruja { 

//For std < c++17
template<typename... Ts> struct make_void { typedef void type;};
template<typename... Ts> using void_t = typename make_void<Ts...>::type;
    
template<typename Ret, typename F, typename... Args>
using enable_if_is_invocable_t = typename std::enable_if<
    boost::hof::is_invocable<F, Args...>::value,
    Ret>::type;

template<typename Ret, typename F, typename... Args>
using enable_if_is_not_invocable_t = typename std::enable_if<
    !boost::hof::is_invocable<F, Args...>::value,
    Ret>::type;
    
//For std < c++14
template<typename T>
using remove_reference_t = typename std::remove_reference<T>::type;

template<bool v, typename T = void>
using enable_if_t = typename std::enable_if<v, T>::type;

template<typename T>
using result_of_t = typename std::result_of<T>::type;
    
template<typename T, typename = void>
struct is_observable : std::false_type {};

/*
Observable

Immediately notifies observers interested in a specific action that
changes the state of an object of type T. There has to be at least one
observable action.

Requirements:
  `Observable::observed_t`
  Type of the object to be observed.
  
  `observed_t observed() const noexcept`
  Returns the observed object.

  For each observable action, there has to be a method like below to
  register a callback 'cbk' to be called when the action occurs. An
  object that represents the connection between callback and
  observable must be returned.

    template<typename Callable>
    connection action(Callable cbk)

  `Connection` represents a connection between a callback and an
  Observable. Requirements:
    void disconnect()
    Disconnect the callback from the Observable.

  An observable must be default constructible.
*/

template<typename T>
struct is_observable<T, void_t<
    typename T::observed_t,
    decltype(
        // std::declval<typename T::observed_t&>() =
        std::declval<const T>().observed(),
        (void)0)
    >
> : std::integral_constant<bool, 
                           std::is_default_constructible<T>::value>

{};

/*
ObservableObject

Refines the Observable concept and represents an observable object of
type `T` that has one general observable action: `after_change`.

Requirements:
  template<typename Callable>
  after_change_connection_t after_change(Callable cbk)

  Registers a callback `cbk` to be notified when a convertible object
  to `T` is assigned to the Observable. The signature of callback
  should be equivalent to the following:
    void(const Observable::observed_t&)

  `Observable::after_change_connection_t`
  Type of the connection returned by `after_change`.

*/
template<typename T, typename = void>
struct _is_observable_object : std::false_type {};

template<typename T>
struct _is_observable_object<T, void_t<
    typename T::after_change_connection_t,
    decltype(
        std::declval<typename T::after_change_connection_t&>() =
        std::declval<T&>().after_change(std::declval<void(*)(const typename T::observed_t&)>()),
        (void)0)
    >
> : std::integral_constant<bool,
                           is_observable<T>::value>
{};

template<typename T>
using is_observable_object = _is_observable_object<remove_reference_t<T>>;
    
/*
ObservableErasableRange

Refines the Observable and Range concept allowing to observe
insertion and removal of elements.

Requirements:
  template<typename Callable>
  for_each_connection_t for_each(Callable cbk)

  Registers a callback `cbk` to be called for each element in the
  container. The signature of callback should be equivalent to the
  following:
    void(Observable&, Observable::iterator)
    or 
    void(ranges::range_reference_t<Observable>)

  Registers a callback `cbk` to be called when an element in the
  container is erased. The signature of callback should be equivalent
  to the following:
    void(Observable&, Observable::iterator)
    or 
    void(ranges::range_reference_t<Observable>)

  `Observable::before_erase_connection_t`
  Type of the connection returned by `before_erase`.
*/
template<typename T, typename = void>
struct is_observable_erasable_range : std::false_type {};

template<typename T>
struct is_observable_erasable_range<T, void_t<
    typename T::for_each_connection_t,
    typename T::before_erase_connection_t,
    decltype(
        std::declval<typename T::for_each_connection_t&>() =
        std::declval<T&>().for_each
        (std::declval<void(*)(T&, decltype(begin(std::declval<T>())))>()),
        std::declval<typename T::for_each_connection_t&>() =
        std::declval<T&>().for_each
        (std::declval<void(*)(decltype(*begin(std::declval<T>())))>()),
        std::declval<typename T::before_erase_connection_t&>() =
             std::declval<T>().before_erase
             (std::declval<void(*)(T&, decltype(begin(std::declval<T>())))>()),
        std::declval<typename T::before_erase_connection_t&>() =
        std::declval<T&>().before_erase
        (std::declval<void(*)(decltype(*begin(std::declval<T>())))>()),
        (void)0)
    >
> : std::integral_constant<bool,
                           is_observable<T>::value &&
                           ranges::Range<T>::value>
{};
    
}
    
