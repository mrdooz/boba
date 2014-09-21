#include "editor.hpp"
#include "property_window.hpp"
#include "timeline_window.hpp"
#include "webby/webby.h"
#include "row_var.hpp"
#include <SFML/OpenGL.hpp>

//#define GLEW_STATIC
//#include "glew/include/GL/glew.h"
//#include "glfw/include/GLFW/glfw3.h"
//#define STB_IMAGE_IMPLEMENTATION
#include "imgui/stb_image.h"                  // for .png loading
#include "imgui/imgui.h"
#ifdef _MSC_VER
#pragma warning (disable: 4996)         // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif

using namespace editor;
using namespace google::protobuf;

Editor* Editor::_instance;

static GLuint g_fontTex;
u32 g_width, g_height;

//----------------------------------------------------------------------------------
// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
// - try adjusting ImGui::GetIO().PixelCenterOffset to 0.5f or 0.375f
static void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
{
  if (cmd_lists_count == 0)
    return;

  // We are using the OpenGL fixed pipeline to make the example code simpler to read!
  // A probable faster way to render would be to collate all vertices from all cmd_lists into a single vertex buffer.
  // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  // Setup texture
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, g_fontTex);

  // Setup orthographic projection matrix
  const float width = ImGui::GetIO().DisplaySize.x;
  const float height = ImGui::GetIO().DisplaySize.y;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, width, height, 0.0f, -1.0f, +1.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Render command lists
  for (int n = 0; n < cmd_lists_count; n++)
  {
    const ImDrawList* cmd_list = cmd_lists[n];
    const unsigned char* vtx_buffer = (const unsigned char*)cmd_list->vtx_buffer.begin();
    glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer));
    glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer+8));
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer+16));

    int vtx_offset = 0;
    const ImDrawCmd* pcmd_end = cmd_list->commands.end();
    for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
    {
      glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
      glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
      vtx_offset += pcmd->vtx_count;
    }
  }

  glDisable(GL_SCISSOR_TEST);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

#if 0
static const char* ImImpl_GetClipboardTextFn()
{
  // TODO: magnus
  return nullptr;
  //return glfwGetClipboardString(window);
}

static void ImImpl_SetClipboardTextFn(const char* text, const char* text_end)
{
  // TODO: magnus
/*
  if (!text_end)
    text_end = text + strlen(text);

  if (*text_end == 0)
  {
    // Already got a zero-terminator at 'text_end', we don't need to add one
    glfwSetClipboardString(window, text);
  }
  else
  {
    // Add a zero-terminator because glfw function doesn't take a size
    char* buf = (char*)malloc(text_end - text + 1);
    memcpy(buf, text, text_end-text);
    buf[text_end-text] = '\0';
    glfwSetClipboardString(window, buf);
    free(buf);
  }
*/
}


// GLFW callbacks to get events
// static void glfw_error_callback(int error, const char* description)
// {
//   fputs(description, stderr);
// }
// 
// static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
// {
//   ImGuiIO& io = ImGui::GetIO();
//   io.MouseWheel = (yoffset != 0.0f) ? yoffset > 0.0f ? 1 : -1 : 0;           // Mouse wheel: -1,0,+1
// }
// 
// static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
// {
//   ImGuiIO& io = ImGui::GetIO();
//   if (action == GLFW_PRESS)
//     io.KeysDown[key] = true;
//   if (action == GLFW_RELEASE)
//     io.KeysDown[key] = false;
//   io.KeyCtrl = (mods & GLFW_MOD_CONTROL) != 0;
//   io.KeyShift = (mods & GLFW_MOD_SHIFT) != 0;
// }
// 
// static void glfw_char_callback(GLFWwindow* window, unsigned int c)
// {
//   if (c > 0 && c <= 255)
//     ImGui::GetIO().AddInputCharacter((char)c);
// }
// 
// // OpenGL code based on http://open.gl tutorials
// void InitGL()
// {
//   glfwSetErrorCallback(glfw_error_callback);
// 
//   if (!glfwInit())
//     exit(1);
// 
// //   glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
// //   window = glfwCreateWindow(1280, 720, "ImGui OpenGL example", NULL, NULL);
// //   glfwMakeContextCurrent(window);
// //   glfwSetKeyCallback(window, glfw_key_callback);
// //   glfwSetScrollCallback(window, glfw_scroll_callback);
// //   glfwSetCharCallback(window, glfw_char_callback);
// // 
// //   glewInit();
// }
#endif

