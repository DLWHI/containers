#ifndef DLWHI_CONTAINERS_NODE_ITERATOR_H_
#define DLWHI_CONTAINERS_NODE_ITERATOR_H_
#include <iterator>
#include <cstdint>

namespace dlwhi {

using diff_t = int64_t;

// T - node type
// Container - is not used inside of class, but allows different containers
//             with same template type produce different iterators
template <typename T, typename Container>
class node_iterator {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using node_type = T;
  using value_type = typename T::value_type;
  using pointer = typename T::pointer;
  using reference = typename T::reference;
  using diff_t = int64_t;
  using difference_type = diff_t;

  constexpr node_iterator() noexcept : node_(nullptr){};
  constexpr explicit node_iterator(T* data) noexcept : node_(data){};
  constexpr virtual ~node_iterator() = default;

  constexpr T* base() const noexcept { return node_; }

  // static type checking?
  constexpr decltype(T().value()) operator*() noexcept { return node_->value(); }
  constexpr decltype(&T().value()) operator->() noexcept { return &node_->value(); }

  constexpr bool operator==(const node_iterator& other) const noexcept {
    return node_ == other.node_;
  }

  constexpr bool operator!=(const node_iterator& other) const noexcept {
    return node_ != other.node_;
  }

//   constexpr bool operator>(const node_iterator& other) const noexcept {
//     return (node_ - other.node_) > 0;
//   }

//   constexpr bool operator<(const node_iterator& other) const noexcept {
//     return (node_ - other.node_) < 0;
//   }

//   constexpr bool operator>=(const node_iterator& other) const noexcept {
//     return (node_ - other.node_) >= 0;
//   }

//   constexpr bool operator<=(const node_iterator& other) const noexcept {
//     return (node_ - other.node_) <= 0;
//   }

//   constexpr node_iterator operator+(diff_t delta) const noexcept {
//     return node_iterator(node_ + delta);
//   }

//   constexpr node_iterator operator-(diff_t delta) const noexcept {
//     return node_iterator(node_ - delta);
//   }

//   constexpr diff_t operator-(const node_iterator& other) const noexcept {
//     return node_ - other.node_;
//   }

//   constexpr node_iterator& operator+=(diff_t delta) noexcept {
//     node_ += delta;
//     return *this;
//   }

//   constexpr node_iterator& operator-=(diff_t delta) noexcept {
//     node_ -= delta;
//     return *this;
//   }

  constexpr node_iterator operator++(int) noexcept { 
    return node_iterator(node_ = node_->next()); 
  }

  constexpr node_iterator operator--(int) noexcept { 
    return node_iterator(node_ = node_->prev()); 
  }

  constexpr node_iterator& operator++() noexcept {
    node_ = node_->next();
    return *this;
  }

  constexpr node_iterator& operator--() noexcept {
    node_ = node_->prev();
    return *this;
  }

  constexpr operator node_iterator<const T, Container>() const noexcept {
    return node_iterator<const T, Container>(const_cast<const T*>(node_));
  }

 protected:
  node_type* node_;
};
}  // namespace s21
#endif  // DLWHI_CONTAINERS_NODE_ITERATOR_H_
