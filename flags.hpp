#pragma once

// pretty much stolen from http://molecularmusings.wordpress.com/2011/08/23/flags-on-steroids/

namespace boba
{
  // input is a struct of the form
  // struct Mode { enum Enum { type1, type2 }; struct Bits { u32 type1 : 1; u32 type2 : 2 } }
  template <typename F>
  struct Flags
  {

    typedef typename F::Enum Enum;
    typedef typename F::Bits Bits;

    Flags() : _value(0) {}
    explicit Flags(Enum f) : _value(f) {}

    bool IsSet(Enum f) const
    {
      return !!(_value & f);
    }

    void Set(Enum f)
    {
      _value |= f;
    }

    void Clear(Enum f)
    {
      _value &= ~f;
    }

    void Reset()
    {
      _value = 0;
    }

    Flags& operator|=(Enum f)
    {
      _value |= f;
      return *this;
    }

    Flags operator|(Enum f)
    {
      return Flags(_value | f);
    }

    Flags operator|(Flags rhs)
    {
      return Flags(_value | rhs._value);
    }

    union
    {
      u32 _value;
      Bits _bits;
    };
  };
}
