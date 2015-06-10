/* future.hpp
Non-allocating constexpr future-promise
(C) 2015 Niall Douglas http://www.nedprod.com/
File Created: May 2015


Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef BOOST_SPINLOCK_FUTURE_HPP
#define BOOST_SPINLOCK_FUTURE_HPP

#include "monad.hpp"

BOOST_SPINLOCK_V1_NAMESPACE_BEGIN
namespace lightweight_futures {
  
template<typename R, class _error_type=std::error_code, class _exception_type=std::exception_ptr> class promise;

namespace detail
{
  template<class promise_type, class future_type> struct lock_guard
  {
    promise_type *_p;
    future_type  *_f;
    lock_guard(const lock_guard &)=delete;
    lock_guard(lock_guard &&)=delete;
    BOOST_SPINLOCK_FUTURE_MSVC_HELP lock_guard(promise_type *p) : _p(nullptr), _f(nullptr)
    {
      // constexpr fold
      if(!p->_need_locks)
      {
        _p=p;
        if(p->_storage.type==promise_type::value_storage_type::storage_type::future)
          _f=p->_storage.future_;
        return;
      }
      else for(;;)
      {
        p->_lock.lock();
        if(p->_storage.type==promise_type::value_storage_type::storage_type::future)
        {
          if(p->_storage.future_->_lock.try_lock())
          {
            _p=p;
            _f=p->_storage.future_;
            break;
          }
        }
        else
        {
          _p=p;
          break;
        }
        p->_lock.unlock();
      }
    }
    BOOST_SPINLOCK_FUTURE_MSVC_HELP lock_guard(future_type *f) : _p(nullptr), _f(nullptr)
    {
      // constexpr fold
      if(!f->_need_locks)
      {
        _p=f->_promise;
        _f=f;
        return;
      }
      else for(;;)
      {
        f->_lock.lock();
        if(f->_promise)
        {
          if(f->_promise->_lock.try_lock())
          {
            _p=f->_promise;
            _f=f;
            break;
          }
        }
        else
        {
          _f=f;
          break;
        }
        f->_lock.unlock();
      }
    }
    BOOST_SPINLOCK_FUTURE_MSVC_HELP ~lock_guard()
    {
      unlock();
    }
    void unlock()
    {
      if(_p)
      {
        if(_p->_need_locks)
          _p->_lock.unlock();
        _p=nullptr;
      }
      if(_f)
      {
        if(_f->_need_locks)
          _f->_lock.unlock();
        _f=nullptr;
      }
    }
  };
  
  struct throw_future_promise_error
  {
    BOOST_SPINLOCK_FUTURE_CXX14_CONSTEXPR throw_future_promise_error(monad_errc ec)
    {
      switch(ec)
      {
        case monad_errc::already_set:
          throw std::future_error(std::future_errc::promise_already_satisfied);
        case monad_errc::no_state:
          throw std::future_error(std::future_errc::no_state);
        default:
          abort();
      }
    }
  };
}

template<typename R, class _error_type, class _exception_type> class promise
{
public:
  typedef R value_type;
  typedef _error_type error_type;
  typedef _exception_type exception_type;
  typedef future<value_type, error_type, exception_type> future_type;
  friend class future<value_type, error_type, exception_type>;
  friend struct detail::lock_guard<promise, future_type>;
private:
  typedef detail::value_storage<value_type, error_type, exception_type, detail::throw_future_promise_error> value_storage_type;
  value_storage_type _storage;
  bool _need_locks;                 // Used to inhibit unnecessary atomic use, thus enabling constexpr collapse
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4624)
#endif
  union { spinlock<bool> _lock; };  // Delay construction
#ifdef _MSC_VER
#pragma warning(pop)
#endif
public:
  //! \brief EXTENSION: constexpr capable constructor
  BOOST_SPINLOCK_FUTURE_CONSTEXPR promise() : _need_locks(false)
  {
    static_assert(std::is_move_constructible<value_type>::value, "Type must be move constructible to be used in a lightweight promise");    
  }
  // template<class Allocator> promise(allocator_arg_t, Allocator a); // cannot support
  BOOST_SPINLOCK_FUTURE_CXX14_CONSTEXPR promise(promise &&o) noexcept(std::is_nothrow_move_constructible<value_storage_type>::value) : _need_locks(o._need_locks)
  {
    if(_need_locks) new (&_lock) spinlock<bool>();
    detail::lock_guard<promise, future_type> h(&o);
    _storage=std::move(o._storage);
    if(h._f)
      h._f->_promise=this;
  }
  promise &operator=(promise &&o) noexcept(std::is_nothrow_move_constructible<value_storage_type>::value)
  {
    // TODO FIXME: Only safe if both of these are noexcept
    this->~promise();
    new (this) promise(std::move(o));
    return *this;
  }
  promise(const promise &)=delete;
  promise &operator=(const promise &)=delete;
  BOOST_SPINLOCK_FUTURE_MSVC_HELP ~promise() noexcept(std::is_nothrow_destructible<value_storage_type>::value)
  {
    detail::lock_guard<promise, future_type> h(this);
    if(h._f)
    {
      if(!h._f->is_ready())
        h._f->set_exception(make_exception_ptr(std::future_error(std::future_errc::broken_promise)));
      h._f->_promise=nullptr;
    }
    // Destroy myself before locks exit
    _storage.clear();
  }
  
  void swap(promise &o) noexcept(std::is_nothrow_move_constructible<value_storage_type>::value)
  {
    detail::lock_guard<promise, future_type> h1(this), h2(&o);
    _storage.swap(o._storage);
    if(h1._f)
      h1._f->_promise=&o;
    if(h2._f)
      h2._f->_promise=this;
  }
  
  BOOST_SPINLOCK_FUTURE_MSVC_HELP future_type get_future()
  {
    // If no value stored yet, I need locks on from now on
    if(!_need_locks && _storage.type==value_storage_type::storage_type::empty)
    {
      _need_locks=true;
      new (&_lock) spinlock<bool>();
    }
    detail::lock_guard<promise, future_type> h(this);
    if(h._f)
      throw std::future_error(std::future_errc::future_already_retrieved);
    future_type ret(this);
    h.unlock();
    return std::move(ret);
  }
  //! \brief EXTENSION: Does this promise have a future?
  bool has_future() const noexcept
  {
    //detail::lock_guard<value_type> h(this);
    return _storage.type==value_storage_type::storage_type::future;
  }
  
  BOOST_SPINLOCK_FUTURE_MSVC_HELP void set_value(const value_type &v) noexcept(std::is_nothrow_copy_constructible<value_type>::value)
  {
    detail::lock_guard<promise, future_type> h(this);
    if(h._f)
      h._f->set_value(v);
    else
      _storage.set_value(v);
  }
  BOOST_SPINLOCK_FUTURE_MSVC_HELP void set_value(value_type &&v) noexcept(std::is_nothrow_move_constructible<value_type>::value)
  {
    detail::lock_guard<promise, future_type> h(this);
    if(h._f)
      h._f->set_value(std::move(v));
    else
      _storage.set_value(std::move(v));
  }
  //! \brief EXTENSION: Set an error code (doesn't allocate)
  void set_error(error_type e) noexcept(std::is_nothrow_copy_constructible<error_type>::value)
  {
    detail::lock_guard<promise, future_type> h(this);
    if(h._f)
      h._f->set_error(e);
    else
      _storage.set_error(e);
  }
  void set_exception(exception_type e) noexcept(std::is_nothrow_copy_constructible<exception_type>::value)
  {
    detail::lock_guard<promise, future_type> h(this);
    if(h._f)
      h._f->set_exception(e);
    else
      _storage.set_exception(e);
  }
  template<typename E> void set_exception(E &&e)
  {
    set_exception(make_exception_ptr(std::forward<E>(e)));
  }
  
  // Not supported right now
  // void set_value_at_thread_exit(R v);
  // void set_exception_at_thread_exit(R v);

  //! \brief Call F when the future signals, consuming the future. Only one of these may be set.
  // template<class F> typename std::result_of<F(future<value_type>)>::type then(F &&f);

  //! \brief Call F when the future signals, not consuming the future.
  // template<class F> typename std::result_of<F(future<const value_type &>)>::type then(F &&f);
};

// TODO: promise<void>, promise<R&> specialisations
// TODO: future<void>, future<R&> specialisations

/*! \class future
\brief Lightweight next generation future with N4399 Concurrency TS extensions

http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4399.html
*/
template<typename R, class _error_type, class _exception_type> class future : protected monad<R, _error_type, _exception_type, detail::throw_future_promise_error>
{
  typedef monad<R, _error_type, _exception_type, detail::throw_future_promise_error> monad_type;
public:
  typedef typename monad_type::value_type value_type;
  typedef typename monad_type::error_type error_type;
  typedef typename monad_type::exception_type exception_type;
  BOOST_STATIC_CONSTEXPR bool is_consuming=true;
  typedef promise<value_type, error_type, exception_type> promise_type;
  friend class promise<value_type, error_type, exception_type>;
  friend struct detail::lock_guard<promise_type, future>;
private:
  bool _need_locks;                 // Used to inhibit unnecessary atomic use, thus enabling constexpr collapse
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4624)
#endif
  union { spinlock<bool> _lock; };  // Delay construction
