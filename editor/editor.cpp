#include "editor.hpp"
#include "window_event_manager.hpp"
#include "virtual_window_manager.hpp"
#include "editor_windows.hpp"

using namespace editor;

//----------------------------------------------------------------------------------
Editor::Editor()
    : _renderWindow(nullptr)
    , _eventManager(nullptr)
    , _virtualWindowManager(nullptr)
    , _done(false)
{
}

//----------------------------------------------------------------------------------
Editor::~Editor()
{
  delete _renderWindow;
  delete _eventManager;
  delete _virtualWindowManager;
}

//----------------------------------------------------------------------------------
bool Editor::Init()
{
  size_t width, height;
#ifdef _WIN32
  width = GetSystemMetrics(SM_CXFULLSCREEN);
  height = GetSystemMetrics(SM_CYFULLSCREEN);
#else
  auto displayId = CGMainDisplayID();
  width = CGDisplayPixelsWide(displayId);
  height = CGDisplayPixelsHigh(displayId);
#endif

  sf::ContextSettings settings;
  _renderWindow = new RenderWindow(sf::VideoMode(width, height), "...", sf::Style::Default, settings);
  _renderWindow->setVerticalSyncEnabled(true);
  _eventManager = new WindowEventManager(_renderWindow);

  _eventManager->RegisterHandler(Event::KeyPressed, bind(&Editor::OnKeyPressed, this, _1));
  _eventManager->RegisterHandler(Event::KeyReleased, bind(&Editor::OnKeyReleased, this, _1));
  _eventManager->RegisterHandler(Event::LostFocus, bind(&Editor::OnLostFocus, this, _1));
  _eventManager->RegisterHandler(Event::GainedFocus, bind(&Editor::OnGainedFocus, this, _1));
  _eventManager->RegisterHandler(Event::MouseButtonReleased, bind(&Editor::OnMouseButtonReleased, this, _1));

  // add the editor windows
  {
    _virtualWindowManager = new VirtualWindowManager(_renderWindow, _eventManager);

    float w = (float)width/4;
    float rw = (float)width - w;
    float h = (float)height/2;

    _virtualWindowManager->AddWindow(new GridSplitter(
        new PropertyWindow("PROPERTIES",   Vector2f(0, 0), Vector2f(w, h)),
        new PreviewWindow("PREVIEW",       Vector2f(w, 0), Vector2f(rw, h)),
        new ComponentWindow("COMPONENTS",  Vector2f(0, h), Vector2f(w, h)),
        new TimelineWindow("TIMELINE",     Vector2f(w, h), Vector2f(rw, h)),
        Vector2f(width, height)));
  }

  return true;
}

//----------------------------------------------------------------------------------
bool Editor::OnLostFocus(const Event& event)
{
  return false;
}

//----------------------------------------------------------------------------------
bool Editor::OnGainedFocus(const Event& event)
{
  return false;
}

//----------------------------------------------------------------------------------
bool Editor::OnKeyPressed(const Event& event)
{
  Keyboard::Key key = event.key.code;
  switch (key)
  {
    case Keyboard::Escape:
      _done = true;
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------
bool Editor::OnKeyReleased(const Event& event)
{
  return false;
}

//----------------------------------------------------------------------------------
bool Editor::OnMouseButtonReleased(const Event& event)
{
  return false;
}

//----------------------------------------------------------------------------------
void Editor::Update()
{
  _eventManager->Poll();

}

//----------------------------------------------------------------------------------
void Editor::Render()
{
  _renderWindow->clear();
  _virtualWindowManager->Update();
  _renderWindow->display();
}

//----------------------------------------------------------------------------------
bool Editor::Run()
{
  while (_renderWindow->isOpen() && !_done)
  {
    Update();
    Render();
  }

  return true;
}

//----------------------------------------------------------------------------------
bool Editor::Close()
{
  return true;
}


//----------------------------------------------------------------------------------
int main(int argc, char** argv)
{

  Editor editor;
  if (!editor.Init())
    return 1;

  editor.Run();

  editor.Close();

  return 0;
}