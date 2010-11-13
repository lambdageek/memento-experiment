
#include "memento.hpp"
#include <memory>
#include <iostream>
#include <string>


using aleksey::design_patterns::add_memento;

class t : private add_memento<t>
{
public:

  friend std::ostream& operator<<(std::ostream&, const t&);

  t(int i) : theInt(i) {}
  virtual ~t() {}
  void mutate() { ++theInt; }

protected:
  class state {
  public:
    friend class t;
    state(const t& obj) : m_i(obj.theInt) {}
    state(const state& o) : m_i(o.m_i) {}
    virtual ~state() {};
    virtual state* clone() const {
      return new state(m_i);
    }
  private:
    state& operator=(const state&); // not implemented
    int m_i;
  };

  virtual state* reifyState() const {
    return new state(*this);
  }
  virtual void restoreState(const state& s) {
    theInt = s.m_i;
  }

public:
  using add_memento<t>::memento;
  using add_memento<t>::backup;
  using add_memento<t>::restore;

protected:
  int getInt() const { return theInt; }
  void setInt(int i) { theInt = i; }
private:
  friend class add_memento<t>;
  int theInt;
};

class dt : public t {
public:
  friend std::ostream& operator<<(std::ostream&, const dt&);
  dt(int i, double d) : t(i), theD(d) { }
  virtual ~dt() {}
  void otherMutate() { ++theD; }

protected:
  class state : public t::state {
  public:
    friend class dt;
    state(const dt& obj) : t::state(obj), m_d(obj.theD) { }
    virtual ~state() { }
    state(const state& o) : t::state(o), m_d(o.m_d) { }
    virtual dt::state* clone() const {
      return new dt::state(*this);
    }
  private:
    state& operator=(const state&); // not implemented
    double m_d;
  };

  virtual state* reifyState() const {
    return new dt::state(*this);
  }
  virtual void restoreState(const t::state& st) {
    t::restoreState(st);

    if (const dt::state* s2 = dynamic_cast<const dt::state*>(&st)) {
      theD = s2->m_d;
    }
  }
private:
  double theD;
};


std::ostream& operator<<(std::ostream& os, const t& t)
{
  return os << "t (" << t.theInt << ")";
}

std::ostream& operator<<(std::ostream& os, const dt& dt)
{
  return os << "dt {" << static_cast<const t&>(dt) << ", " << dt.theD << "}";
}

int
main (int argc, const char* const * argv)
{
  dt t1(10, 5.0);
  std::cout << "before = " << t1 << std::endl;
  t::memento memo = t1.backup();
  t1.mutate();
  t1.otherMutate();
  std::cout << "after = " << t1 << std::endl;
  t1.restore(memo);
  std::cout << "restored = " << t1 << std::endl;
}
