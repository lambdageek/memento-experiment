
#ifndef _MEMENTO_HPP
#define _MEMENTO_HPP

#include <memory> // for auto_ptr
#include <stdexcept> // for domain_error

namespace aleksey
{
  namespace design_patterns
  {

    namespace {
      template<typename X>
      struct friend_maker
      {
	typedef X type;
      };
    }

    // requirements:
    // D must have inner class state that is Cloneable
    // D must have member functions:
    //    D::state D::reifyState() const;
    //    void D::restoreState(const D::state&);
    // D must declare add_memento<D> аs a friend
    template<class D>
    class add_memento
    {
    public:
      class memento;
    private:
      typedef D derived;
      
      // hack to make memento final
      class memento_base {
      private:
	friend class memento;
	virtual ~memento_base() { 
	}
      };
      
      derived& asDerived() {
	return static_cast<derived&>(*this);
      }
      const derived& asDerived() const {
	return static_cast<const derived&>(*this);
      }
      
    public:
      // mementos may be copied and destroyed, they may not be
      // assigned copying a memento copies the underlying reified
      // state.  unlike state which is assumed to provide no public
      // interface to clients of D, the memento is publically visible.
      // A memento also ensures that only the instance that it
      // originated from may use it to restore itself.
      class memento : private virtual memento_base {
      public:
	memento(const memento& other)
	  : originator(other.originator),
	    st(other.st->clone())
	{}
	~memento() { }
      protected:
      private:
	friend class add_memento;
	friend class friend_maker<derived>::type;
	memento(const derived& t) : originator(t), st(t.reifyState()) 
	{ }
	memento& operator=(const memento&); // not implemented
	memento(); //not implemented
	
	void doRestore(derived& d) const {
	  d.restoreState(*st);
	}

	const derived& originator;
	std::auto_ptr<const typename derived::state> st;
      };

      virtual memento backup() const {
	return memento(asDerived());
      }
      virtual void restore(const memento& m) {
	if (&m.originator == &asDerived()) {
	  m.doRestore(asDerived());
	} else {
	  throw std::domain_error("memo originated in a different instance");
	}
      }
    };

  }// namespace aleksey::design_patterns
}//namespace aleksey
#endif //_MEMENTO_HPP