#ifdef _MSC_VER
#pragma warning(pop)
#endif
  promise_type *_promise;
protected:
  // Called by promise::get_future(), so currently thread safe
  BOOST_SPINLOCK_FUTURE_CXX14_CONSTEXPR future(promise_type *p) : monad_type(std::move(p->_storage)), _need_locks(p->_need_locks), _promise(p)
  {
    if(_need_locks) new (&_lock) spinlock<bool>();
    p->_storage.set_future(this);
  }
public:
  //! \brief EXTENSION: constexpr capable constructor
  BOOST_SPINLOCK_FUTURE_CONSTEXPR future() : _need_locks(false), _promise(nullptr)
  {
    static_assert(std::is_move_constructible<value_type>::value, "Type must be move constructible to be used in a lightweight future");    
  }
  BOOST_SPINLOCK_FUTURE_CXX14_CONSTEXPR future(future &&o) noexcept(std::is_nothrow_move_constructible<monad_type>::value) : _need_locks(o._need_locks), _promise(nullptr)
  {
    if(_need_locks) new (&_lock) spinlock<bool>();
    detail::lock_guard<promise_type, future> h(&o);
    new(this) monad_type(std::move(o));
    if(o._promise)
    {
      _promise=o._promise;
      o._promise=nullptr;
      if(h._p)
        h._p->_storage.future_=this;
    }
  }
  future &operator=(future &&o) noexcept(std::is_nothrow_move_constructible<monad_type>::value)
  {
    // TODO FIXME: Only safe if both of these are noexcept
    this->~future();
    new (this) future(std::move(o));
    return *this;
  }
  future(const future &)=delete;
  future &operator=(const future &)=delete;
  // MSVC needs the destructor force inlined to do the right thing for some reason
  BOOST_SPINLOCK_FUTURE_MSVC_HELP ~future() noexcept(std::is_nothrow_destructible<monad_type>::value)
  {
    detail::lock_guard<promise_type, future> h(this);
    if(h._p)
      h._p->_storage.clear();
    // Destroy myself before locks exit
    monad_type::clear();
  }
  
  void swap(future &o) noexcept(std::is_nothrow_move_constructible<monad_type>::value)
  {
    detail::lock_guard<promise_type, future> h1(this), h2(&o);
    monad_type::swap(o._storage);
    if(h1._p)
      h1._p->_storage.future_=&o;
    if(h2._p)
      h2._p->_storage.future_=this;
  }
  
  // shared_future<value_type> share();  // TODO
  
  BOOST_SPINLOCK_FUTURE_MSVC_HELP value_type get()
  {
    wait();
    detail::lock_guard<promise_type, future> h(this);
    value_type ret(std::move(*this).value());
    monad_type::clear();
    if(h._p)
      h._p->_storage.clear();
    if(h._f)
      h._f->_promise=nullptr;
    return std::move(ret);
  }
  // value_type get_or(const value_type &);  // TODO
  // value_type get_or(value_type &&);  // TODO
  error_type get_error()
  {
    wait();
    detail::lock_guard<promise_type, future> h(this);
    error_type e(monad_type::error());
    if (!e)
      return std::move(e);
    monad_type::clear();
    if (h._p)
      h._p->_storage.clear();
    if (h._f)
      h._f->_promise = nullptr;
    return std::move(e);
  }
  exception_type get_exception()
  {
    wait();
    detail::lock_guard<promise_type, future> h(this);
    exception_type e(monad_type::get_exception());
    if(!e)
      return std::move(e);
    monad_type::clear();
    if(h._p)
      h._p->_storage.clear();
    if(h._f)
      h._f->_promise=nullptr;
    return std::move(e);
  }
  // Compatibility with Boost.Thread
  exception_type get_exception_ptr() { return get_exception(); }
  
  bool valid() const noexcept
  {
    return !!_promise;
  }
  using monad_type::is_ready;
  using monad_type::has_value;
  using monad_type::has_error;
  using monad_type::has_exception;
  
  void wait() const
  {
    if(!valid())
      throw std::future_error(std::future_errc::no_state);
    // TODO Actually sleep
    while(!monad_type::is_ready())
    {
    }
  }
  // template<class R, class P> future_status wait_for(const std::chrono::duration<R, P> &rel_time) const;  // TODO
  // template<class C, class D> future_status wait_until(const std::chrono::time_point<C, D> &abs_time) const;  // TODO
  
  // TODO Where F would return a future<future<...>>, we unwrap to a single future<R>
  // template<class F> typename std::result_of<F(future)>::type then(F &&f);
};

