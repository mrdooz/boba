#include "effect_row.hpp"
#include "editor_windows.hpp"
#include "editor.hpp"

using namespace editor;
using namespace bristol;

//----------------------------------------------------------------------------------
EffectRow::EffectRow(
    const Font& font,
    const string& str,
    EffectRow* parent)
    : str(str)
    , parent(parent)
    , level(0)
{
  rect = STYLE_FACTORY.CreateStyledRectangle("default_row_color");
  keyframeRect = STYLE_FACTORY.CreateStyledRectangle("keyframe_style");
  keyframeRect->_shape.setSize(Vector2f(6, 6));

  text.setFont(font);
  text.setCharacterSize(16);
  flags.Set(EffectRow::RowFlagsF::Expanded);

  if (parent)
    level = parent->level + 1;
}

//----------------------------------------------------------------------------------
void EffectRow::Flatten(EffectRow* cur, vector<EffectRow*>* res)
{
  deque<EffectRow*> q({cur});
  while (!q.empty())
  {
    EffectRow* cur = q.front();
    q.pop_front();

    res->push_back(cur);
    for (EffectRow* c : cur->children)
      q.push_back(c);
  }
}

//----------------------------------------------------------------------------------
void EffectRow::Reposition(EffectRow* cur, float curY, float rowHeight)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float width = TimelineWindow::_instance->_size.x; // settings.effect_view_width();

  deque<EffectRow*> q({ cur });
  while (!q.empty())
  {
    EffectRow* cur = q.front();
    q.pop_front();

    cur->rect->_shape.setPosition(0, curY);
    cur->rect->_shape.setSize(Vector2f(width, (1 + cur->NumVars()) * rowHeight));
    curY += rowHeight;
    if (cur->flags.IsSet(EffectRow::RowFlagsF::Expanded))
    {
      for (EffectRow* c : cur->children)
        q.push_back(c);
    }

    const RectangleShape& shape = cur->rect->_shape;

    cur->expandRect = FloatRect(
        cur->level * 15 + shape.getPosition().x,
        shape.getPosition().y,
        15,
        15);

    u32 numVars = cur->NumVars();
    if (numVars > 0)
    {
      float x = cur->level * 15 + shape.getPosition().x;
      cur->varEditRect = FloatRect(
          x,
          shape.getPosition().y + 1 * rowHeight,
          width - x,
          numVars * rowHeight);
    }
  }
}

//----------------------------------------------------------------------------------
float EffectRow::RowHeight(EffectRow* cur, float rowHeight)
{
  float res = 0;
  deque<EffectRow*> q({ cur });
  while (!q.empty())
  {
    EffectRow* cur = q.front();
    q.pop_front();

    if (cur->flags.IsSet(EffectRow::RowFlagsF::Expanded))
    {
      res += (1 + cur->NumVars()) * rowHeight;
      for (EffectRow* c : cur->children)
        q.push_back(c);
    }
    else
    {
      res += rowHeight;
    }
  }

  return res;
}

