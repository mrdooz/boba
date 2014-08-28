#include "effect_utils.hpp"

namespace editor
{

  //----------------------------------------------------------------------------------
  void DrawRow(RenderTexture& texture, float x, float y, float w, float h, const Color& color)
  {
    VertexArray curLine(sf::Lines);
    curLine.append(Vertex(Vector2f(x, y), color));
    curLine.append(Vertex(Vector2f(w, y), color));
    curLine.append(Vertex(Vector2f(x, y+h), color));
    curLine.append(Vertex(Vector2f(w, y+h), color));
    texture.draw(curLine);
  }


#if 0
  //----------------------------------------------------------------------------------
  Vector3Keyframe* AddKeyframe(
      const time_duration& t,
      const Vector3f& value,
      bool forceAdd,
      Vector3Anim* anim)
  {
    vector<Vector3Keyframe>& keyframes = anim->keyframe;
    u32 time_ms = t.total_milliseconds();

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
  Vector3f Interpolate(const Vector3Anim& anim, const time_duration& t)
  {
    return Interpolate(anim, t.total_milliseconds());
  }

//----------------------------------------------------------------------------------
  Vector3f Interpolate(const Vector3Anim& anim, u32 time_ms)
  {
    const vector<Vector3Keyframe>& keyframes = anim.keyframe;

    if (keyframes.empty())
      return Vector3f(0,0,0);

    u32 idxLower, idxUpper;
    if (!FindKeyframePair(keyframes, time_ms, &idxLower, &idxUpper))
    {
      return keyframes[idxLower].value;
    }

    const Vector3Keyframe& lower = keyframes[idxLower];
    const Vector3Keyframe& upper = keyframes[idxUpper];

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

    return Vector3f(0,0,0);
  }

  #endif
}
