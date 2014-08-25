#pragma once
#include "protocol/effects_proto.hpp"

namespace editor
{
  //----------------------------------------------------------------------------------
  template <typename T>
  bool LoadProto(const char* filename, T* out, bool textFormat = true)
  {
#pragma warning(suppress: 4996)
    FILE* f = fopen(filename, "rb");
    if (!f)
      return false;

    fseek(f, 0, 2);
    size_t s = ftell(f);
    fseek(f, 0, 0);
    std::string str;
    str.resize(s);
    fread((char*)str.c_str(), 1, s, f);
    fclose(f);

    return textFormat
        ? google::protobuf::TextFormat::ParseFromString(str, out)
        : out->ParseFromString(str);
  }

  struct Effect
  {
    enum class Type
    {
      Plexus,
    };

    union {
      Plexus* plexus;
    } data;

  };

  template<typename T> struct KeyframeTraits {};

  template<> struct KeyframeTraits<float>
  {
    typedef float Value;
    typedef FloatAnim Anim;
    typedef FloatKeyframe Keyframe;
    typedef vector<Keyframe> Keyframes;
  };

  template<> struct KeyframeTraits<Vector3f>
  {
    typedef Vector3f Value;
    typedef Vector3Anim Anim;
    typedef Vector3Keyframe Keyframe;
    typedef vector<Keyframe> Keyframes;
  };

  //----------------------------------------------------------------------------------
  // find the keyframes that stradle the given time. return false is only a single
  // keyframe is found (ie before the first frame, or after the last one)
  template <typename T>
  bool FindKeyframePair(
      const typename KeyframeTraits<T>::Keyframes& keyframes,
      u32 time_ms,
      u32* idxLower,
      u32* idxUpper)
  {
    *idxLower = *idxUpper = 0xffffffff;

    if (time_ms <= keyframes.front().time)
    {
      *idxLower = 0;
      return false;
    }

    if (time_ms >= keyframes.back().time)
    {
      *idxLower = (u32)keyframes.size() - 1;
      return false;
    }

    // find upper idx
    int tmp = 0;
    while (time_ms > keyframes[tmp].time)
      ++tmp;

    *idxUpper = (u32)tmp;
    *idxLower = (u32)max(0, tmp-1);
    return true;
  }

  //----------------------------------------------------------------------------------
  template <typename T>
  typename KeyframeTraits<T>::Keyframe* AddKeyframe(
      const time_duration& t,
      const T& value,
      bool forceAdd,
      typename KeyframeTraits<T>::Anim* anim)
  {
    typedef typename KeyframeTraits<T>::Anim Anim;
    typedef typename KeyframeTraits<T>::Keyframe Keyframe;
    typedef typename KeyframeTraits<T>::Keyframes Keyframes;

    Keyframes& keyframes = anim->keyframe;
    s32 time_ms = t.total_milliseconds();

    if (keyframes.empty())
    {
      keyframes.push_back({time_ms, value});
      return &keyframes.back();
    }

    if (time_ms <= keyframes.front().time)
    {
      keyframes.insert(keyframes.begin(), {time_ms, value});

      return &keyframes.front();
    }
    else if (time_ms >= keyframes.back().time)
    {
      keyframes.push_back({time_ms, value});
      return &keyframes.back();
    }

    // Insert the new keyframe after the lower index
    u32 idxLower, idxUpper;
    FindKeyframePair<T>(keyframes, time_ms, &idxLower, &idxUpper);

    // check if the lower key has the same time as the current one, in which
    // case we'll just replace it
    if (!forceAdd && keyframes[idxLower].time == time_ms)
    {
      keyframes[idxLower].value = value;
      return &keyframes[idxLower];
    }
    else
    {
      u32 idx = idxUpper == 0xffffffff ? idxLower : idxUpper;
      auto it = keyframes.insert(keyframes.begin() + idx, {time_ms, value});
      return &(*it);
    }
  }

  //----------------------------------------------------------------------------------
  template <typename T>
  T Interpolate(const typename KeyframeTraits<T>::Anim& anim, const time_duration& t)
  {
    return Interpolate<T>(anim, t.total_milliseconds());
  }

//----------------------------------------------------------------------------------
  template <typename T>
  T Interpolate(const typename KeyframeTraits<T>::Anim& anim, u32 time_ms)
  {
    typedef typename KeyframeTraits<T>::Anim Anim;
    typedef typename KeyframeTraits<T>::Keyframe Keyframe;
    typedef typename KeyframeTraits<T>::Keyframes Keyframes;

    const Keyframes& keyframes = anim.keyframe;

    if (keyframes.empty())
      return T();

    u32 idxLower, idxUpper;
    if (!FindKeyframePair<T>(keyframes, time_ms, &idxLower, &idxUpper))
    {
      return keyframes[idxLower].value;
    }

    const Keyframe& lower = keyframes[idxLower];
    const Keyframe& upper = keyframes[idxUpper];

    float t = (time_ms - lower.time) / (float)(upper.time - lower.time);

    if (anim.type == 0 || keyframes.size() == 2)
    {
      // linear interpolation
      return lower.value + t * (upper.value - lower.value);
    }
    else
    {
      // hermite splines
    }

    return T();
  }

//  Vector3Keyframe* AddKeyframe(
//      const time_duration& t,
//      const Vector3f& value,
//      bool forceAdd,
//      Vector3Anim* anim);

  template<typename T>
  sf::Vector3<T> Min(const sf::Vector3<T>& a, const sf::Vector3<T>& b)
  {
    return sf::Vector3<T>(
        min(a.x, b.x),
        min(a.y, b.y),
        min(a.z, b.z)
    );
  }

  template<typename T>
  sf::Vector3<T> Max(const sf::Vector3<T>& a, const sf::Vector3<T>& b)
  {
    return sf::Vector3<T>(
        max(a.x, b.x),
        max(a.y, b.y),
        max(a.z, b.z)
    );
  }

}