//----------------------------------------------------------------------------------
void InitImGui()
{
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2((float)g_width, (float)g_height);        // Display size, in pixels. For clamping windows positions.
  io.DeltaTime = 1.0f/60.0f;                          // Time elapsed since last frame, in seconds (in this sample app we'll override this every frame because our timestep is variable)
  io.PixelCenterOffset = 0.0f;                        // Align OpenGL texels
  io.KeyMap[ImGuiKey_Tab] = sf::Keyboard::Tab;             // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
  io.KeyMap[ImGuiKey_LeftArrow] = sf::Keyboard::Left;
  io.KeyMap[ImGuiKey_RightArrow] = sf::Keyboard::Right;
  io.KeyMap[ImGuiKey_UpArrow] = sf::Keyboard::Up;
  io.KeyMap[ImGuiKey_DownArrow] = sf::Keyboard::Down;
  io.KeyMap[ImGuiKey_Home] = sf::Keyboard::Home;
  io.KeyMap[ImGuiKey_End] = sf::Keyboard::End;
  io.KeyMap[ImGuiKey_Delete] = sf::Keyboard::Delete;
  io.KeyMap[ImGuiKey_Backspace] = sf::Keyboard::BackSpace;
  io.KeyMap[ImGuiKey_Enter] = sf::Keyboard::Return;
  io.KeyMap[ImGuiKey_Escape] = sf::Keyboard::Escape;
  io.KeyMap[ImGuiKey_A] = sf::Keyboard::A;
  io.KeyMap[ImGuiKey_C] = sf::Keyboard::C;
  io.KeyMap[ImGuiKey_V] = sf::Keyboard::V;
  io.KeyMap[ImGuiKey_X] = sf::Keyboard::X;
  io.KeyMap[ImGuiKey_Y] = sf::Keyboard::Y;
  io.KeyMap[ImGuiKey_Z] = sf::Keyboard::Z;

  io.RenderDrawListsFn = ImImpl_RenderDrawLists;
  //io.SetClipboardTextFn = ImImpl_SetClipboardTextFn;
  //io.GetClipboardTextFn = ImImpl_GetClipboardTextFn;

  // Load font texture
  glGenTextures(1, &g_fontTex);

  glBindTexture(GL_TEXTURE_2D, g_fontTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  const void* png_data;
  unsigned int png_size;
  ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
  int tex_x, tex_y, tex_comp;
  void* tex_data = stbi_load_from_memory((const unsigned char*)png_data, (int)png_size, &tex_x, &tex_y, &tex_comp, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_x, tex_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
  stbi_image_free(tex_data);
}

//----------------------------------------------------------------------------------
void UpdateImGui()
{
  ImGuiIO& io = ImGui::GetIO();

  static sf::Clock ticker;
  io.DeltaTime = max(.0001f, ticker.getElapsedTime().asSeconds());

  // Setup inputs
  // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
  // TODO: handle keypresses etc
  double mouse_x, mouse_y;
  Vector2i mousePos = sf::Mouse::getPosition(*EDITOR._renderWindow);
  mouse_x = mousePos.x;
  mouse_y = mousePos.y;
  io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);                           // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
  io.MouseDown[0] = sf::Mouse::isButtonPressed(sf::Mouse::Left);
  io.MouseDown[1] = sf::Mouse::isButtonPressed(sf::Mouse::Right);

  // Start the frame
  ImGui::NewFrame();
}

//----------------------------------------------------------------------------------
void Editor::Create()
{
  if (!_instance)
  {
    _instance = new Editor();
  }
}

//----------------------------------------------------------------------------------
void Editor::Destroy()
{
  if (_instance)
  {
    delete exch_null(_instance);
  }
}

//----------------------------------------------------------------------------------
Editor& Editor::Instance()
{
  return *_instance;
}

//----------------------------------------------------------------------------------
Editor::Editor()
    : _renderWindow(nullptr)
    , _eventManager(nullptr)
    , _virtualWindowManager(nullptr)
    , _lastUpdate(boost::posix_time::not_a_date_time)
    , _curTime(seconds(0))
    , _fileWatchAcc(seconds(0))
    , _readBuffer(256 * 1024)
    , _timelineWindow(nullptr)
    , _propertyWindow(nullptr)
{
  _stateFlags.Set(StateFlagsF::Paused);
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
  _fileWatcher.AddFileWatch(GetAppRoot() + "config/editor_settings.pb", 0, true, 0, [this](const string& filename, void* token)
  {
      return LoadProto(filename.c_str(), &_settings);
  });

  _fileWatcher.AddFileWatch(GetAppRoot() + "config/editor_styles.pb", 0, true, 0, [this](const string& filename, void* token)
  {
      return _styleFactory.Init(filename.c_str());
  });


  protocol::effect::plexus::PlexusConfig plexusSettings;
  if (!LoadProto("config/plexus1.pb", &plexusSettings, true))
    return false;

  //_plexus = FromProtocol(plexusSettings);

#ifdef _WIN32
  g_width = GetSystemMetrics(SM_CXFULLSCREEN);
  g_height = GetSystemMetrics(SM_CYFULLSCREEN);
#else
  auto displayId = CGMainDisplayID();
  g_width = CGDisplayPixelsWide(displayId);
  g_height = CGDisplayPixelsHigh(displayId);
  _appRoot = "/Users/dooz/projects/boba/editor/";
#endif

  g_width = (u32)(0.9f * g_width);
  g_height = (u32)(0.9 * g_height);

  sf::ContextSettings settings;
  _renderWindow = new RenderWindow(sf::VideoMode(g_width, g_height), "...", sf::Style::Default, settings);
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

    float w = (float)g_width/4;
    float h = (float)g_height/2;

    _propertyWindow = new PropertyWindow("PROPERTIES", Vector2f(0, 0), Vector2f(w, h));
    _timelineWindow = new TimelineWindow("TIMELINE", Vector2f(0, h), Vector2f((float)g_width, h));
    _virtualWindowManager->AddWindow(_propertyWindow);
    _virtualWindowManager->AddWindow(_timelineWindow);
  }

  if (!InitWebby())
    return false;

  //InitGL();
  InitImGui();

  return true;
}

