#ifndef __OPTIONAL_H__
#define  __OPTIONAL_H__

#include <cassert>

enum Dummy{};

template<typename T>
class Optional {
private:
    const union {
      T data;
    };
    const bool present;

  public:
    Optional(const Optional & that) : present(that.present) {
      if (present) {
        data = that.data;
      }
    }

    explicit Optional(Optional && that) : present(that.present) {
      if (present) {
        data = std::move(that.data);
      }
    }

    Optional& operator=(Optional&) = delete;

    bool isPresent() const {
      return present;
    }

    const T& get() const {
      if (!isPresent()) {
        throw "Called get() on absent optional";
      }
      return data;
    }

    ~Optional() {
      if (isPresent()) {
        data.~T();
      }
    }

  private:
    Optional() : present(false) {}

    explicit Optional(T&& value) : data(std::forward<T>(value)), present(true) {}

    template<typename ...Args>
    Optional(Dummy dummy, Args&&... args) :
      data(std::forward<Args>(args)...),
      present(true) {}

  public:
    static Optional<T> absent() {
        return Optional<T>();
    }
    static Optional<T> of(T&& t) {
        return Optional<T>(std::forward<T&&>(t));
    }
    template<typename ...Args>
    static Optional<T> of_emplaced(Args&&... args) {
      return Optional<T>(Dummy(), std::forward<Args>(args)...);
    }
};


#endif // __OPTIONAL_H__