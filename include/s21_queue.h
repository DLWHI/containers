#ifndef SRC_CONTAINERS_S21_QUEUE_H_
#define SRC_CONTAINERS_S21_QUEUE_H_
#include "s21_list.h"

namespace s21 {
template <typename T, typename containers = s21::list<T>>

class queue {
    public:
        typedef T value_type;
        typedef T& reference;
        typedef const T& const_reference;
        typedef typename containers::iterator iterator;
        typedef typename containers::const_iterator const_iterator;
        typedef size_t size_type;

        //  Default constructor
        queue() : base() {}

        //  Initializer list constructor
        queue(std::initializer_list<value_type> const& items) : base(items) {}

        //  Copy 
        queue(const queue& other) : base(other.base) {}

        //  Move constructor (use move to make shure the argument is rvalue)
        queue(queue&& other) : base(std::move(other.base)) {}

        //  Assignment operator overload for moving an object
        queue& operator=(queue&& s) {
        base = std::move(s.base);
        return *this;
        }

        //  Destructor:  
        ~queue() = default;

        //  Queue Element access:

        const_reference front() {
            return base.front();
        }
        const_reference back() {
            return base.back();
        }

        //  Queue Capacity:

        bool empty() {
            return base.empty();
        }
        size_type size() {
            return base.size();
        }

        void pop() { base.pop_front(); }
        void push(const_reference value) { base.push_back(value); }
        void swap(queue& other) { base.swap(other.base); }

  
    private:
    containers base;
    };
} // namespace s21

#endif  // SRC_CONTAINERS_S21_QUEUE_H_