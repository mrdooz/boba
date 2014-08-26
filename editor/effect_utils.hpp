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

  template<typename T> struct KeyframeTraits {};

  template<> struct KeyframeTraits<float>
  {
    typedef float Value;
    typedef FloatAnim Anim;
    typedef FloatKeyframe Keyframe;
    typedef vector<Keyframe> Keyframes;
  };

  template<> struct KeyframeTraits<Vector2f>
  {
    typedef Vector2f Value;
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
      s64 time_ms,
      u32* idxLower,
      u32* idxUpper)
  {
    *idxLower = *idxUpper = 0xffffffff;

    if (time_ms <= keyframes.front().key.time)
    {
      *idxLower = 0;
      return false;
    }

    if (time_ms >= keyframes.back().key.time)
    {
      *idxLower = (u32)keyframes.size() - 1;
      return false;
    }

    // find upper idx
    int tmp = 0;
    while (time_ms > keyframes[tmp].key.time)
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
    s64 time_ms = t.total_milliseconds();

    if (keyframes.empty())
    {
      keyframes.push_back({time_ms, value});
      return &keyframes.back();
    }

    if (time_ms <= keyframes.front().key.time)
    {
      keyframes.insert(keyframes.begin(), {time_ms, value});

      return &keyframes.front();
    }
    else if (time_ms >= keyframes.back().key.time)
    {
      keyframes.push_back({time_ms, value});
      return &keyframes.back();
    }

    // Insert the new keyframe after the lower index
    u32 idxLower, idxUpper;
    FindKeyframePair<T>(keyframes, time_ms, &idxLower, &idxUpper);

    // check if the lower key has the same time as the current one, in which
    // case we'll just replace it
    if (!forceAdd && keyframes[idxLower].key.time == time_ms)
    {
      keyframes[idxLower].key.value = value;
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
  T Bezier(const T& p0, const T& p1, const T& p2, const T& p3, float t)
  {
    float t2 = t*t;
    float t3 = t2*t;

    float u = (1-t);
    float u2 = u*u;
    float u3 = u2*u;

    return u3 * p0 + 3 * u2 * t * p1 + 3 * u * t2 * p2 + t3 * p3;
  }

  //----------------------------------------------------------------------------------
  template <typename T>
  T Interpolate(const typename KeyframeTraits<T>::Anim& anim, const time_duration& t)
  {
    return Interpolate<T>(anim, t.total_milliseconds());
  }

  //----------------------------------------------------------------------------------
  template <typename T>
  T Interpolate(const typename KeyframeTraits<T>::Anim& anim, s64 time_ms)
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
      return keyframes[idxLower].key.value;
    }

    const Keyframe& lower = keyframes[idxLower];
    const Keyframe& upper = keyframes[idxUpper];

    float t = (time_ms - lower.key.time) / (float)(upper.key.time - lower.key.time);

    if (anim.type == 0 || keyframes.size() == 2)
    {
      // linear interpolation
      return lower.key.value + t * (upper.key.value - lower.key.value);
    }
    else
    {
      // bezier splines
      return Bezier(lower.key.value, lower.cpOut.value, upper.cpIn.value, upper.key.value, t);
    }

    return T();
  }

  //----------------------------------------------------------------------------------
  template<typename T>
  sf::Vector3<T> Min(const sf::Vector3<T>& a, const sf::Vector3<T>& b)
  {
    return sf::Vector3<T>(
        min(a.x, b.x),
        min(a.y, b.y),
        min(a.z, b.z)
    );
  }

  //----------------------------------------------------------------------------------
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