#ifndef SP_CONTAINERS_REVERSE_ITERATOR_H_
#define SP_CONTAINERS_REVERSE_ITERATOR_H_
#include <cstdint>

namespace sp {

using diff_t = int64_t;

// BidIt satisfies BidirectionalIterator
template <typename BidIt>
class reverse_iterator {
 public:
  using iterator_category = typename BidIt::iterator_category;
  using value_type = typename BidIt::value_type;
  using pointer = typename BidIt::pointer;
  using reference = typename BidIt::reference;

  constexpr explicit reverse_iterator(const BidIt& it) : fwd_it_(it){};
  constexpr virtual ~reverse_iterator() = default;

  constexpr pointer* base() const noexcept { return fwd_it_; }

  constexpr value_type& operator*() const { return *(fwd_it_ - 1); }
  constexpr pointer* operator->() const { return (fwd_it_ - 1)->operator->(); }

  constexpr bool operator==(const reverse_iterator& other) const {
    return fwd_it_ == other.fwd_it_;
  }

  constexpr bool operator!=(const reverse_iterator& other) const {
    return fwd_it_ != other.fwd_it_;
  }

  constexpr bool operator>(const reverse_iterator& other) const {
    return (fwd_it_ - other.ptr_) < 0;
  }

  constexpr bool operator<(const reverse_iterator& other) const {
    return (fwd_it_ - other.ptr_) > 0;
  }

  constexpr bool operator>=(const reverse_iterator& other) const {
    return (fwd_it_ - other.ptr_) <= 0;
  }

  constexpr bool operator<=(const reverse_iterator& other) const {
    return (fwd_it_ - other.ptr_) >= 0;
  }

  constexpr reverse_iterator operator+(diff_t delta) const {
    return reverse_iterator(fwd_it_ - delta);
  }

  constexpr reverse_iterator operator-(diff_t delta) const {
    return reverse_iterator(fwd_it_ + delta);
  }

  constexpr diff_t operator-(const reverse_iterator& other) const {
    return other.fwd_it_ - fwd_it_;
  }

  constexpr reverse_iterator& operator+=(diff_t delta) {
    fwd_it_ += delta;
    return *this;
  }

  constexpr reverse_iterator& operator-=(diff_t delta) {
    fwd_it_ -= delta;
    return *this;
  }

  constexpr reverse_iterator operator++(int) {
    return reverse_iterator(fwd_it_--);
  }
  constexpr reverse_iterator operator--(int) {
    return reverse_iterator(fwd_it_++);
  }

  constexpr reverse_iterator& operator++() {
    --fwd_it_;
    return *this;
  }

  constexpr reverse_iterator& operator--() {
    ++fwd_it_;
    return *this;
  }

  constexpr operator reverse_iterator<const BidIt>() const {
    return reverse_iterator<const BidIt>(const_cast<const BidIt>(fwd_it_));
  }

 protected:
  BidIt fwd_it_;
};
}  // namespace sp
#endif  // SP_CONTAINERS_REVERSE_ITERATOR_H_
