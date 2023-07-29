#ifndef DLWHI_TESTS_TEST_HELPERS_H_
#define DLWHI_TESTS_TEST_HELPERS_H_
#include <string>
#include <memory>
// This is testing subject class with restrictions
// Several implementations satisfy different named requirements
// Vector requires object to be at least
// CopyAssignable, CopyConstructible and Erasable
// Some methods could alse require MoveAssignable and MoveConstructible

// CopyConstructible and Erasable dummy
class dummyCpy
{
  public:
    dummyCpy() : id_("default"), leak_(new int()) { };
    explicit dummyCpy(const std::string& name) : id_(name), leak_(new int()) { };
    dummyCpy(dummyCpy&& other) = delete;
    dummyCpy(const dummyCpy& other) : id_(other.id_), leak_(new int()) { };
    virtual ~dummyCpy() { delete leak_;}

    bool operator==(const dummyCpy& other) const { return other.id_ == id_;}
    bool operator!=(const dummyCpy& other) const { return other.id_ != id_;}

    friend std::ostream& operator<<(std::ostream& os, const dummyCpy& obj) {
      os << obj.id_;
      return os;
    }
  protected:
    std::string id_;
    int* leak_;
};

// + MoveAssignable and MoveConstructible dummy
class dummyMv : public dummyCpy
{
  public:
    dummyMv() : dummyCpy() { };
    explicit dummyMv(const std::string& name) : dummyCpy(name) { };
    dummyMv(const dummyMv& other) : dummyCpy(other.id_) { };
    // speed dies as this is test class
    dummyMv(dummyMv&& other) { 
      id_ = std::move(other.id_);
      delete leak_;
      leak_ = other.leak_;
      other.leak_ = nullptr;
    }
    virtual ~dummyMv() { }
};

template <typename T>
class state_allocator : public std::allocator<T> {
  public:
    state_allocator(const std::string& state) : std::allocator<T>(), state_(state) {};

    bool operator==(const state_allocator& other) {
      return state_ == other.state_;
    }
    bool operator!=(const state_allocator& other) {
      return state_ != other.state_;
    }
  private:
    std::string state_;
};
#endif  // DLWHI_TESTS_TEST_HELPERS_H_
