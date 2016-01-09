#define CATCH_CONFIG_MAIN
#include "catch.h"
#include "optional.h"

#include <vector>
#include <string>

using std::string;
using std::vector;

TEST_CASE("Absent is absent", "[absent]") {
  Optional<int> x = Optional<int>::absent();
  REQUIRE(!x.isPresent());
}

TEST_CASE("Of is present", "[present]") {
  Optional<int> x = Optional<int>::of(13);
  REQUIRE(x.isPresent());
  REQUIRE(x.get() == 13);
}

TEST_CASE("Of_emplaced is present", "[present]") {
  Optional<int> x = Optional<int>::of_emplaced(13);
  REQUIRE(x.isPresent());
  REQUIRE(x.get() == 13);
}

TEST_CASE("Copy constructor works for absent") {
  const Optional<char> x = Optional<char>::absent();
  Optional<char> y = x;
  REQUIRE(!x.isPresent());
  REQUIRE(!y.isPresent());
}

TEST_CASE("Copy constructor works for present") {
  const Optional<char> x = Optional<char>::of(21);
  Optional<char> y = x;
  REQUIRE(x.isPresent());
  REQUIRE(y.isPresent());
  REQUIRE(x.get() == 21);
  REQUIRE(y.get() == 21);
}

class NonConstructable {
 private:
  NonConstructable() { throw "can't construct"; }
};

TEST_CASE("Non-constructable objects can be put in Optional::absent") {
  Optional<NonConstructable> x = Optional<NonConstructable>::absent();
}

static vector<string> events;

class MoveTracker {
  string id;

 public:
  MoveTracker(const string& id) : id(id) { events.push_back("ctor " + id); }

  MoveTracker(const MoveTracker& that) : id(that.id + ".copy") {
    events.push_back("copy " + id);
  }

  MoveTracker(MoveTracker&& that) : id(that.id + ".move") {
    that.id += ".defunct";
    events.push_back("move " + id);
  }

  MoveTracker& operator=(const MoveTracker& that) {
    id = that.id + ".copyassign";
    events.push_back("copyassign " + id);
    return *this;
  }

  MoveTracker& operator=(MoveTracker&& that) {
    id = that.id + ".moveassign";
    that.id += ".assigndefunct";
    events.push_back("moveassign " + id);
    return *this;
  }

  ~MoveTracker() { events.push_back("dtor " + id); }
};

class CopyTracker {
  string id;

 public:
  CopyTracker(const string& id) : id(id) { events.push_back("ctor " + id); }

  CopyTracker(const CopyTracker& that) : id(that.id + ".copy") {
    events.push_back("copy " + id);
  }

  CopyTracker& operator=(const CopyTracker& that) {
    id = that.id + ".copyassign";
    events.push_back("copyassign " + id);
    return *this;
  }

  ~CopyTracker() { events.push_back("dtor " + id); }
};

TEST_CASE("No construction if absent") {
  events.clear();
  Optional<MoveTracker> t = Optional<MoveTracker>::absent();
  REQUIRE(events.empty());
}

TEST_CASE("of MoveTracker constructs once, moves once, and destructs both") {
  events.clear();
  vector<string> expected = {"ctor my", "move my.move", "dtor my.defunct"};
  {
    Optional<MoveTracker> t = Optional<MoveTracker>::of(MoveTracker("my"));
    REQUIRE(events == expected);
  }
  expected.push_back("dtor my.move");
  REQUIRE(events == expected);
}

TEST_CASE("of_emplaced MoveTracker constructs once and destructs once") {
  events.clear();
  vector<string> expected = {"ctor empl"};
  {
    Optional<MoveTracker> t = Optional<MoveTracker>::of_emplaced("empl");
    REQUIRE(events == expected);
  }
  expected.push_back("dtor empl");
  REQUIRE(events == expected);
}

TEST_CASE("of CopyTracker constructs once, copies once, and destructs both") {
  events.clear();
  vector<string> expected = {"ctor my", "copy my.copy", "dtor my"};
  {
    Optional<CopyTracker> t = Optional<CopyTracker>::of(CopyTracker("my"));
    REQUIRE(events == expected);
  }
  expected.push_back("dtor my.copy");
  REQUIRE(events == expected);
}

TEST_CASE("of_emplaced CopyTracker constructs once and destructs once") {
  events.clear();
  vector<string> expected = {"ctor empl"};
  {
    Optional<CopyTracker> t = Optional<CopyTracker>::of_emplaced("empl");
    REQUIRE(events == expected);
  }
  expected.push_back("dtor empl");
  REQUIRE(events == expected);
}

struct ConstructByValue {
  ConstructByValue(CopyTracker) {}
};

struct ConstructByConstValue {
  ConstructByConstValue(const CopyTracker) {}
};

struct ConstructByReference {
  ConstructByReference(CopyTracker&) {}
};

struct ConstructByConstReference {
  ConstructByConstReference(const CopyTracker&) {}
};

struct ConstructByXReference {
  ConstructByXReference(CopyTracker&&) {}
};

struct ConstructByConstXReference {
  ConstructByConstXReference(const CopyTracker&&) {}
};


TEST_CASE("of_emplaced arguments are perfectly forwarded when passed by value") {
  events.clear();
  vector<string> expected = {"ctor arg", "copy arg.copy", "dtor arg.copy", "dtor arg"};
  auto t = Optional<ConstructByValue>::of_emplaced(CopyTracker("arg"));
  REQUIRE(events == expected);
}

TEST_CASE("of_emplaced arguments are perfectly forwarded when passed by const value") {
  events.clear();
  vector<string> expected = {"ctor arg", "copy arg.copy", "dtor arg.copy", "dtor arg"};
  auto t = Optional<ConstructByConstValue>::of_emplaced(CopyTracker("arg"));
  REQUIRE(events == expected);
}

TEST_CASE("of_emplaced arguments are perfectly forwarded when passed by reference") {
  events.clear();
  vector<string> expected = {"ctor arg"};
  CopyTracker arg("arg");
  auto t = Optional<ConstructByReference>::of_emplaced(arg);
  REQUIRE(events == expected);
}

TEST_CASE("of_emplaced arguments are perfectly forwarded when passed by const reference") {
  events.clear();
  vector<string> expected = {"ctor arg"};
  CopyTracker arg("arg");
  auto t = Optional<ConstructByConstReference>::of_emplaced(arg);
  REQUIRE(events == expected);
}

TEST_CASE("of_emplaced arguments are perfectly forwarded when passed by x-reference") {
  events.clear();
  vector<string> expected = {"ctor arg", "dtor arg"};
  auto t = Optional<ConstructByXReference>::of_emplaced(CopyTracker("arg"));
  REQUIRE(events == expected);
}

TEST_CASE("of_emplaced arguments are perfectly forwarded when passed by const x-reference") {
  events.clear();
  vector<string> expected = {"ctor arg", "dtor arg"};
  auto t = Optional<ConstructByConstXReference>::of_emplaced(CopyTracker("arg"));
  REQUIRE(events == expected);
}


// TODO if <T> has no copy/move constructor, then nothing works. Is that fine?
// TEST_CASE("non-movable and non-copyable is emplace-able") {
//   const Optional<NoMoveNoCopy> n = Optional<NoMoveNoCopy>::of_emplaced();
// }
