#include "editor_windows.hpp"
#include "editor.hpp"

#include "protocol/effects_proto.hpp"

using namespace editor;
using namespace bristol;

#pragma warning(disable: 4244)

//----------------------------------------------------------------------------------
PropertyWindow::PropertyWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size)
    : VirtualWindow(title, pos, size, bristol::WindowFlags(bristol::WindowFlag::StaticWindow))
{
}

//----------------------------------------------------------------------------------
PreviewWindow::PreviewWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size)
    : VirtualWindow(title, pos, size, bristol::WindowFlags(bristol::WindowFlag::StaticWindow))
{
}

//----------------------------------------------------------------------------------
TimelineWindow::TimelineWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size)
    : VirtualWindow(title, pos, size, bristol::WindowFlags(bristol::WindowFlag::StaticWindow))
    , _panelOffset(seconds(0))
    , _pixelsPerSecond(EDITOR.Settings().timeline_zoom_default())
    , _lastDragPos(-1, -1)
{
}

//----------------------------------------------------------------------------------
TimelineWindow::~TimelineWindow()
{
}

//----------------------------------------------------------------------------------
bool TimelineWindow::Init()
{
  if (!VirtualWindow::Init())
    return false;

  _windowManager->RegisterHandler(Event::MouseButtonPressed, nullptr, bind(&TimelineWindow::OnMouseButtonPressed, this, _1));
  _windowManager->RegisterHandler(Event::MouseMoved, nullptr, bind(&TimelineWindow::OnMouseMoved, this, _1));
  _windowManager->RegisterHandler(Event::MouseButtonReleased, nullptr, bind(&TimelineWindow::OnMouseButtonReleased, this, _1));
  _windowManager->RegisterHandler(Event::MouseWheelMoved, nullptr, bind(&TimelineWindow::OnMouseWheelMoved, this, _1));

  if (!_font.loadFromFile(EDITOR.GetAppRoot() + "gfx/coders_crux.ttf"))
    return false;

  const editor::protocol::Settings& settings = EDITOR.Settings();
  int width = settings.effect_view_width();
  int height = 20;

  // create render textures and sprites
  _effectTexture.create(settings.effect_view_width(), _size.y);
  _effectSprite.setTexture(_effectTexture.getTexture(), true);

  _timelineTexture.create(_size.x - settings.effect_view_width(), _size.y);
  _timelineSprite.setTexture(_timelineTexture.getTexture(), true);
  _timelineSprite.setPosition(settings.effect_view_width(), 0);

  int rowHeight = settings.effect_row_height();
  int curY = rowHeight;

  // create the effect rows
  const Plexus& p = EDITOR._plexus;

  for (int i = 0; i < 5; ++i)
  {
    EffectRow* parent = new EffectRow(_font, "PLEXUS", IntRect(0, curY, width, rowHeight));
    _effectRows.push_back(parent);
    curY += rowHeight;

    for (const TextPath& t : p.textPaths)
    {
      string str = to_string("TextPath: %s", t.text.c_str());
      parent->children.push_back(
        new EffectRow(_font, str, IntRect(0, curY, width, rowHeight), parent));
      curY += rowHeight;
    }

    for (const NoiseEffector& e : p.noiseEffectors)
    {
      string str = to_string("Noise (%s)",
        e.applyTo == NoiseEffector::ApplyTo::Position ? "POS" : "SCALE");
      parent->children.push_back(
        new EffectRow(_font, str, IntRect(0, curY, width, rowHeight), parent));
      curY += rowHeight;
    }
  }

  RecalcEffecRows();

  return true;
}