//----------------------------------------------------------------------------------
void Editor::WebbyLog(const char* text)
{
  printf("[debug] %s\n", text);
}

//----------------------------------------------------------------------------------
int Editor::WebbyDispatch(WebbyConnection* connection)
{
  if (0 == strcmp("/foo", connection->request.uri))
  {
    WebbyBeginResponse(connection, 200, 14, NULL, 0);
    WebbyWrite(connection, "Hello, world!\n", 14);
    WebbyEndResponse(connection);
    return 0;
  }
  else if (0 == strcmp("/bar", connection->request.uri))
  {
    WebbyBeginResponse(connection, 200, -1, NULL, 0);
    WebbyWrite(connection, "Hello, world!\n", 14);
    WebbyWrite(connection, "Hello, world?\n", 14);
    WebbyEndResponse(connection);
    return 0;
  }
  else if (0 == strcmp(connection->request.uri, "/quit"))
  {
    WebbyBeginResponse(connection, 200, -1, NULL, 0);
    WebbyPrintf(connection, "Goodbye, cruel world\n");
    WebbyEndResponse(connection);
    return 0;
  }
  else
    return 1;
}

//----------------------------------------------------------------------------------
int Editor::WebbyOnWsConnect(WebbyConnection* connection)
{
  return 0;
}

//----------------------------------------------------------------------------------
void Editor::WebbyOnWsConnected(WebbyConnection* connection)
{
  EDITOR._connections.push_back(connection);

}

//----------------------------------------------------------------------------------
void Editor::WebbyOnWsClosed(WebbyConnection* connection)
{
  auto& connections = EDITOR._connections;
  auto it = find(connections.begin(), connections.end(), connection);
  if (it != connections.end())
  {
    connections.erase(it);
  }
}

//----------------------------------------------------------------------------------

int Editor::WebbyOnWsFrame(WebbyConnection* connection, const WebbyWsFrame* frame)
{
  // We should only be getting a single frame when the engine starts up, telling us
  // the current effects
  if (frame->payload_length == 0)
    return 0;

  vector<u8>& buf = EDITOR._readBuffer;
  WebbyRead(connection, buf.data(), frame->payload_length);

  protocol::effect::EffectSettings settings;
  settings.ParseFromArray(buf.data(), frame->payload_length);

  EDITOR._timelineWindow->Reset(settings);
  return 0;
}