template<typename R> future<typename std::decay<R>::type> make_ready_future(R &&v)
{
  return future<typename std::decay<R>::type>(std::forward<R>(v));
}
template<typename R> future<R> make_errored_future(std::error_code v)
{
  return future<R>(v);
}
template<typename R> future<R> make_exceptional_future(std::exception_ptr v)
{
  return future<R>(v);
}

// TODO
// template<class InputIterator> ? when_all(InputIterator first, InputIterator last);
// template<class... Futures> ? when_all(Futures &&... futures);
// template<class Sequence> struct when_any_result;
// template<class InputIterator> ? when_any(InputIterator first, InputIterator last);
// template<class... Futures> ? when_any(Futures &&... futures);

// TODO packaged_task

#ifdef _MSC_VER
#undef value
#undef exception
#undef error
#undef future_
#endif

}
BOOST_SPINLOCK_V1_NAMESPACE_END

namespace std
{
  template<typename R> void swap(BOOST_SPINLOCK_V1_NAMESPACE::lightweight_futures::promise<R> &a, BOOST_SPINLOCK_V1_NAMESPACE::lightweight_futures::promise<R> &b)
  {
    a.swap(b);
  }
  template<typename R> void swap(BOOST_SPINLOCK_V1_NAMESPACE::lightweight_futures::future<R> &a, BOOST_SPINLOCK_V1_NAMESPACE::lightweight_futures::future<R> &b)
  {
    a.swap(b);
  }
}

#endif