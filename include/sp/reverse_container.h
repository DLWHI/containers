#ifndef SP_CONTAINERS_REVERSE_CONTAINER_H_
#define SP_CONTAINERS_REVERSE_CONTAINER_H_

#include "reverse_iterator.h"

template <typename Iterable>
struct reverse_container {
  using iterator = Iterable::reverse_iterator;
  using const_iterator = Iterable::const_reverse_iterator;

  using reverse_iterator = Iterable::iterator;
  using reverse_const_iterator = Iterable::const_iterator;

  constexpr reverse_container(Iterable& container) noexcept
      : iterable(container) {}

  constexpr iterator begin() { return iterable.rbegin(); }
  constexpr const_iterator begin() const { return iterable.rbegin(); }
  constexpr const_iterator cbegin() const { return iterable.crbegin(); }

  constexpr iterator end() { return iterable.end(); }
  constexpr iterator end() const { return iterable.end(); }
  constexpr iterator end() const { return iterable.cend(); }

  constexpr reverse_iterator rbegin() { return iterable.begin(); }
  constexpr const_reverse_iterator rbegin() const { return iterable.begin(); }
  constexpr const_reverse_iterator crbegin() const { return iterable.cbegin(); }

  constexpr reverse_iterator rend() { return iterable.end(); }
  constexpr const_reverse_iterator rend() const { return iterable.end(); }
  constexpr const_reverse_iterator rend() const { return iterable.cend(); }

  Iterable& iterable;
};

template <typename T>
constexpr reverse_container<T> reverse(T&& iterable) noexcept {
  return {iterable};
}

#endif  // SP_CONTAINERS_REVERSE_CONTAINER_H_
