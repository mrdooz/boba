#include "editor_windows.hpp"
#include "editor.hpp"
#include "proto_utils.hpp"

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
    , _selectedModule(nullptr)
    , _selectedEffect(nullptr)
    , _lastDragPos(-1, -1)
{
}

//----------------------------------------------------------------------------------
TimelineWindow::~TimelineWindow()
{
  SeqDelete(&_rows);
  SeqDelete(&_modules);
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

  const editor::Settings& settings = EDITOR.Settings();

  // create render textures and sprites
  _effectTexture.create(settings.module_view_width(), _size.y);
  _effectSprite.setTexture(_effectTexture.getTexture(), true);

  _timelineTexture.create(_size.x - settings.module_view_width(), _size.y);
  _timelineSprite.setTexture(_timelineTexture.getTexture(), true);
  _timelineSprite.setPosition(settings.module_view_width(), 0);

  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseButtonPressed(const Event& event)
{
  const editor::Settings& settings = EDITOR.Settings();

  int x = (int)(event.mouseButton.x - _pos.x);
  int y = (int)(event.mouseButton.y - _pos.y);
  Vector2i mousePos(x, y);

  if (x < (int)settings.module_view_width())
  {
    // effect select
    EffectRow* selectedRow = nullptr;

    for (EffectRow* row : _effectRows)
    {
      if (row->bounds.contains(mousePos))
      {
        selectedRow = row;
        break;
      }
    }

    if (selectedRow)
    {
      selectedRow->flags.Toggle(EffectRow::RowFlagsF::Selected);
    }
  }
  else if (y < (int)settings.module_row_height())
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
  const editor::Settings& settings = EDITOR.Settings();
  Vector2i posModule = PointToLocal<int>(event.mouseMove.x, event.mouseMove.y);
  Vector2i posTimeline(posModule.x - settings.module_view_width(), posModule.y);

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
  const editor::Settings& settings = EDITOR.Settings();

  if (event.mouseWheel.delta < 0)
    _pixelsPerSecond *= 2;
  else if (event.mouseWheel.delta > 0)
    _pixelsPerSecond /= 2;

  _pixelsPerSecond = Clamp(settings.timeline_zoom_min(), settings.timeline_zoom_max(), _pixelsPerSecond);

  return true;
}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawEffect(const EffectInstance& effect, const Row& row, Text& text)
{
  const editor::Settings& settings = EDITOR.Settings();
  Color hoverCol = FromProtocol(settings.hover_row_color());
  Color selectedCol = FromProtocol(settings.selected_row_color());

  int start = TimeToPixel(effect.startTime);
  int end = TimeToPixel(effect.endTime);

  int diff = (row.rect.height - settings.effect_height()) / 2;
  Vector2f pos(start, row.rect.top + diff);
  Vector2f size(end - start, settings.effect_height());
  DrawRectOutline(_timelineTexture, pos, size,
      effect.flags.IsSet(EffectInstanceFlagsF::Selected) ? selectedCol : hoverCol, 2, settings.resize_handle());

  text.setString(effect.module->name);
  Vector2f ofs(text.getLocalBounds().width, text.getLocalBounds().height);
  text.setPosition(pos + (size - ofs) / 2.0f);
  _timelineTexture.draw(text);

}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawTimeline()
{
  _timelineTexture.clear();

  const editor::Settings& settings = EDITOR.Settings();

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
    time_duration t = PixelToTime(x + settings.module_view_width());
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

  // draw the rows
  x = 0;
  y = settings.ticker_height();
  text.setCharacterSize(16);
  text.setFont(_font);
  for (const Row* row : _rows)
  {
    DrawRectOutline(_timelineTexture, Vector2f(row->rect.left, row->rect.top), 
        Vector2f(row->rect.width, row->rect.height), rowCol, 2);

    for (const EffectInstance* e : row->effects)
    {
      DrawEffect(*e, *row, text);
    }
  }

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

//----------------------------------------------------------------------------------
void TimelineWindow::DrawModule(float x, float y, const Module& module)
{
  const editor::Settings& settings = EDITOR.Settings();
  Color rowCol = FromProtocol(settings.default_row_color());
  Color selectedRowCol = FromProtocol(settings.selected_row_color());

  Vector2f size(module.rect.width, module.rect.height);
  Color col = module.flags.IsSet(ModuleFlagsF::Selected) ? selectedRowCol : rowCol;
  DrawRectOutline(_effectTexture, Vector2f(x,y), size, col, 2);
  Text text;
  text.setString(module.name);
  text.setCharacterSize(16);
  text.setFont(_font);
  Vector2f ofs(text.getLocalBounds().width, text.getLocalBounds().height);
  text.setPosition(Vector2f(x, y) + (size - ofs) / 2.0f);
  _effectTexture.draw(text);
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
void TimelineWindow::DrawPlexus(const Plexus& plexus)
{
  const editor::Settings& settings = EDITOR.Settings();
  Color rowCol = FromProtocol(settings.default_row_color());

  int fontSize = 16;
  int y = 100;

  Text text;
  text.setCharacterSize(fontSize);
  text.setFont(_font);

  RowDrawer row(text, fontSize + 1, _effectTexture, 10, &y);
  row.DrawText("PLEXUS");

  // draw paths
  for (const TextPath& p : plexus.textPaths)
  {
    row.DrawText(to_string(">> TEXT PATH: %s", p.text.c_str()).c_str());
  }

  // draw effectors
  for (const NoiseEffector& p : plexus.noiseEffectors)
  {
    row.DrawText(to_string(">> NOISE EFFECTOR (%s)", p.applyTo == NoiseEffector::ApplyTo::Scale ? "SCALE" : "POS").c_str());

    // calc the interpolated values
    Vector3f v = Interpolate(p.displacement, EDITOR.CurTime().total_milliseconds());
    row.DrawText(to_string("    X: %.2f", v.x).c_str());
    row.DrawText(to_string("    Y: %.2f", v.y).c_str());
    row.DrawText(to_string("    Z: %.2f", v.z).c_str());
  }

}

//----------------------------------------------------------------------------------
void TimelineWindow::EffectRow::Draw(RenderTexture& texture)
{
  const editor::Settings& settings = EDITOR.Settings();
  Color rowCol = FromProtocol(settings.default_row_color());

  RectangleShape rect;
  rect.setFillColor(rowCol);
  rect.setPosition(bounds.left, bounds.top);
  rect.setSize(Vector2f(bounds.width, bounds.height));
  texture.draw(rect);
}

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

  static bool hax = false;
  if (!hax)
  {
    EffectRow* row = new EffectRow();
    row->text = "magnus";
    row->bounds = IntRect(0, 0, 100, 20);
    _effectRows.push_back(row);
    hax = true;
  }

  for (EffectRow* row : _effectRows)
  {
    row->Draw(_effectTexture);
  }

  DrawPlexus(EDITOR._plexus);

  for (const Module* module : _modules)
  {
    DrawModule(module->rect.left, module->rect.top, *module);
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

  x -= EDITOR.Settings().module_view_width();
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
TimelineWindow::Row::~Row()
{
  SeqDelete(&effects);
}
