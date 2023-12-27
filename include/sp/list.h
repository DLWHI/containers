#ifndef SP_CONTAINERS_LIST_H_
#define SP_CONTAINERS_LIST_H_

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <sp/node_iterator.h>
#include <sp/reverse_iterator.h>

namespace sp {
template <typename T, typename Allocator = std::allocator<T>>
class list {
  struct Node;
  struct ValueNode;

  using rebind_alloc = typename std::allocator_traits<
      Allocator>::template rebind_alloc<ValueNode>;
  using rebind_traits = std::allocator_traits<rebind_alloc>;

 public:
  template <typename U>
  class ListIterator;

  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using move_reference = T&&;
  using size_type = int64_t;

  using allocator_type = Allocator;
  using al_traits = std::allocator_traits<allocator_type>;

  using iterator = sp::node_iterator<T, Node, list>;
  using const_iterator = sp::node_iterator<const T, Node, list>;
  using reverse_iterator = sp::reverse_iterator<iterator>;
  using const_reverse_iterator = sp::reverse_iterator<const_iterator>;

  list() noexcept(std::is_nothrow_default_constructible<rebind_alloc>::value)
      : size_(0) {}

  explicit list(const Allocator& al) noexcept
      : al_(static_cast<rebind_alloc>(al)), size_(0) {}

  list(size_type count, const_reference val, const Allocator& al = Allocator())
      : al_(static_cast<rebind_alloc>(al)), size_(count) {
    push_nodes(&head_, count, val);
  };

  list(size_type count, const Allocator& al = Allocator())
      : al_(static_cast<rebind_alloc>(al)), size_(count) {
    push_nodes(&head_, count);
  }

  template <typename InputIterator>
  list(InputIterator first, InputIterator last,
       const Allocator& al = Allocator())
      : al_(static_cast<rebind_alloc>(al)), size_(std::distance(first, last)) {
    push_nodes(&head_, first, last);
  }

  list(std::initializer_list<T> vals, const Allocator& al = Allocator())
      : list(vals.begin(), vals.end(), al) {}

  list(const list& other) : list(other.begin(), other.end(), other.al_) {}

  list(const list& other, const Allocator& al)
      : al_(al_traits::select_on_container_copy_construction(other.al_)),
        size_(other.size_) {
    push_nodes(&head_, other.begin(), other.end());
  };

  list(list&& other) : size_(other.size_) {
    if constexpr (!al_traits::is_always_equal::value) {
      al_ = std::move(other.al_);
    }
    head_.bind(other.head_.prev_node, other.head_.next_node);
    other.head_.unbind();
    other.size_ = 0;
  }

  list(list&& other, const Allocator& al) : list(al) {
    if (al == other.al_) {
      swap(other);
    } else {
      pointer_buffer temp(other.size_, &al_);
      move_from(temp.ptr, other.buf_.ptr, other.size_);
      buf_.swap(temp);
      size_ = other.size_;
    }
    std::swap(size_, other.size_);
    std::swap(head_, other.head_);
  }

  // list& operator=(list& other) {
  //   list cpy(other);
  //   *this = std::move(cpy);
  //   return *this;
  // }

  // list& operator=(list&& other) {
  //   std::swap(size_, other.size_);
  //   std::swap(head_, other.head_);
  //   node_al_ = std::move(other.node_al_);
  //   al_ = std::move(other.al_);
  //   return *this;
  // }

  virtual ~list() noexcept { clear(); }

  //============================================================================
  allocator_type get_allocator() const noexcept {
    return static_cast<Allocator>(al_);
  }

  reference front() { return cast(head_.next_node)->data; }
  reference back() { return cast(head_.prev_node)->data; }

  const_reference front() const { return cast(head_.next_node)->data; }
  const_reference back() const { return cast(head_.prev_node)->data; }

  iterator begin() { return iterator(head_.next_node); }
  const_iterator begin() const { return const_iterator(head_.next_node); }
  const_iterator cbegin() const { return const_iterator(head_.next_node); }

  iterator end() { return iterator(head_); }
  const_iterator end() const { return const_iterator(head_); }
  const_iterator cend() const { return const_iterator(head_); }

  bool empty() const noexcept { return !size_; }
  size_type max_size() const noexcept { return rebind_traits::max_size(al_); }
  size_type size() const noexcept { return size_; }
  //============================================================================

  bool integrity() const {
    size_type count = 0;
    Node* ptr = head_.next_node;
    for (; ptr != &head_; ptr = ptr->next_node) {
      ++count;
    }
    return count == size_;
  }