//----------------------------------------------------------------------------------
void EffectRow::Draw(RenderTexture& texture, bool drawKeyframes)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  Color rowCol = FromProtocol(flags.IsSet(EffectRow::RowFlagsF::Selected)
      ? settings.selected_row_color()
      : settings.default_row_color());

  // draw background
  rect->_shape.setFillColor(rowCol);
  texture.draw(rect->_shape);

  // draw text
  text.setString(str);
  text.setPosition(rect->_shape.getPosition() + Vector2f(20 + level * 15, 0) );
  texture.draw(text);

  // draw expanded indicator
  VertexArray tri(sf::Triangles);
  float left = level * 15 + rect->_shape.getPosition().x;
  float top = rect->_shape.getPosition().y;
  float y = top;

  if (!flags.IsSet(EffectRow::RowFlagsF::Expanded))
  {
    // draw expanded indicator

    // y increases downwards
    const Style* s = STYLE_FACTORY.GetStyle("effect_icon_collapsed_color");
    tri.append(sf::Vertex(Vector2f(left+5, y+5), s->fillColor));
    tri.append(sf::Vertex(Vector2f(left+5, y+15), s->fillColor));
    tri.append(sf::Vertex(Vector2f(left+15, y+10), s->fillColor));
    texture.draw(tri);
  }
  else
  {
    // y increases downwards
    const Style* s = STYLE_FACTORY.GetStyle("effect_icon_expanded_color");
    tri.append(sf::Vertex(Vector2f(left+5, y+5), s->fillColor));
    tri.append(sf::Vertex(Vector2f(left+15, y+5), s->fillColor));
    tri.append(sf::Vertex(Vector2f(left+10, y+15), s->fillColor));
    texture.draw(tri);

    DrawVars(texture, TimelineWindow::_instance->_size, drawKeyframes);

    for (EffectRow* child : children)
    {
      child->Draw(texture, drawKeyframes);
    }
  }
}

//----------------------------------------------------------------------------------
EffectRowNoise::EffectRowNoise(
    const Font& font,
    const string& str,
    EffectRow* parent)
    : EffectRow(font, str, parent)
    , editingIdx(-1)
    , selectedKeyframe(nullptr)
{
}

//----------------------------------------------------------------------------------
void EffectRowNoise::DrawVars(RenderTexture& texture, const Vector2f& size, bool drawKeyframes)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float h = settings.effect_row_height();

  Vector3f v = Interpolate(effector.displacement, EDITOR.CurTime().total_milliseconds());

  const Style* editingStyle = STYLE_FACTORY.GetStyle("var_text_editing");
  const Style* normalStyle = STYLE_FACTORY.GetStyle("var_text_normal");

  static const char* suffix[] = {
      "x: ",
      "y: ",
      "z: "
  };

  const auto& fnStyle = [this, editingStyle, normalStyle, v](int curIdx){
      if (curIdx == editingIdx)
      {
        text.setString(to_string("%s%s", suffix[curIdx], curEdit.c_str()).c_str());
        text.setColor(editingStyle->fillColor);
        text.setStyle(editingStyle->fontStyle);
      }
      else
      {
        text.setString(to_string("%s%.2f", suffix[curIdx], *(&v.x + curIdx)).c_str());
        text.setColor(normalStyle->fillColor);
        text.setStyle(normalStyle->fontStyle);
      }
  };

  fnStyle(0);
  text.setPosition(rect->_shape.getPosition() + Vector2f(20 + level * 15, h*1));
  texture.draw(text);

  fnStyle(1);
  text.setPosition(rect->_shape.getPosition() + Vector2f(20 + level * 15, h*2));
  texture.draw(text);

  fnStyle(2);
  text.setPosition(rect->_shape.getPosition() + Vector2f(20 + level * 15, h*3));
  texture.draw(text);

  if (drawKeyframes)
  {
    DrawKeyframes(texture, size);
  }

  // Reset style
  fnStyle(-2);
}

//----------------------------------------------------------------------------------
void EffectRowNoise::DrawKeyframes(RenderTexture& texture, const Vector2f& size)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float h = settings.effect_row_height();

  // draw the keyframes
  VertexArray curLine(sf::Lines);
  int w = size.x - settings.effect_view_width();
  int x = settings.effect_view_width();
  float shapeY = rect->_shape.getPosition().y;

  for (int i = 0; i < 3; ++i)
  {
    int y = shapeY + h * (i+1.5f);
    curLine.append(sf::Vertex(Vector2f(x, y), Color::White));
    curLine.append(sf::Vertex(Vector2f(x + w, y), Color::White));
  }
  texture.draw(curLine);

  for (u32 i = 0; i < effector.displacement.keyframes.size(); ++i)
  {
    const Vector3Keyframe& keyframe = effector.displacement.keyframes[i];

    ApplyStyle(STYLE_FACTORY.GetStyle(&keyframe == selectedKeyframe ?
        "keyframe_style_selected" : "keyframe_style"), &keyframeRect->_shape);

    int x = TimelineWindow::_instance->TimeToPixel(milliseconds(keyframe.time));
    if (x < size.x)
    {
      // draw for each var
      for (u32 j = 0; j < 3; ++j)
      {
        int y = shapeY + h * (j+1.5f) - 3;
        keyframeRect->_shape.setPosition(x - 3, y);
        texture.draw(keyframeRect->_shape);
      }
    }
  }
}

