#include <cassert>

#include <iostream>
using std::cout;
using std::endl;


enum Dummy{};

template<typename T>
class Optional {
private:
    const union {
      T data;
    };
    const bool present;

  public:
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

    explicit Optional(T&& value) : data(value), present(true) {}

    template<typename ...Args>
    Optional(Dummy dummy, Args&&... args) :
      data(T(std::forward<Args>(args)...)),
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


class A {
public:
  A() {
    cout << "ctor" << endl;
  }

  ~A() {
    cout << "dtor" << endl;
  }
};


int lemain() {
  Optional<int> y = Optional<int>::of(99999999);
  cout << y.isPresent() << endl;
  cout << y.get() << endl;

  Optional<int> x = Optional<int>::absent();
  cout << x.isPresent() << endl;

  cout << endl << endl << "first" << endl;
  {
    Optional<A> a(Optional<A>::of(A()));
  }


  cout << endl << endl << "second" << endl;
  {
    Optional<A> b = Optional<A>::absent();
  }


  cout << endl << endl << "third" << endl;
  {
    Optional<A> a(Optional<A>::of_emplaced());
  }
}