  void clear() noexcept {
    pop_nodes(head_.next_node, &head_);
    size_ = 0;
    head_.next_node = &head_;
    head_.prev_node = &head_;
  }

  // iterator insert(const_iterator pos, const_reference value) {
  //   Node* some = node_al_.allocate(1);
  //   Node* target = const_cast<Node*>(pos.base());
  //   some->construct(target->prev_node, target, al_, value);
  //   some->bind(target->prev_node, target);
  //   size_ += 1;
  //   return iterator(some);
  // }

  // iterator insert(const_iterator pos, move_reference value) {
  //   Node* some = node_al_.allocate(1);
  //   Node* target = const_cast<Node*>(pos.base());
  //   some->construct(target->prev_node, target, al_,
  //                   std::forward<value_type>(value));
  //   some->bind(target->prev_node, target);
  //   size_ += 1;
  //   return iterator(some);
  // }

  // iterator insert(const_iterator pos, size_type count, const_reference value);

  // template <typename InputIterator>
  // iterator insert(const_iterator first, InputIterator end, InputIterator
  // last);

  // iterator insert(const_iterator pos, std::initializer_list<T> vals);

  // //  Deletes an element at pos
  // void erase(iterator pos) {
  //   Node* nextNode = (pos + 1).ptr_;
  //   Node* prevNode = (pos - 1).ptr_;
  //   prevNode->next_ = nextNode;
  //   nextNode->prev_ = prevNode;
  //   delete pos.ptr_;
  //   counter_ -= 1;
  // }

  // void push_back(const_reference value) {
  //   //  insert puts a node at a position before the one iterator is pointing
  //   to insert(end(), value);
  // }

  // void pop_back() { erase(end() - 1); }

  // void push_front(const_reference value) { insert(begin(), value); }

  // void pop_front() { erase(begin()); }

  void swap(list& other) noexcept(
      al_traits::propagate_on_container_swap::value ||
      al_traits::is_always_equal::value) {
    if constexpr (al_traits::propagate_on_container_swap::value ||
                  !al_traits::is_always_equal::value) {
      std::swap(al_, other.al_);
    }
    std::swap(size_, other.size_);
    head_.swap(other.head_);
  }

  // void merge(list& other) {
  //   iterator iter = begin();
  //   iterator iter_other = other.begin();
  //   while (iter != end() && iter_other != other.end()) {
  //     if (*iter > *iter_other) {
  //       insert(iter, *iter_other);
  //       other.erase(iter_other);
  //       ++iter_other;
  //     } else {
  //       ++iter;
  //     }
  //   }
  // }

  // //  No elements are copied or moved, only the internal
  // //  pointers of the list nodes are re-pointed. Transfers all
  // //  elements from other into *this. The elements are inserted
  // //  before the element pointed to by pos. The container other
  // //  becomes empty after the operation.
  // void splice(const_iterator pos, list& other) {
  //   Node* nextNode = const_cast<Node*>((pos).ptr_);
  //   Node* prevNode = const_cast<Node*>((pos - 1).ptr_);
  //   prevNode->next_ = other.begin().ptr_;
  //   nextNode->prev_ = other.end().ptr_->prev_;
  //   other.head_.prev_ = &other.head_;
  //   other.head_.next_ = &other.head_;
  //   counter_ += other.counter_;
  //   other.counter_ = 0;
  // }
  // void reverse() {
  //   Node* ptr = head_.next_;
  //   for (size_type i = 0; i < counter_; ++i) {
  //     insert(ptr, back());
  //     pop_back();
  //   }
  // }

  // void unique() {
  //   iterator first = begin();
  //   iterator second = first + 1;
  //   while (second != end()) {
  //     if (*second == *first) {
  //       erase(second);
  //       second = first;
  //     } else {
  //       ++first;
  //     }
  //     ++second;
  //   }
  // }

  // // void sort() {
  // //   if (counter_ > 1) {
  // //       MergeSorting(&this);
  // //   }
  // // }

  // //  rename to sort

  // // void sort() {
  // //     if (counter_ < 2) {
  // //         return;
  // //       }
  // //     list a();
  // //     list b();
  // //     FrontBackSplit(*this, a, b);
  // //     a.sort();
  // //     b.sort();
  // //     *this = a.merge(b);
  // //     return;
  // // }