//----------------------------------------------------------------------------------
void TimelineWindow::RecalcEffecRows()
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float rowHeight = settings.effect_row_height();

  float curY = rowHeight;
  for (EffectRow* row : _effectRows)
  {
    EffectRow::Reposition(row, curY, rowHeight);
    curY += EffectRow::RowHeight(row, rowHeight);
  }
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseButtonPressed(const Event& event)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();

  int x = (int)(event.mouseButton.x - _pos.x);
  int y = (int)(event.mouseButton.y - _pos.y);
  Vector2f mousePos(x, y);

  if (x < (int)settings.effect_view_width())
  {
    // effect select
    EffectRow* selectedRow = nullptr;

    vector<EffectRow*> effects;
    for (EffectRow* row : _effectRows)
      EffectRow::Flatten(row, &effects);

    for (EffectRow* row : effects)
    {
      if (row->rect->_shape.getGlobalBounds().contains(mousePos))
      {
        row->flags.Toggle(EffectRow::RowFlagsF::Expanded);
        selectedRow = row;
        break;
      }
    }

    if (selectedRow)
    {
      selectedRow->flags.Toggle(EffectRow::RowFlagsF::Selected);
      RecalcEffecRows();
    }
  }
  else if (y < (int)settings.effect_row_height())
  {
    time_duration t = PixelToTime(event.mouseButton.x - (int)_pos.x);
    if (!t.is_not_a_date_time())
    {
      EDITOR.SetCurTime(t);
    }
  }
  else
  {
    // check for keyframe intersection
  }
  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseMoved(const Event& event)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  Vector2i posModule = PointToLocal<int>(event.mouseMove.x, event.mouseMove.y);
  Vector2i posTimeline(posModule.x - settings.effect_view_width(), posModule.y);

  time_duration curTime = PixelToTime(posModule.x);

  // timeline scrolling has priority
  if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
  {
    if (_lastDragPos.x == -1)
    {
      _lastDragPos = Vector2i(event.mouseMove.x, event.mouseMove.y);
      return true;
    }
    int delta = event.mouseMove.x - _lastDragPos.x;
    time_duration d = AbsPixelToTime(delta);
    if (_panelOffset + d < seconds(0))
      _panelOffset = seconds(0);
    else 
    _panelOffset += AbsPixelToTime(delta);

    _lastDragPos = Vector2i(event.mouseMove.x, event.mouseMove.y);
    return true;
  }

  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseButtonReleased(const Event& event)
{
  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseWheelMoved(const Event& event)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();

  if (event.mouseWheel.delta < 0)
    _pixelsPerSecond *= 2;
  else if (event.mouseWheel.delta > 0)
    _pixelsPerSecond /= 2;

  _pixelsPerSecond = Clamp(settings.timeline_zoom_min(), settings.timeline_zoom_max(), _pixelsPerSecond);

  return true;
}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawTimeline()
{
  _timelineTexture.clear();

  const editor::protocol::Settings& settings = EDITOR.Settings();

  Color rowCol = FromProtocol(settings.default_row_color());
  time_duration curTime = EDITOR.CurTime();

  // draw the ticker
  RectangleShape ticker;
  ticker.setFillColor(rowCol);
  ticker.setSize(Vector2f(_size.x, settings.ticker_height()));
  _timelineTexture.draw(ticker);

  VertexArray lines(sf::Lines);
  int x = 0;
  int y = settings.ticker_height() - 25;
  int minorInc = settings.ticker_interval() / settings.ticks_per_interval();
  Text text;
  text.setFont(_font);
  text.setCharacterSize(16);
  while (x < _size.x)
  {
    // need to cheese the 'x' value to make it relative the whole window, and not just the
    // timline part
    time_duration t = PixelToTime(x + settings.effect_view_width());
    text.setString(to_string("%.1f", t.total_milliseconds() / 1000.0f));
    text.setPosition(x, y-20);
    _timelineTexture.draw(text);
    lines.append(sf::Vertex(Vector2f(x, y + 0)));
    lines.append(sf::Vertex(Vector2f(x, y + 20)));
    int tmpX = x;
    for (int i = 0; i < (int)settings.ticks_per_interval(); ++i)
    {
      lines.append(sf::Vertex(Vector2f(x + i * minorInc, y + 5)));
      lines.append(sf::Vertex(Vector2f(x + i * minorInc, y + 15)));
    }
    x = tmpX + settings.ticker_interval();
  }
  _timelineTexture.draw(lines);

  // draw time line
  VertexArray curLine(sf::Lines);
  int w = _size.x;
  y = settings.ticker_height() - 25;
  while (TimeToPixel(curTime - _panelOffset) > w)
  {
    _panelOffset += PixelToTime(w);
  }

  x = TimeToPixel(EDITOR.CurTime());
  curLine.append(sf::Vertex(Vector2f(x, y), Color::Red));
  curLine.append(sf::Vertex(Vector2f(x, _size.y), Color::Red));
  _timelineTexture.draw(curLine);

  _timelineTexture.display();
  _texture.draw(_timelineSprite);
}

struct RowDrawer
{
  RowDrawer(Text& text, int rowHeight, RenderTexture& texture, int x, int* y) 
    : text(text)
    , rowHeight(rowHeight)
    , texture(texture)
    , x(x)
    , y(y)
  {}

  void DrawText(const char* str)
  {
    text.setString(str);
    text.setPosition(10, *y);
    texture.draw(text);
    *y += rowHeight + 1;
  }