//----------------------------------------------------------------------------------
void EffectRowNoise::DrawGraph(RenderTexture& texture, const Vector2f& size)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float topY = settings.ticker_height();
  float ofs = settings.effect_view_width();
  float w = size.x - ofs;
  float h = size.y - topY;
  float bottom = size.y - 1;

  TimelineWindow* timeline = TimelineWindow::_instance;

  // find min/max values for keyframes within the view
  time_duration minTime = timeline->PixelToTime(0);
  time_duration maxTime = timeline->PixelToTime(size.x);

  Vector3f value0 = Interpolate(effector.displacement, minTime);
  Vector3f valueLast = Interpolate(effector.displacement, maxTime);

  Vector3f minValue = Min(
      Interpolate(effector.displacement, minTime),
      Interpolate(effector.displacement, maxTime));

  Vector3f maxValue = Max(
      Interpolate(effector.displacement, minTime),
      Interpolate(effector.displacement, maxTime));

  vector<const Vector3Keyframe*> keyframes;

  for (const Vector3Keyframe& keyframe : effector.displacement.keyframes)
  {
    if (keyframe.time < minTime.total_milliseconds())
      continue;

    if (keyframe.time > maxTime.total_milliseconds())
      break;

    keyframes.push_back(&keyframe);

    minValue = Min(minValue, keyframe.value);
    maxValue = Max(maxValue, keyframe.value);
  }

  minValue -= 0.1f * minValue;
  maxValue += 0.1f * maxValue;

  Vector3f span = maxValue - minValue;

  const auto& fnRescale = [&](const Vector3f& value){

      // scale the value so min/max covers the entire height
      Vector3f t = value - minValue;
      switch (graphMode)
      {
        case 1: return bottom - h * t.x / span.x;
        case 2: return bottom - h * t.y / span.y;
        case 3: return bottom - h * t.z / span.z;
        default: return 0.f;
      }
  };

  // draw the keyframes normalized to the min/max values
  VertexArray curLine(sf::LinesStrip);

  curLine.append(sf::Vertex(Vector2f(ofs, fnRescale(value0))));

  for (const Vector3Keyframe* keyframe : keyframes)
  {
    Vector2f p = Vector2f(
        timeline->TimeToPixel(milliseconds(keyframe->time)),
        fnRescale(keyframe->value));

    curLine.append(sf::Vertex(p));

    keyframeRect->_shape.setPosition(p.x - 3, p.y - 3);
    texture.draw(keyframeRect->_shape);
  }

  curLine.append(sf::Vertex(Vector2f(size.x, fnRescale(valueLast))));

  texture.draw(curLine);

}

//----------------------------------------------------------------------------------
u32 EffectRowNoise::NumVars()
{
  return 3;
}

//----------------------------------------------------------------------------------
void EffectRowNoise::BeginEditVars(float x, float y)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  int h = settings.effect_row_height();
  prevValue = effector.displacement.keyframes[0].value;

  editingIdx = (y - rect->_shape.getPosition().y - h) / h;
  assert(editingIdx >= 0 && editingIdx <= 2);

  Vector3f v = Interpolate(effector.displacement, EDITOR.CurTime());
  switch (editingIdx)
  {
    case 0: curEdit = to_string("%.2f", v.x); break;
    case 1: curEdit = to_string("%.2f", v.y); break;
    case 2: curEdit = to_string("%.2f", v.z); break;
  }
}

