#define CATCH_CONFIG_MAIN
#include "catch.h"
#include "optional.h"

TEST_CASE("Absent is absent", "[absent]") {
  Optional<int> x = Optional<int>::absent();
  REQUIRE( ! x.isPresent() );
}

TEST_CASE("Of is present", "[present]") {
  Optional<int> x = Optional<int>::of(13);
  REQUIRE( x.isPresent() );
  REQUIRE( x.get() == 13 );
}

TEST_CASE("Of_emplaced is present", "[present]") {
  Optional<int> x = Optional<int>::of_emplaced(13);
  REQUIRE( x.isPresent() );
  REQUIRE( x.get() == 13 );
}

TEST_CASE("Copy constructor works for absent") {
  const Optional<char> x = Optional<char>::absent();
  Optional<char>y = x;
  REQUIRE( ! x.isPresent() );
  REQUIRE( ! y.isPresent() );
}

TEST_CASE("Copy constructor works for present") {
  const Optional<char> x = Optional<char>::of(21);
  Optional<char>y = x;
  REQUIRE( x.isPresent() );
  REQUIRE( y.isPresent() );
  REQUIRE( x.get() == 21 );
  REQUIRE( y.get() == 21 );
}

class NonConstructable {
  private:
    NonConstructable() { throw "can't construct"; }
};

TEST_CASE("Non-constructable objects can be Optional<absent>") {
  Optional<NonConstructable> x = Optional<NonConstructable>::absent();
}

#include <vector>
#include <string>
using std::string;
using std::vector;

static vector<string> events;

class Tracker {
  string id;
  public:
  Tracker(const string& id) : id(id) {
    events.push_back("ctor");
  }

  Tracker(const Tracker& that) : id(that.id + ".copy") {
    events.push_back("copy");
  }

  Tracker(Tracker&& that) : id(that.id + ".move") {
    that.id += ".defunct";
    events.push_back("move");
  }

  Tracker& operator=(const Tracker& that) {
    id = that.id + ".copyassign";
    events.push_back("copyassign");
  }

  Tracker& operator=(Tracker&& that) {
    id = that.id + ".moveassign";
    that.id += ".assigndefunct";
    events.push_back("moveassign");
  }

  ~Tracker() {
    events.push_back("dtor");
  }
};


// TODO if <T> has no copy constructor, then nothing works

TEST_CASE("Construct only twice") {
  events.clear();
  Optional<Tracker> t = Optional<Tracker>::absent();
  REQUIRE( events.empty() );
}