  Text& text;
  int rowHeight;
  RenderTexture& texture;
  int x;
  int* y;
};


//----------------------------------------------------------------------------------
void TimelineWindow::DrawEffects()
{
  _effectTexture.clear();

  // draw the header
  time_duration t = EDITOR.CurTime();
  Text text(to_string("TIME: %.3f, OFS: %.3f, SCALE: %.3f",
    t.total_milliseconds() / 1000.0f,
    _panelOffset.total_milliseconds() / 1000.0f,
    _pixelsPerSecond / 100.0f), _font);
  text.setPosition(10, 0);
  text.setCharacterSize(14);
  _effectTexture.draw(text);

  for (EffectRow* row : _effectRows)
  {
    row->Draw(_effectTexture);
  }

  _effectTexture.display();
  _texture.draw(_effectSprite);
}

//----------------------------------------------------------------------------------
int TimelineWindow::TimeToPixel(const time_duration& t)
{
  // p = s * (t + a) + m
  // t = (p - m) / s - a
  double s = (double)_pixelsPerSecond / 1000.0;
  return s * (t.total_milliseconds() - _panelOffset.total_milliseconds());
}

//----------------------------------------------------------------------------------
time_duration TimelineWindow::PixelToTime(int x)
{
  double s = (double)_pixelsPerSecond / 1000.0;

  x -= EDITOR.Settings().effect_view_width();
  if (x < 0)
    return time_duration(boost::posix_time::not_a_date_time);

  return milliseconds(x / s) + _panelOffset;
}

//----------------------------------------------------------------------------------
time_duration TimelineWindow::AbsPixelToTime(int x)
{
  return milliseconds(1000 * x / (int)_pixelsPerSecond);
}

//----------------------------------------------------------------------------------
void TimelineWindow::Draw()
{
  _texture.clear();

  DrawEffects();
  DrawTimeline();

  _texture.display();
}

//----------------------------------------------------------------------------------
TimelineWindow::EffectRow::EffectRow(
    const Font& font,
    const string& str,
    const IntRect& bounds,
    EffectRow* parent)
    : str(str)
    , parent(parent)
    , level(0)
{
  rect = STYLE_FACTORY.CreateStyledRectangle("default_row_color");
  rect->_shape.setPosition(bounds.left, bounds.top);
  rect->_shape.setSize(Vector2f(bounds.width, bounds.height));
  text.setFont(font);
  text.setCharacterSize(16);
  flags.Set(EffectRow::RowFlagsF::Expanded);

  if (parent)
    level = parent->level + 1;
}

//----------------------------------------------------------------------------------
void TimelineWindow::EffectRow::Flatten(EffectRow* cur, vector<TimelineWindow::EffectRow*>* res)
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
void TimelineWindow::EffectRow::Reposition(EffectRow* cur, float curY, float rowHeight)
{
  deque<EffectRow*> q({ cur });
  while (!q.empty())
  {
    EffectRow* cur = q.front();
    q.pop_front();

    const Vector2f& pos = cur->rect->_shape.getPosition();
    const Vector2f& size = cur->rect->_shape.getSize();
    cur->rect->_shape.setPosition(pos.x, curY);
    cur->rect->_shape.setSize(Vector2f(size.x, rowHeight));
    curY += rowHeight;
    if (cur->flags.IsSet(EffectRow::RowFlagsF::Expanded))
    {
      for (EffectRow* c : cur->children)
        q.push_back(c);
    }
  }
}

//----------------------------------------------------------------------------------
float TimelineWindow::EffectRow::RowHeight(EffectRow* cur, float rowHeight)
{
  float res = 0;
  deque<EffectRow*> q({ cur });
  while (!q.empty())
  {
    EffectRow* cur = q.front();
    q.pop_front();

    res += rowHeight;
    if (cur->flags.IsSet(EffectRow::RowFlagsF::Expanded))
    {
      for (EffectRow* c : cur->children)
        q.push_back(c);
    }
  }

  return res;
}

//----------------------------------------------------------------------------------
void TimelineWindow::EffectRow::Draw(RenderTexture& texture)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  Color rowCol = FromProtocol(settings.default_row_color());

  // draw background
  texture.draw(rect->_shape);

  // draw text
  text.setString(str);
  text.setPosition(rect->_shape.getPosition() + Vector2f(20 + level * 15, 0) );
  texture.draw(text);

  // draw expanded indicator
  VertexArray triangle;

  VertexArray tri(sf::Triangles);
  float left = rect->_shape.getPosition().x;
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

    for (EffectRow* child : children)
    {
      child->Draw(texture);
    }
  }
}
