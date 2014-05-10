#pragma once

namespace boba
{
  class CriticalSection
  {
  public:
    CriticalSection();
    ~CriticalSection();
    void enter();
    void leave();
  private:
    CRITICAL_SECTION _cs;
  };

  class ScopedCs
  {
  public:
    ScopedCs(CriticalSection &cs);
    ~ScopedCs();
  private:
    CriticalSection &_cs;
  };

  class ScopedHandle
  {
  public:
    ScopedHandle(HANDLE h) : _handle(h) {}
    ~ScopedHandle() {
      if (_handle != INVALID_HANDLE_VALUE)
        CloseHandle(_handle);
    }
    operator bool() { return _handle != INVALID_HANDLE_VALUE; }
    operator HANDLE() { return _handle; }
    HANDLE handle() const { return _handle; }
  private:
    HANDLE _handle;
  };

  struct ScopedObj
  {
    typedef function<void()> Fn;
    ScopedObj(const Fn& fn) : fn(fn) {}
    ~ScopedObj() { fn(); }
    Fn fn;
  };

  // Macro for creating "local" names
#define GEN_NAME2(prefix, line) prefix##line
#define GEN_NAME(prefix, line) GEN_NAME2(prefix, line)
#define MAKE_SCOPED(type) type GEN_NAME(ANON, __LINE__)

#define SCOPED_CONC_CS(x) MAKE_SCOPED(Concurrency::critical_section::scoped_lock)(x);
#define SCOPED_OBJ(x) MAKE_SCOPED(ScopedObj)(x);
#define DEFER(x) MAKE_SCOPED(ScopedObj)(x);
#define SCOPED_CS(cs) ScopedCs lock(cs);

  // A macro to disallow the copy constructor and operator= functions
  // This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName)  \
  TypeName(const TypeName&) = delete;       \
  void operator=(const TypeName&) = delete; \


  template<class T> 
  void SeqDelete(T *seq)
  {
    for (auto it = begin(*seq); it != end(*seq); ++it)
      delete *it;
    seq->clear();
  }

  template<class T> 
  void AssocDelete(T* t)
  {
    for (T::iterator it = t->begin(); it != t->end(); ++i)
      delete it->second;
    t->clear();
  }

  template<class T>
  void ReleaseObj(T t)
  {
    if (t)
      t->Release();
  }

  template<class T>
  void DeleteObj(T t)
  {
    delete t;
  }

  template <class T>
  T exch_null(T &t)
  {
    T tmp = t;
    t = nullptr;
    return tmp;
  }
  template <class T>
  T exch(T &t, const T &v)
  {
    T tmp = t;
    t = v;
    return tmp;
  }

  template<typename T>
  T max3(const T &a, const T &b, const T &c) {
    return max(a, max(b, c));
  }

  template<typename T>
  T max4(const T &a, const T &b, const T &c, const T &d) {
    return max(a, max3(b, c, d));
  }


#define SAFE_RELEASE(x) if( (x) != 0 ) { (x)->Release(); (x) = 0; }
#define SAFE_FREE(x) if( (x) != 0 ) { free((void*)(x)); (x) = 0; }
#define SAFE_DELETE(x) if( (x) != 0 ) { delete (x); (x) = 0; }
#define SAFE_ADELETE(x) if( (x) != 0 ) { delete [] (x); (x) = 0; }

#define ELEMS_IN_ARRAY(x) sizeof(x) / sizeof((x)[0])

  template<typename Container, typename Key>
  bool contains(const Container &c, const Key key)
  {
    Container::const_iterator it = c.find(key);
    return it != c.end();
  }

  template<typename Key, typename Value>
  Value lookup_default(Key str, const unordered_map<Key, Value> &candidates, Value default_value)
  {
    auto it = candidates.find(str);
    return it != candidates.end() ? it->second : default_value;
  }

  template <typename T>
  T randf(T a, T b)
  {
    float t = (float)rand() / RAND_MAX;
    return lerp(a, b, t);
  }

  template <typename T, typename U>
  T lerp(T a, T b, U v)
  {
    return (1-v) * a + v * b;
  }

  template <typename T>
  T clamp(T minValue, T maxValue, T value)
  {
    return value < minValue ? minValue : value > maxValue ? maxValue : value;
  }

  float GaussianRand(float mean, float variance);
  string ToString(const char* format, ...);
  void DebugOutput(const char* fmt, ...);
}