//----------------------------------------------------------------------------------
void EffectRowNoise::EndEditVars(bool commit)
{
  if (commit)
  {
    // create a new keyframe at the current position (if one doesn't exist)
    Vector3f v = prevValue;
    switch (editingIdx)
    {
      case 0: v.x = (float)atof(curEdit.c_str()); break;
      case 1: v.y = (float)atof(curEdit.c_str()); break;
      case 2: v.z = (float)atof(curEdit.c_str()); break;
    }

    AddKeyframe(EDITOR.CurTime(), v, false, &effector.displacement);
  }

  editingIdx = -1;
}

//----------------------------------------------------------------------------------
void EffectRowNoise::UpdateEditVar(Keyboard::Key key)
{
  if (key >= Keyboard::Key::Num0 && key <= Keyboard::Key::Num9)
  {
    curEdit += '0' + key - Keyboard::Key::Num0;
  }
  else if (key == Keyboard::Key::BackSpace)
  {
    if (curEdit.size() > 0)
    {
      curEdit.pop_back();
    }
  }
  else if (key == Keyboard::Key::Period && curEdit.find('.') == curEdit.npos)
  {
    curEdit += '.';
  }
}

//----------------------------------------------------------------------------------
bool EffectRowNoise::KeyframeIntersect(const Vector2f& pt, const Vector2f& size)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();

  int w = size.x - settings.effect_view_width();
  int x = settings.effect_view_width();
  float shapeY = rect->_shape.getPosition().y;
  float h = settings.effect_row_height();

  for (u32 i = 0; i < effector.displacement.keyframes.size(); ++i)
  {
    Vector3Keyframe& keyframe = effector.displacement.keyframes[i];
    int x = TimelineWindow::_instance->TimeToPixel(milliseconds(keyframe.time));
    if (x < size.x)
    {
      // draw for each var
      for (u32 j = 0; j < 3; ++j)
      {
        int y = shapeY + h * (j+1.5f) - 3;
        FloatRect rect(x, y, 6, 6);
        if (rect.contains(pt))
        {
          selectedKeyframe = &keyframe;
          oldKeyframe = keyframe;
          return true;
        }
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------------
void EffectRowNoise::UpdateKeyframe(const time_duration &t)
{
  if (!selectedKeyframe)
    return;

  selectedKeyframe->time = t.total_milliseconds();
}

//----------------------------------------------------------------------------------
void EffectRowNoise::BeginKeyframeUpdate(bool copy)
{
  copyingKeyframe = copy;
  if (copyingKeyframe)
  {
    // insert a copy of the existing keyframe
    selectedKeyframe = AddKeyframe(
        milliseconds(selectedKeyframe->time),
        selectedKeyframe->value,
        true,
        &effector.displacement);
  }
}

//----------------------------------------------------------------------------------
void EffectRowNoise::EndKeyframeUpdate(bool commit)
{
  if (!commit)
  {
    if (copyingKeyframe)
    {
      // delete the tmp keyframe
      selectedKeyframe = nullptr;
    }
    else
    {
      // revert the old keyframe
      *selectedKeyframe = oldKeyframe;
    }
  }
}


//----------------------------------------------------------------------------------
void EffectRowNoise::DeselectKeyframe()
{
  selectedKeyframe = nullptr;
}

//----------------------------------------------------------------------------------
void EffectRowNoise::DeleteKeyframe()
{
  if (!selectedKeyframe)
    return;
}

//----------------------------------------------------------------------------------
bool EffectRowNoise::ToggleDisplayMode()
{
  // toggle between keyframe, x graph, y graph, z graph
  graphMode = (graphMode + 1) % 4;

  // switching mode if going from graph 3 -> keyframe, or keyframe -> graph 1
  return graphMode == 0 || graphMode == 1;
}

//----------------------------------------------------------------------------------
void EffectRowNoise::GraphMouseMove(const Event& event)
{

}