//----------------------------------------------------------------------------------
bool Editor::InitWebby()
{
#if _WIN32
  WORD wsa_version = MAKEWORD(2, 2);
  WSADATA wsa_data;
  if (0 != WSAStartup(wsa_version, &wsa_data))
  {
    fprintf(stderr, "WSAStartup failed\n");
    return false;
  }
#endif

  WebbyServerConfig config;
  memset(&config, 0, sizeof config);
  config.bind_address = "127.0.0.1";
  config.listening_port = 13337;
  config.flags = WEBBY_SERVER_WEBSOCKETS | WEBBY_SERVER_LOG_DEBUG;
  config.connection_max = 4;
  config.request_buffer_size = 2048;
  config.io_buffer_size = 8192;
  config.dispatch = &WebbyDispatch;
  config.log = &WebbyLog;
  config.ws_connect = &WebbyOnWsConnect;
  config.ws_connected = &WebbyOnWsConnected;
  config.ws_closed = &WebbyOnWsClosed;
  config.ws_frame = &WebbyOnWsFrame;

  _serverMemory.resize(WebbyServerMemoryNeeded(&config));
  _server = WebbyServerInit(&config, _serverMemory.data(), _serverMemory.size());
  return !!_server;
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
    case Keyboard::Escape: _stateFlags.Set(StateFlagsF::Done); return true;
  }
  return false;
}

//----------------------------------------------------------------------------------
bool Editor::OnKeyReleased(const Event& event)
{
  switch (event.key.code)
  {
    case Keyboard::Key::Space: _stateFlags.Toggle(StateFlagsF::Paused); return true;
  }

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
  ptime now = microsec_clock::local_time();

  WebbyServerUpdate(_server);

  if (_lastUpdate.is_not_a_date_time())
    _lastUpdate = now;

  time_duration delta = now - _lastUpdate;
  _fileWatchAcc += delta;

  if (_fileWatchAcc > seconds(1))
  {
    _fileWatchAcc -= seconds(1);
    _fileWatcher.Tick();
  }
  _lastUpdate = now;

  if (!_stateFlags.IsSet(StateFlagsF::Paused))
  {
    _curTime += delta;
  }

  _eventManager->Poll();
  _virtualWindowManager->Update();
}

//----------------------------------------------------------------------------------
void Editor::SetCurTime(const time_duration& t)
{
  _curTime = t;
}

//----------------------------------------------------------------------------------
void Editor::Render()
{
  _renderWindow->clear();

  _renderWindow->pushGLStates();
  _virtualWindowManager->Draw();
  _renderWindow->popGLStates();

  ImGuiIO& io = ImGui::GetIO();

  UpdateImGui();

  io.MouseWheel = 0;
  //glfwPollEvents();

  static bool xx = true;
  ImGui::Begin("Another Window", &xx, ImVec2(200, 100));
  ImGui::Text("Hello");
  ImGui::End();

  // Rendering
  ImGui::Render();

  _renderWindow->display();
}

//----------------------------------------------------------------------------------
bool Editor::Run()
{
  while (_renderWindow->isOpen() && !_stateFlags.IsSet(StateFlagsF::Done))
  {
    Update();
    Render();
  }

  return true;
}

//----------------------------------------------------------------------------------
bool Editor::Close()
{
  WebbyServerShutdown(_server);
  return true;
}

//----------------------------------------------------------------------------------
void Editor::SettingsUpdated(const protocol::effect::EffectSettings& settings)
{
  for (WebbyConnection* conn : _connections)
  {
    WebbyBeginSocketFrame(conn,WEBBY_WS_OP_BINARY_FRAME);
    string str = settings.SerializeAsString();
    WebbyWrite(conn, str.data(), str.size());
    WebbyEndSocketFrame(conn);
  }
}

//----------------------------------------------------------------------------------
void Editor::SendEffectEvent(RowVar* sender, const EffectRowEvent& event)
{
  _timelineWindow->SendEffectEvent(sender, event);
  _propertyWindow->SendEffectEvent(sender, event);
}

//----------------------------------------------------------------------------------
int main(int argc, char** argv)
{

  Editor::Create();
  if (!EDITOR.Init())
    return 1;

  EDITOR.Run();

  EDITOR.Close();

  Editor::Destroy();

  return 0;
}