  // // void FrontBackSplit(list &initial, list &a, list &b) {
  // //       iterator middle = initial.begin();
  // //       size_type limit = (initial.counter_ % 2 > 0) ? initial.counter_ / 2 +
  // 1 : initial.counter_ / 2;
  // //       size_type i = 0;
  // //       for (; i < limit; ++i) {
  // //           ++middle;
  // //           // middle = middle.ptr_->next;
  // //       }
  // //       b.head_.next_ = middle.ptr_;
  // //       b.head_.prev_ = initial.end().ptr_->prev_;
  // //       initial.end().ptr_->prev_.next_ = &b.head_;
  // //       b.counter_ = initial.counter_ / 2;

  // //       a.head_.next_ = initial.begin().ptr_;
  // //       a.head_.prev_ = middle.ptr_->prev_;
  // //       middle.ptr_->prev_.next_ = &a.head_;
  // //       initial.begin().ptr_->prev = &a.head_;
  // //       a.counter_ = i;

  // //       initial.head_.prev_ = initial.head_;
  // //       initial.head_.next_ = initial.head_.prev_;
  // //       initial.counter_ = 0;
  // // }

  bool operator==(const list& other) const {
    if (size_ != other.size_) {
      return false;
    }
    const_iterator second = other.begin();
    for (auto first = begin(); first != end(); ++first) {
      if (*first != *second) return false;
    }
    return true;
  }

  bool operator!=(const list& other) const { return !(*this == other); }

  friend std::ostream& operator<<(std::ostream& os, const list& target) {
    const Node* ptr = target.head_.next_node;
    for (; ptr->next_node != target.head_; ptr = ptr->next_node) {
      os << ptr->data << ' ';
    }
    os << ptr->data;
    return os;
  }

 private:
  struct Node {
    Node() noexcept : prev_node(this), next_node(this) {}
    Node(Node* prev, Node* next) noexcept : prev_node(prev), next_node(next) {
      prev->next_node = this;
      next->prev_node = this;
    }
    virtual ~Node() = default;

    Node* next() const noexcept { return next_node; }
    Node* prev() const noexcept { return prev_node; }

    void bind(Node* prev, Node* next) noexcept {
      prev_node = prev;
      next_node = next;
      prev->next_node = this;
      next->prev_node = this;
    }

    void unbind() noexcept {
      next_node->prev_node = prev_node;
      prev_node->next_node = next_node;
      prev_node = nullptr;
      next_node = nullptr;
    }

    void swap(Node& other) {
      prev_node->next = &other;
      next_node->prev = &other;
      other.prev_node->next = this;
      other.next_node->prev = this;
      std::swap(prev_node, other.prev_node);
      std::swap(next_node, other.next_node);
    }

    Node* prev_node;
    Node* next_node;
  };

  struct ValueNode final : public Node {
    ValueNode() = default;
    ValueNode(const T& value) noexcept(
        std::is_nothrow_copy_constructible<T>::value)
        : data(value) {}
    ValueNode(const T& value, Node* prev,
              Node* next) noexcept(std::is_nothrow_copy_constructible<T>::value)
        : Node(prev, next), data(value) {}

    template <typename... Args>
    ValueNode(Node* prev, Node* next) noexcept(
        std::is_nothrow_constructible<T, Args...>::value)
        : Node(prev, next), data(value) {}

    T& value() { return data; }
    const T& value() const { return data; }

    T data;
  };

  constexpr ValueNode* cast(Node* ptr) const {
    return dynamic_cast<ValueNode*>(ptr);
  }

  template <typename... Args>
  Node* push_nodes(Node* where, sp::size_type count, Args&&... args) {
    for (size_type i = 0; i < count; ++i, where = where->next_node) {
      ValueNode* some = al_.allocate(1);
      rebind_traits::construct(al_, some, std::forward<Args>(args)...);
      some->bind(where, &head_);
    }
    return where;
  }

  template <typename InputIterator>
  Node* push_nodes(Node* where, InputIterator first, InputIterator last) {
    for (; first != last; ++first, where = where->next_node) {
      ValueNode* some = al_.allocate(1);
      rebind_traits::construct(al_, some, *first);
      some->bind(where, &head_);
    }
    return where;
  }

  void pop_nodes(Node* first, Node* last) {
    Node* bwd = first;
    Node* fwd = first->next_node;
    while (bwd != last) {
      ValueNode* x = cast(bwd);
      rebind_traits::destroy(al_, x);
      al_.deallocate(x, 1);
      bwd = fwd;
      fwd = fwd->next_node;
    }
  }

  rebind_alloc al_;
  size_type size_;
  Node head_;
};

}  // namespace sp

#endif  // SP_CONTAINERS_LIST_H_
